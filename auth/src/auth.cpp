#include <functional>
#include <tuple>
#include <stdexcept>

#include <cocaine/logging.hpp>

#include "cocaine/service/auth.hpp"
#include "cocaine/service/auth/authenticate.hpp"

using namespace std::placeholders;

using namespace cocaine::service;

auth_t::auth_t(cocaine::context_t &context,
               cocaine::io::reactor_t &reactor,
               const std::string &name,
               const Json::Value &args) :
    service_t(context, reactor, name, args),
    m_log(new logging::log_t(context, name))
{
    COCAINE_LOG_DEBUG(m_log, "Auth started");

    const Json::Value types = args["types"];
//    if (types.isNull) {
//
//    }

    on<io::auth::authenticate>("authenticate", std::bind(&auth_t::authenticate, this, _1, _2));
    on<io::auth::authorize>("authorize", std::bind(&auth_t::authorize, this, _1, _2));
}

cocaine::deferred<response::authenticate>
auth_t::authenticate(const std::string &name, const std::string &salt) {
    cocaine::deferred<response::authenticate> deferred;
    deferred.write(std::make_tuple(true, name));
    return deferred;
}

cocaine::deferred<response::authorize>
auth_t::authorize(const std::string &token, const std::string &perm) {
    cocaine::deferred<response::authenticate> deferred;
    deferred.write(std::make_tuple(true, token));
    return deferred;
}
