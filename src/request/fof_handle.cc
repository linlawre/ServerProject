#include "request/fof_handle.h"
#include "server.h"
#include <iostream>
#include <cassert>
#include "session.h"
using namespace std;

Requesthandle* FofHandle::Init(std::string location_prefix, const NginxConfig& config) {
    Requesthandle* fof_handler_ptr = nullptr;
    std::map<PathUri, Requesthandle*>::const_iterator fof_entry = server::uri2handler.find(location_prefix);
    if (fof_entry != server::uri2handler.end()) {
      fof_handler_ptr = fof_entry->second;
    }
    return fof_handler_ptr;
}

Response FofHandle::handleRequest(const Request &request_)
{
  std::cerr << "file not found!" << '\n';
  logger->log(0, "fof Request");
  reply_.status = Response::not_found;


  reply_.content = "<h1>404</h1><p>Dude, we don't have the file you requested! :(</p>";


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
  logger->log(0, "[ResponseMetrics] 404|" + request_.uri + "|" + session::soc_string + "|" + "FofHandler");
  return reply_;
}
