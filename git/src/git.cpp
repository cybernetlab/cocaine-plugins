#include <cocaine/logging.hpp>
#include <git2/threads.h>

#include "cocaine/service/git.hpp"
#include "cocaine/service/git/repository.hpp"

using namespace std::placeholders;
using namespace cocaine::service;
using namespace cocaine::service::git;

git_t::git_t(cocaine::context_t & context,
             cocaine::io::reactor_t & reactor,
             const std::string & name,
             const Json::Value & args) :
    service_t(context, reactor, name, args),
    m_context(context),
    m_log(std::make_shared<logging::log_t>(context, name))
{
    git_threads_init();
    on<io::git::create>("create", std::bind(&git_t::create, this, _1));
    COCAINE_LOG_DEBUG(m_log, "Git service started");
}

git_t::~git_t()
{
    git_threads_shutdown();
}

cocaine::deferred<response::create>
git_t::create(const std::string & path) {
    cocaine::deferred<response::create> deferred;
    try {
        repository_t repo(m_context, path);
        deferred.write(std::make_tuple<bool>(true));
    } catch (const error_t & e) {
        COCAINE_LOG_INFO(m_log, "Error while processing create request. %s", e.what());
        deferred.write(std::make_tuple<bool>(false));
    }
    return deferred;
}

/*cocaine::deferred<response::logout>
git_t::logout(const std::string &token) {
    cocaine::deferred<response::logout> deferred;
    deferred.write(m_root.logout(token));
    return deferred;
}

cocaine::deferred<response::authorize>
git_t::authorize(const std::string &token, const std::string &perm) {
    cocaine::deferred<response::authenticate> deferred;
    deferred.write(m_root.authorize(token, perm));
    return deferred;
}*/
