#pragma once

#include "cocaine/service/auth/authentication/base.hpp"

namespace cocaine { namespace service { namespace auth { namespace authentication {

class md5 : public authenticator_t {
public:
    md5(const Json::Value & args) : authenticator_t(args) {};

    static const char *type_name;
    bool operator()(const Json::Value & user, const std::string & data) const;
};

} } } }
