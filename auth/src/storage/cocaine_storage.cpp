#include "cocaine/service/auth/storage/cocaine_storage.hpp"

#include <boost/filesystem/operations.hpp>

using namespace cocaine::service::auth::storage;

namespace fs = boost::filesystem;

cocaine_storage::cocaine_storage(context_t & context
                                 const Json::Value & args) :
    storage_t(),
    m_storage { make_unqiue<api::storage_t>(api::storage(context, args.get("name", "auth").asString())) }
{}

std::unique_ptr<Json::Value>
cocaine_storage::load(const std::string & ns,
                      const std::string & type,
                      const std::string & name) const
{
    const fs::path path { ns / type };

    try {
        return make_unqiue<Json::Value>(m_storage->get<Json::Value>(path, name))
    } catch(const storage_error_t& e) {
        return nullptr
        // throw cocaine::error_t("unable to initialize the routing groups - %s", e.what());
    }
}
