#include "request/echo_handle.h"
#include "session.h"
#include "server.h"
#include <iostream>
#include <cassert>

typedef std::string PathUri;

Requesthandle* EchoHandle::Init(std::string location_prefix, const NginxConfig& config) {
    
    Requesthandle* echo_handler_ptr = nullptr;
    std::map<PathUri, Requesthandle*>::const_iterator echo_entry = server::uri2handler.find(location_prefix);
    if (echo_entry != server::uri2handler.end()) {
        echo_handler_ptr = echo_entry->second;
    }
    return echo_handler_ptr;
}

Response EchoHandle::handleRequest(const Request &request_)
{
    // for the reply we want HTTP/1.1 200 ok crlf rquest
    // and text/plain
    logger->log(0, "[ResponseMetrics] 200|" + request_.uri + "|" + session::soc_string + "|" + "EchoHandler");
    reply_.status = Response::ok;
    reply_.content = ToString(request_);
    std::pair<std::string, std::string> a ("Content-Length", std::to_string((reply_.content).length()));
    reply_.headers_.insert(a);
    std::pair<std::string, std::string> b ("Content-Type", "text/plain");
    reply_.headers_.insert(b);
    
    for (auto x : reply_.headers_)
    {
    http::server::header a;
    a.name = x.first;
    a.value = x.second;
    reply_.bypass.push_back(a);
    }

    return reply_;
}

std::string EchoHandle::ToString(const Request &request_)
{
    // encode headers and also the org requestline
    std::string m_replay_packet = "";
    std::string http_method = request_.method + " ";
    std::string http_link = request_.uri + " ";

    m_replay_packet.append(http_method);
    m_replay_packet.append(http_link);
    m_replay_packet.append("HTTP/1.1\r\n");


    for (auto x : request_.headers_)
    {
        m_replay_packet.append(x.first);
        m_replay_packet.append(" ");
        m_replay_packet.append(x.second);
        m_replay_packet.append("\r\n");
    }

    m_replay_packet.append("\r\n");

    return m_replay_packet;
}
