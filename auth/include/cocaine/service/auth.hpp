#pragma once

#include <memory>

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>
#include <cocaine/rpc/tags.hpp>
#include <cocaine/traits/tuple.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/mpl/list.hpp>

#include "cocaine/service/auth/authentication/plain.hpp"
#include "cocaine/service/auth/storage/cocaine_storage.hpp"

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
            /* token */  std::string
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

namespace auth {
    typedef boost::mpl::list<
        storage::cocaine_storage
    > storages;

    typedef boost::mpl::list<
        authentication::plain
    > authenticators;
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
    context_t & m_context;
    //std::shared_ptr<auth::storage_t> m_storage;
    auth::storage_t & m_storage;

    /*class storage_initializer {
    public:
        storage_initializer(auth_t & parent, const Json::Value & args) : m_parent(parent), m_args(args) {};
        template<typename U>
        void operator()(U) {
            if (boost::lexical_cast<std::string>(auth::storage::type_name<U>::value) != m_args.get("type", "").asString()) return;
            m_parent.m_storage = std::make_shared(new U(m_parent.m_context, m_args));
        }
    private:
        auth_t & m_parent;
        const Json::Value & m_args;
    };*/
};

} // namespace service

} // namespace cocaine
