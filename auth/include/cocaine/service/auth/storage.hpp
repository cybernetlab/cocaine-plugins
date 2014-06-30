#pragma once

#include "cocaine/service/auth/storage/cocaine_storage.hpp"

#include <memory>
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/placeholders.hpp>

#include <json/value.h>
#include <cocaine/exceptions.hpp>
#include <cocaine/context.hpp>

using namespace boost;

namespace cocaine { namespace service { namespace auth { namespace storage {

// list of available storage types
typedef mpl::list<
    cocaine_storage
> list;

/**
 * @brief Storage factory
 * @details This factory creates a single storage with type and params specified in config
 */
class factory {
    // holds result storage
    std::shared_ptr<storage_t> m_result;
    // holds self reference for operator() as mpl::for_each copy (not move) instance of factory
    factory * m_factory;
    std::string m_name;
    const Json::Value & m_args;
    context_t & m_context;

    // needed to wrap storage type to prevent their creation while lookup with for_each
    template <typename T> struct wrap {};

public:
    // lookups for storage type with `m_name` type name and creates instance when it finded
    template <typename T>
    void operator()(wrap<T>) {
        // avoid invalid usage and multiple storage creation
        if (m_factory == nullptr || m_factory->m_result != nullptr) return;
        if (m_name == T::type_name) {
            m_factory->m_result = std::make_shared<T>(m_context, m_args);
        }
    };

    std::shared_ptr<storage_t>
    create(context_t & context, const Json::Value & args) {
        m_factory = this;
        m_result = nullptr;
        m_context = context;
        m_args = args;
        m_name = args["type"].asString();
        mpl::for_each<list, wrap<mpl::placeholders::_1> >(*this);
        if (m_result == nullptr) throw cocaine::error_t("wrong storage type (%s)", m_name);
        return m_result;
    };
};

} } } }
