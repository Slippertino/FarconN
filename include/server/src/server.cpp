#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t server::working_flows_count = 1;

const std::unordered_map<std::string, std::function<void(server*, const command_entity*, command_response*)>> server::command_handlers =
{
	{"echo",         [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_echo(in, out); }},
	{"signup",       [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_signup(in, out); }},
	{"login",        [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_login(in, out); }},
	{"logout",       [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_logout(in, out); }},
	{"profile_get",  [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_profile_get(in, out); }},
	{"profile_set",  [&](server* obj, const command_entity* in, command_response* out) { obj->repository.handle_profile_set(in, out); }},
};

server::server(const server_config& config) : 
	repository(config.get_db_config(), config.get_files_storage_path()) {
	networking_storage::storage();

	repository.setup();

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
	if (commands_to_handle.empty()) {
		return;
	}

	auto socket_command = commands_to_handle.wait_and_erase();
	utf8_encoder::from_utf8_to_local(socket_command.second);

	LOG() << "�������� �������: " << socket_command.second << "\n";

	std::shared_ptr<command_entity> com;
	auto resp = std::make_shared<command_response>();

	try {
		com = protocol_interpreter::interpret(socket_command.second);

		if (command_handlers.find(com->command) != command_handlers.end()) {
			command_handlers.at(com->command)(this, com.get(), resp.get());
			resp->command = com->command;
			resp->options = com->options;
		}
		else {
			LOG() << "������! ����������� �������: " << com->command << "\n";
			resp->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND_ERROR);
		}
	}
	catch (const std::exception& ex) {
		LOG() << "������������ ����! " << ex.what() << "\n";
		resp->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND_ERROR);
	}

	com->clear();
	resp->to_entity(*com);

	command_builder builder(*com);

	std::string message_to_send;
	builder.build(message_to_send);

	LOG() << "� ������� �� ��������: " << message_to_send << "\n";

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