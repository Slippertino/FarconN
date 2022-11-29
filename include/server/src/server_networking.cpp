#include "../network/server_networking.hpp"
#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

std::string server_networking::build_logging_prefix(const std::string& event) const {
	return (std::ostringstream() << "Server socket #" << socket << " : " << event << ". ").str();
}

void server_networking::setup(const std::string& ipv4, uint16_t port) {
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;

	try {
		addr = create_sockaddr_in(ipv4, port);
	}
	catch (const std::exception& ex) {
		LOG() << build_logging_prefix("ошибка") << BUILD_ERROR_MESSAGE(ex.what()) << "\n";

		event_invoke(error_occured)(const_cast<server_networking*>(this));

		return;
	}

	auto res = bind(socket, (sockaddr*)&addr, sizeof(addr));

	if (res) {

		LOG() 
			<< build_logging_prefix("ошибка") 
			<< BUILD_ERROR_MESSAGE("Ошибка при попытке привязки сокета к локальному адресу!") << "\n";

		event_invoke(error_occured)(const_cast<server_networking*>(this));

		return;
	}
	else {
		LOG() << build_logging_prefix("привязка сокета к локальному адресу удалась") << "\n";
	}

	networking_storage::storage().add_socket(socket);
}

void server_networking::setup_contexts() { }

void server_networking::run() {
	auto res = listen(socket, SOMAXCONN);

	if (res) {
		LOG()
			<< build_logging_prefix("ошибка")
			<< BUILD_ERROR_MESSAGE("Ошибка при попытке сокета начать прослушивание входящих соединений!") << "\n";

		event_invoke(error_occured)(const_cast<server_networking*>(this));

		return;
	}
	else {
		LOG() << build_logging_prefix("начато прослушивание входящих соединений") << "\n";
	}

	cancellation_flag = false;

	run_context_loop(&server_networking::working_context, mt_sleep_time(1));
}

void server_networking::stop() {
	multithread_context<server_networking>::stop();

	if (socket != INVALID_SOCKET) {
		LOG() << build_logging_prefix("остановка...") << "\n";
		shutdown(socket, SD_BOTH);
	}
}

void server_networking::working_context() {
	try {
		if (networking_storage::storage().can_read(socket)) {
			sockaddr_in client_info;
			ZeroMemory(&client_info, sizeof(client_info));

			int client_size = sizeof(client_info);

			auto client_sock = ::accept(socket, (sockaddr*)&client_info, &client_size);

			if (client_sock != INVALID_SOCKET) {
				LOG() << build_logging_prefix("новый клиент") << "Сокет: " << client_sock << "\n";

				event_invoke(connection_incoming)(client_sock);
			} else {
				LOG() << build_logging_prefix("неудачное подключение клиента") << "\n";
			}
		}
	} catch (const std::runtime_error& error) {
		LOG() << build_logging_prefix("ошибка") << error.what() << "\n";

		cancellation_flag = true;

		event_invoke(error_occured)(const_cast<server_networking*>(this), false);
	}
}

server_networking::~server_networking() {
	multithread_context<server_networking>::stop();

	if (socket != INVALID_SOCKET) {
		LOG() << build_logging_prefix("уничтожение...") << "\n";
		shutdown(socket, SD_BOTH);
	}
}

FARCONN_NAMESPACE_END