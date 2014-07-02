#pragma once

#include <cocaine/context.hpp>
#include <cocaine/api/storage.hpp>

#include "cocaine/service/auth/storage/abstract_storage.hpp"

namespace cocaine { namespace service { namespace auth { namespace storage {

class cocaine_storage_t : public storage_t {
public:
    static const char *type_name;

    cocaine_storage_t(context_t & context,
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
    std::string m_storage_name;
};

} } } }
