#pragma once

#include <mutex>
#include "logger_stream.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class logger {
public:	
	static logger& get_instance();
	
	logger_stream&& log();

public:
	logger(const logger&) = delete;
	void operator=(const logger&) = delete;

private:
	logger() = default;

private:
	std::mutex stream_locker;
};

#define LOG() logger::get_instance().log()

FARCONN_NAMESPACE_END
