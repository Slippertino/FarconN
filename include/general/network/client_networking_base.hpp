#pragma once

#include <atomic>
#include <thread>
#include <queue>
#include "../multithread_context/multithread_context.hpp"
#include "networking.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class client_networking_base : public multithread_context<client_networking_base>, 
							   public networking {
public:
	client_networking_base();
	client_networking_base(SOCKET);

	void push_message(const std::string&);

	virtual ~client_networking_base();

protected:
	void setup_contexts() override;

	void working_context();

	virtual void store_message(const std::string&) = 0;

protected:
	std::queue<std::string> messages_to_send;
	std::mutex messages_to_send_locker;
};

FARCONN_NAMESPACE_END