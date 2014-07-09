#include "cocaine/service/git.hpp"

using namespace cocaine;
using namespace cocaine::service;

extern "C" {
    auto
    validation() -> api::preconditions_t {
        return api::preconditions_t { COCAINE_MAKE_VERSION(0, 10, 5) };
    }

    void
    initialize(api::repository_t& repository) {
        repository.insert<git_t>("git");
    }
}
