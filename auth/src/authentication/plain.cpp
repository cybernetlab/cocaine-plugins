#include "cocaine/service/auth/authentication/plain.hpp"

#include "md5.h"

using namespace cocaine::service::auth::authentication;

const char *plain::type_name = "plain";

bool
plain::operator()(const Json::Value & user, const std::string & data) const
{
    return md5(data) == user["password"].asString();
}
