#ifndef REQUEST_HANDLE_H
#define REQUEST_HANDLE_H

#include <iostream>

#include "config_parser.h"
#include "request/header.h"
#include "request/response.h"
#include "request/request_parser.h"
#include "request/request.h"


using http::server::Request;
using http::server::request_parser;
using http::server::Response;


class Requesthandle {
public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    virtual Response handleRequest(const Request &request_)=0;

private:
    Response reply_;
};

#endif  // REQUEST_HANDLE_H
