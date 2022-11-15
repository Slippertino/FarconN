#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t server::working_flows_count = 1;

const std::unordered_map<std::string, std::function<void(const server::arguments_t&, server::arguments_t&)>> server::command_handlers =
{ };

server::server(const server_config& config) : 
	networking(this), 
	repository(config.db_config, config.files_storage_path) {
	networking.setup(config.ipv4, config.port);
}

void server::run() {
	networking.run();

	cancellation_flag = false;
	for (auto i = 0; i < working_flows_count; ++i) {
		working_flows[i] = std::thread(&server::handle_command, this);
	}
}

void server::add_client(SOCKET socket) {
	std::lock_guard<std::mutex> locker(clients_mutex);
	clients.insert({ socket, std::make_unique<client_networking_proxy>(this, socket)});
	clients[socket]->run();
}

void server::push_command(SOCKET socket, const std::string& command) {
	std::lock_guard<std::mutex> locker(commands_mutex);
	commands_to_handle.push({ socket, command });
}

server::~server() {
	cancellation_flag = true;
	for (auto& flow : working_flows) {
		flow.join();
	}
}

void server::handle_command() {
	while (!cancellation_flag) {
		commands_mutex.lock();
		auto socket_command = commands_to_handle.front();
		commands_to_handle.pop();
		commands_mutex.unlock();

		command_analyzer analyzer;
		command com;

		analyzer.parse(socket_command.second).to_command(com);

		arguments_t output_args;
		command_handlers.at(com.cmd_name)(com.params, output_args);

		analyzer.clear_parameters();
		for (auto& arg : output_args) {
			analyzer.push_parameter(arg);
		}

		clients[socket_command.first]->push_message(
			analyzer.to_string()
		);
	}
}

FARCONN_NAMESPACE_END