#pragma once

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>
#include <cocaine/rpc/tags.hpp>
#include <cocaine/traits/tuple.hpp>

namespace cocaine {

namespace io {

struct auth_tag;

namespace auth {
    struct authenticate {
        typedef auth_tag tag;

        typedef boost::mpl::list<
            /* name */ std::string,
            /* salt */ std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool,
            /* token */ std::string
        > result_type;
    };

    struct authorize {
        typedef auth_tag tag;

        typedef boost::mpl::list<
            /* token */ std::string,
            /* perm */ std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool,
            /* ticket */ std::string
        > result_type;
    };
} // namespace auth

template<>
struct protocol<auth_tag> {
    typedef mpl::list<
        auth::authenticate,
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
}

class auth_t: public api::service_t {
public:
    auth_t(context_t& context,
           io::reactor_t& reactor,
           const std::string& name,
           const Json::Value& args);

private:
    cocaine::deferred<response::authenticate>
    authenticate(const std::string &name, const std::string &salt);

    cocaine::deferred<response::authorize>
    authorize(const std::string &token, const std::string &perm);

    std::shared_ptr<logging::log_t> m_log;
};

} // namespace service

} // namespace cocaine