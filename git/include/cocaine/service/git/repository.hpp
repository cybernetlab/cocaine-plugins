#pragma once

#include <assert.h>
#include <string.h>
#include <git2.h>
#include <git2/odb.h>
//#include <git2/refdb.h>
#include <git2/sys/odb_backend.h>
//#include <git2/sys/refdb_backend.h>
#include <json/value.h>

#include <cocaine/context.hpp>

#include "cocaine/service/git/cocaine_odb_backend.hpp"

namespace cocaine { namespace service { namespace git {

typedef std::unique_ptr<git_repository>      repository_ptr;
typedef std::unique_ptr<git_odb>             odb_ptr;
//typedef std::unique_ptr<git_refdb>           refdb_ptr;
typedef std::unique_ptr<git_odb_backend>     odb_backend_ptr;
//typedef std::unique_ptr<git_refdb_backend>   refdb_backend_ptr;

class repository_t {
public:
    repository_t(context_t & context, const std::string & path);
    ~repository_t();
private:
    context_t & m_context;

    git_repository                  * m_repository;
    std::shared_ptr<api::storage_t> m_storage;
//    odb_ptr           m_odb;
//    odb_backend_ptr   m_odb_backend;
//    refdb_ptr         m_refdb;
//    refdb_backend_ptr m_refdb_backend;
};

} } }
