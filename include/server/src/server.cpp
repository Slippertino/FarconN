#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t server::working_flows_count = 1;

bool server::is_run() const {
	return is_server_running;
}

bool server::is_setup() const {
	return is_server_setup;
}

void server::setup(const server_config& config) {
	if (is_server_setup) {
		return;
	}

	networking_storage::storage();

	repository.setup(config);

	networking.connection_incoming += event_create(const_cast<server*>(this), server::on_connection_incoming);
	networking.error_occured += event_create(const_cast<server*>(this), server::on_server_net_error_occured);

	networking.setup(config.get_ip(), config.get_port());

	is_server_setup = true;
}

void server::setup_contexts() {
	for (auto i = 0; i < working_flows_count; ++i) {
		add_context(&server::handle_command, mt_sleep_time(1));
	}
}

void server::run() {
	if (!is_server_setup || is_server_running) {
		return;
	}

	multithread_context<server>::run();

	networking.run();

	is_server_running = true;
}

void server::stop() {
	if (!is_server_setup || !is_server_running) {
		return;
	}

	multithread_context<server>::stop();

	networking.stop();

	repository.refresh();

	is_server_running = false;
}

server::~server() {
	stop();
}

void server::handle_command() {
	if (commands_to_handle.empty()) {
		return;
	}

	auto socket_command = commands_to_handle.wait_and_erase();
	utf8_encoder::from_utf8_to_local(socket_command.second);

	LOG(SERVER) << "Входящая команда: " << socket_command.second << "\n";

	std::shared_ptr<command_entity> com;
	auto resp = std::make_shared<command_response>();

	try {
		com = protocol_interpreter::interpret(socket_command.second);
		resp->command = com->command;
		resp->options = com->options;

		repository.handle(com.get(), resp.get());
	}
	catch (const std::exception& ex) {
		LOG(SERVER) << "Недопустимый ввод! " << ex.what() << "\n";
		resp->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND_ERROR);
	}

	com->clear();
	resp->to_entity(*com);

	command_builder builder(*com);

	std::string message_to_send;
	builder.build(message_to_send);

	LOG(SERVER) << "В очередь на отправку: " << message_to_send << "\n";

	utf8_encoder::from_local_to_utf8(message_to_send);

	clients[socket_command.first]->push_message(message_to_send);
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

void server::on_message_received(SOCKET client, std::string msg) {
	commands_to_handle.add({ client, msg });
}

void server::on_client_net_error_occured(void* net) {
	auto* client_net = static_cast<client_networking_proxy*>(net);

	client_net->stop();

	std::lock_guard<std::mutex> locker(clients_mutex);

	clients.erase(client_net->get_socket_desc());
}

FARCONN_NAMESPACE_END