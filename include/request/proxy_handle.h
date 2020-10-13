#ifndef PROXY_HANDLE_H
#define PROXY_HANDLE_H

#include <iostream>

#include "request_handle.h"
#include "logger.h"

class ProxyHandle : public Requesthandle {
  public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    Response handleRequest(const Request &request_);
    void set_host(std::string host);
    void set_location(std::string location);
    void set_port_number(std::string port_number);
    std::string resolvePathIssues(std::string path, std::string to_search);
    Logger* logger = Logger::getLogger();
  private:
    std::string ToString(const Request &request_);
    std::string port_number_;
    std::string host_;
    std::string location_prefix_;
};

#endif