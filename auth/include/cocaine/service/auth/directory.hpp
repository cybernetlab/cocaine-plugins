#pragma once

#include <memory>
#include <string>
#include <utility>

#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/placeholders.hpp>

#include <json/value.h>
#include <cocaine/logging.hpp>

#include "cocaine/service/auth/storage.hpp"
#include "cocaine/service/auth/directory/base.hpp"
#include "cocaine/service/auth/directory/permissions.hpp"
#include "cocaine/service/auth/directory/user.hpp"

#include <iostream>

namespace cocaine { namespace service { namespace auth {

namespace directory {
    typedef boost::mpl::list<
        permissions_t,
        user_t
    > types;
}

class directory_t;
typedef std::shared_ptr<directory_t> directory_ptr;
typedef std::shared_ptr<directory::object_t> object_ptr;
typedef std::pair<std::string, directory_t &> resolve_result;
typedef std::vector<std::string>::const_iterator string_iter;

class directory_t {
public:
    directory_t(std::shared_ptr<logging::log_t> log,
                std::shared_ptr<storage_t> storage,
                const std::string & path);

    template <typename T>
    std::shared_ptr<T>
    get(const std::string & type, const std::string & name)
    {
        std::cout << "searching for " << name << " in: '" << m_path << "'\n";
        resolve_result result = resolve(name);
        directory::object_t * ptr = result.second.m_factories[type]->create(result.first).get();
        std::cout << "find " << result.first << " in: '" << result.second.m_path << "'\n";
        return std::shared_ptr<T>(reinterpret_cast<T*>(ptr));
        //return std::shared_ptr<T>(dynamic_cast<T*>(result.second->m_factories[type]->create(result.first).get()));
    }

    resolve_result
    resolve(const std::string & name);
protected:
    resolve_result
    resolve(string_iter begin, string_iter end, const std::string & name);
    directory_t & self() { return *this; };
private:
    template <typename T> struct wrap {};

    class abstract_factory {
    protected:
        directory_ptr m_directory;
    public:
        abstract_factory(directory_ptr directory) : m_directory(directory) {
            std::cout << "in abstract factory path is: '" << m_directory->m_path << "'\n";
        };
        virtual object_ptr create(const Json::Value & value) const = 0;
        virtual object_ptr create(const std::string & name) const = 0;
        virtual object_ptr create(const char * name) const = 0;
    };

    template <typename T>
    struct factory : public abstract_factory {
    public:
        factory(directory_ptr directory) : abstract_factory(directory) {
            std::cout << "in factory path is: '" << m_directory->m_path << "'\n";
        };
        virtual object_ptr create(const Json::Value & value) const {
            return std::make_shared<T>(*m_directory, value);
        }

        virtual object_ptr create(const std::string & name) const {
            std::cout << "creating object in dir '" << m_directory->m_path << "'\n";
            boost::filesystem::path path(m_directory->m_path);
            path /= T::type_path;
            std::cout << "load path: '" << path.string() << "'\n";
            return std::make_shared<T>(*m_directory, m_directory->m_storage->load(path.string(), name));
        }

        virtual object_ptr create(const char * name) const {
            return create(std::string(name));
        }
    };

    typedef std::shared_ptr<abstract_factory> factory_ptr;

    struct type_iterator {
        directory_ptr m_directory;

        type_iterator(directory_t * directory) : m_directory(directory_ptr(directory)) {};
        template <typename T>
        void operator()(wrap<T>) {
            std::cout << "in iterator path is: '" << m_directory->m_path << "'\n";
            m_directory->m_factories[T::type_name] = std::make_shared<factory<T> >(m_directory);// factory_ptr(new factory<T>(m_directory));
        };
    };

    std::shared_ptr<storage_t> m_storage;
    std::shared_ptr<logging::log_t> m_log;
    object_ptr m_permissions;
    const std::string m_path;
    std::map<std::string, factory_ptr> m_factories;
    std::map<std::string, directory_ptr> m_children; // children cache
};

} } }
