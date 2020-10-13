#include "request/health_handle.h"
#include "server.h"
#include <iostream>
#include <cassert>
using namespace std;

Requesthandle* HealthHandle::Init(std::string location_prefix, const NginxConfig& config) {
    Requesthandle* health_handler_ptr = nullptr;
    std::map<PathUri, Requesthandle*>::const_iterator health_entry = server::uri2handler.find(location_prefix);
    if (health_entry != server::uri2handler.end()) {
      health_handler_ptr = health_entry->second;
    }
    return health_handler_ptr;
}

Response HealthHandle::handleRequest(const Request &request_)
{
  logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "HealthHandler");
  std::cerr << "handle the server health!" << '\n';
  logger->log(0, "Health Request");
  reply_.status = Response::ok;
  reply_.content = "<h1>OK</h1>";

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
  return reply_;
}
