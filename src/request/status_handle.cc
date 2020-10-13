#include "request/status_handle.h"
#include "server.h"
#include <iostream>
#include <cassert>
using namespace std;

Requesthandle* StatusHandle::Init(std::string location_prefix, const NginxConfig& config) {
    Requesthandle* status_handler_ptr = nullptr;
    std::map<PathUri, Requesthandle*>::const_iterator status_entry = server::uri2handler.find(location_prefix);
    if (status_entry != server::uri2handler.end()) {
      status_handler_ptr = status_entry->second;
    }
    return status_handler_ptr;
}

Response StatusHandle::handleRequest(const Request &request_)
{
  std::cerr << "handle the server status!" << '\n';
  logger->log(0, "Status Request");

  std::string url_status = "";

  for (auto it = session::request_history.begin(); it != session::request_history.end(); ++it){
      for(int j = 0; j <  (it->second).size(); j++){
        url_status += "<tr><td>" + it->first +"</td><td>" + std::to_string((it->second)[j]) + "</td></tr>";
      }
  }

  reply_.status = Response::ok;

  reply_.content = "<html></head><h1>Server Status</h1></head>"
    "<body><h2>Total requests handled: "+ std::to_string(session::request_counter) +"</h2>"
    "<br><h2>Detail Request Status</h2>"
    "<table>"
    "<tr><th>URL</th><th>Status Code</th></tr>" + url_status + "</table>"
    "<br><h2>Request Handlers</h2>"
    "<table>"
    "<tr><th>URL Prefix</th><th>Handler</th></tr><tr><th>/echo</th><th>Echo Handler</th></tr>"
    "<tr><th>/static</th><th>Static File Handler</th></tr><tr><th>/status</th><th>Status Handler</th></tr>"
    "<tr><th>All Other Cases</th><th>Fof Handler</th></tr></table>"
    "</body>"
    "</html>";

    reply_.headers_.clear();
    reply_.bypass.clear();

    std::pair<std::string, std::string> a ("Content-Length", std::to_string((reply_.content).length()));
    reply_.headers_.insert(a);
    std::pair<std::string, std::string> b ("Content-Type", "text/html");
    reply_.headers_.insert(b);

    for (auto x : reply_.headers_)
    {
      http::server::header a;
      a.name = x.first;
      a.value = x.second;
      reply_.bypass.push_back(a);
    }
  logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "StatusHandler");
  return reply_;
}
