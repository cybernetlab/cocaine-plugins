#include <assert.h>
#include <json/value.h>
#include <cocaine/api/storage.hpp>

#include "cocaine/service/git/cocaine_odb_backend.hpp"

using namespace cocaine;
using namespace cocaine::service::git;

namespace cocaine { namespace service { namespace git {

typedef std::shared_ptr<std::string>    string_ptr;

typedef struct {
    git_odb_backend parent;
    storage_ptr     db;
    string_ptr      path;
} cocaine_odb_backend;

#define OID_SIZE sizeof(git_oid::id)
#define COCAINE_GIT_TAG    "git"
#define COCAINE_HEADER_TAG "header"
#define COCAINE_DATA_TAG   "data"

int cocaine_odb_backend__read_header(size_t * len_p,
                                     git_otype * type_p,
                                     git_odb_backend * _backend,
                                     const git_oid * oid)
{
    assert(len_p && type_p && _backend && oid);

    cocaine_odb_backend * backend = (cocaine_odb_backend *) _backend;
    std::string key((const char *) oid->id, OID_SIZE);
    key += ".header";
    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;
    try {
        std::string result = backend->db->read(*backend->path, key);
        if (reader.parse(result, obj) && obj.isArray() && obj.size() >= 2) {
            *type_p = (git_otype) obj[0].asInt();
            *len_p = (size_t) obj[1].asInt();
            return GIT_OK;
        } else {
            return GIT_ERROR;
        }
    } catch (storage_error_t) {
        return GIT_ENOTFOUND;
    }
}

int cocaine_odb_backend__read(void ** data_p,
                              size_t * len_p,
                              git_otype * type_p,
                              git_odb_backend * _backend,
                              const git_oid * oid)
{
    assert(data_p && len_p && type_p && _backend && oid);

    cocaine_odb_backend * backend = (cocaine_odb_backend *) _backend;
    int error = GIT_ERROR;
    std::string key((const char *) oid->id, OID_SIZE);
    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;
    try {
        std::string header = backend->db->read(*backend->path, key + ".header");
        if (reader.parse(header, obj) && obj.isArray() && obj.size() >= 2) {
            *type_p = (git_otype) obj[0].asInt();
            *len_p = (size_t) obj[1].asInt();
            std::string data = backend->db->read(*backend->path, key);
            *data_p = malloc(*len_p);
            if (*data_p == NULL) {
                return GIT_ERROR;
            } else {
                memcpy(*data_p, obj[2].asCString(), *len_p);
                return GIT_OK;
            }
        } else {
            return GIT_ERROR;
        }
    } catch (storage_error_t) {
        return GIT_ENOTFOUND;
    }
}

/*int cocaine_odb_backend__read_prefix(git_oid *out_oid,
                                     void **data_p,
                                     size_t *len_p,
                                     git_otype *type_p,
                                     git_odb_backend *_backend,
                                     const git_oid *short_oid,
                                     size_t len)
{
    if (len >= GIT_OID_HEXSZ) {
        // Just match the full identifier
        int error = cocaine_odb_backend__read(data_p, len_p, type_p, _backend, short_oid);
        if (error == GIT_OK) git_oid_cpy(out_oid, short_oid);
        return error;
    } else if (len < GIT_OID_HEXSZ) {
        // TODO: implement it
        return GIT_ERROR;
    }
}*/

int cocaine_odb_backend__exists(git_odb_backend *_backend,
                                const git_oid *oid)
{
    assert(_backend && oid);

    cocaine_odb_backend * backend = (cocaine_odb_backend *) _backend;
    std::vector<std::string> query;
    query.push_back(std::string(COCAINE_GIT_TAG));
    query.push_back(std::string((const char *) oid->id, OID_SIZE));
    std::vector<std::string> result = backend->db->find(*backend->path, query);
    if (result.size() > 0) return 1;
    return 0;
}

int cocaine_odb_backend__write(git_odb_backend *_backend,
                               const git_oid *id,
                               const void *data,
                               size_t len,
                               git_otype type)
{
    assert(id && _backend && data);

    cocaine_odb_backend * backend = (cocaine_odb_backend *) _backend;
    std::ostringstream stream;

    std::string oid(std::string((const char *) id->id, OID_SIZE));
    std::vector<std::string> tags;
    tags.push_back(std::string(COCAINE_GIT_TAG));
    tags.push_back(std::string(COCAINE_HEADER_TAG));
    tags.push_back(oid);

    Json::Value header(Json::ValueType::arrayValue);
    header.append(Json::Value((Json::Int) type));
    header.append(Json::Value((Json::Int) len));
    stream << header;

    try {
        backend->db->write(*backend->path, oid + ".header", stream.str(), tags);
        tags[1] = std::string(COCAINE_DATA_TAG);
        std::string blob(std::string((const char *) data, len));
        backend->db->write(*backend->path, oid, blob, tags);
        return GIT_OK;
    } catch (storage_error_t) {
        return GIT_ERROR;
    }
}

void cocaine_odb_backend__free(git_odb_backend *_backend)
{
  assert(_backend);
  cocaine_odb_backend * backend = (cocaine_odb_backend *) _backend;
  backend->db.reset();
  backend->path.reset();
  free(backend);
}

int git_odb_backend_cocaine(git_odb_backend **backend_out,
                            storage_ptr & storage,
                            const std::string & path)
{
    cocaine_odb_backend * backend = (cocaine_odb_backend *) calloc(1, sizeof(cocaine_odb_backend));
    if (backend == NULL) return GIT_ERROR;

    /*backend->db = m_context.get<api::storage_t>(
            m_context.config.network.gateway.get().type,
            m_context,
            "service/locator",
            m_context.config.network.gateway.get().args
        );*/

    backend->db = storage;//std::make_shared<api::storage_t>(storage);
    backend->path = std::make_shared<std::string>(path);

    backend->parent.read = &cocaine_odb_backend__read;
    // backend->parent.read_prefix = &cocaine_odb_backend__read_prefix;
    backend->parent.read_header = &cocaine_odb_backend__read_header;
    backend->parent.write = &cocaine_odb_backend__write;
    backend->parent.exists = &cocaine_odb_backend__exists;
    backend->parent.free = &cocaine_odb_backend__free;
    backend->parent.version = GIT_ODB_BACKEND_VERSION;

    *backend_out = (git_odb_backend *) backend;
    return GIT_OK;
}

} } }
