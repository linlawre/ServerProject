#ifndef STATIC_FILE_HANDLE_H
#define STATIC_FILE_HANDLE_H

#include <iostream>

#include "request_handle.h"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem/fstream.hpp>
#include "logger.h"

class StaticFileHandle : public Requesthandle {
public:
    static Requesthandle* Init(std::string location_prefix, const NginxConfig& config);
    Response handleRequest(const Request &request_);
    Logger* logger = Logger::getLogger();
private:
    std::string find_Mime_Type(std::string ext);
    Response reply_;
};

#endif
