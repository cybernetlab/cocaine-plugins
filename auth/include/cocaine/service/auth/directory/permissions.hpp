#include "cocaine/service/auth/directory/base.hpp"

namespace cocaine { namespace service { namespace auth { namespace directory {

class permissions_t : public object_t {
public:
    static const char * type_name;
    static const char * type_path;
    permissions_t(const directory_t & directory, const Json::Value & value);
};

} } } }
