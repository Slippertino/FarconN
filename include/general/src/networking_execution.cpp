#include "../network/networking_execution.hpp"

#include <iostream>

FARCONN_NAMESPACE_BEGIN(general)

const std::chrono::milliseconds networking_execution::default_context_delay_ms = std::chrono::milliseconds(1);

networking_execution::networking_execution() = default;

void networking_execution::run() {
	cancellation_flag = false;
	work_flow = std::thread(&networking_execution::working_loop, this);
}

void networking_execution::stop() {
	cancellation_flag = true;

	if (work_flow.joinable()) {
		work_flow.join();
	}
}

networking_execution::~networking_execution() {
	stop();
}

void networking_execution::working_loop() {
	while (!cancellation_flag) {
		working_context();
	}
}

FARCONN_NAMESPACE_END