#include "request/covid_handle.h"
#include "server.h"
#include <iostream>
#include <cassert>
#include "session.h"
using namespace std;

Requesthandle* CovidHandle::Init(std::string location_prefix, const NginxConfig& config) {
    Requesthandle* covid_handler_ptr = nullptr;
    std::map<PathUri, Requesthandle*>::const_iterator covid_entry = server::uri2handler.find(location_prefix);
    if (covid_entry != server::uri2handler.end()) {
      covid_handler_ptr = covid_entry->second;
    }
    return covid_handler_ptr;
}

Response CovidHandle::handleRequest(const Request &request_)
{
    logger->log(0, "Covid Request");

    // set path to get covid_index.html
    std::string path = "../static/covid_index.html";

    if (!boost::filesystem::exists(boost::filesystem::path(path))){

        logger->log(4, "Invalid");

        std::cerr << "file not found!" << '\n';

        reply_.status = Response::not_found;

        reply_.content = "<h1>404</h1><p>Error loading the Covid-19 Tracker</p>";

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
        logger->log(0, "[ResponseMetrics] 404|" + request_.uri + "|" + session::soc_string + "|" + "CovidHandler");
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
        std::pair<std::string, std::string> b ("Content-Type", "text/html");
        reply_.headers_.insert(b);

        for (auto x : reply_.headers_) {
            http::server::header header_val;
            header_val.name = x.first;
            header_val.value = x.second;
            reply_.bypass.push_back(header_val);
        }

        logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "CovidHandler");
        return reply_;
    }
}
