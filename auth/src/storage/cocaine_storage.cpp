#include "cocaine/service/auth/storage/cocaine_storage.hpp"

using namespace cocaine::service::auth::storage;

const char *cocaine_storage::type_name = "cocaine_storage";

cocaine_storage::cocaine_storage(context_t & context,
                                 const Json::Value & args) :
    storage_t(context, args),
    m_storage_name(args.get("name", "core").asString())
{}

/*Json::Value
cocaine_storage::load(const boost::filesystem::path & path,
                      const std::string & name) const
{
    return load(path.string(), name);
}*/

Json::Value
cocaine_storage::load(const std::string & path,
                      const std::string & name) const
{
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;

    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;

    auto storage = api::storage(m_context, m_storage_name);
    try {
        if (!reader.parse(storage->read(full_path.string(), name + ".json"), obj)) {
            return Json::Value::null;
        }
        return obj;
    } catch(const storage_error_t& e) {
        return Json::Value::null;
    }
}

/*bool
cocaine_storage::save(const boost::filesystem::path & path,
                      const std::string & name,
                      const Json::Value & data) const
{
    return save(path.string(), name, data);
}*/

bool
cocaine_storage::save(const std::string & path,
                      const std::string & name,
                      const Json::Value & data) const
{
    std::ostringstream stream;
    auto tags = std::vector<std::string>();
    boost::filesystem::path full_path(m_namespace);
    full_path /= path;
    auto storage = api::storage(m_context, m_storage_name);
    stream << data;
    try {
        storage->write(full_path.string(), name + ".json", stream.str(), tags);
        return true;
    } catch(const storage_error_t& e) {
        return false;
    }
}
