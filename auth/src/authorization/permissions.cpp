#include "cocaine/service/auth/authorization/permissions.hpp"

using namespace cocaine::service::auth::authorization;

permissions_t::permissions_t(std::shared_ptr<logging::log_t> log,
                             std::shared_ptr<auth::storage_t> storage) :
    m_log(log),
    m_storage(storage),
    m_data(m_storage->load("", "permissions"))
{};

bool
permissions_t::check(const Json::Value & user,
                     const std::string & perm) const
{
    // check for denied premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: m_data) {
            if (!inArray(permission["deny"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            return false;
        }
    }
    // check for allowed premissions
    for (auto & role: user["roles"]) {
        for (auto & permission: m_data) {
            if (!inArray(permission["allow"], role.asString())) continue;
            if (!inArray(permission["to"], perm)) continue;
            return true;
        }
    }
}

bool
permissions_t::inArray(const Json::Value & arr, const std::string & value) const
{
    if (!arr.isArray() || arr.empty()) return false;
    for (auto & item: arr) {
        if (!item.isString()) continue;
        if (item.asString() == value) return true;
    }
    return false;
}
