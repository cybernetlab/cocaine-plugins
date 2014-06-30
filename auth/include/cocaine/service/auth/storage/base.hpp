#pragma once

#include <string>
#include <json/value.h>

#include <cocaine/context.hpp>

namespace cocaine { namespace service { namespace auth {

class storage_t {
public:
    storage_t(context_t & context,
              const Json::Value & args);

    virtual
    Json::Value
    load(const std::string & type,
         const std::string & name) const = 0;

    virtual
    bool
    save(const std::string & type,
         const std::string & name,
         const Json::Value & data) const = 0;
protected:
    context_t & m_context;
    const std::string m_namespace;
};

} } }
