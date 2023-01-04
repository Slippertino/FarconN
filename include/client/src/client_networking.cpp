#include "../network/client_networking.hpp"

FARCONN_NAMESPACE_BEGIN(client)

bool client_networking::connect(const std::string& ipv4, uint16_t port) {
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

	auto addr = create_sockaddr_in(ipv4, port);

	auto result = ::connect(socket, (sockaddr*)&addr, sizeof(addr));

	if (result) {
		LOG(NETWORK) << build_logging_prefix("ошибка") << BUILD_ERROR_MESSAGE("Неудачная попытка подключиться к серверу!") << "\n";

		event_invoke(error_occured)(const_cast<client_networking*>(this));
	} else {
		LOG(NETWORK) << build_logging_prefix("подключение к серверу завершено");

		farconn::general::networking_storage::storage().add_socket(socket);
	}

	return !result;
}

FARCONN_NAMESPACE_END