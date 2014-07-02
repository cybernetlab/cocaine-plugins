#pragma once

#include "cocaine/service/auth/authentication/authenticator.hpp"

namespace cocaine { namespace service { namespace auth { namespace authentication {

class plain : public authenticator_t {
public:
    plain(const Json::Value & args) : authenticator_t(args) {};

    static const char *type_name;
    bool operator()(const Json::Value & user, const std::string & data) const;
};

} } } }
