#include "cocaine/service/auth/directory/base.hpp"

namespace cocaine { namespace service { namespace auth { namespace directory {

class user_t : public object_t {
public:
    static const char * type_name;
    static const char * type_path;
    user_t(const directory_t & directory, const Json::Value & value);
    user_t(const user_t & other);
    user_t & operator=(const user_t &other);
};

} } } }
