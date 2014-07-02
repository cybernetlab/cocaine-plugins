#include "cocaine/service/auth/storage/memory_storage.hpp"

using namespace cocaine::service::auth::storage;

const char *memory_storage_t::type_name = "memory";

memory_storage_t::memory_storage_t(context_t & context,
                                   const Json::Value & args) :
    storage_t(context, args)
{}

Json::Value
memory_storage_t::load(const std::string & path,
                       const std::string & name) const
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    full_path /= name + ".json";
    auto it = m_data.find(full_path.string());
    if (it == m_data.end()) throw storage::not_found_error();
    return it->second;
}

void
memory_storage_t::save(const std::string & path,
                       const std::string & name,
                       const Json::Value & data)
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    full_path /= name + ".json";
    m_data[full_path.string()] = data;
}

void
memory_storage_t::remove(const std::string & path,
                         const std::string & name)
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    full_path /= name + ".json";
    auto it = m_data.find(full_path.string());
    if (it == m_data.end()) throw storage::not_found_error();
    m_data.erase(it);
}
