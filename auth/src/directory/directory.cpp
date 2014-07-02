#include "cocaine/service/auth/directory.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "unique_id.hpp"

using namespace cocaine::service::auth;

directory_t::directory_t(std::shared_ptr<logging::log_t> log,
                         storage_ptr storage,
                         storage_ptr cache,
                         const std::string & path):
    m_log(log),
    m_storage(storage),
    m_cache(cache),
    m_path(""),
    m_config(get("config")),
    m_permissions(get("permissions")),
    m_index(get("index"))
{
    authentication::factory().fill(m_authenticators, m_config["authentication"]);
    if (m_authenticators.size() == 0) {
        COCAINE_LOG_WARNING(m_log, "Where are no authentication methods specified in config");
    }
}

directory_t::directory_t(std::shared_ptr<logging::log_t> log,
                         storage_ptr storage,
                         storage_ptr cache):
    directory_t(log, storage, cache, "")
{
    m_sessions = std::make_shared<Json::Value>(get("sessions", "sessions"));
}

directory_t::directory_t(std::shared_ptr<logging::log_t> log,
                         storage_ptr storage,
                         storage_ptr cache,
                         const std::string & path,
                         std::shared_ptr<Json::Value> sessions):
    directory_t(log, storage, cache, path)
{
    m_sessions = sessions;
}

Json::Value
directory_t::get(const std::string & name)
{
    return get(name, "");
}

Json::Value
directory_t::get(const std::string & name, const char * type)
{
    return get(name, std::string(type));
}

Json::Value
directory_t::get(const std::string & name, const std::string & type)
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
directory_t::save(const char * name, Json::Value & data)
{
    save(std::string(name), data);
}

void
directory_t::save(const std::string & name, const char * type, Json::Value & data)
{
    save(name, std::string(type), data);
}

void
directory_t::save(const char * name, const char * type, Json::Value & data)
{
    save(std::string(name), std::string(type), data);
}

void
directory_t::save(const std::string & name, Json::Value & data)
{
    save(name, "", data);
}

void
directory_t::save(const std::string & name, const std::string & type, Json::Value & data)
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

directory_t::resolve_token_result
directory_t::resolve_token(const std::string & token)
{
    Json::Value session = m_sessions->get(token, Json::Value::null);
    if (session.isNull() || !session.isObject() ||
        !session["user_path"].isString() ||
        !session["user"].isString()) throw not_found_error();
    std::string path_str = session["user_path"].asString();
    std::vector<std::string> path;
    boost::split(path, path_str, boost::is_any_of("./"), boost::token_compress_on);
    string_iter begin = path.begin();
    string_iter end = path.end();
    auto r = resolve(begin, end, session["user"].asString(), false);
    Json::Value user = get(r.first, "user");
    if (user.isNull() || !inArray(user["sessions"], token)) {
        m_sessions->removeMember(token);
        try { save("sessions", "sessions", *m_sessions); } catch (storage::error) {}
        throw not_found_error();
    }
    return resolve_token_result(user, r.second);
}

directory_t::resolve_result
directory_t::resolve(const std::string & name)
{
    size_t at_pos = name.find('@');
    if (at_pos == std::string::npos) {
        return resolve_result(std::string(name), *this);
    }
    std::string path_str = name.substr(at_pos + 1);
    std::vector<std::string> path;
    boost::split(path, path_str, boost::is_any_of("./"), boost::token_compress_on);
    std::string name_ = name.substr(0, at_pos);
    string_iter begin = path.begin();
    string_iter end = path.end();
    return resolve(begin, end, name_, true);
}

directory_t::resolve_result
directory_t::resolve(string_iter & begin,
                     string_iter & end,
                     const std::string & name,
                     bool create_child)
{
    if (m_children.find(*begin) == m_children.end()) {
        // if (!create_child) throw not_found_error();
        boost::filesystem::path path(m_path);
        path /= *begin;
        m_children.insert(children_map_t::value_type(
            *begin,
            directory_t(m_log, m_storage, m_cache, path.string(), m_sessions)
        ));
    }
    directory_t & child = m_children.at(*begin);
    begin++;
    if (begin == end) return resolve_result(std::string(name), child);
    return child.resolve(begin, end, name, create_child);
}

directory_t::bool_result
directory_t::authenticate(const std::string & type,
                          const std::string & name,
                          const std::string & data)
{
    try {
        auto r = resolve(name);
        Json::Value user = get(r.first, "user");
        if (user.isNull()) return std::make_tuple(false, "user not found");
        user["name"] = r.first;
        user["type"] = "user";
        return r.second.authenticate(type, user, data);
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}

directory_t::bool_result
directory_t::logout(const std::string & token)
{
    try {
        auto r = resolve_token(token);
        if (r.first.isNull()) return std::make_tuple(false, "wrong token");
        Json::Value arr(Json::ValueType::arrayValue);
        for (auto item : r.first["sessions"]) {
            if (!item.isString() || item.asString() == token) continue;
            arr.append(item);
        }
        r.first["sessions"] = arr;
        m_sessions->removeMember(token);
        // save all
        r.second.save(r.first["name"].asString(), "user", r.first);
        save("sessions", "sessions", *m_sessions);
        return std::make_tuple(true, "");
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}

directory_t::bool_result
directory_t::authorize(const std::string & token,
                       const std::string & perm)
{
    try {
        auto r = resolve_token(token);
        if (r.first.isNull()) return std::make_tuple(false, "wrong token");
        return r.second.authorize(r.first, perm);
    } catch (storage::error) {
        return std::make_tuple(false, "internal server error");
    } catch (not_found_error & e) {
        return std::make_tuple(false, e.what());
    }
}


directory_t::bool_result
directory_t::authorize(Json::Value & user,
                       const std::string & perm)
{
    if (user.isNull() || !user.isObject()) return std::make_tuple(false, "user not found");
    // check for denied premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: m_permissions) {
            if (!inArray(permission["deny"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            return std::make_tuple(false, "");
        }
    }
    // check for allowed premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: m_permissions) {
            if (!inArray(permission["allow"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            return std::make_tuple(true, "");
        }
    }
    return std::make_tuple(false, "");
}

directory_t::bool_result
directory_t::authenticate(const std::string & type,
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
    if (user["sessions"].isNull()) user["sessions"] = Json::Value(Json::ValueType::arrayValue);
    user["sessions"][user["sessions"].size()] = uuid_str;
    (*m_sessions)[uuid_str] = Json::Value(Json::ValueType::objectValue);
    (*m_sessions)[uuid_str]["user"] = user["name"];
    (*m_sessions)[uuid_str]["user_path"] = m_path;
    // save all
    save(user["name"].asString(), "user", user);
    save("sessions", "sessions", *m_sessions);
    // return token
    return std::make_tuple(true, uuid_str);
}

std::string
directory_t::path(const std::string & type)
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
directory_t::inArray(const Json::Value & arr, const std::string & value) const
{
    if (!arr.isArray() || arr.empty()) return false;
    for (auto & item: arr) {
        if (!item.isString()) continue;
        if (item.asString() == value) return true;
    }
    return false;
}
