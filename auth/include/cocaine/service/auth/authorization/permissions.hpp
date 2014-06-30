#pragma once

#include "cocaine/service/auth/storage.hpp"

#include <memory>
#include <string>
#include <json/value.h>

namespace cocaine { namespace service { namespace auth { namespace authorization {

class permissions_t {
public:
    permissions_t(std::shared_ptr<logging::log_t> log,
                  std::shared_ptr<auth::storage_t> storage);

    bool
    check(const Json::Value & user, const std::string & perm) const;
private:
    bool
    inArray(const Json::Value & arr, const std::string & value) const;

    std::shared_ptr<logging::log_t> m_log;
    std::shared_ptr<auth::storage_t> m_storage;
    Json::Value m_data;
};

} } } }
