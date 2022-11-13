#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <shared_mutex>
#include "../tools/macro.hpp"
#include "networking_execution.hpp"

#pragma comment(lib, "Ws2_32.lib")

FARCONN_NAMESPACE_BEGIN(general)

#define BUILD_ERROR_MESSAGE(message) (std::ostringstream() << message << "\t���: " << WSAGetLastError() << "\n").str()

class networking_storage : public networking_execution {
public:
	networking_storage();

	void add_socket(SOCKET);
	void remove_socket(SOCKET);

	bool can_read(SOCKET);
	bool can_write(SOCKET);

	~networking_storage();

protected:
	void working_context() override;

private:
	bool can_do(SOCKET, std::map<UINT, WSAPOLLFD>&, SHORT);
	void update_descs(std::map<UINT, WSAPOLLFD>&);

private:
	std::map<UINT, WSAPOLLFD> descs_to_read;
	std::map<UINT, WSAPOLLFD> descs_to_write;
	std::shared_mutex descs_events_mutex;
};

FARCONN_NAMESPACE_END