#pragma once

#include <memory>
#include "cocaine/service/auth/storage/cocaine_storage.hpp"

namespace cocaine { namespace service { namespace auth {

typedef boost::mpl::list<
    storage::cocaine_storage
> storages;

template <typename U>
struct ConvertToNames {
    typedef typename storage::type_name<U>::value type;
};

struct storage_factory {
    const std::string & type_name;
    const Json::Value & args;
    storage_t *storage_ptr;

    storage_factory(const Json::Value & args) :
        args(args),
        type_name(args.get("type", Json::Value::null).asString()),
        storage_ptr(NULL)
    { };

    template<typename U> void operator()(U x) {
        if (storage::type_name<U>::value == type_name) storage_ptr = new U(args);
    };
};

std::shared_ptr<storage_t>
create_storage(const Json::Value & args) {
    typedef boost::mpl::transform<storages, ConvertToNames<boost::mpl::_> > names;
    storage_factory factory(args);
    boost::mpl::for_each<names>(factory);
    // boost::mpl::for_each<storages, ConvertToNames<boost::mpl::_> >(factory);
    return std::make_shared<storage_t>(factory.storage_ptr);
}

} } }
