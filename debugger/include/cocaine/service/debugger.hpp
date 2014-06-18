#pragma once

#include <cocaine/api/service.hpp>
#include <cocaine/asio/reactor.hpp>
#include <cocaine/rpc/dispatch.hpp>
#include <cocaine/rpc/tags.hpp>

#include "cocaine/idl/debugger.hpp"

namespace cocaine { namespace service {

namespace response {
typedef result_of<io::debugger::start>::type start;
}


class debugger_t:
    public api::service_t,
    public dispatch<io::debugger_tag>
{
    public:
        debugger_t(context_t& context,
                   io::reactor_t& reactor,
                   const std::string& name,
                   const dynamic_t& args);
        ~debugger_t();

        virtual
        auto
        prototype() -> io::basic_dispatch_t& {
            return *this;
        }

    private:
        deferred<response::start>
        start(const std::string& service);

        std::shared_ptr<logging::log_t> m_log;
        context_t& m_context;
/*
        ioremap::swarm::logger m_logger;
        boost::asio::io_service m_service;
        ioremap::swarm::boost_event_loop m_loop;
        ioremap::swarm::url_fetcher m_manager;
        std::unique_ptr<boost::asio::io_service::work> m_work;
        boost::thread m_thread;*/
};

} } // namespace cocaine::service
