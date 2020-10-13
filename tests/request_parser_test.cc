#include "gtest/gtest.h"
#include "request/request_parser.h"
#include "request/request.h"

class RequestParserTestFixture : public ::testing::Test {
  protected:
    http::server::request_parser req_parser;
    http::server::request_parser::result_type result;
    http::server::Request req_type;
};

// Test valid HTTP 1.1 request
TEST_F(RequestParserTestFixture, Valid_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with HTTP backslash
TEST_F(RequestParserTestFixture, Backslash_Request) {
  char request[512] = "GET / HTTP\1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with invalid version
TEST_F(RequestParserTestFixture, Version_Request) {
  char request[512] = "GET / HTTP/1C.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with no carriage return
TEST_F(RequestParserTestFixture, No_Carriage_Request) {
  char request[512] = "GET / HTTP/1.1\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with no colon after header
TEST_F(RequestParserTestFixture, No_Colon_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with no space after colon
TEST_F(RequestParserTestFixture, No_Space_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost:www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request other than GET
TEST_F(RequestParserTestFixture, Post_Request) {
  char request[512] = "POST / HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with special character at start
TEST_F(RequestParserTestFixture, Special_Char_Start_Request) {
  char request[512] = "£GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with special character in method
TEST_F(RequestParserTestFixture, Special_Char_Method_Request) {
  char request[512] = "GET£ / HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}
// Test request with control character
TEST_F(RequestParserTestFixture, Control_Request) {
  char request[512] = "GET \r HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with bad first HTTP character
TEST_F(RequestParserTestFixture, First_HTTP_Request) {
  char request[512] = "GET / STTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with bad second HTTP character
TEST_F(RequestParserTestFixture, Second_HTTP_Request) {
  char request[512] = "GET / HSTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with bad third HTTP character
TEST_F(RequestParserTestFixture, Third_HTTP_Request) {
  char request[512] = "GET / HTSP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with bad fourth HTTP character
TEST_F(RequestParserTestFixture, Fourth_HTTP_Request) {
  char request[512] = "GET / HTTS/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with bad version start
TEST_F(RequestParserTestFixture, Bad_Version_Start_Request) {
  char request[512] = "GET / HTTP/G.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with double digit major version start
TEST_F(RequestParserTestFixture, Double_Digit_Major_Request) {
  char request[512] = "GET / HTTP/10.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with bad minor version
TEST_F(RequestParserTestFixture, Minor_Version_Request) {
  char request[512] = "GET / HTTP/1.G\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with double digit minor version
TEST_F(RequestParserTestFixture, Double_Digit_Minor_Request) {
  char request[512] = "GET / HTTP/1.12\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request without newline
TEST_F(RequestParserTestFixture, No_Newline_Request) {
  char request[512] = "GET / HTTP/1.1\rHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with linear white space 1
TEST_F(RequestParserTestFixture, LWS1_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\tConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with linear white space 2
TEST_F(RequestParserTestFixture, LWS2_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n Connection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with header line start tspecial char
TEST_F(RequestParserTestFixture, HLS_TSpecial_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n=Connection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with header line start non char
TEST_F(RequestParserTestFixture, HLS_NonChar_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nȀConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with tab
TEST_F(RequestParserTestFixture, Tab_Request) {
  char request[512] = "GET / HTTP/1.1\r\n\tHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with linear white space followed by carriage return
TEST_F(RequestParserTestFixture, LWS_Carriage_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\t\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with linear white space followed by tab
TEST_F(RequestParserTestFixture, LWS_Tab_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\t\tConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with linear white space followed by space
TEST_F(RequestParserTestFixture, LWS_Space_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\t Connection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::good);
  EXPECT_TRUE(success);
}

// Test request with linear white space followed by control char
TEST_F(RequestParserTestFixture, LWS_Control_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\t\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with expected newline 2
TEST_F(RequestParserTestFixture, Expected_Newline_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\t\rConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with control char as header value
TEST_F(RequestParserTestFixture, Bad_Header_Val_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: \nwww.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with non-character
TEST_F(RequestParserTestFixture, Non_Char_Request) {
  int requestInt[2] = {0, 1};
  std::tie(result, std::ignore) = req_parser.parse(req_type, requestInt, requestInt + sizeof(requestInt));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with starting tspecial char
TEST_F(RequestParserTestFixture, Tspecial_Char_Request) {
  char request[512] = "=GET / HTTP/1.1\r\nHost: \nwww.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with tspecial char in method
TEST_F(RequestParserTestFixture, Method_TSpecial_Char_Request) {
  char request[512] = "GE=T / HTTP/1.1\r\nHost: \nwww.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with control char in method
TEST_F(RequestParserTestFixture, Method_Control_Request) {
  char request[512] = "GE\rT / HTTP/1.1\r\nHost: \nwww.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with control char instead of colon
TEST_F(RequestParserTestFixture, Control_Colon_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost\r www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with non char instead of colon
TEST_F(RequestParserTestFixture, NonChar_Colon_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHostȀ www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}

// Test request with newline 3
TEST_F(RequestParserTestFixture, Newline3_Request) {
  char request[512] = "GET / HTTP/1.1\r\nHost: www.test.com\r\n\rConnection: Keep-Alive\r\n\r\n";
  std::tie(result, std::ignore) = req_parser.parse(req_type, request, request + strlen(request));
  bool success = (result == http::server::request_parser::bad);
  EXPECT_TRUE(success);
}
