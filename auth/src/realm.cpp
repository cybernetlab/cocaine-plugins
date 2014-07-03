#include "cocaine/service/auth/realm.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include "unique_id.hpp"

using namespace cocaine::service::auth;

realm_t::realm_t(std::shared_ptr<logging::log_t> log,
                 storage_ptr storage,
                 storage_ptr cache,
                 const std::string & path):
    m_log(log),
    m_storage(storage),
    m_cache(cache),
    m_path(path),
    m_config(get("config")),
    m_permissions(get("permissions")),
    m_index(get("index"))
{
    authentication::factory().fill(m_authenticators, m_config["authentication"]);
    if (m_authenticators.size() == 0) {
        COCAINE_LOG_WARNING(m_log, "Where are no authentication methods specified in config");
    }
    m_parent = nullptr;
}

realm_t::realm_t(std::shared_ptr<logging::log_t> log,
                 storage_ptr storage,
                 storage_ptr cache):
    realm_t(log, storage, cache, "")
{
    m_sessions = std::make_shared<Json::Value>(get("sessions", "sessions"));
}

realm_t::realm_t(std::shared_ptr<logging::log_t> log,
                 storage_ptr storage,
                 storage_ptr cache,
                 const std::string & path,
                 realm_t * parent):
    realm_t(log, storage, cache, path)
{
    m_sessions = parent->m_sessions;
    m_parent = parent;
}

Json::Value
realm_t::get(const std::string & name)
{
    return get(name, "");
}

Json::Value
realm_t::get(const std::string & name, const char * type)
{
    return get(name, std::string(type));
}

// retrieve any object in this realm (directory)
Json::Value
realm_t::get(const std::string & name, const std::string & type)
{
    std::string path_str = path(type);
    if (!m_cache) return m_storage->load_safe(path_str, name);
    try {
        return m_cache->load(path_str, name);
    } catch (storage::not_found_error) {
        Json::Value obj(m_storage->load_safe(path_str, name));
        if (!obj.isNull()) m_cache->save_safe(path_str, name, obj);
        return obj;
    } catch (storage::error) {
        return Json::Value::null;
    }
}

void
realm_t::save(const char * name, Json::Value & data)
{
    save(std::string(name), data);
}

void
realm_t::save(const std::string & name, const char * type, Json::Value & data)
{
    save(name, std::string(type), data);
}

void
realm_t::save(const char * name, const char * type, Json::Value & data)
{
    save(std::string(name), std::string(type), data);
}

void
realm_t::save(const std::string & name, Json::Value & data)
{
    save(name, "", data);
}

// save object in realm
void
realm_t::save(const std::string & name, const std::string & type, Json::Value & data)
{
    std::string path_str = path(type);
    if (m_cache) m_cache->save(path_str, name, data);
    try {
        m_storage->save(path_str, name, data);
    } catch (storage::error & e) {
        if (m_cache) m_cache->remove_safe(path_str, name);
        throw e;
    }
}

// find object and target realm by token
realm_t::resolve_token_result
realm_t::resolve_token(const std::string & token)
{
    // get session by token
    Json::Value session = m_sessions->get(token, Json::Value::null);
    // raise error on invalid token
    if (session.isNull() || !session.isObject() ||
        !session["user_path"].isString() ||
        !session["user"].isString()) throw not_found_error();
    // get user info from session
    std::string path_str = session["user_path"].asString();
    Json::Value user;
    realm_t * realm;
    // find user and target realm
    if (path_str == "") {
        user = get(session["user"].asString(), "user");
        realm = this;
    } else {
        std::vector<std::string> path;
        boost::split(path, path_str, boost::is_any_of("./"), boost::token_compress_on);
        string_iter begin = path.begin();
        string_iter end = path.end();
        // resolve user realm
        auto r = resolve(begin, end, session["user"].asString(), false);
        // retrieve user object
        user = r.second.get(r.first, "user");
        realm = &r.second;
    }
    // check sessions integrity
    if (user.isNull() || !inArray(user["sessions"], token)) {
        m_sessions->removeMember(token);
        try { save("sessions", "sessions", *m_sessions); } catch (storage::error) {}
        throw not_found_error();
    }
    return resolve_token_result(user, *realm);
}

// find object name and target realm by fully-qualified name
realm_t::resolve_result
realm_t::resolve(const std::string & name)
{
    size_t at_pos = name.find('@');
    // if user name has no '@' symbol, assume it is in root namespace
    if (at_pos == std::string::npos) {
        return resolve_result(std::string(name), *this);
    }
    // assume all after '@' is realm path
    std::string path_str = name.substr(at_pos + 1);
    if (path_str == "") {
        return resolve_result(std::string(name), *this);
    }
    std::vector<std::string> path;
    boost::split(path, path_str, boost::is_any_of("./"), boost::token_compress_on);
    std::string name_ = name.substr(0, at_pos);
    string_iter begin = path.begin();
    string_iter end = path.end();
    // resolve realm and user name
    return resolve(begin, end, name_, true);
}

realm_t::resolve_result
realm_t::resolve(string_iter & begin,
                 string_iter & end,
                 const std::string & name,
                 bool create_child)
{
    if (m_children.find(*begin) == m_children.end()) {
        // create new realm if where are no one cached
        boost::filesystem::path path(m_path);
        path /= *begin;
        m_children.insert(children_map_t::value_type(
            *begin,
            realm_t(m_log, m_storage, m_cache, path.string(), this)
        ));
    }
    realm_t & child = m_children.at(*begin);
    // skip to next part of realm path
    begin++;
    // return if end of realm path reached
    if (begin == end) return resolve_result(std::string(name), child);
    // recursive search
    return child.resolve(begin, end, name, create_child);
}

// authenticate user
realm_t::bool_result
realm_t::authenticate(const std::string & type,
                      const std::string & name,
                      const std::string & data)
{
    try {
        // retrieve target realm and user name
        auto r = resolve(name);
        // get user object
        Json::Value user = r.second.get(r.first, "user");
        if (user.isNull()) return std::make_tuple(false, "user not found");
        // ensure required fields are right filled
        user["name"] = r.first;
        user["type"] = "user";
        // ask target realm to authenticate user
        return r.second.authenticate(type, user, data);
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}

// remove user session
realm_t::bool_result
realm_t::logout(const std::string & token)
{
    try {
        // retreive user and target realm by token
        auto r = resolve_token(token);
        if (r.first.isNull()) return std::make_tuple(false, "wrong token");
        // remove session from user["sessions"] array
        Json::Value arr(Json::ValueType::arrayValue);
        for (auto item : r.first["sessions"]) {
            if (!item.isString() || item.asString() == token) continue;
            arr.append(item);
        }
        r.first["sessions"] = arr;
        // clear session
        m_sessions->removeMember(token);
        // save all
        r.second.save(r.first["name"].asString(), "user", r.first);
        save("sessions", "sessions", *m_sessions);

        COCAINE_LOG_DEBUG(m_log, "User %s successfully logged out. Session ID: %s", r.first["name"].asString(), token);

        return std::make_tuple(true, "");
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}

// authorize user to specific permission
realm_t::bool_result
realm_t::authorize(const std::string & token,
                   const std::string & perm)
{
    try {
        // retrieve user and target realm by token
        auto r = resolve_token(token);
        if (r.first.isNull()) return std::make_tuple(false, "wrong token");
        // ask target realm to authorize user
        return r.second.authorize(r.first, perm);
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}

realm_t::bool_result
realm_t::authorize(Json::Value & user,
                   const std::string & perm)
{
    if (user.isNull() || !user.isObject()) return std::make_tuple(false, "user not found");
    // generate chain of realms up to root realm
    std::list<realm_t *> realms;
    for (realm_t * cur = this; cur != nullptr; cur = cur->m_parent) {
        realms.push_back(cur);
    }
    // get fully-qualified roles names
    Json::Value roles = user["roles"];
    qualify_names(roles);
    // check for denies from root to target realm
    for (auto realm : boost::adaptors::reverse(realms)) {
        if (realm->check_rights(roles, "deny", perm)) {
            return std::make_tuple(false, "");
        }
    }
    // check for allows from target realm to root
    for (auto realm : realms) {
        if (realm->check_rights(roles, "allow", perm)) {
            COCAINE_LOG_DEBUG(m_log, "User %s successfully authorized to %s", user["name"].asString(), perm);
            return std::make_tuple(true, "");
        }
    }
    return std::make_tuple(false, "");
}

void
realm_t::qualify_names(Json::Value & names)
{
    if (!m_parent) return;
    std::string realm_name = m_path;
    std::replace(realm_name.begin(), realm_name.end(), '/', '.');
    for (auto & name : names) {
        if (!name.isString()) continue;
        if (name.asString().find('@') != std::string::npos) continue;
        name = name.asString() + "@" + realm_name;
    }
}

bool
realm_t::check_rights(const Json::Value & roles,
                      const char * action,
                      const std::string & perm)
{
    for (auto & role: roles) {
        for (auto & permission: m_permissions) {
            if (!permission.isMember(action)) continue;
            Json::Value roles_to_check = permission[action];
            if (!roles_to_check.isArray()) continue;
            qualify_names(roles_to_check);
            if (!inArray(roles_to_check, role.asString(), '@')) continue;
            if (!inArray(permission["to"], perm, ':')) continue;
            return true;
        }
    }
    return false;
}

realm_t::bool_result
realm_t::authenticate(const std::string & type,
                      Json::Value & user,
                      const std::string & data)
{
    if (user.isNull() || !user.isObject()) return std::make_tuple(false, "user not found");
    // find authentication method
    auto it = m_authenticators.find(type);
    if (it == m_authenticators.end()) return std::make_tuple(false, "wrong authentication type");
    // authenticate user
    if (!(*it->second)(user, data)) return std::make_tuple(false, "authentication failed");
    // return immediately if user is single-session and it allready logged in
    if (user["singleSession"].asBool() && !user["sessions"].empty()) {
        return std::make_tuple(true, user["sessions"][0].asString());
    }
    // generate token
    unique_id_t uuid = unique_id_t();
    const std::string uuid_str(uuid.string());
    // add session to user["sessions"] array
    if (user["sessions"].isNull()) user["sessions"] = Json::Value(Json::ValueType::arrayValue);
    user["sessions"][user["sessions"].size()] = uuid_str;
    // fill session object
    (*m_sessions)[uuid_str] = Json::Value(Json::ValueType::objectValue);
    (*m_sessions)[uuid_str]["user"] = user["name"];
    (*m_sessions)[uuid_str]["user_path"] = m_path;
    // save all
    save(user["name"].asString(), "user", user);
    save("sessions", "sessions", *m_sessions);

    COCAINE_LOG_DEBUG(m_log, "User %s successfully authenticated. Session ID: %s", user["name"].asString(), uuid_str);

    // return token
    return std::make_tuple(true, uuid_str);
}

std::string
realm_t::path(const std::string & type)
{
    boost::filesystem::path p(m_path);
    if (type == "user") {
        p /= "users";
    } else if (type == "sessions") {
        p = "";
    }
    return p.string();
}

bool
realm_t::inArray(const Json::Value & arr,
                 const std::string & value) const
{
    if (!arr.isArray() || arr.empty()) return false;
    for (auto & item: arr) {
        if (!item.isString()) continue;
        if (item.asString() == value) return true;
    }
    return false;
}

bool
realm_t::inArray(const Json::Value & arr,
                 const std::string & value,
                 const char delimiter) const
{
    if (!arr.isArray() || arr.empty()) return false;

    std::string sanitize_str("[^a-zA-Z0-9_.*");
    sanitize_str += delimiter;
    sanitize_str += "-]";

    std::string star_repl("[^.");
    star_repl += delimiter;
    star_repl += "]+";

    std::string stars_repl("[^");
    stars_repl += delimiter;
    stars_repl += "]+";

    boost::regex sanitize(sanitize_str, boost::regex::extended);
    boost::regex dots("\\.", boost::regex::extended);
    boost::regex stars("\\*\\*", boost::regex::extended);
    boost::regex star("\\*", boost::regex::extended);

    for (auto & item: arr) {
        if (!item.isString()) continue;
        std::string check(item.asString());
        if (check == "*") return true;
        if (check.find('*') == std::string::npos) {
            if (check == value) {
                return true;
            } else {
                continue;
            }
        }

        std::string re_str(
            "^" +
            boost::regex_replace(
                boost::regex_replace(
                    boost::regex_replace(
                        boost::regex_replace(check, sanitize, ""),
                    dots, "\\."),
                stars, stars_repl),
            star, star_repl)
            + "$"
        );

        boost::regex re(re_str, boost::regex::extended);
        if (boost::regex_match(value, re)) return true;
    }
    return false;
}
