//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef REPLY_H
#define REPLY_H

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include "request/header.h"

namespace http {
namespace server {


/// A reply to be sent to a client.
class Response
{
  public:
  /// The status of the reply.
  enum status_type
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;

  std::map<std::string, std::string> headers_;

  std::string content;
  std::vector<header> bypass;

  std::vector<boost::asio::const_buffer> to_buffers();

  /// Get a stock reply.
  static Response stock_reply(status_type status);
  
};

} // namespace server
} // namespace http

#endif // REPLY_H
