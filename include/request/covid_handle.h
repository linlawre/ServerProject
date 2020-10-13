#ifndef COVID_HANDLE_H
#define COVID_HANDLE_H

#include <iostream>
#include "request_handle.h"
#include "logger.h"

class CovidHandle : public Requesthandle {
public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    Response handleRequest(const Request &request_);
    Logger* logger = Logger::getLogger();
private:
    Response reply_;
};

#endif
