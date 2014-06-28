#pragma once

#include <memory>
#include <json/value.h>

namespace cocaine { namespace service { namespace auth {

class storage_t {
public:
    virtual
    std::shared_ptr<Json::Value>
    load(const std::string & ns,
         const std::string & type,
         const std::string & name) const = 0;
};

namespace storage {

template<typename T>
struct type_name { static const char * value; };

}

} } }
