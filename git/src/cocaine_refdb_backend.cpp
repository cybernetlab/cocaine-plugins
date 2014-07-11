#include <assert.h>
#include <json/value.h>
#include <cocaine/api/storage.hpp>

#include <git2/refs.h>
#include <git2/errors.h>

#include "cocaine/service/git/cocaine_refdb_backend.hpp"

using namespace cocaine;
using namespace cocaine::service::git;

namespace cocaine { namespace service { namespace git {

#define COCAINE_GIT_REF_TAG "ref"
#define COCAINE_GIT_REFSYM_TAG "ref-symbolic"
#define COCAINE_GIT_REFOID_TAG "ref-oid"

typedef struct {
    git_refdb_backend   parent;
    storage_ptr         db;
    string_ptr          path;
} cocaine_refdb_backend;


/**
* Queries the refdb backend to determine if the given ref_name
* exists. A refdb implementation must provide this function.
*/
int cocaine_refdb_backend__exists(int *exists,
                                  git_refdb_backend *_backend,
                                  const char *ref_name)
{
    assert(_backend && oid);

    cocaine_refdb_backend * backend = (cocaine_refdb_backend *) _backend;
    std::vector<std::string> query;
    query.push_back(std::string(COCAINE_GIT_REF_TAG));
    query.push_back(std::string(ref_name));
    std::vector<std::string> result = backend->db->find(*backend->path, query);
    if (result.size() > 0) return 1;
    return 0;
}

/**
* Queries the refdb backend for a given reference. A refdb
* implementation must provide this function.
*/
int cocaine_refdb_backend__lookup(git_reference **out,
                                  git_refdb_backend *_backend,
                                  const char *ref_name)
{
    assert(_backend);

    if (out) *out = nullptr;
    cocaine_refdb_backend * backend = (cocaine_refdb_backend *) _backend;
    query.push_back(std::string(COCAINE_GIT_REF_TAG));
    query.push_back(std::string(COCAINE_GIT_REFSYM_TAG));
    query.push_back(std::string(ref_name));
    std::vector<std::string> result = backend->db->find(*backend->path, query);
    if (result.size() > 0) {
        if (out) {
            std::string target;
            try {
                target = backend->db->read(*backend->path, result[0]);
            } catch (storage_error_t) {
                giterr_set(GITERR_OS, "IO error while reading %s/%s",
                           backend->path.c_str(), ref_name);
                return GIT_ERROR;
            }
            *out = git_reference__alloc_symbolic(ref_name, target.c_str());
        }
    } else {
        query[1] = std::string(COCAINE_GIT_REFOID_TAG);
        result = backend->db->find(*backend->path, query);
        if (result.size() > 0) {
            if (out) {
                std::string str;
                git_oid oid;
                try {
                    str = backend->db->read(*backend->path, result[0]);
                } catch (storage_error_t) {
                    giterr_set(GITERR_OS, "IO error while reading %s/%s",
                               backend->path.c_str(), ref_name);
                    return GIT_ERROR;
                }
                if (str.size() < GIT_OID_HEXSZ || git_oid_fromstr(&oid, str.c_str()) < 0) {
                    giterr_set(GITERR_REFERENCE, "Corrupted reference file %s/%s",
                               backend->path.c_str(), ref_name);
                    return GIT_ERROR;
                }
                *out = git_reference__alloc(ref_name, &oid, nullptr);
            }
        } else {
            return GIT_ENOTFOUND;
        }
    }
    return GIT_OK;
}

/**
* Allocate an iterator object for the backend.
*
* A refdb implementation must provide this function.
*/
int cocaine_refdb_backend__iterator(git_reference_iterator **iter,
                                    struct git_refdb_backend *backend,
                                    const char *glob)
{}

/*
* Writes the given reference to the refdb. A refdb implementation
* must provide this function.
*/
int cocaine_refdb_backend__write(git_refdb_backend *backend,
                                 const git_reference *ref,
                                 int force,
                                 const git_signature *who,
                                 const char *message,
                                 const git_oid *old,
                                 const char *old_target)
{}

int cocaine_refdb_backend__rename(git_reference **out,
                                  git_refdb_backend *backend,
                                  const char *old_name,
                                  const char *new_name,
                                  int force,
                                  const git_signature *who,
                                  const char *message)
{}

/**
* Deletes the given reference from the refdb. A refdb implementation
* must provide this function.
*/
int cocaine_refdb_backend__del(git_refdb_backend *backend,
                               const char *ref_name,
                               const git_oid *old_id,
                               const char *old_target)
{}

/**
* Suggests that the given refdb compress or optimize its references.
* This mechanism is implementation specific. (For on-disk reference
* databases, this may pack all loose references.) A refdb
* implementation may provide this function; if it is not provided,
* nothing will be done.
*/
int cocaine_refdb_backend__compress(git_refdb_backend *backend)
{}

/**
* Query whether a particular reference has a log (may be empty)
*/
int cocaine_refdb_backend__has_log(git_refdb_backend *backend,
                                   const char *refname)
{}

/**
* Make sure a particular reference will have a reflog which
* will be appended to on writes.
*/
int cocaine_refdb_backend__ensure_log(git_refdb_backend *backend,
                                      const char *refname)
{}

/**
* Frees any resources held by the refdb. A refdb implementation may
* provide this function; if it is not provided, nothing will be done.
*/
void cocaine_refdb_backend__free(git_refdb_backend *backend)
{}

/**
* Read the reflog for the given reference name.
*/
int cocaine_refdb_backend__reflog_read(git_reflog **out,
                                       git_refdb_backend *backend,
                                       const char *name)
{}

/**
* Write a reflog to disk.
*/
int cocaine_refdb_backend__reflog_write(git_refdb_backend *backend,
                                        git_reflog *reflog)
{}

/**
* Rename a reflog
*/
int cocaine_refdb_backend__reflog_rename(git_refdb_backend *_backend,
                                         const char *old_name,
                                         const char *new_name)
{}

/**
* Remove a reflog.
*/
int cocaine_refdb_backend__reflog_delete(git_refdb_backend *backend,
                                         const char *name)
{}

} } }
