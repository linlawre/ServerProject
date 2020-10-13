
#include "request/request_handle.h"
#include <iostream>
#include <cassert>
using namespace std;

reply Requesthandle::handleRequest(const Request &request_)
{
    // for the reply we want HTTP/1.1 200 ok crlf rquest
    // and tes/plain
    reply_.status = reply::ok;
    reply_.headers.resize(2);
    reply_.content = ToString(request_, rest);
    reply_.headers[0].name = "Content-Length";
    reply_.headers[0].value = std::to_string((reply_.content).length());
    reply_.headers[1].name = "Content-Type";
    reply_.headers[1].value = "text/plain";

    return reply_;
}
