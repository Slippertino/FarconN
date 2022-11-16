#pragma once

#include <unordered_map>
#include <string>
#include "config/server_config.hpp"
#include "network/client_networking_proxy.hpp"
#include "network/server_networking.hpp"
#include "middleware/server_middleware.hpp"
#include "../general/logger/logger.hpp"
#include "../general/multithread_context/multithread_context.hpp"
#include "protocol/command_analyzer.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class server : public multithread_context<server> {
public:
	server() = delete;
	server(const server_config&);
	
	void run() override;
	void stop() override;
	void setup_contexts() override;

	void add_client(SOCKET);
	void push_command(SOCKET, const std::string&);

	~server();

private:
	using arguments_t = server_middleware::arguments_t;

private:
	void handle_command();

private:
	static const size_t working_flows_count;
	static const std::unordered_map<std::string, std::function<void(const arguments_t&, arguments_t&)>> command_handlers;

	std::queue<std::pair<SOCKET, std::string>> commands_to_handle;
	std::mutex commands_mutex;

	std::unordered_map<SOCKET, std::unique_ptr<client_networking_proxy>> clients;
	std::mutex clients_mutex;

	server_networking networking;

	server_middleware repository;
};

FARCONN_NAMESPACE_END