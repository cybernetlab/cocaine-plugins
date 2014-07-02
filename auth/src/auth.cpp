#include <cocaine/logging.hpp>

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
    m_cache(auth::storage::factory().create(context, args["cache"])),
    m_directory(m_log, m_storage, m_cache)
{
    if (!m_storage) throw cocaine::error_t("wrong storage config");

    COCAINE_LOG_DEBUG(m_log, "Auth service started");

    on<io::auth::authenticate>("authenticate", std::bind(&auth_t::authenticate, this, _1, _2, _3));
    on<io::auth::logout>("logout", std::bind(&auth_t::logout, this, _1));
    on<io::auth::authorize>("authorize", std::bind(&auth_t::authorize, this, _1, _2));
}

cocaine::deferred<response::authenticate>
auth_t::authenticate(const std::string & type,
                     const std::string & name,
                     const std::string & data) {
    cocaine::deferred<response::authenticate> deferred;
    deferred.write(m_directory.authenticate(type, name, data));
    return deferred;
}

cocaine::deferred<response::logout>
auth_t::logout(const std::string &token) {
    cocaine::deferred<response::logout> deferred;
    deferred.write(m_directory.logout(token));
    return deferred;
}

cocaine::deferred<response::authorize>
auth_t::authorize(const std::string &token, const std::string &perm) {
    cocaine::deferred<response::authenticate> deferred;
    deferred.write(m_directory.authorize(token, perm));
    return deferred;
}
