#pragma once

#include <unordered_map>
#include <string>
#include "config/server_config.hpp"
#include "network/client_networking_proxy.hpp"
#include "network/server_networking.hpp"
#include "middleware/server_middleware.hpp"
#include "../general/tools/thread_safe_containers/thread_safe_queue.hpp"
#include "../general/logger/logger.hpp"
#include "../general/multithread_context/multithread_context.hpp"
#include "protocol/protocol_interpreter.hpp"
#include "protocol/command_builder.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class server : public multithread_context<server> {
public:
	server() = delete;
	server(const server_config&);
	
	void run() override;
	void stop() override;

	~server();

protected:
	void setup_contexts() override;

private:
	void handle_command();

private:
	void on_connection_incoming(SOCKET);
	void on_server_net_error_occured(void*);
	void on_message_received(SOCKET, std::string);
	void on_client_net_error_occured(void*);

private:
	static const size_t working_flows_count;

	thread_safe_queue<std::pair<SOCKET, std::string>> commands_to_handle;

	std::unordered_map<SOCKET, std::unique_ptr<client_networking_proxy>> clients;
	std::mutex clients_mutex;

	server_networking networking;

	server_middleware repository;
};

FARCONN_NAMESPACE_END