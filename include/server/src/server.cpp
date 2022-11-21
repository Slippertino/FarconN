#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t server::working_flows_count = 1;

const std::unordered_map<std::string, std::function<void(server*, const server::arguments_t&, server::arguments_t&)>> server::command_handlers =
{
	{"echo", [&](server* obj, const server::arguments_t& in, server::arguments_t& out) { obj->repository.handle_echo(in, out); }},
};

server::server(const server_config& config) : 
	repository(config.get_db_config(), config.get_files_storage_path()) {
	networking_storage::storage();

	networking.connection_incoming += event_create(const_cast<server*>(this), server::on_connection_incoming);
	networking.error_occured	   += event_create(const_cast<server*>(this), server::on_server_net_error_occured);

	networking.setup(config.get_ip(), config.get_port());
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

	try {
		analyzer.parse(socket_command.second).to_command(com);
	}
	catch (const std::exception& ex) {
		LOG() << "Недопустимый ввод! " << ex.what() << "\n";
		return;
	}

	arguments_t output_args;
	
	if (command_handlers.find(com.cmd_name) != command_handlers.end()) {
		command_handlers.at(com.cmd_name)(this, com.params, output_args);
	} else {
		LOG() << "Ошибка! Неизвестная команда: " << com.cmd_name << "\n";
		return;
	}

	analyzer.clear_parameters();
	for (auto& arg : output_args) {
		analyzer.push_parameter(arg);
	}

	clients[socket_command.first]->push_message(
		analyzer.to_string()
	);
}

void server::on_connection_incoming(SOCKET con) {
	std::lock_guard<std::mutex> locker(clients_mutex);
	clients.insert({ con, std::make_unique<client_networking_proxy>(con) });

	clients[con]->message_received += event_create(const_cast<server*>(this), server::on_message_received);
	clients[con]->error_occured    += event_create(const_cast<server*>(this), server::on_client_net_error_occured);

	clients[con]->run();
}

void server::on_server_net_error_occured(void* net) {
	auto* server_net = static_cast<server_networking*>(net);
	server_net->stop();
}

void server::on_message_received(SOCKET client, const std::string& msg) {
	std::lock_guard<std::mutex> locker(commands_mutex);
	commands_to_handle.push({ client, msg });
}

void server::on_client_net_error_occured(void* net) {
	auto* client_net = static_cast<client_networking_proxy*>(net);

	client_net->stop();

	std::lock_guard<std::mutex> locker(clients_mutex);

	clients.erase(client_net->get_socket_desc());
}

FARCONN_NAMESPACE_END