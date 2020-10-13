#ifndef ECHO_HANDLE_H
#define ECHO_HANDLE_H

#include <iostream>

#include "request_handle.h"
#include "logger.h"

class EchoHandle : public Requesthandle {
public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    Response handleRequest(const Request &request_);
    Logger* logger = Logger::getLogger();
private:
    std::string ToString(const Request &request_);
    Response reply_;
};

#endif
