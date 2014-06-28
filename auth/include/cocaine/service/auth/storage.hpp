#pragma once

#include "cocaine/service/auth/storage/cocaine_storage.hpp"

#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/placeholders.hpp>

#include <json/value.h>
#include <cocaine/exceptions.hpp>
#include <cocaine/context.hpp>
#include <cocaine/logging.hpp>

using namespace boost;

namespace cocaine { namespace service { namespace auth { namespace storage {

typedef mpl::list<
    cocaine_storage
> storage_list;

struct factory {
    storage_t * result;
    std::string search_for;
    const Json::Value & m_args;
    context_t & m_context;
    factory * m_factory;

    std::shared_ptr<logging::log_t> m_log;

    factory(context_t & context, const Json::Value & args) :
        result(NULL),
        m_context(context),
        m_args(args),
        search_for(args["type"].asString()),
        m_log(new logging::log_t(context, "STORAGE_FACTORY"))
    {
        // COCAINE_LOG_DEBUG(m_log, "STANDARD CONSTRUCTOR CALLED");
    };

    template <typename T> struct wrap {};

    template <typename T>
    void operator()(wrap<T>) {
        if (search_for == type_name<T>::value) {
            m_factory->result = new T(m_context, m_args);
        }
    };

    storage_t *
    create() {
        m_factory = this;
        mpl::for_each<storage_list, wrap<mpl::placeholders::_1> >(*this);
        if (result == NULL) {
            throw cocaine::error_t("wrong storage type (%s)", search_for);
        }
        return result;
    };
};

} } } }
