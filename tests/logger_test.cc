#include "gtest/gtest.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "request/request_handle.h"
#include "request/response.h"
#include "logger.h"
#include "request/request_parser.h"

class LoggerTest : public :: testing::Test
{
protected:
    boost::asio::io_service io_service;
    NginxConfig config;
    session* new_session = new session(io_service,config);
    Logger * logger = Logger::getLogger();
    char c;
 
    std::ifstream logfile;

    http::server::Request req;
    NginxConfigParser config_parser;
    http::server::request_parser::result_type result;
    http::server::request_parser request_parser;
};

TEST_F(LoggerTest, logServerInitialization)
{
	std::string expected_msg = "Trace: Server has been initialized\n";
	logger->log(0, "Server has been initialized");
  
  	logfile.open("../static/SYSLOG_0.log");
 
  	std::string body;
    while (logfile.get(c)) body += c;
    logfile.close();
    int start_idx = body.rfind("]") + 2;
    std::string real_msg = body.substr(start_idx);

    EXPECT_TRUE(real_msg == expected_msg);

}


TEST_F(LoggerTest, logServerdebug)
{
	std::string expected_msg = "Debug: Some thing wrong\n";
	logger->log(1, "Some thing wrong");

  	logfile.open("../static/SYSLOG_0.log");
 
  	std::string body;
    while (logfile.get(c)) body += c;
    logfile.close();
    int start_idx = body.rfind("]") + 2;
    std::string real_msg = body.substr(start_idx);

    EXPECT_TRUE(real_msg == expected_msg);

}

TEST_F(LoggerTest, logServerError)
{
	std::string expected_msg = "Error: Some thing wrong\n";
	logger->log(4, "Some thing wrong");

  	logfile.open("../static/SYSLOG_0.log");
 
  	std::string body;
    while (logfile.get(c)) body += c;
    logfile.close();
    int start_idx = body.rfind("]") + 2;
    std::string real_msg = body.substr(start_idx);

    EXPECT_TRUE(real_msg == expected_msg);

}

TEST_F(LoggerTest, logServerWarnning)
{
	std::string expected_msg = "Warning: Some thing wrong\n";
	logger->log(3, "Some thing wrong");
  	logfile.open("../static/SYSLOG_0.log");
 
  	std::string body;
    while (logfile.get(c)) body += c;
    logfile.close();
    int start_idx = body.rfind("]") + 2;
    std::string real_msg = body.substr(start_idx);

    EXPECT_TRUE(real_msg == expected_msg);

}

TEST_F(LoggerTest, logTraceHTTPrequest)
{
    std::fstream myfile;
    myfile.open ("example.txt");
    config_parser.Parse("example_config", &config);

    char request[1024] = "GET /static/doesnotexist.html HTTP/1.1\r\nHost: www.foo.com\r\nConnection: close\r\n\r\n";

    std::tie(result, std::ignore)  = request_parser.parse(req, request, request + strlen(request));
    
    std::string expected_msg = "Trace: GET /static/doesnotexist.html HTTP 1.1 Sender IP: 127.0.0.1\n";
myfile<<expected_msg;
myfile<<"/n";



    logger->logTraceHTTPrequest(req, "127.0.0.1");
 
    logfile.open("../static/SYSLOG_0.log");

    std::string body;
    while (logfile.get(c)) body += c;
    logfile.close();

    int start_idx = body.rfind("]") + 2;
    std::string real_msg = body.substr(start_idx);
myfile<<real_msg;
    EXPECT_TRUE(real_msg == expected_msg);
}