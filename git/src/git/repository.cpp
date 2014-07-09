#include "cocaine/service/git/repository.hpp"
#include <cocaine/api/storage.hpp>

using namespace cocaine;
using namespace cocaine::service::git;

repository_t::repository_t(context_t & context,
                           const std::string & path) :
    m_context(context)
{
    int error = 0;

    git_repository      * p_repository;
    git_odb             * p_odb;
//    git_refdb           * p_refdb;
    git_odb_backend     * p_odb_backend;
//    git_refdb_backend   * p_refdb_backend;

    Json::Value args;
    auto storage = m_context.get<api::storage_t>(std::string("core"), context, "service/git", args);

    error = git_odb_new(&p_odb);
    if (error) throw std::runtime_error("Couldn't create odb");
    //m_odb.reset(p_odb);

    error = git_repository_wrap_odb(&p_repository, p_odb);
    if (error) {
        git_odb_free(p_odb);
        throw std::runtime_error("Couldn't create repository");
    }
    //m_repository.reset(p_repository);

    error = git_odb_backend_cocaine(&p_odb_backend, storage, path);
    if (error) {
        git_odb_free(p_odb);
        throw std::runtime_error("Couldn't create odb backend");
    }
    //m_odb_backend.reset(p_odb_backend);

    error = git_odb_add_backend(p_odb, p_odb_backend, 1);
    if (error) {
        git_odb_free(p_odb);
        // git_odb_backend_free(p_odb_backend);
        throw std::runtime_error("Couldn't add odb backend to odb database");
    }

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
    git_repository_free(m_repository);
}
