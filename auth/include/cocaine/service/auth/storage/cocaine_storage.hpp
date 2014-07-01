#pragma once

#include <memory>

#include <cocaine/context.hpp>
#include <cocaine/api/storage.hpp>

#include "cocaine/service/auth/storage/base.hpp"

namespace cocaine { namespace service { namespace auth { namespace storage {

class cocaine_storage : public storage_t {
public:
    static const char *type_name;

    cocaine_storage(context_t & context,
                    const Json::Value & args);

    Json::Value
    load(const std::string & path,
         const std::string & name) const;

    /*Json::Value
    load(const boost::filesystem::path & path,
         const std::string & name) const;*/

    bool
    save(const std::string & path,
         const std::string & name,
         const Json::Value & data) const;

    /*bool
    save(const boost::filesystem::path & path,
         const std::string & name,
         const Json::Value & data) const;*/

private:
    std::string m_storage_name;
};

} } } }
