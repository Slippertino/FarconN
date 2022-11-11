#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <sstream>
#include "../tools/macro.hpp"

#pragma comment(lib, "Ws2_32.lib")

FARCONN_NAMESPACE_BEGIN(general)

class networking {
public:
	networking();
	networking(SOCKET);

	void send_message(const std::string&);
	void receive_message(std::string&);

	virtual void run() = 0;

	~networking();

protected:
	sockaddr_in create_sockaddr_in(const std::string&, uint16_t);
	std::string build_error_message(const std::string&);

protected:
	static const uint32_t buffer_size;
	static bool is_networking_setup;
	SOCKET socket;
};

FARCONN_NAMESPACE_END