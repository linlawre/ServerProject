#include <iostream>

#include "request/handler_dispatcher.h"
#include "request/request_handle.h"
#include "request/echo_handle.h"
#include "request/fof_handle.h"
#include "request/status_handle.h"
#include "request/static_file_handle.h"
#include "request/proxy_handle.h"
#include "request/health_handle.h"
#include "request/covid_handle.h"
#include "logger.h"
#include "config_parser.h"
#include "server.h"


RequestHandlerDispatcher::RequestHandlerDispatcher(NginxConfig &config) {
    uri2info = config.getMappings();
     logger = Logger::getLogger();
     logger->log(0, "In Dispatcher");
}

std::string RequestHandlerDispatcher:: longestPrefixMatch(std::string uri)
{

    std::string longest_match=" ";
    for (auto it = uri2info.begin(); it != uri2info.end(); ++it) {
        std::string curr_path = it->first;
        int curr_path_size= curr_path.length();
        std::string req_path = uri.substr(0, curr_path_size);
        if (!req_path.compare(curr_path)) {   // Prefix match
            if (curr_path_size > longest_match.length()) { // Longest
                longest_match = curr_path;
            }
        }
    }
    return longest_match;
}


Requesthandle* RequestHandlerDispatcher::getRequestHandler(const Request &request_)
const {
   std::string uri= request_.uri;
   std::string longest_match=" ";
    for (auto it = uri2info.begin(); it != uri2info.end(); ++it) {
        std::string curr_path = it->first;
        int curr_path_size= curr_path.length();
        std::string req_path = uri.substr(0, curr_path_size);
        if (!req_path.compare(curr_path)) {   // Prefix match
            if (curr_path_size > longest_match.length()) { // Longest
                longest_match = curr_path;
            }
        }
    }
   std::map<std::string, std::pair<std::string, std::vector<std::string>>>::const_iterator ret=uri2info.find(longest_match);

    // newly created config file
    NginxConfig post_parsed_config;

    // declared here b/c fixes undefined reference issue in handlers - need to fix eventually
    std::map<PathUri, Requesthandle*>::const_iterator it = server::uri2handler.find(longest_match);

    if ( ret != uri2info.end())
    {
        HandlerType handler_type = ret->second.first;
        if (handler_type == StaticHandler)
        {
            // put base directory in config file
            post_parsed_config.statements_.emplace_back(new NginxConfigStatement);
            post_parsed_config.statements_.back().get()->tokens_.push_back("base");

            std::string base_directory = ret->second.second[0];
            post_parsed_config.statements_.back().get()->tokens_.push_back(base_directory);

            logger->log(0, "Handle static file");

            return StaticFileHandle::Init(longest_match, post_parsed_config);
        } else if (handler_type == EchoHandler)
        {
            logger->log(0, "Handle ping");
            return EchoHandle::Init(longest_match, post_parsed_config);
        } else if (handler_type == StatusHandler)
        {
            logger->log(0, "Handle status");
            return StatusHandle::Init(longest_match, post_parsed_config);
        } else if (handler_type == ProxyHandler) {
            post_parsed_config.statements_.emplace_back(new NginxConfigStatement);
            post_parsed_config.statements_.back().get()->tokens_.push_back("host");
            std::string base_directory = ret->second.second[0];
            post_parsed_config.statements_.back().get()->tokens_.push_back(base_directory);

            post_parsed_config.statements_.emplace_back(new NginxConfigStatement);
            post_parsed_config.statements_.back().get()->tokens_.push_back("port_number");
            std::string base_directory_2 = ret->second.second[1];
            post_parsed_config.statements_.back().get()->tokens_.push_back(base_directory_2);


            logger->log(0, "Handle proxy");
            return ProxyHandle::Init(longest_match, post_parsed_config);
        } else if (handler_type == HealthHandler) {
            logger->log(0, "Handle health");
            return HealthHandle::Init(longest_match, post_parsed_config);
        } else if (handler_type == CovidHandler) {
            logger->log(0, "Handle covid");
            return CovidHandle::Init(longest_match, post_parsed_config);
        }
    }
    
    logger->log(3, "Invalid");
    return FofHandle::Init(longest_match, post_parsed_config);
}
