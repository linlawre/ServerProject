#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <string>
// #include "request/request_handle.h"
#include "request/echo_handle.h"
#include "request/static_file_handle.h"
#include "request/request.h"
#include "request/request_parser.h"
#include "request/response.h"
#include <map>
// #include "request/handler_dispatcher.h"

typedef std::string PathUri;
typedef std::string HandlerType;
typedef std::vector<std::string> Root;

using boost::asio::ip::tcp;
class Logger;
class NginxConfig;
class Requesthandle;
class RequestHandlerDispatcher;
class session
{
public:
  session(boost::asio::io_service& io_service, NginxConfig& config);
  tcp::socket& socket();
  void start();
  Logger * logger;
  tcp::socket socket_;
  static std::string soc_string;
  static int request_counter;
  static std::map<std::string, std::vector<int>> request_history;


  void  handle_read(const boost::system::error_code& error,size_t bytes_transferred);
  void  handle_write(const boost::system::error_code& error);
  void  handle_read2(boost::system::error_code error, std::size_t bytes_transferred);
  void  handle_write2(boost::system::error_code error);
  void  clearData();
  void  handleBadRequest(boost::system::error_code error);
  void  handleGoodRequest(boost::system::error_code error, std::string rest);


  enum { max_length = 1024 };
  size_t byteCount1 =0;
  char data_[max_length];
  char data_2_[max_length];

  // added private var
  NginxConfig* config_ = NULL;
  boost::asio::io_service& io_service_;
  request_parser request_parser_;

  // patcher
  Requesthandle* m_request_handler=nullptr;
  RequestHandlerDispatcher* m_dispatcher=nullptr;

  Request request_;
  Response reply_;

  // unit testing class
  friend class SessionTestFixture;
};

#endif  // SESSION_H
