
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <vector>

// include hpp file
#include "server.h"
#include "session.h"
// #include "config_parser.h"
#include "request/request_handle.h"
#include "request/echo_handle.h"
#include "request/fof_handle.h"
#include "request/status_handle.h"
#include "request/static_file_handle.h"
#include "request/proxy_handle.h"
#include "request/health_handle.h"
#include "request/covid_handle.h"
#include "logger.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, NginxConfig& config)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(),port)),
      m_config(config),
      uri2info(config.getMappings())
  {
      create_handler_mappings(); // set values for uri2handler
      start_accept();
  }

std::map<PathUri, Requesthandle*> server::uri2handler;

void server::start_accept()
{
    session* new_session = new session(io_service_,m_config);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
      const boost::system::error_code& error)
{
    if (!error)
    {
      new_session->start();
    }

    start_accept();
}

void server::create_handler_mappings() {
    for (std::map<std::string, std::pair<std::string, std::vector<std::string>>>::const_iterator it = uri2info.begin(); it != uri2info.end(); ++it) {
      std::string handler_type = it->second.first;
      std::string path_val = it->first;

      if (handler_type == "EchoHandler") {
        // initialize an echo handler
        EchoHandle* echo_handler = new EchoHandle;
        std::pair<std::string, Requesthandle*> echo_pair(path_val, echo_handler);
        uri2handler.insert(echo_pair);
      } else if (handler_type == "StaticHandler") {
        // initialize a static handler
        StaticFileHandle* sf_handler = new StaticFileHandle;
        std::pair<std::string, Requesthandle*> sf_pair(path_val, sf_handler);
        uri2handler.insert(sf_pair);
      } else if (handler_type == "StatusHandler") {
        // initialize a status handler
        StatusHandle* status_handler = new StatusHandle;
        std::pair<std::string, Requesthandle*> status_pair(path_val, status_handler);
        uri2handler.insert(status_pair);
      } else if (handler_type == "ErrorHandler") {
        // initialize an error handler
        FofHandle* error_handler = new FofHandle;
        std::pair<std::string, Requesthandle*> error_pair(path_val, error_handler);
        uri2handler.insert(error_pair);
      } else if (handler_type == "ProxyHandler") {
        // initialize a proxy handler
        ProxyHandle* proxy_handler = new ProxyHandle;
        std::pair<std::string, Requesthandle*> proxy_pair(path_val, proxy_handler);
        uri2handler.insert(proxy_pair);
      } else if (handler_type == "HealthHandler") {
        // initialize a health handler
        HealthHandle* health_handler = new HealthHandle;
        std::pair<std::string, Requesthandle*> health_pair(path_val, health_handler);
        uri2handler.insert(health_pair);
      } else if (handler_type == "CovidHandler") {
        // initialize a covid handler
        CovidHandle* covid_handler = new CovidHandle;
        std::pair<std::string, Requesthandle*> covid_pair(path_val, covid_handler);
        uri2handler.insert(covid_pair);
      }
    }
}
