#include <cocaine/logging.hpp>

#include "unique_id.hpp"
#include "cocaine/service/auth.hpp"

using namespace std::placeholders;
using namespace cocaine::service;

auth_t::auth_t(cocaine::context_t & context,
               cocaine::io::reactor_t & reactor,
               const std::string & name,
               const Json::Value & args) :
    service_t(context, reactor, name, args),
    m_context(context),
    m_log(std::make_shared<logging::log_t>(context, name)),
    m_storage(auth::storage::factory().create(context, args["storage"])),
    m_authenticators(auth::authentication::factory(args["authentication"]).create()),
    m_directory(m_log, m_storage, ""),
    m_permissions(m_log, m_storage)
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
    std::shared_ptr<auth::directory::user_t> user_ptr = m_directory.get<auth::directory::user_t>("user", name);
    std::cout << "USER: " << user_ptr->get("name", "test") << "\n";
    auth::directory::user_t user = *(user_ptr.get());
    // Json::Value user = user_.data();
    std::cout << "received user " << user["name"] << "\n";
    // Json::Value user = m_storage->load("users", name);
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
    Json::Value sessions = m_storage->load("", "sessions");
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
    Json::Value sessions = m_storage->load("", "sessions");
    if (sessions[token].isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    try {
        // find user for session
        Json::Value user = m_storage->load("users", sessions[token].asString());
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
    Json::Value sessions = m_storage->load("", "sessions");
    if (sessions[token].isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    // find user for session
    Json::Value user = m_storage->load("users", sessions[token].asString());
    if (user.isNull()) {
        deferred.write(std::make_tuple(false, "wrong token"));
        return deferred;
    }
    // return permission check result
    deferred.write(std::make_tuple(m_permissions.check(user, perm), ""));
    return deferred;
}

void
auth_t::save_user(Json::Value & user)
{
    if (!m_storage->save("users", user["name"].asString(), user)) {
        COCAINE_LOG_INFO(m_log, "Error while writing user %s to storage", user["name"].asString());
        throw false;
    }
}

void
auth_t::save_sessions(Json::Value & sessions)
{
    if (!m_storage->save("", "sessions", sessions)) {
        COCAINE_LOG_INFO(m_log, "Error while writing sessions to storage");
        throw false;
    }
}
