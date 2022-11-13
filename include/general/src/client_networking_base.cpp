#include "../network/client_networking_base.hpp"

FARCONN_NAMESPACE_BEGIN(general)

client_networking_base::client_networking_base() = default;

client_networking_base::client_networking_base(SOCKET sock) : networking(sock)
{ }

void client_networking_base::push_message(const std::string& msg) {
	std::lock_guard<std::mutex> locker(messages_to_send_locker);
	messages_to_send.push(msg);
}

client_networking_base::~client_networking_base() = default;

void client_networking_base::working_context() {
	std::string msg;

	try {
		if (storage.can_read(socket)) {
			receive_message(msg);
			store_message(msg);
		}

		if (storage.can_write(socket)) {
			std::string msg;

			{
				std::lock_guard<std::mutex> locker(messages_to_send_locker);
				msg = messages_to_send.front();
			}

			send_message(msg);
		}
	}
	catch (const std::runtime_error& error) {
		cancellation_flag = true;
		throw error;
	}

	std::this_thread::sleep_for(default_context_delay_ms);
}

FARCONN_NAMESPACE_END