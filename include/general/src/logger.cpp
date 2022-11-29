#include "../logger/logger.hpp"

FARCONN_NAMESPACE_BEGIN(general)

logger& logger::get_instance() {
	static logger logger;

	return logger;
}

logger_stream&& logger::log() {
	stream_locker.lock();

	std::cout << build_prefix();

	return logger_stream(stream_locker);
}

std::string logger::build_prefix() const {
	return (std::ostringstream() << "[ " << get_time() << " ] ").str();
}

std::string logger::get_time() const {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	return (std::ostringstream() << std::put_time(&tm, "%d/%m/%Y %H:%M:%S")).str();
}

FARCONN_NAMESPACE_END