#include "cocaine/service/git/repository.hpp"
#include <cocaine/api/storage.hpp>

using namespace cocaine;
using namespace cocaine::service::git;

repository_t::repository_t(context_t & context,
                           const std::string & path) :
    m_context(context),
    m_storage(api::storage(context, std::string("core")))
{
    int error = 0;

//    git_repository      * p_repository;
    git_odb             * p_odb;
//    git_refdb           * p_refdb;
    git_odb_backend     * p_odb_backend;
//    git_refdb_backend   * p_refdb_backend;

    try {
/*        if (git_odb_new(&p_odb) != GIT_OK)
            throw std::runtime_error("Couldn't create odb");
        if (git_repository_wrap_odb(&m_repository, p_odb) != GIT_OK)
            throw std::runtime_error("Couldn't create repository");
        if (git_odb_backend_cocaine(&p_odb_backend, m_storage, path) != GIT_OK)
            throw std::runtime_error("Couldn't create odb backend");
        if (git_odb_add_backend(p_odb, p_odb_backend, 1) != GIT_OK)
            throw std::runtime_error("Couldn't add odb backend to odb database");*/
        if (git_repository_init(&m_repository, path.c_str(), 0) != GIT_OK)
            throw std::runtime_error("Couldn't create repository");
    } catch(const std::runtime_error & e) {
        if (p_odb) git_odb_free(p_odb);
        throw e;
    }
    git_odb_free(p_odb);

/*  error = git_refdb_new(&refdb, repo);
if (!error)
  error = git_refdb_backend_voldemort(&voldemort_refdb_backend, refdb, "my_repo", "tcp://localhost:6666", "git_refdb");
if (!error)
  error = git_refdb_set_backend(refdb, voldemort_refdb_backend);
if (!error)
  git_repository_set_refdb(repo, refdb);*/
}

repository_t::~repository_t()
{
    if (m_repository) git_repository_free(m_repository);
}
