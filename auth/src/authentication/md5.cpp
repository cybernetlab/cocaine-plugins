#include "cocaine/service/auth/authentication/md5.hpp"

using namespace cocaine::service::auth::authentication;

const char *md5::type_name = "md5";

bool
md5::operator()(const Json::Value & user, const std::string & data) const
{
    return data == user["password"].asString();
}
