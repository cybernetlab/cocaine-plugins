#include "cocaine/service/auth/directory/permissions.hpp"

using namespace cocaine::service::auth::directory;

const char * permissions_t::type_name = "permissions";
const char * permissions_t::type_path = "";

permissions_t::permissions_t(const directory_t & directory, const Json::Value & value) :
    object_t(directory, value)
{}
