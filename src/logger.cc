#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include "../include/request/request.h"
#include <boost/signals2.hpp>
#include "../include/logger.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;
using boost::asio::ip::tcp;


Logger::Logger()
{
    init();
    logging::add_common_attributes();
    logging::record rec = lg.open_record();
    logging::add_console_log(std::cout, boost::log::keywords::format = ">> %Message%");
}


void Logger::init()
{
    logging::add_file_log
    (
        keywords::file_name = "../static/SYSLOG_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%",
        keywords::auto_flush = true
    );
}

void Logger::log(int tag, std::string message)
{

    /*
    trace 0
    debug 1
    warning 3
    error 4
    */

    switch (tag)
    {
    case 0:
        BOOST_LOG_SEV(lg, trace) << "Trace: " << message;
        break;

    case 1:
        BOOST_LOG_SEV(lg, debug) << "Debug: " << message;
        break;

    case 3:
        BOOST_LOG_SEV(lg, warning) << "Warning: " << message;
        break;

    case 4:
        BOOST_LOG_SEV(lg, error) << "Error: " << message;
        break;

    } 
}


void Logger::logTraceHTTPrequest(Request http_request, std::string soc_string) {
    std::stringstream stream;
    stream << "Trace: ";
    stream << http_request.method << " " << http_request.uri 
    << " HTTP " << http_request.http_version_major << "." << http_request.http_version_minor;
    stream << " Sender IP: " << soc_string;
    BOOST_LOG_SEV(lg, trace) << stream.str();
}

Logger* Logger::logger = 0;