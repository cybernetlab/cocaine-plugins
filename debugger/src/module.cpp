#include "cocaine/service/debugger.hpp"

using namespace cocaine;
using namespace cocaine::service;

extern "C" {
    auto
    validation() -> api::preconditions_t {
        return api::preconditions_t { COCAINE_MAKE_VERSION(0, 12, 0) };
    }

    void
    initialize(api::repository_t& repository) {
        repository.insert<debugger_t>("debugger");
    }
}
