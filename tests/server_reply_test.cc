#include "gtest/gtest.h"
#include "request/response.h"
#include <vector>
#include <string>

class ServerReplayTest : public :: testing::Test{
  protected:
    http::server::Response reply_message;
    std::vector<boost::asio::const_buffer> buffer;
};
  // test the case where the Response status is okay
  TEST_F(ServerReplayTest, OK) {
    reply_message = http::server::Response::stock_reply(http::server::Response::ok);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }

    bool check_status_result = (reply_message.status == http::server::Response::ok);
    bool check_content = (reply_message.content == "");
    
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";

    EXPECT_TRUE(check_header_one_name && check_header_two_name && check_header_two_value && check_header_one_value && check_header_size && check_status_result && check_content);
  }

  // test the case where the Response status is created
  TEST_F(ServerReplayTest, CREATED) {
    reply_message = http::server::Response::stock_reply(http::server::Response::created);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }


    bool check_status_result = (reply_message.status == http::server::Response::created);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";

    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is accepted
  TEST_F(ServerReplayTest, ACCEPTED) {
    reply_message = http::server::Response::stock_reply(http::server::Response::accepted);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::accepted);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is no_content
  TEST_F(ServerReplayTest, NO_CONTENT) {
    reply_message = http::server::Response::stock_reply(http::server::Response::no_content);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::no_content);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is multiple_choices
  TEST_F(ServerReplayTest, MULTIPLE_CHOICES) {
    reply_message = http::server::Response::stock_reply(http::server::Response::multiple_choices);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::multiple_choices);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is moved_permanently
  TEST_F(ServerReplayTest, MOVED_PERMANENTLY) {
    reply_message = http::server::Response::stock_reply(http::server::Response::moved_permanently);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::moved_permanently);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is moved_temporarily
  TEST_F(ServerReplayTest, MOVED_TEMPORARILY) {
    reply_message = http::server::Response::stock_reply(http::server::Response::moved_temporarily);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::moved_temporarily);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is not_modified
  TEST_F(ServerReplayTest, NOT_MODIFIED) {
    reply_message = http::server::Response::stock_reply(http::server::Response::not_modified);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::not_modified);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is bad_request
  TEST_F(ServerReplayTest, BAD_REQUEST) {
    reply_message = http::server::Response::stock_reply(http::server::Response::bad_request);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::bad_request);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is unauthorized
  TEST_F(ServerReplayTest, UNAUTHORIZED) {
    reply_message = http::server::Response::stock_reply(http::server::Response::unauthorized);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::unauthorized);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is forbidden
  TEST_F(ServerReplayTest, FORBIDDEN) {
    reply_message = http::server::Response::stock_reply(http::server::Response::forbidden);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::forbidden);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is not_found
  TEST_F(ServerReplayTest, NOT_FOUND) {
    reply_message = http::server::Response::stock_reply(http::server::Response::not_found);
    std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::not_found);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is internal_server_error
  TEST_F(ServerReplayTest, INTERNAL_SERVER_ERROR) {
    reply_message = http::server::Response::stock_reply(http::server::Response::internal_server_error);
        std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::internal_server_error);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // test the case where the Response status is not_implemented
  TEST_F(ServerReplayTest, NOT_IMPLEMENTED) {
    reply_message = http::server::Response::stock_reply(http::server::Response::not_implemented);
      std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::not_implemented);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>");

    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // // test the case where the Response status is bad_gateway
  TEST_F(ServerReplayTest, BAD_GATEWAY) {
    reply_message = http::server::Response::stock_reply(http::server::Response::bad_gateway);
      std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::bad_gateway);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }

  // // test the case where the Response status is service_unavailable
  TEST_F(ServerReplayTest, SERVICE_UNAVAILABLE) {
    reply_message = http::server::Response::stock_reply(http::server::Response::service_unavailable);
      std::vector<bool> check;
    std::vector<std::string> save;

    for (auto x : reply_message.headers_)
    {
      save.push_back(x.first);
      save.push_back(x.second);
    }
    bool check_status_result = (reply_message.status == http::server::Response::service_unavailable);
    bool check_content = (reply_message.content == "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>");
    bool check_header_size = (reply_message.headers_.size() == 2);
    bool check_header_one_name = save[0] == "Content-Length";
    bool check_header_one_value = save[1] == std::to_string(reply_message.content.size());
    bool check_header_two_name = save[2] == "Content-Type";
    bool check_header_two_value = save[3] == "text/html";
    buffer = reply_message.to_buffers();
    EXPECT_TRUE(check_status_result && check_content && check_header_size
               && check_header_one_name && check_header_one_value &&
               check_header_two_name && check_header_two_value);
  }
