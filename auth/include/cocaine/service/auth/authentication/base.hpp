#pragma once

#include <memory>
#include <json/value.h>

namespace cocaine { namespace service { namespace auth {

class authenticator_t {
public:
    authenticator_t(const Json::Value & args) : m_args(args) {};

    virtual
    bool
    operator()(const Json::Value & user, const std::string & data) const = 0;
private:
    const Json::Value & m_args;
};

} } }
