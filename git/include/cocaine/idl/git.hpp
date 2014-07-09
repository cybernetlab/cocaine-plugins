#pragma once

#include <cocaine/rpc/tags.hpp>
#include <cocaine/traits/tuple.hpp>

namespace cocaine { namespace io {

struct git_tag;

namespace git {
    struct create {
        typedef git_tag tag;

        typedef boost::mpl::list<
            /* path */ std::string
        > tuple_type;

        typedef boost::mpl::list<
            /* result */ bool
        > result_type;
    };
} // namespace git

template<>
struct protocol<git_tag> {
    typedef boost::mpl::list<
        git::create
    > type;

    typedef boost::mpl::int_<
        1
    >::type version;
};

} // namespace io
