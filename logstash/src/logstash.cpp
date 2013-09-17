/*
    Copyright (c) 2011-2013 Evgeny Safronov <division494@gmail.com>

    Copyright (c) 2011-2013 Other contributors as noted in the AUTHORS file.

    This file is part of Cocaine.

    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <chrono>
#include <iostream>

#include <json/json.h>

#include "cocaine/logging/formatters/logstash.hpp"

using namespace cocaine::logging::formatters;

class json_visitor : public boost::static_visitor<> {
    Json::Value& root;
public:
    json_visitor(Json::Value& root) :
        root(root)
    {}

    template<typename P>
    void operator ()(const std::string& name, const P& property) {
        root[name] = property;
    }
};

class logstash_t::impl {
public:
    Json::Value build_json(const log_property_map_t& properties) const {
        Json::Value root;
        json_visitor visitor(root);
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            boost::variant<std::string> name = it->first;
            const log_property_t& property = it->second;
            boost::apply_visitor(visitor, name, property);
        }

        return root;
    }

    template<typename T>
    T
    pop_property(log_property_map_t& map, const std::string& key, const T& default_value = T()) const {
        auto it = map.find(key);
        if (it != map.end()) {
            auto value = boost::get<T>(std::move(it->second));
            map.erase(it);
            return value;
        }

        return default_value;
    }
};

logstash_t::logstash_t(const cocaine::formatter_config_t& config):
    category_type(config),
    d(new impl)
{
}

std::string
logstash_t::format(const std::string& message, priorities level, log_property_map_t&& properties) const {
    properties.emplace("level", format_level(level));

    const std::string& hostname = d->pop_property<std::string>(properties, "source_host");
    Json::Value root;
    root["@message"]     = message;
    root["@source"]      = cocaine::format("udp://%s", hostname);
    root["@source_host"] = hostname;
    root["@source_path"] = d->pop_property<std::string>(properties, "source");
    root["@timestamp"]   = format_time(std::time(nullptr));
    root["@fields"]      = d->build_json(properties);

    Json::FastWriter writer;
    return writer.write(root);
}

std::string
logstash_t::format_message(const log_property_map_t& properties) const {
    return "";
}

std::string
logstash_t::format_time(const std::time_t& time) const {
    char mbstr[128];
    if (std::strftime(mbstr, 128, "%FT%T", std::localtime(&time))) {
        auto now = std::chrono::high_resolution_clock::now();
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        std::size_t fractional_seconds = now_ms.count() % 1000;
        return std::string(cocaine::format("%s.%03ldZ", mbstr, fractional_seconds));
    }

    return std::string("?");
}
