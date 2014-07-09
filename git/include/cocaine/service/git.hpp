#pragma once

#include <memory>

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>

#include "cocaine/idl/git.hpp"

namespace service {

namespace response {
    typedef io::event_traits<io::git::create>::result_type create;
}

class git_t: public api::service_t {
public:
    git_t(context_t & context,
          io::reactor_t & reactor,
          const std::string & name,
          const Json::Value & args);
    ~git_t();

private:
    cocaine::deferred<response::create>
    create(const std::string & path);

/*    cocaine::deferred<response::logout>
    logout(const std::string & token);

    cocaine::deferred<response::authorize>
    authorize(const std::string & token, const std::string & perm);*/

    context_t & m_context;
    std::shared_ptr<logging::log_t> m_log;
};

} // namespace service

} // namespace cocaine
