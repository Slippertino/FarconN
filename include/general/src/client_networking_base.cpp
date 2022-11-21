/*#include "../network/client_networking_base.hpp"

FARCONN_NAMESPACE_BEGIN(general)

client_networking_base::client_networking_base() = default;

client_networking_base::client_networking_base(SOCKET sock) : networking(sock)
{ }

void client_networking_base::push_message(const std::string& msg) {
	std::lock_guard<std::mutex> locker(messages_to_send_locker);
	messages_to_send.push(msg);
}

client_networking_base::~client_networking_base() {
	if (socket != INVALID_SOCKET) {
		LOG() << build_logging_prefix("уничтожение...");
		shutdown(socket, SD_BOTH);
	}
}

std::string client_networking_base::build_logging_prefix(const std::string& event) const {
	return (std::ostringstream() << "Client socket #" << socket << " : " << event << "\n").str();
}


void client_networking_base::working_context() {
	std::string msg;

	auto& storage = networking_storage::storage();

	try {
		if (storage.can_read(socket)) {
			receive_message(msg);

			LOG() << build_logging_prefix("получено сообщение") << msg << "\n";

			event_invoke(message_received)(socket, msg);
		}

		if (storage.can_write(socket)) {
			std::string msg;

			messages_to_send_locker.lock();

			msg = messages_to_send.front();
			messages_to_send.pop();

			messages_to_send_locker.unlock();

			send_message(msg);

			LOG() << build_logging_prefix("отправлено сообщение") << msg << "\n";
		}
	}
	catch (const std::runtime_error& error) {
		LOG() << build_logging_prefix("ошибка") << error.what() << "\n";

		cancellation_flag = true;

		event_invoke(error_occured)(const_cast<client_networking_base*>(this));
	}
}

void client_networking_base::setup_contexts() {
	add_context(&client_networking_base::working_context, mt_sleep_time(1));
}

FARCONN_NAMESPACE_END*/