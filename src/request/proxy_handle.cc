#include "request/proxy_handle.h"
#include "session.h"
#include "server.h"
#include "logger.h"
#include <iostream>
#include <cassert>
#include <boost/algorithm/string/trim.hpp>

typedef std::string PathUri;
using boost::asio::ip::tcp;

// compare the `location_prefix` param to the entries in the
// `server::uri2handler` map and return a pointer to proxy handler
Requesthandle* ProxyHandle::Init(std::string location_prefix, const NginxConfig& config) {
  Requesthandle* proxy_handler_ptr = nullptr;
  std::map<PathUri, Requesthandle*>::const_iterator proxy_entry = server::uri2handler.find(location_prefix);
  if (proxy_entry != server::uri2handler.end()) {
    proxy_handler_ptr = proxy_entry->second;
    ProxyHandle* proxy_handler = static_cast<ProxyHandle*>(proxy_handler_ptr);
    // Extract the config values host and port num from 
    // config file.
    std::string host_value = config.getConfigValue("host");
    std::string port_number = config.getConfigValue("port_number");
    proxy_handler->set_host(host_value);
    proxy_handler->set_location(location_prefix);
    proxy_handler->set_port_number(port_number);
  }
  return proxy_handler_ptr;
}
// Check through each occurence of absolute path and prepend the host to the string.
std::string ProxyHandle::resolvePathIssues(std::string path, std::string to_search) {
  // Get the first occurrence href
  size_t pos = path.find(to_search);

  while( pos != std::string::npos) {
    // Keep moving the index until you hit quotation mark.
    while (pos < path.size() && path[pos] != '\"') {
      pos += 1;
    }
    if (pos == path.size())
        break;
    // Check if it is an absolute path,
    // If it is prepend the host onto the path
    if (pos + 1 < path.size() && path[pos + 1] == '/') {
      if (location_prefix_[location_prefix_.size() - 1] == '/') {
        path.insert(pos + 1, location_prefix_.substr(0, location_prefix_.size() - 1));
      } else {
        path.insert(pos + 1, location_prefix_);
      }
    }
    // Get the next occurrence of href from the current position
    pos = path.find(to_search, pos + to_search.size());
  }
  return path;
}
// 1) Work on redirect 302 errors
// 2) TODO(Michael) Add checks on whether it is http response and status code is correct
// 3) TODO(Michael) Figure out how to read port number from config file
Response ProxyHandle::handleRequest(const Request &request_) {
  try {
    Response reply_;
    boost::asio::io_service io_service;

    // Get a list of endpoints corresponding to the server name
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host_, port_number_, boost::asio::ip::resolver_query_base::numeric_service);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);

    // Setting up the request by gathering info from request object
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    std::string remain_uri = request_.uri.substr(location_prefix_.size());
    if (remain_uri == "") {
      remain_uri = "/";
    }
    std::string http_ver = " HTTP/1.0\r\n";
    request_stream << request_.method + " " << remain_uri << http_ver;
    request_stream << "Host: " << host_ << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request
    boost::asio::write(socket, request);
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK TODO(Michael): Check http version and status code with if statements
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);

    // Read the response headers, which are terminated by a blank line
    boost::asio::read_until(socket, response, "\r\n\r\n");
    std::string content_length_value = "";
    std::string content_type_value = "";

    std::string header;

    while (std::getline(response_stream, header) && header != "\r") {
      if (header.find("Transfer-Encoding: chunked") != std::string::npos) {
        continue;
      }
      size_t pos = header.find(": ", 0);
      if (pos != std::string::npos) {
        // std::cout << header << std::endl;
        std::string header_name = header.substr(0, pos);
        std::string header_value = header.substr(pos+2);
        // gets rid of the \r at the end of the header
        header_value.pop_back();
        std::pair<std::string, std::string> a (header_name, header_value);
        reply_.headers_.insert(a);
      }
    }

    // Check if 302 Redirect
    if(status_code == 302){
      std::string new_redir_location;
      std::string new_remain_uri;

      // Find new location to redirect to from response
      std::map<std::string, std::string>::iterator it;
      it = reply_.headers_.find("Location");
      if (it != reply_.headers_.end()){
        new_redir_location = it->second;
        int slash_counter = 0;

        // Parse for new remain URI
        for(int i = 0; i < new_redir_location.size(); i++){
          if (new_redir_location[i] == '/'){
            slash_counter ++;
          }
          if (slash_counter >= 3){
            new_remain_uri += new_redir_location[i];
          }
        }
        
        std::string new_request_uri = location_prefix_;
        
        // Append remain uri to location prefix for updated request uri
        for(int i = 0; i < new_remain_uri.size(); i++){
          new_request_uri += new_remain_uri[i];
        }

        // Call handle_request again
        Request new_request = request_;
        new_request.uri = new_request_uri;
        return handleRequest(new_request);

      } else{
        std::cerr << "No Location header found" << std::endl;
      }
    }

    std::ostringstream response_content;

    // Write whatever content we already have to output
    if (response.size() > 0) {
      response_content << &response;
    }

    // Read until EOF, writing data to output as we go
    boost::system::error_code error;
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
      response_content << &response;
    }
    // If we don't reach end of file, throw an error
    if (error != boost::asio::error::eof) {
      logger->log(0, "noninoinoinionionionionoininoninoi");
      throw boost::system::system_error(error);
    }

    // Add response status
    reply_.status = Response::ok;

    // Add response content
    std::string temp_absolute_path_content = resolvePathIssues(response_content.str(), "src=");
    std::string changed_absolute_path_content = resolvePathIssues(temp_absolute_path_content, "href=");
    reply_.content = changed_absolute_path_content;

    // Add the response headers
    for (auto x : reply_.headers_) {
      http::server::header header_val;
      header_val.name = x.first;
      header_val.value = x.second;
      reply_.bypass.push_back(header_val);
    }
    logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "ProxyHandler");
    return reply_;
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }
}

void ProxyHandle::set_host(std::string host) {
  if (host != "localhost") {
    if (host.size() >= 7 && host.substr(0, 7) == "http://") {
      host = host.substr(7);
    } else if (host.size() >= 8 && host.substr(0, 8 ) == "https://") {
      host = host.substr(8);
    }
    if (host.size() >= 4 && host.substr(0, 4) != "www.") {
      host = "www." + host;
    }
  }
  host_ = host;
}

void ProxyHandle::set_location(std::string location_prefix) {
  location_prefix_ = location_prefix;
}

void ProxyHandle::set_port_number(std::string port_number) {
  port_number_ = port_number;
}
