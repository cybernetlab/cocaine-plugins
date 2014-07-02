#pragma once

#include <memory>
#include <string>
#include <utility>
#include <map>
#include <stdexcept>

#include <json/value.h>
#include <cocaine/logging.hpp>

#include "cocaine/service/auth/storage.hpp"
#include "cocaine/service/auth/authentication.hpp"

#include <iostream>

namespace cocaine { namespace service { namespace auth {

class not_found_error : public std::runtime_error {
public:
    not_found_error(): std::runtime_error("auth object not found") {};
    not_found_error(const char *msg): std::runtime_error(msg) {};
};

class realm_t {
public:
    typedef std::pair<std::string, realm_t &> resolve_result;
    typedef std::pair<Json::Value, realm_t &> resolve_token_result;
    typedef std::tuple<bool, std::string> bool_result;
    typedef std::map<std::string, realm_t> children_map_t;

    realm_t(std::shared_ptr<logging::log_t> log,
                storage_ptr storage,
                storage_ptr cache);

    bool_result
    authenticate(const std::string & type,
                 const std::string & name,
                 const std::string & data);

    bool_result
    logout(const std::string & token);

    bool_result
    authorize(const std::string & token,
              const std::string & perm);

    resolve_result
    resolve(const std::string & name);

    resolve_token_result
    resolve_token(const std::string & token);

    Json::Value
    get(const std::string & name);

    Json::Value
    get(const std::string & name, const std::string & type);

    Json::Value
    get(const std::string & name, const char * type);

    void
    save(const std::string & name, Json::Value & data);

    void
    save(const char * name, Json::Value & data);

    void
    save(const std::string & name, const std::string & type, Json::Value & data);

    void
    save(const std::string & name, const char * type, Json::Value & data);

    void
    save(const char * name, const char * type, Json::Value & data);

protected:
    typedef std::vector<std::string>::const_iterator string_iter;

    realm_t(std::shared_ptr<logging::log_t> log,
                storage_ptr storage,
                storage_ptr cache,
                const std::string & path);

    realm_t(std::shared_ptr<logging::log_t> log,
                storage_ptr storage,
                storage_ptr cache,
                const std::string & path,
                realm_t * parent);

    resolve_result
    resolve(string_iter & begin, string_iter & end, const std::string & name, bool create_child);

    bool_result
    authenticate(const std::string & type, Json::Value & user, const std::string &  data);

    bool_result
    authorize(Json::Value & user, const std::string & perm);

    std::string
    path(const std::string & type);

    bool
    inArray(const Json::Value & arr, const std::string & value) const;

    void
    qualify_names(Json::Value & names);

    bool
    check_permission(const Json::Value & roles,
                     const char * action,
                     const std::string & perm);

private:
    realm_t * m_parent;
    storage_ptr m_storage;
    storage_ptr m_cache;
    std::shared_ptr<logging::log_t> m_log;
    const std::string m_path;
    Json::Value m_permissions;
    Json::Value m_index;
    Json::Value m_config;
    std::shared_ptr<Json::Value> m_sessions;
    children_map_t m_children; // children cache
    authentication::map_t m_authenticators;
};

} } }
