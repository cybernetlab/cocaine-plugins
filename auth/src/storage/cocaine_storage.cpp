#include "cocaine/service/auth/storage/cocaine_storage.hpp"

using namespace cocaine::service::auth::storage;

const char *cocaine_storage_t::type_name = "cocaine_storage";

cocaine_storage_t::cocaine_storage_t(context_t & context,
                                     const Json::Value & args) :
    storage_t(context, args),
    m_storage_name(args.get("name", "core").asString())
{}

Json::Value
cocaine_storage_t::load(const std::string & path,
                        const std::string & name) const
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;

    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;

    auto storage = api::storage(m_context, m_storage_name);
    try {
        if (!reader.parse(storage->read(full_path.string(), name + ".json"), obj)) {
            throw format_error();
        }
        return obj;
    } catch (const storage_error_t& e) {
        throw storage::not_found_error();
    }
}

void
cocaine_storage_t::save(const std::string & path,
                        const std::string & name,
                        const Json::Value & data)
{
    std::ostringstream stream;
    auto tags = std::vector<std::string>();
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    auto storage = api::storage(m_context, m_storage_name);
    stream << data;
    try {
        storage->write(full_path.string(), name + ".json", stream.str(), tags);
    } catch (const storage_error_t& e) {
        throw io_error();
    }
}

void
cocaine_storage_t::remove(const std::string & path,
                          const std::string & name)
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    auto storage = api::storage(m_context, m_storage_name);
    try {
        storage->remove(full_path.string(), name + ".json");
    } catch (const storage_error_t& e) {
        throw not_found_error();
    }
}
