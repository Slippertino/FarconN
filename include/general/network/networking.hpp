#pragma once

#include <cstdint>
#include <string>
#include "networking_storage.hpp"
#include <cpp_events/event.hpp>
#include "../tools/macro.hpp"
#include "../logger/logger.hpp"

#pragma comment(lib, "Ws2_32.lib")

FARCONN_NAMESPACE_BEGIN(general)

class networking {
public:
	networking();
	networking(SOCKET);

	SOCKET get_socket_desc() const;

	virtual ~networking();

protected:
	void send_message(const std::string&);
	void receive_message(std::string&);

	sockaddr_in create_sockaddr_in(const std::string&, uint16_t);

protected:
	static const uint32_t buffer_size;

	SOCKET socket;
};

FARCONN_NAMESPACE_END