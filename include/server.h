#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <string>
#include "config_parser.h"
#include "request/request_handle.h"
#include <map>

typedef std::string PathUri;
typedef std::string HandlerType;
typedef std::vector<std::string> Root;

using boost::asio::ip::tcp;
class session;   // using this 

class server {
public:
    server(boost::asio::io_service& io_service, short port, NginxConfig& config);
    static std::map<PathUri, Requesthandle*> uri2handler;


    void start_accept();
    void handle_accept(session* new_session,
    const boost::system::error_code& error);
    void create_handler_mappings();

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;

    std::map<PathUri, std::pair<HandlerType, Root>> uri2info;

    NginxConfig m_config;

    std::string status_line = "HTTP/1.1  200 OK\r\n";

    // unit testing class
    friend class ServerTestFixture;
};
#endif  // SERVER_H

