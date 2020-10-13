//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <thread>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "request/request_handle.h"
#include "request/response.h"
#include "logger.h"
#include "request/handler_dispatcher.h"
using boost::asio::ip::tcp;
using namespace http;
using namespace server;

session::session(boost::asio::io_service& io_service, NginxConfig& config)
: socket_(io_service), io_service_(io_service)
{
  m_dispatcher=new RequestHandlerDispatcher(config);


  logger = Logger::getLogger();
  logger->log(0, "Server listening");
}

tcp::socket& session::socket() {
  return socket_;
}

int session::request_counter = 0;
std::map<std::string, std::vector<int>> session::request_history;
std::string session::soc_string;

void session::start() {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&session::handle_read2, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));

      logger->log(1, "Read handler");
}

void  session::clearData()// clear mem for next use avoid garbage
{
    memset(data_, 0, max_length);// clear mem for next use avoid garbage
}

void session::handle_read(const boost::system::error_code& error,size_t bytes_transferred){
  if (!error)
  {
    clearData();// clear mem for next use avoid garbage
    this->start();
  }
}
void session::handle_write(const boost::system::error_code& error)
{
  if (!error){
    boost::asio::async_write(socket_,
        reply_.to_buffers(),
        boost::bind(&session::handle_write2, this,
          boost::asio::placeholders::error));

    logger->log(1, "Write handler");
  }
}

//https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
void session::handle_read2(boost::system::error_code error, std::size_t bytes_transferred) {
    std::string sen = "";

    // Find index of start of param values
    // If no param values, index will just be set to last index in char array
    auto find_itr = std::find(data_, data_ + bytes_transferred, '{');
    int param_start = std::distance(data_, find_itr);

    // Param values found
    if (param_start != bytes_transferred)
    {
      for (int i = param_start; i < bytes_transferred; i++)
      {
        sen.push_back(data_[i]);
      }
    }

    if (!sen.empty())
      logger->log(0, "Additional context");

    if (!error) {
        using namespace std;
        request_parser::result_type result; // enum result_type { good, bad, indeterminate }
        std::tie(result, std::ignore) = request_parser_.parse(request_, data_, data_ + bytes_transferred);
//////////////////////////////////
        soc_string = socket_.remote_endpoint().address().to_string();
///////////////////////////////////
        logger->logTraceHTTPrequest(request_, soc_string);
  
        if (result == request_parser::good) {
            handleGoodRequest(error, sen);
        } else if (result == request_parser::bad) {
            handleBadRequest(error);
        } else {
            handle_read(error,bytes_transferred);
            memset(data_, 0, max_length);// clear mem for next use avoid garbage
        }
        clearData();// clear mem for next use avoid garbage
        return;
    }
    handle_read(error,bytes_transferred);

    clearData();// clear mem for next use avoid garbage
    return;
}

void session::handleGoodRequest(boost::system::error_code error, std::string rest) {
    logger->log(1, "Good Request");
    std::cerr<<request_.uri<<std::endl;
    logger->log(0,  "Check uri");

    for (auto x: request_.headers)
    {
      std::pair<std::string, std::string> a (x.name, x.value);
      request_.headers_.insert(a);
    }

    m_request_handler = m_dispatcher->getRequestHandler(request_);
    if(m_request_handler != nullptr)
    {
      logger->log(0, "IN HANDLER");
      reply_ = m_request_handler->handleRequest(request_);
      request_counter++;
      request_history[request_.uri].push_back(reply_.status);
    }else {
        // we should return 404 for all urls that are either not an echo or an static file path
        // logger->log(0, "Invalid path");
        reply_.status = Response::not_found;
        reply_.content = "<h1>404</h1><p>Dude, we don't have the file you requested! :(</p>";
        logger->log(0, "[ResponseMetrics] 404|" + request_.uri + "|" + session::soc_string + "|" + "InvalidPath");
        request_counter++;
        request_history[request_.uri].push_back(reply_.status);
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
    }

    handle_write(error);
    memset(data_, 0, max_length);// clear mem for next use avoid garbage
}

void session::handleBadRequest(boost::system::error_code error) {
    std::cerr<<"Bad Request"<<std::endl;
    logger->log(3, "Bad Request");
    reply_ = Response::stock_reply(Response::bad_request);    // Bad request
    logger->log(0, "[ResponseMetrics] 400|" + request_.uri + "|" + session::soc_string + "|" + "BadRequest");
    handle_write(error);
    memset(data_, 0, max_length);// clear mem for next use avoid garbage
}


//https://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio/example/http/server/connection.cpp
void session::handle_write2( boost::system::error_code error) {
    if (!error)
    {
        boost::system::error_code ignored_ec;
        logger->log(1, "Write closed");
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                            ignored_ec);
    }

    clearData();// clear mem for next use avoid garbage
    return ;
}
