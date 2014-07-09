#pragma once

#include <string.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/sys/odb_backend.h>
#include <cocaine/context.hpp>

namespace cocaine { namespace service { namespace git {

int git_odb_backend_cocaine(git_odb_backend **backend_out,
                            std::shared_ptr<api::storage_t> & storage,
                            const std::string & path);

} } }
