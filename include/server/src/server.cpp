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

void server::setup_contexts() {
	for (auto i = 0; i < working_flows_count; ++i) {
		add_context(&server::handle_command, mt_sleep_time(1));
	}
}

void server::run() {
	multithread_context<server>::run();

	networking.run();
}

void server::stop() {
	multithread_context<server>::stop();

	networking.stop();
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
	stop();
}

void server::handle_command() {
	commands_mutex.lock();

	if (commands_to_handle.empty()) {
		commands_mutex.unlock();
		return;
	}

	auto socket_command = commands_to_handle.front();
	commands_to_handle.pop();

	commands_mutex.unlock();

	LOG() << "Входящая команда: " << socket_command.second << "\n";

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

FARCONN_NAMESPACE_END