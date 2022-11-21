#include "../logger/logger_stream.hpp"

FARCONN_NAMESPACE_BEGIN(general)

logger_stream::logger_stream(std::mutex& locker) : stream_locker(locker) 
{ }

logger_stream::~logger_stream() {
	stream_locker.unlock();
}

FARCONN_NAMESPACE_END