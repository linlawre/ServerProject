#include "gtest/gtest.h"
#include "session.h"
#include <iostream>
#include <boost/system/system_error.hpp>
#include "logger.h" 
#include "request/request.h"
#include "request/response.h"
#include "config_parser.h"

class SessionTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfig out_config;
    session new_session = session(io_service,out_config);
    void setData(session* new_session, char* new_data, Request new_request);
    Response getReply(session* new_session);
    void handleError(boost::system::error_code error, session* new_session);
    void handleBadRequest(session* new_session);
    void handleGoodRequest(session* new_session);
    void handleWrite(session* new_session);
    char m_data[1024];
    Request new_request;
};

void SessionTestFixture::setData(session* new_session, char* new_data, Request new_request) {
    strcpy(new_session->data_, new_data);
    new_session->request_ = new_request;
}

Response SessionTestFixture::getReply(session* new_session){
    return new_session->reply_;
}

void SessionTestFixture::handleError(boost::system::error_code error, session* new_session) {
    new_session->handle_read2(error, strlen(new_session->data_));
}

void SessionTestFixture::handleGoodRequest(session* new_session) {
    new_session->handleGoodRequest(boost::system::error_code(), "");
}

void SessionTestFixture::handleBadRequest(session* new_session) {
    new_session->handleBadRequest(boost::system::error_code());
}

void SessionTestFixture::handleWrite(session* new_session) {
    new_session->handle_write2(boost::system::error_code());
}

// Test to make sure TCP socket function runs without error
TEST_F(SessionTestFixture, Socket_Test) {
    bool success = true;
    try
    {
        tcp::socket& socket_val = new_session.socket();
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}

// Test to make sure start() function
TEST_F(SessionTestFixture, Start_Test) {
    bool success = true;
    try
    {
        new_session.start();
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}

// Test to make sure session can handle read of valid echo request
TEST_F(SessionTestFixture, Valid_Echo_Request_Test) {
    bool success = true;
    try {
        session* session_ptr = &new_session;
        char new_data[1024] = "GET /echo HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
        
        // set fake request values
        new_request.method = "GET";
        new_request.uri = "/echo";
        new_request.http_version_major = 1;
        new_request.http_version_minor = 1;

        // set fake header values
        new_request.headers.resize(1);
        new_request.headers[0].name = "Host";
        new_request.headers[0].value = "www.test.com";
        
        setData(session_ptr, new_data, new_request);

        handleGoodRequest(session_ptr);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}

// Test to make sure session can handle read of valid static file request
TEST_F(SessionTestFixture, Valid_SF_Request_Test) {
    bool success = true;
    try {
        session* session_ptr = &new_session;
        char new_data[1024] = "GET /static HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";
        
        // set fake request values
        new_request.method = "GET";
        new_request.uri = "/static";
        new_request.http_version_major = 1;
        new_request.http_version_minor = 1;

        // set fake header values
        new_request.headers.resize(1);
        new_request.headers[0].name = "Host";
        new_request.headers[0].value = "www.test.com";
        
        setData(session_ptr, new_data, new_request);

        handleGoodRequest(session_ptr);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}

// // Test to make sure session can handle read of invalid request
TEST_F(SessionTestFixture, Invalid_Request_Test) {
    session* session_ptr = &new_session;

    try {
        handleBadRequest(session_ptr);
    }
    catch (int e)
    {
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    Response rec_reply = getReply(session_ptr);
    
    EXPECT_EQ(rec_reply.status, Response::bad_request);
}

// Test to make sure session can handle error code
TEST_F(SessionTestFixture, Error_Test) {
    bool success = true;
    try {
        session* session_ptr = &new_session;
        char new_data[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: Keep-Alive\r\n\r\n";

        // set fake request values
        new_request.method = "GET";
        new_request.uri = "/echo";
        new_request.http_version_major = 1;
        new_request.http_version_minor = 1;

        // set fake header values
        new_request.headers.resize(1);
        new_request.headers[0].name = "Host";
        new_request.headers[0].value = "www.test.com";
        
        setData(session_ptr, new_data, new_request);

        boost::system::error_code ec = make_error_code(boost::system::errc::timed_out);
        handleError(ec, session_ptr);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }
    EXPECT_TRUE(success);
}

// Test to make sure session can handle write callback
TEST_F(SessionTestFixture, Write2_Test) {
    bool success = true;
    try {
        session* session_ptr = &new_session;
        handleWrite(session_ptr);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}
