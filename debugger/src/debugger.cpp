// #define BOOST_BIND_NO_PLACEHOLDERS

#include "cocaine/service/debugger.hpp"

#include <cocaine/logging.hpp>
#include <uuid/uuid.h>

/*#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <cocaine/traits/tuple.hpp>
#include <swarm/logger.hpp>
#include <swarm/urlfetcher/stream.hpp>*/

using namespace cocaine;
using namespace cocaine::io;
using namespace cocaine::service;

// using namespace ioremap;

debugger_t::debugger_t(context_t& context,
                       reactor_t& reactor,
                       const std::string& name,
                       const dynamic_t& args):
    service_t(context, reactor, name, args),
    dispatch<io::debugger_tag>(name),
    m_context(context),
    m_log(new logging::log_t(context, name))
//    log_(new logging::log_t(context, name)),
//    m_logger(new urlfetch_logger_interface(log_), swarm::SWARM_LOG_DEBUG),
//    m_loop(m_service),
//    m_manager(m_loop, m_logger),
//    m_work(new boost::asio::io_service::work(m_service))
{
    using namespace std::placeholders;
    on<io::debugger::start>(std::bind(&debugger_t::start, this, _1));
}

debugger_t::~debugger_t()
{}

namespace {

class start_handler_t {
    const std::unique_ptr<logging::log_t> m_log;
    context_t& m_context;
public:
    cocaine::deferred<response::start> deferred;

    start_handler_t(context_t& context, const std::string& name) :
        m_context(context),
        m_log(new logging::log_t(context, name))
    {}

    cocaine::deferred<response::start>
    run() {
        std::vector<std::string> sessions;
        std::array<uint64_t, 2> uuid;
        char uuid_str[37];
        uuid_generate(reinterpret_cast<unsigned char*>(uuid.data()));
        uuid_unparse_lower(reinterpret_cast<const unsigned char*>(uuid.data()), uuid_str);
        sessions.push_back(cocaine::format("debug_session_%s", uuid_str));
        deferred.write(std::make_tuple(true, sessions));
        return deferred;
    }
};

}

deferred<response::start>
debugger_t::start(const std::string& service)
{
    start_handler_t handler(m_context, service);
    return handler.run();
}
