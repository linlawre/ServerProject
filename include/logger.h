#ifndef LOGGER_H
#define LOGGER_H

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/signals2.hpp>
#include "request/request.h"
using boost::asio::ip::tcp;
using namespace boost::log::trivial;
using http::server::Request;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


class Logger 
{
    public:
    src::severity_logger<severity_level> lg;
    Logger();
    void init();

    void log(int tag, std::string message);

    void logTraceHTTPrequest(Request http_request, std::string soc_String);


    static Logger * logger;


    static Logger * getLogger() {
        if (Logger::logger==0) 
            Logger::logger = new Logger();
        return Logger::logger;
    }
};
#endif
