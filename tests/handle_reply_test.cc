#include <iostream>
#include "gtest/gtest.h"
#include "request/request.h"
#include "request/request_parser.h"
#include "config_parser.h"
#include "server.h"
#include "request/response.h"
#include "request/request_handle.h"
#include "request/proxy_handle.h"
#include "request/echo_handle.h"
#include "request/static_file_handle.h"
#include "request/status_handle.h"
#include "request/health_handle.h"
#include "request/covid_handle.h"
#include <string>
#include <fstream>
#include <streambuf>

class HandlerTest :public :: testing::Test{
  protected:
    NginxConfigParser parser;
    Requesthandle* proxy_handler_ptr;
    ProxyHandle* proxy;
    Requesthandle* echo_handler_ptr;
    EchoHandle* echo;
    Requesthandle* static_file_handler_ptr;
    StaticFileHandle* static_file;
    Requesthandle* status_handler_ptr;
    StatusHandle* status;
    Requesthandle* health_handler_ptr;
    HealthHandle* health;
    CovidHandle* covid;
    http::server::Request request_;
    NginxConfig config;
    char c;
    std::ifstream file;

  void SetProxyHandler(std::string location_prefix, std::string uri) {
    server::uri2handler["location_prefix"] = new ProxyHandle();
    proxy = new ProxyHandle();
    proxy->set_host("www.ucla.edu");
    proxy->set_location(location_prefix);
    proxy->set_port_number("80");
    request_.method = "GET";
    request_.uri = location_prefix + uri;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.0";
  }
  void SetStaticFileHandler(std::string location_prefix, std::string uri) {
    server::uri2handler["location_prefix"] = new StaticFileHandle();
    static_file = new StaticFileHandle();
    request_.method = "GET";
    request_.uri = location_prefix + uri;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.1";
  }
  void SetStatusHandler(std::string location_prefix) {
    server::uri2handler["location_prefix"] = new StatusHandle();
    status = new StatusHandle();
    session::request_counter = 0;
    session::request_history;
    request_.method = "GET";
    request_.uri = location_prefix;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.1";
  }
  void SetEchoHandler(std::string location_prefix) {
    server::uri2handler["location_prefix"] = new EchoHandle();
    echo = new EchoHandle();
    request_.method = "GET";
    request_.uri = location_prefix;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.1";
  }
  void SetHealthHandler(std::string location_prefix) {
    server::uri2handler["location_prefix"] = new HealthHandle();
    health = new HealthHandle();
    request_.method = "GET";
    request_.uri = location_prefix;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.1";
  }
  void SetCovidHandler(std::string location_prefix) {
    server::uri2handler["location_prefix"] = new CovidHandle();
    covid = new CovidHandle();
    request_.method = "GET";
    request_.uri = location_prefix;
    request_.http_version_major = 1;
    request_.http_version_minor = 0;
    request_.version = "HTTP/1.1";
  }
};

TEST_F(HandlerTest, uclaTest) {
  SetProxyHandler("/ucla", "/js/search.js");
  http::server::Response reply_ = proxy->handleRequest(request_);
  std::string expected;
  std::ifstream send_file;
  send_file.open("../tests/proxy_test.txt");
  if (!send_file.good()) {
      EXPECT_TRUE(false);
  } else {
      std::vector<char> send_data;
      char c = '\0';
      while (send_file.get(c)) {
          send_data.push_back(c);
      }
      std::string send_data_string(send_data.begin(), send_data.end());
      expected = send_data_string;
  }
  EXPECT_EQ(reply_.status, Response::ok);
  EXPECT_EQ(reply_.content, expected);
}

// test 404 static file not found
TEST_F(HandlerTest, StaticFileNotFoundTest){
  SetStaticFileHandler("/static", "/notexist.js");
  http::server::Response reply_ = static_file->handleRequest(request_);

  bool status = reply_.status == http::server::Response::not_found;
  bool content = reply_.content == "<h1>404</h1><p>Dude, we don't have the file you requested! :(</p>";
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && content && hdr_content);
}
// test static jpeg file
TEST_F(HandlerTest, ServingJPEGFileTest){
  SetStaticFileHandler("/static", "/royce.jpg");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/royce.jpg");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "image/jpeg";
  EXPECT_TRUE(status && content && hdr_content);
}

// // test static png file
TEST_F(HandlerTest, ServingPNGFileTest){
  SetStaticFileHandler("/static", "/image.png");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/image.png");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  std::cerr << "content length" << '\n';
  std::cerr << (reply_.content).length() << '\n';

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"]  == "image/png";
  EXPECT_TRUE(status && content && hdr_content);
}

// // test static gif file not found
TEST_F(HandlerTest, ServingGIFFileTest){
  SetStaticFileHandler("/static", "/illusion.gif");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/illusion.gif");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "image/gif";
  EXPECT_TRUE(status && content && hdr_content);
}

// // test static txt file not found
TEST_F(HandlerTest, ServingTXTFileTest){
  SetStaticFileHandler("/static", "/test.txt");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/test.txt");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/plain";
  EXPECT_TRUE(status && content && hdr_content);
}

// // test static html file not found
TEST_F(HandlerTest, ServingHTMLFileTest){
  SetStaticFileHandler("/static", "/strings.html");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/strings.html");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && content && hdr_content);
}

// // test static zip file not found
TEST_F(HandlerTest, ServingZIPFileTest){
  SetStaticFileHandler("/static", "/test.zip");
  http::server::Response reply_ = static_file->handleRequest(request_);

  file.open("../static/test.zip");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "application/zip";
  EXPECT_TRUE(status && content && hdr_content);
}

// test health handler
TEST_F(HandlerTest, HealthHandlerTest){
  SetHealthHandler("/health");
  http::server::Response reply_ = health->handleRequest(request_);

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == "<h1>OK</h1>";
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && content && hdr_content);
}

//test echo handler
TEST_F(HandlerTest, EchoHandlerTest){
  SetEchoHandler("/echo");
  http::server::Response reply_ = echo->handleRequest(request_);

  std::string echo_content = "";
  std::string http_method = request_.method + " ";
  std::string http_link = request_.uri + " ";

  echo_content.append(http_method);
  echo_content.append(http_link);
  echo_content.append("HTTP/1.1\r\n");

  for (auto x : request_.headers_)
  {
      echo_content.append(x.first);
      echo_content.append(" ");
      echo_content.append(x.second);
      echo_content.append("\r\n");
  }

  echo_content.append("\r\n");

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == echo_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/plain";
  EXPECT_TRUE(status && content && hdr_content);
}

// test status handler
TEST_F(HandlerTest, StatusHandlerTest){
  SetStatusHandler("/status");
  http::server::Response reply_ = status->handleRequest(request_);

  std::string url_status = "";
  for (auto it = session::request_history.begin(); it != session::request_history.end(); ++it){
      for(int j = 0; j <  (it->second).size(); j++){
        url_status += "<tr><td>" + it->first +"</td><td>" + std::to_string((it->second)[j]) + "</td></tr>";
      }
  }
  std::string status_content = "<html></head><h1>Server Status</h1></head>"
    "<body><h2>Total requests handled: "+ std::to_string(session::request_counter) +"</h2>"
    "<br><h2>Detail Request Status</h2>"
    "<table>"
    "<tr><th>URL</th><th>Status Code</th></tr>" + url_status + "</table>"
    "<br><h2>Request Handlers</h2>"
    "<table>"
    "<tr><th>URL Prefix</th><th>Handler</th></tr><tr><th>/echo</th><th>Echo Handler</th></tr>"
    "<tr><th>/static</th><th>Static File Handler</th></tr><tr><th>/status</th><th>Status Handler</th></tr>"
    "<tr><th>All Other Cases</th><th>Fof Handler</th></tr></table>"
    "</body>"
    "</html>";
  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == status_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string(status_content.length()) &&
  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && content && hdr_content);
}

// Test Basic Covid Handler 
TEST_F(HandlerTest, CovidHandlerTest){
  SetCovidHandler("/covid");
  http::server::Response reply_ = covid->handleRequest(request_);
  bool status = reply_.status == http::server::Response::ok;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && hdr_content);
}


// Test if the return HTML page match with the location html 
TEST_F(HandlerTest, CovidPageTest){
  SetCovidHandler("/covid");
  http::server::Response reply_ = covid->handleRequest(request_);

  file.open("../static/covid_index.html");
  std::string file_content;
  while (file.get(c)) file_content += c;
  file.close();

  bool status = reply_.status == http::server::Response::ok;
  bool content = reply_.content == file_content;
  bool hdr_content = reply_.headers_["Content-Length"] == std::to_string((reply_.content).length()) &&
  reply_.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(status && content && hdr_content);
}
