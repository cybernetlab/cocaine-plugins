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

#pragma once

#include <cocaine/api/logging/formatter.hpp>

namespace cocaine { namespace logging { namespace formatters {

class logstash_t:
    public api::formatter_t
{
    public:
        logstash_t(const cocaine::formatter_config_t& config);

        std::string
        format(const std::string& message, priorities level, log_property_map_t&& properties) const;

    protected:
        std::string
        format_message(const logging::log_property_map_t& properties) const;

        std::string
        format_time(const std::time_t &time) const;

    private:
        class impl;
        const std::unique_ptr<impl> d;
};

} } } // namespace cocaine::logging::formatters
