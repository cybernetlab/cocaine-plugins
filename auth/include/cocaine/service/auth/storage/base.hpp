#pragma once

#include <string>
#include <json/value.h>

namespace cocaine { namespace service { namespace auth {

class storage_t {
public:
    virtual
    Json::Value
    load(const std::string & ns,
         const std::string & type,
         const std::string & name) const = 0;

    virtual
    bool
    save(const std::string & ns,
         const std::string & type,
         const std::string & name,
         const Json::Value & data) const = 0;
};

} } }
