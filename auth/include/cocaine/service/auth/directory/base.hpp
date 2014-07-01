#pragma once

#include <string>

#include <json/value.h>

#include <iostream>

namespace cocaine { namespace service { namespace auth {

class directory_t;

namespace directory {

class object_t : public Json::Value {
public:
    object_t(const directory_t & directory, const Json::Value & value);
    // const Json::Value & data();
    object_t(const object_t &other);
    object_t & operator=(const object_t &other);
    // ~object_t() {};
private:
    // const Json::Value m_data;
    const directory_t * m_directory;
};

} } } }
