#include "../client.hpp"

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
	add_context(&client::working_context, mt_sleep_time(10000), mt_mode::ASYNCHRONIZED);
}

void client::working_context() {
	farconn::general::command_analyzer anal;

	anal.set_id("id");
	anal.set_command("echo");

	int n_params;
	std::cout << "Введите кол-во параметров: "; 
	std::cin >> n_params;

	for (auto i = 0; i < n_params; ++i) {
		std::cin.clear();
		std::string temp;
		std::getline(std::cin, temp);

		anal.push_parameter(temp);
	}

	networking.push_message(anal.to_string());

	LOG() << "Сообщение отправлено!\n";
}

void client::on_message_received(SOCKET sock, const std::string& msg) {
	LOG() << "Получено сообщение: " << msg << "\n";
}

void client::on_client_net_error_occured(void* net) {
	stop();
}


FARCONN_NAMESPACE_END