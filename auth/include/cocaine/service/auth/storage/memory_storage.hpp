#pragma once

#include <map>

#include <cocaine/context.hpp>
#include <cocaine/api/storage.hpp>

#include "cocaine/service/auth/storage/abstract_storage.hpp"

namespace cocaine { namespace service { namespace auth { namespace storage {

class memory_storage_t : public storage_t {
public:
    static const char *type_name;

    memory_storage_t(context_t & context,
                     const Json::Value & args);

    virtual
    Json::Value
    load(const std::string & path,
         const std::string & name) const;

    virtual
    void
    save(const std::string & path,
         const std::string & name,
         const Json::Value & data);

    virtual
    void
    remove(const std::string & path,
           const std::string & name);

private:
    typedef std::map<std::string, Json::Value> data_map_t;

    data_map_t m_data;
};

} } } }
