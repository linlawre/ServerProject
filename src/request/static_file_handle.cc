#include "request/static_file_handle.h"
#include "session.h"
#include "server.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <cassert>
#include "logger.h"


using namespace std;

Requesthandle* StaticFileHandle::Init(std::string location_prefix, const NginxConfig& config) {
  Requesthandle* sf_handler_ptr = nullptr;
  std::map<PathUri, Requesthandle*>::const_iterator sf_entry = server::uri2handler.find(location_prefix);
  if ( sf_entry != server::uri2handler.end()) {
    sf_handler_ptr = sf_entry->second;
  }
  return sf_handler_ptr;
}

Response StaticFileHandle::handleRequest(const Request &request_)
{
    logger->log(0, "Static Request");
    // for the reply we want HTTP/1.1 200 ok crlf rquest
    // and text/plain
    // std::string path = ".." + request_.uri;

    // get filename
    std::size_t file_pos = request_.uri.find_last_of("/");
    std::string filename =  request_.uri.substr(file_pos, request_.uri.size());
    std::string path = ".." + request_.uri;

    // // get request uri minus the static file path
    reply_.headers_.clear();
    reply_.bypass.clear();

    if (!boost::filesystem::exists(boost::filesystem::path(path))){

      logger->log(4, "Invalid");

      std::cerr << "file not found!" << '\n';

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
      logger->log(0, "[ResponseMetrics] 404|" + request_.uri + "|" + session::soc_string + "|" + "StaticHandler");
      // reply_.map_to_bypass();


      return reply_;
    } else {
      std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
      std::cerr << "file found!" << '\n';
      std::string file_content;
      char c;
      while (f.get(c)) file_content += c;
      f.close();

      reply_.status = Response::ok;

      reply_.content = file_content;

      std::pair<std::string, std::string> a ("Content-Length", std::to_string(file_content.length()));
      reply_.headers_.insert(a);
      std::string mime = find_Mime_Type(path);
      std::pair<std::string, std::string> b ("Content-Type", mime);
      reply_.headers_.insert(b);

      for (auto x : reply_.headers_) {
        http::server::header header_val;
        header_val.name = x.first;
        header_val.value = x.second;
        reply_.bypass.push_back(header_val);
      }
      logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "StaticHandler");
      return reply_;
    }
}

std::string StaticFileHandle::find_Mime_Type(std::string filename)
{
  std::string ext = "";
  boost::filesystem::path p(filename);
  if(p.has_extension()){
    ext = p.extension().string();
  }

  if(ext.compare("") == 0 || ext.compare(".txt") == 0){
    return "text/plain";
  }else if(ext.compare(".html") == 0 || ext.compare(".htm") == 0){
    return "text/html";
  }else if(ext.compare(".jpeg") == 0 || ext.compare(".jpg") == 0){
    return "image/jpeg";
  }else if(ext.compare(".png") == 0){
    return "image/png";
  }else if(ext.compare(".gif") == 0){
    return "image/gif";
  }else if(ext.compare(".zip") == 0){
    return "application/zip";
  }else{
    return "application/octet-stream";
  }
}
