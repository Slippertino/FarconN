#include "../client.hpp"
#include "../../general/protocol/protocol_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(client)

client::client(const client_config& client_cfg) : config(client_cfg) {
	is_connected = networking.connect(config.get_ip(), config.get_port());

	if (is_connected) {
		networking.error_occured    += event_create(const_cast<client*>(this), client::on_client_net_error_occured);
		networking.message_received += event_create(const_cast<client*>(this), client::on_message_received);
	}
}

void client::run() {
	if (!is_connected) {
		return;
	}

	multithread_context<client>::run();

	networking.run();
}

void client::stop() {
	multithread_context<client>::stop();

	networking.stop();
}

void client::setup_contexts() {
	add_context(&client::working_context, mt_sleep_time(2000), mt_mode::ASYNCHRONIZED);
}

void client::working_context() {
	std::string message_to_send;
	LOG() << "Сообщение: ";
	std::getline(std::cin, message_to_send);

	LOG() << "В очередь на отправку: " << message_to_send << "\n";

	utf8_encoder::from_local_to_utf8(message_to_send);

	networking.push_message(message_to_send);
}

void client::on_message_received(SOCKET sock, std::string msg) { 
	utf8_encoder::from_utf8_to_local(msg);
	LOG() << "Получено сообщение: " << msg << "\n";
}

void client::on_client_net_error_occured(void* net) {
	stop();
}


FARCONN_NAMESPACE_END