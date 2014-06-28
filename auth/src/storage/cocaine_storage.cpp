#include "cocaine/service/auth/storage/cocaine_storage.hpp"

#include <boost/filesystem/operations.hpp>

    using namespace cocaine::service::auth::storage;

template<> const char * type_name<cocaine_storage>::value = "cocaine_storage";

cocaine_storage::cocaine_storage(context_t & context,
                                 const Json::Value & args) :
    storage_t(),
    m_context(context),
    m_storage_name(args.get("name", "auth").asString())
    // m_storage { make_unique(api::storage_t(context, args.get("name", "auth").asString())) }
{}

std::shared_ptr<Json::Value>
cocaine_storage::load(const std::string & ns,
                      const std::string & type,
                      const std::string & name) const
{
    boost::filesystem::path path(ns);
    path /= type;

    Json::Reader reader(Json::Features::strictMode());
    Json::Value obj;
    try {
        if (!reader.parse(api::storage(m_context, m_storage_name)->read(path.string(), name), obj)) {
            return std::make_shared<Json::Value>(Json::Value::null);
        }
        return std::make_shared<Json::Value>(obj);
    } catch(const storage_error_t& e) {
        return std::make_shared<Json::Value>(Json::Value::null);
        // throw cocaine::error_t("unable to initialize the routing groups - %s", e.what());
    }
}
