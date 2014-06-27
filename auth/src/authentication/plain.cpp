#include "cocaine/service/auth/authentication/plain.hpp"

#include "md5.hpp"

using namespace cocaine::service::auth::authentication;

bool
plain::operator()(const Json::Value & user, const std::string & password)
{
    return md5(password) == user.get("password", "").asString();
}
