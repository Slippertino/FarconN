#pragma once

#include <iomanip>
#include <sstream>
#include <mutex>
#include "logger_stream.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class logger {
public:	
	static logger& get_instance();
	
	logger_stream&& log(const std::string& = "");

public:
	logger(const logger&) = delete;
	void operator=(const logger&) = delete;

private:
	logger() = default;

	std::string build_prefix(const std::string&) const;
	std::string get_time() const;

private:
	std::mutex stream_locker;
};

#define LOG(section) logger::get_instance().log(#section)

FARCONN_NAMESPACE_END
