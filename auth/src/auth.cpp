#include <cocaine/logging.hpp>

#include "unique_id.hpp"
#include "cocaine/service/auth.hpp"
#include "cocaine/service/auth/authenticate.hpp"

using namespace std::placeholders;
using namespace cocaine::service;

auth_t::auth_t(cocaine::context_t & context,
               cocaine::io::reactor_t & reactor,
               const std::string & name,
               const Json::Value & args) :
    service_t(context, reactor, name, args),
    m_context(context),
    m_log(std::make_shared<logging::log_t>(context, name)),
    m_storage(auth::storage::factory(context, args["storage"]).create()),
    m_authenticators(auth::authentication::factory(args["authentication"]).create()),
    m_namespace(args.get("namespace", "").asString())
{
    COCAINE_LOG_DEBUG(m_log, "Auth started");

    if (m_authenticators.size() == 0) {
        COCAINE_LOG_WARNING(m_log, "Where are no authentication methods specified in config");
    }

    on<io::auth::authenticate>("authenticate", std::bind(&auth_t::authenticate, this, _1, _2, _3));
    on<io::auth::logout>("logout", std::bind(&auth_t::logout, this, _1));
    on<io::auth::authorize>("authorize", std::bind(&auth_t::authorize, this, _1, _2));
}

cocaine::deferred<response::authenticate>
auth_t::authenticate(const std::string & type,
                     const std::string & name,
                     const std::string & data) {
    cocaine::deferred<response::authenticate> deferred;
    // load user
    Json::Value user = m_storage->load(m_namespace, "users", name);
    if (user.isNull()) {
        deferred.write(std::make_tuple(false, "user not found"));
        return deferred;
    }
    if (user["name"] != name || user["type"] != "users") {
        deferred.write(std::make_tuple(false, "invalid user"));
        return deferred;
    }
    // find authentication method
    auto it = m_authenticators.find(type);
    if (it == m_authenticators.end()) {
        deferred.write(std::make_tuple(false, "wrong authentication type"));
        return deferred;
    }
    // authenticate user
    if (!(*it->second)(user, data)) {
        deferred.write(std::make_tuple(false, "authentication failed"));
        return deferred;
    }
    // return immediately if user is allready logged in
    if (!user["session"].isNull()) {
        deferred.write(std::make_tuple(true, user["session"].asString()));
        return deferred;
    }
    // generate token
    unique_id_t uuid = unique_id_t();
    std::string uuid_str(uuid.string());
    Json::Value sessions = m_storage->load(m_namespace, "", "sessions");
    user["session"] = uuid_str;
    sessions[uuid_str] = name;
    // save all
    try {
        save_user(user);
        save_sessions(sessions);
    } catch (bool e) {
        deferred.write(std::make_tuple(false, "internal server error"));
        return deferred;
    }
    // return token
    deferred.write(std::make_tuple(true, uuid_str));
    return deferred;
}

cocaine::deferred<response::logout>
auth_t::logout(const std::string &token) {
    cocaine::deferred<response::logout> deferred;
    // find session
    Json::Value sessions = m_storage->load(m_namespace, "", "sessions");
    if (sessions[token].isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    try {
        // find user for session
        Json::Value user = m_storage->load(m_namespace, "users", sessions[token].asString());
        if (user.isNull() || user["session"].asString() != token) {
            // if no valid user remove this token as it is invalid, return :ok
            sessions.removeMember(token);
            save_sessions(sessions);
            if (!user.isNull()) {
                user.removeMember("session");
                save_user(user);
            }
            deferred.write(std::make_tuple(true, ""));
            return deferred;
        }
        user.removeMember("session");
        sessions.removeMember(token);
        save_user(user);
        save_sessions(sessions);
    } catch (bool e) {
        deferred.write(std::make_tuple(false, "internal server error"));
        return deferred;
    }

    deferred.write(std::make_tuple(true, ""));
    return deferred;
}

cocaine::deferred<response::authorize>
auth_t::authorize(const std::string &token, const std::string &perm) {
    cocaine::deferred<response::authenticate> deferred;
    // find session
    Json::Value sessions = m_storage->load(m_namespace, "", "sessions");
    if (sessions[token].isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    // find user for session
    Json::Value user = m_storage->load(m_namespace, "users", sessions[token].asString());
    if (user.isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    // load permissions
    Json::Value permissions = m_storage->load(m_namespace, "", "permissions");
    // check for denied premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: permissions) {
            if (!inArray(permission["deny"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            deferred.write(std::make_tuple(false, ""));
            return deferred;
        }
    }
    // check for allowed premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: permissions) {
            if (!inArray(permission["allow"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            deferred.write(std::make_tuple(true, ""));
            return deferred;
        }
    }
    // deny by default
    deferred.write(std::make_tuple(false, ""));
    return deferred;
}

void
auth_t::save_user(Json::Value & user)
{
    if (!m_storage->save(m_namespace, "users", user["name"].asString(), user)) {
        COCAINE_LOG_INFO(m_log, "Error while writing user %s to storage", user["name"].asString());
        throw false;
    }
}

void
auth_t::save_sessions(Json::Value & sessions)
{
    if (!m_storage->save(m_namespace, "", "sessions", sessions)) {
        COCAINE_LOG_INFO(m_log, "Error while writing sessions to storage");
        throw false;
    }
}

bool
auth_t::inArray(Json::Value & arr, const std::string & value) const
{
    if (!arr.isArray() || arr.empty()) return false;
    for (auto & item: arr) {
        if (!item.isString()) continue;
        if (item.asString() == value) return true;
    }
    return false;
}
