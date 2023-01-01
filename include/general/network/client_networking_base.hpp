#pragma once

#include <atomic>
#include <thread>
#include <queue>
#include "../multithread_context/multithread_context.hpp"
#include <cpp_events/event.hpp>
#include "networking.hpp"

FARCONN_NAMESPACE_BEGIN(general)

#undef ERROR

template<class Derived>
class client_networking_base : public multithread_context<Derived>,
							   public networking {
	event(message_received, SOCKET, std::string);

public:
	client_networking_base() = default;
	client_networking_base(SOCKET sock) : networking(sock)
	{ }

	void push_message(const std::string& msg) {
		std::lock_guard<std::mutex> locker(messages_to_send_locker);
		messages_to_send.push(msg);
	}

	virtual ~client_networking_base() {
		if (socket != INVALID_SOCKET) {
			LOG(NETWORK) << build_logging_prefix("уничтожение...");
			shutdown(socket, SD_BOTH);
		}
	}

protected:
	void setup_contexts() override {
		add_context(&client_networking_base<Derived>::working_context, mt_sleep_time(1));
	}

	std::string build_logging_prefix(const std::string& event) const {
		return (std::ostringstream() << "Client socket #" << socket << " : " << event << "\n").str();
	}

	bool is_connected() const {
		int result = ::send(socket, nullptr, 0, 0);

		return (WSAGetLastError() != WSAECONNRESET);
	}

	void working_context() {
		std::string msg;

		auto& storage = networking_storage::storage();

		try {
			if (!is_connected()) {
				LOG(NETWORK) << build_logging_prefix("клиент отключен");

				cancellation_flag = true;

				event_invoke(error_occured)(const_cast<client_networking_base*>(this), false);

				return;
			}

			if (storage.can_read(socket)) {
				receive_message(msg);
				networking_storage::storage().update();

				LOG(NETWORK) << build_logging_prefix("получено сообщение");

				event_invoke(message_received)(socket, msg);
			}

			if (storage.can_write(socket)) {
				std::string msg;

				messages_to_send_locker.lock();

				if (!messages_to_send.empty()) {
					LOG(NETWORK) << "ќтправл€ю...\n";

					msg = messages_to_send.front();
					messages_to_send.pop();

					messages_to_send_locker.unlock();

					send_message(msg);
					networking_storage::storage().update();

					LOG(NETWORK) << build_logging_prefix("отправлено сообщение");
				}
				else {
					messages_to_send_locker.unlock();
				}
			}
		}
		catch (const std::runtime_error& error) {
			LOG(NETWORK) << build_logging_prefix("ошибка") << error.what() << "\n";

			cancellation_flag = true;

			event_invoke(error_occured)(const_cast<client_networking_base*>(this), false);
		}
	}

protected:
	std::queue<std::string> messages_to_send;
	std::mutex messages_to_send_locker;
};

FARCONN_NAMESPACE_END