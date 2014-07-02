#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>

#include <json/value.h>
#include <cocaine/context.hpp>

namespace cocaine { namespace service { namespace auth {

class storage_t {
public:
    storage_t(context_t & context,
              const Json::Value & args);

    virtual
    Json::Value
    load(const std::string & path,
         const std::string & name) const = 0;

    virtual
    Json::Value
    load_safe(const std::string & path,
              const std::string & name) const;

    virtual
    void
    save(const std::string & path,
         const std::string & name,
         const Json::Value & data) = 0;

    virtual
    bool
    save_safe(const std::string & path,
              const std::string & name,
              const Json::Value & data);

    virtual
    void
    remove(const std::string & path,
           const std::string & name) = 0;

    virtual
    bool
    remove_safe(const std::string & path,
                const std::string & name);
protected:
    context_t & m_context;
    const std::string m_namespace;
};

typedef std::shared_ptr<storage_t> storage_ptr;

namespace storage {
    class error : public std::runtime_error {
    public:
        error(): std::runtime_error("storage error") {};
        error(const char *msg): std::runtime_error(msg) {};
    };

    class not_found_error: public error {
    public:
        not_found_error(): error("file not found") {};
        not_found_error(const char *msg): error(msg) {};
    };

    class io_error: public error {
    public:
        io_error(): error("storage io error") {};
        io_error(const char *msg): error(msg) {};
    };

    class format_error: public error {
    public:
        format_error(): error("file format error") {};
        format_error(const char *msg): error(msg) {};
    };
}

} } }
