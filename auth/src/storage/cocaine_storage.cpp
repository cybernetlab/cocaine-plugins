#include "cocaine/service/auth/storage/cocaine_storage.hpp"

#include <boost/filesystem/operations.hpp>

using namespace cocaine::service::auth::storage;

const char *cocaine_storage::type_name = "cocaine_storage";

cocaine_storage::cocaine_storage(context_t & context,
                                 const Json::Value & args) :
    storage_t(context, args),
    m_storage_name(args.get("name", "core").asString())
{}

Json::Value
cocaine_storage::load(const std::string & type,
                      const std::string & name) const
{
    boost::filesystem::path path(m_namespace);
    path /= type;

    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;

    auto storage = api::storage(m_context, m_storage_name);
    try {
        if (!reader.parse(storage->read(path.string(), name + ".json"), obj)) {
            return Json::Value::null;
        }
        return obj;
    } catch(const storage_error_t& e) {
        return Json::Value::null;
    }
}

bool
cocaine_storage::save(const std::string & type,
                      const std::string & name,
                      const Json::Value & data) const
{
    std::ostringstream stream;
    auto tags = std::vector<std::string>();
    boost::filesystem::path path(m_namespace);
    path /= type;
    auto storage = api::storage(m_context, m_storage_name);
    stream << data;
    try {
        storage->write(path.string(), name + ".json", stream.str(), tags);
        return true;
    } catch(const storage_error_t& e) {
        return false;
    }
}
