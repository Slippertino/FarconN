#include "../network/client_networking.hpp"

FARCONN_NAMESPACE_BEGIN(client)

bool client_networking::connect(const std::string& ipv4, uint16_t port) {
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

	auto addr = create_sockaddr_in(ipv4, port);

	auto result = ::connect(socket, (sockaddr*)&addr, sizeof(addr));

	if (result) {
		LOG() << build_logging_prefix("������") << BUILD_ERROR_MESSAGE("��������� ������� ������������ � �������!") << "\n";

		event_invoke(error_occured)(const_cast<client_networking*>(this));
	} else {
		LOG() << build_logging_prefix("����������� � ������� ���������");

		farconn::general::networking_storage::storage().add_socket(socket);
	}

	return !result;
}

void client_networking::run() {
	cancellation_flag = false;

	run_context_loop(&client_networking::working_context, mt_sleep_time(1));
}

FARCONN_NAMESPACE_END