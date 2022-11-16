#include "../logger/logger.hpp"

FARCONN_NAMESPACE_BEGIN(general)

logger& logger::get_instance() {
	static logger logger;

	return logger;
}

logger_stream&& logger::log() {
	return logger_stream(stream_locker);
}

FARCONN_NAMESPACE_END