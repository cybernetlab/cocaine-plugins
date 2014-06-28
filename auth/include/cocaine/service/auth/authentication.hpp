#pragma once

#include "cocaine/service/auth/authentication/plain.hpp"
#include "cocaine/service/auth/authentication/md5.hpp"

#include <string>
#include <map>
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/placeholders.hpp>

#include <json/value.h>
#include <cocaine/exceptions.hpp>
#include <cocaine/context.hpp>

using namespace boost;

namespace cocaine { namespace service { namespace auth { namespace authentication {

// list of available authentication methods
typedef mpl::list<
    plain,
    md5
> list;

typedef std::map<std::string, std::shared_ptr<authenticator_t> > map_t;

/**
 * @brief Authentication methods factory
 * @details This factory creates a map of available authentication methods with params specified
 *          in config
 */
class factory {
    // holds result authentication methods map
    map_t m_result;
    // holds self reference for operator() as mpl::for_each copy (not move) instance of factory
    factory * m_factory;
    const Json::Value & m_args;

public:
    factory(const Json::Value & args) :
        m_args(args)
    {};

    // needed to wrap authentication methods to prevent their creation while lookup with for_each
    template <typename T> struct wrap {};

    /**
     * @brief lookups for authentication methods in config and generates map of them
     */
    template <typename T>
    void operator()(wrap<T>) {
        // avoid invalid usage
        if (m_factory == nullptr || m_factory->m_args.isNull()) return;
        const Json::Value & args = m_factory->m_args;
        Json::Value params = Json::Value::null;
        bool founded = false;
        // inspect config for authentication method list
        switch (args.type()) {
        case Json::ValueType::stringValue:
            // single auth method specified as string
            if (args.asString() != T::type_name) return;
            break;
        case Json::ValueType::arrayValue:
            // an array of auth methods specified
            for (auto it: args) {
                switch (it.type()) {
                case Json::ValueType::stringValue:
                    break;
                case Json::ValueType::objectValue:
                    params = it;
                    it = params["type"];
                    if (it.isNull() || !it.isString()) {
                        throw cocaine::error_t("authentication method object should have `type` member");
                    }
                    break;
                default:
                    throw cocaine::error_t("wrong authentication methods array");
                }
                if (it.asString() != T::type_name) continue;
                founded = true;
                break;
            }
            if (!founded) return;
            break;
        case Json::ValueType::objectValue:
            // auth methods specified as hash of { method_name => method_params }
            for (auto it = args.begin(); it != args.end(); it++) {
                if (it.key().asString() != T::type_name) continue;
                founded = true;
                params = *it;
                break;
            }
            if (!founded) return;
            break;
        default:
            throw cocaine::error_t("wrong authentication methods type");
        }

        m_factory->m_result[T::type_name] = std::make_shared<T>(params);
    };

    map_t
    create() {
        m_factory = this;
        mpl::for_each<list, wrap<mpl::placeholders::_1> >(*this);
        return m_result;
    };
};

} } } }
