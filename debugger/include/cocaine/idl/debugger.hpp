#pragma once

#include <cocaine/rpc/protocol.hpp>

namespace cocaine { namespace io {

struct debugger_tag;

struct debugger {
    struct start {
        typedef debugger_tag tag;

        static const char* alias() {
            return "start";
        }

        typedef boost::mpl::list<
            /* service */ std::string
        > tuple_type;

        typedef stream_of<
            /* success */ bool,
            /* debug_sessions */ std::vector<std::string>
        >::tag drain_type;
    };
};

template<>
struct protocol<debugger_tag> {
    typedef boost::mpl::int_<
        1
    >::type version;

    typedef mpl::list<
        debugger::start
    > messages;

    typedef debugger type;
};

} } // namespace cocaine::io
