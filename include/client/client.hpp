#pragma once

#include "../general/\protocol/command_analyzer.hpp"
#include "../general/multithread_context/multithread_context.hpp"
#include "network/client_networking.hpp"
#include "config/client_config.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(client)

class client : public farconn::general::multithread_context<client> {
public:
	client() = delete;
	client(const client_config&);

	void run() override;
	void stop() override;

protected:
	void setup_contexts() override;
	void working_context();

private:
	void on_message_received(SOCKET, std::string);
	void on_client_net_error_occured(void*);

private:
	bool is_connected = false;
	client_config config;
	client_networking networking;
};

FARCONN_NAMESPACE_END