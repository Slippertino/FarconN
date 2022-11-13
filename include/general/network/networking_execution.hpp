#pragma once

#include <thread>
#include <atomic>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class networking_execution {
public:
	networking_execution();

	void run();
	void stop();

	virtual ~networking_execution();

protected:
	void working_loop();

	virtual void working_context() = 0;

protected:
	static const std::chrono::milliseconds default_context_delay_ms;

	std::thread work_flow;
	std::atomic<bool> cancellation_flag;
};

FARCONN_NAMESPACE_END