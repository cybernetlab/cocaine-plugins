#pragma once

#include <memory>

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>
#include <cocaine/rpc/tags.hpp>
#include <cocaine/traits/tuple.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/mpl/list.hpp>

#include "cocaine/service/auth/authentication.hpp"
#include "cocaine/service/auth/authorization.hpp"
#include "cocaine/service/auth/storage.hpp"

namespace cocaine {

namespace io {

struct auth_tag;

namespace auth {
    struct authenticate {
        typedef auth_tag tag;

        typedef boost::mpl::list<
            /* type */ std::string,
            /* name */ std::string,
            /* data */ std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool,
            /* token */  std::string
        > result_type;
    };

    struct logout {
        typedef auth_tag tag;

        typedef boost::mpl::list<
            /* token */ std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool,
            /* reason */  std::string
        > result_type;
    };

    struct authorize {
        typedef auth_tag tag;

        typedef boost::mpl::list<
            /* token */ std::string,
            /* perm */  std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool,
            /* ticket */ std::string
        > result_type;
    };
} // namespace auth

template<>
struct protocol<auth_tag> {
    typedef boost::mpl::list<
        auth::authenticate,
        auth::logout,
        auth::authorize
    > type;

    typedef boost::mpl::int_<
        1
    >::type version;
};

} // namespace io

namespace service {

namespace response {
    typedef io::event_traits<io::auth::authenticate>::result_type authenticate;
    typedef io::event_traits<io::auth::authorize>::result_type authorize;
    typedef io::event_traits<io::auth::logout>::result_type logout;
}

class auth_t: public api::service_t {
public:
    auth_t(context_t & context,
           io::reactor_t & reactor,
           const std::string & name,
           const Json::Value & args);

private:
    cocaine::deferred<response::authenticate>
    authenticate(const std::string & type, const std::string & name, const std::string & data);

    cocaine::deferred<response::logout>
    logout(const std::string & token);

    cocaine::deferred<response::authorize>
    authorize(const std::string & token, const std::string & perm);

    void
    save_user(Json::Value & user);

    void
    save_sessions(Json::Value & user);

    context_t & m_context;
    std::shared_ptr<logging::log_t> m_log;
    std::shared_ptr<auth::storage_t> m_storage;
    auth::authentication::map_t m_authenticators;
    auth::authorization::permissions_t m_permissions;
};

} // namespace service

} // namespace cocaine
