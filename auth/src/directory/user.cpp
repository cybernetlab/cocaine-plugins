#include "cocaine/service/auth/directory/user.hpp"

using namespace cocaine::service::auth::directory;

const char * user_t::type_name = "user";
const char * user_t::type_path = "users";

user_t::user_t(const directory_t & directory, const Json::Value & value) :
    object_t(directory, value)
{
    std::cout << "creating user\n";
}

user_t::user_t(const user_t & other) : object_t(other)
{
    std::cout << "copying user\n";
}

user_t &
user_t::operator=(const user_t &other)
{
    if (this == &other) return *this;
    object_t::operator=(dynamic_cast<const object_t &>(other));
    return *this;
};
