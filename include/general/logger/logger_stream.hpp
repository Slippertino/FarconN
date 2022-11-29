#pragma once

#include <iostream>
#include <sstream>
#include <mutex>
#include "../tools/utf8_encoder.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class logger_stream {
public:
	logger_stream() = delete;
	logger_stream(std::mutex&);

	template<class T>
	logger_stream& operator<<(const T& obj) {
		std::cout << obj;
		return *this;
	}

	~logger_stream();

private:
	std::mutex& stream_locker;
};

FARCONN_NAMESPACE_END