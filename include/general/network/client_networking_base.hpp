#pragma once

#include <atomic>
#include <thread>
#include <queue>
#include "networking_execution.hpp"
#include "networking.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class client_networking_base : public networking_execution, 
							   public networking {
public:
	client_networking_base();
	client_networking_base(SOCKET);

	void push_message(const std::string&);

	virtual ~client_networking_base();

protected:
	void working_context() override;

	virtual void store_message(const std::string&) = 0;

protected:
	std::queue<std::string> messages_to_send;
	std::mutex messages_to_send_locker;
};

FARCONN_NAMESPACE_END