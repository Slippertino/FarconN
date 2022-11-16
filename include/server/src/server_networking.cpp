#include "../network/server_networking.hpp"
#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

server_networking::server_networking(server* serv) : main(serv)
{ }

void server_networking::setup(const std::string& ipv4, uint16_t port) {
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

	auto addr = create_sockaddr_in(ipv4, port);

	auto res = bind(socket, (sockaddr*)&addr, sizeof(addr));

	if (!res) {
		throw std::runtime_error(
			BUILD_ERROR_MESSAGE("Ошибка при попытке привязки сокета к локальному адресу!")
		);
	}

	storage.add_socket(socket);
}

void server_networking::setup_contexts() { 
	add_context(&server_networking::working_context, mt_sleep_time(1));
}

void server_networking::run() {
	auto res = listen(socket, SOMAXCONN);

	if (!res) {
		throw std::runtime_error(
			BUILD_ERROR_MESSAGE("Ошибка при попытке привязки сокета к локальному адресу!")
		);
	}

	run_context_loop(&server_networking::working_context, mt_sleep_time(1));
}

void server_networking::stop() {
	multithread_context<server_networking>::stop();

	if (socket != INVALID_SOCKET) {
		shutdown(socket, SD_BOTH);
	}
}

void server_networking::working_context() {
	try {
		if (storage.can_read(socket)) {
			sockaddr_in client_info;
			ZeroMemory(&client_info, sizeof(client_info));

			int client_size = sizeof(client_info);

			auto client_sock = ::accept(socket, (sockaddr*)&client_info, &client_size);

			if (client_sock != INVALID_SOCKET) {
				main->add_client(client_sock);
			}
		}
	} catch (const std::runtime_error& error) {
		cancellation_flag = true;
		throw error;
	}
}

server_networking::~server_networking() {
	stop();

	if (socket != INVALID_SOCKET) {
		shutdown(socket, SD_BOTH);
	}
}

FARCONN_NAMESPACE_END