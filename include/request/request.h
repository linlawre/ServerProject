//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include "request/header.h"
#include <map>

namespace http {
namespace server {

class Request
{
  public:

  enum method_
  {
    GET, PUT, POST, DELETE
  };

  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::string version;
  std::vector<header> headers;
  std::map<std::string, std::string> headers_;
};

} // namespace server
} // namespace http

#endif // REQUEST_H