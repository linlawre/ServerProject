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
#include <thread>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "server.h"
#include "config_parser.h"
#include "logger.h"

using boost::asio::ip::tcp;

// define the number of threads used to handle requests
const int MAX_THREADS = 4;

int main(int argc, char* argv[])
{
  Logger *logger = Logger::getLogger();

  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: bin/<server> <config file>\n";
      logger->log(4, "wrong usage port");
      return 1;
    }

    // Added the parser and config for parsing the input file
    NginxConfigParser parser; // m_parser
    NginxConfig config; // org out config

    logger->log(0, "Parsing the config file...");
    if( !(parser.Parse(argv[1], &config)))
    {
      std::cerr<<"Error Parsing the Input File"<<std::endl;
      logger->log(4, "Pasing is failed");
      return -1;
    }
    logger->log(0, "Done parsing!");
    boost::asio::io_service io_service;

    int port = config.getPort();
    server s(io_service, port, config);

    logger->log(0, "Server has been initialized");
    logger->log(0, "Starting server on port: " + std::to_string(port));

    std::vector<std::thread*> request_threads;
    for (int i = 0; i < MAX_THREADS; i++) {
      std::thread *new_thread = new std::thread(boost::bind(&boost::asio::io_service::run, &io_service));
      request_threads.push_back(new_thread);
    }

    for (int j = 0; j < request_threads.size(); j++) {
      request_threads[j]->join();
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
