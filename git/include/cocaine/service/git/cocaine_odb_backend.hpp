#pragma once

#include <string.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/sys/odb_backend.h>
#include <cocaine/context.hpp>

namespace cocaine { namespace service { namespace git {

typedef std::shared_ptr<api::storage_t> storage_ptr;

int git_odb_backend_cocaine(git_odb_backend **backend_out,
                            storage_ptr & storage,
                            const std::string & path);

} } }
