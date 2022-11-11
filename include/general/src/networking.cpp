#include "../network/networking.hpp"

FARCONN_NAMESPACE_BEGIN(general)

/* 5 КБ = 5 * 1024Б */
const uint32_t networking::buffer_size = 5120;

bool networking::is_networking_setup = false;

networking::networking() {
	WSADATA wsData;

	if (!is_networking_setup) {
		auto erState = WSAStartup(MAKEWORD(2, 2), &wsData);

		if (!erState) {
			throw std::runtime_error("Ошибка инициализации сокетных интерфейсов!");
		}

		is_networking_setup = true;
	}
}

networking::networking(SOCKET sock) : networking(), socket(sock) 
{ }

void networking::send_message(const std::string& msg) {
	int sent_bytes_count = 0;

	do {
		auto cur = send(socket, &msg[sent_bytes_count], msg.size() - sent_bytes_count, 0);

		switch (cur) {

		case 0:
			return;

		case SOCKET_ERROR:
			throw std::runtime_error(build_error_message("Ошибка при отправке данных!"));

		default:
			break;
		}

		sent_bytes_count += cur;

	} while (sent_bytes_count != msg.size());
}

void networking::receive_message(std::string& msg) {
	std::string buffer(buffer_size);
	int received_bytes_count;

	do {
		received_bytes_count = recv(socket, buffer.data(), buffer.size(), 0);

		switch (received_bytes_count) {

		case 0:
			return;

		case SOCKET_ERROR:
			throw std::runtime_error(build_error_message("Ошибка при получении данных!"));

		default:
			break;
		}

		msg += buffer;
	} while (received_bytes_count == buffer_size);
}

networking::~networking() {
	closesocket(socket);
}

std::string networking::build_error_message(const std::string& general) {
	std::ostringstream result;
	result << general << "\tКод: " << WSAGetLastError() << "\n";
	return result.str();
}

sockaddr_in networking::create_sockaddr_in(const std::string& ipv4, uint16_t port) {
	in_addr ip_to_num;

	if (inet_pton(AF_INET, ipv4.c_str(), &ip_to_num) <= 0) {
		throw std::runtime_error("Ошибке при переводе эндпоинта из формата представления в формат работы с сетью!");
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_port = htons(port);
	servInfo.sin_addr = ip_to_num;

	return servInfo;
}

FARCONN_NAMESPACE_END