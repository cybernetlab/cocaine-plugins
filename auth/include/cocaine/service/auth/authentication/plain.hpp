#pragma once

namespace cocaine { namespace service { namespace auth { namespace authentication {

class plain {
    bool operator()(const Json::Value & user, const std::string & password);
};

} } } }

