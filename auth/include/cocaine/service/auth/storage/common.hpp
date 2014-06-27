#pragma once

#include <json/value.h>

namespace cocaine { namespace service { namespace auth {

class storage_t {
public:
    virtual
    std::unique_ptr<Json::Value>
    load(const std::string & ns,
         const std::string & type,
         const std::string & name) const = 0;
};

namespace storage {

template<typename T>
struct type_name { static const char * value; };

template<typename List>
storage_t &
create(const Json::Value & args) {
    //typedef auth::storages::type types;
    typedef typename boost::mpl::front<List>::type current_t;
    if (type_name<current_t>::value == args.get("type", Json::Value::null).asString()) {
        return new current_t();
    }
    return create<boost::mpl::pop_front<List>::type>(args);
}

template<>
storage_t &
create<boost::mpl::list<>::type>(const Json::Value & args) {
    throw cocaine::error_t("unknown storage type (%s)", args.get("type", Json::Value::null).asString());
}

} } } }
