#include "gtest/gtest.h"
#include "server.h"
#include "session.h"
#include "config_parser.h"
#include <iostream>

class ServerTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfig out_config;
    short port = 8172;
    void handleAccept(server* new_server, session* new_session, boost::system::error_code error);
};

void ServerTestFixture::handleAccept(server* new_server, session* new_session, boost::system::error_code error) {
    new_server->handle_accept(new_session, error);
}

// Test to make sure server initialized with no error
TEST_F(ServerTestFixture, Socket_Test) {
    bool success = true;
    try
    {
        server new_server = server(io_service, port, out_config);
        server* server_ptr = &new_server;
        session new_session = session(io_service, out_config);
        session* session_ptr = &new_session;
        boost::system::error_code ec = boost::system::error_code();

        handleAccept(server_ptr, session_ptr, ec);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}

// Test if error during handling accept
TEST_F(ServerTestFixture, Accept_Test) {
    bool success = true;
    try
    {
        server new_server = server(io_service, port, out_config);
        server* server_ptr = &new_server;
        session new_session = session(io_service, out_config);
        session* session_ptr = &new_session;
        boost::system::error_code ec = make_error_code(boost::system::errc::timed_out);

        handleAccept(server_ptr, session_ptr, ec);
    }
    catch (int e)
    {
        success = false;
        std::cout << "An exception occurred. Exception #" << e << std::endl;
    }

    EXPECT_TRUE(success);
}
