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

    std::shared_ptr<Json::Value>
    load(const std::string & ns,
         const std::string & type,
         const std::string & name) const;
private:
    context_t & m_context;
    const std::string & m_storage_name;
    //std::unique_ptr<api::storage_t> m_storage;
};

} } } }
