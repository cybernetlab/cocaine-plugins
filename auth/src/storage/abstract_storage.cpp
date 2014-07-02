#include "cocaine/service/auth/storage/abstract_storage.hpp"

using namespace cocaine::service::auth;

storage_t::storage_t(context_t & context,
                     const Json::Value & args):
    m_context(context),
    m_namespace(args.get("namespace", "").asString())
{}

Json::Value
storage_t::load_safe(const std::string & path,
                     const std::string & name) const
{
    try {
        return load(path, name);
    } catch (storage::error) {
        return Json::Value::null;
    }
}

bool
storage_t::save_safe(const std::string & path,
                     const std::string & name,
                     const Json::Value & data)
{
    try {
        save(path, name, data);
        return true;
    } catch (storage::error) {
        return false;
    }
}

bool
storage_t::remove_safe(const std::string & path,
                       const std::string & name)
{
    try {
        remove(path, name);
        return true;
    } catch (storage::error) {
        return false;
    }
}
