#pragma once

#include <string>
#include <memory>

#include <cocaine/context.hpp>
#include <cocaine/api/storage.hpp>

#include "cocaine/service/auth/storage/common.hpp"

namespace cocaine { namespace service { namespace auth { namespace storage {

class cocaine_storage : public storage_t {
public:
    cocaine_storage(context_t & context,
                    const Json::Value & args);

    std::unique_ptr<Json::Value>
    load(const std::string & ns,
         const std::string & type,
         const std::string & name) const;
private:
    std::unique_ptr<api::storage_t> m_storage;
};

template<> const char * type_name<cocaine_storage>::value = "cocaine_storage";

} } } }
