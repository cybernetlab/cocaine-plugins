#include "cocaine/service/auth/storage/base.hpp"

using namespace cocaine::service::auth;

storage_t::storage_t(context_t & context,
                     const Json::Value & args):
    m_context(context),
    m_namespace(args.get("namespace", "").asString())
{}
