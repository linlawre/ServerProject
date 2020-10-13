// my guard
#ifndef HANDLER_DISPATCHER_H
#define HANDLER_DISPATCHER_H

#include <iostream>
#include "config_parser.h"
#include "request/request_handle.h"
#include <map>

typedef std::string PathUri;
typedef std::string HandlerType;
typedef std::vector<std::string> Root;
class Logger;

static const HandlerType StaticHandler = "StaticHandler";
static const HandlerType EchoHandler =   "EchoHandler";
static const HandlerType ErrorHandler =   "ErrorHandler";
static const HandlerType StatusHandler =   "StatusHandler";
static const HandlerType ProxyHandler =   "ProxyHandler";
static const HandlerType HealthHandler =   "HealthHandler";
static const HandlerType CovidHandler = "CovidHandler";


class RequestHandlerDispatcher {
public:
    RequestHandlerDispatcher(NginxConfig &config);
    Requesthandle* getRequestHandler(const Request &request_) const;
    std::string longestPrefixMatch(std::string uri);
private:
    Logger * logger;
    std::map<PathUri, std::pair<HandlerType, Root>> uri2info;
};

#endif  //HANDLER_DISPATCHER_H
