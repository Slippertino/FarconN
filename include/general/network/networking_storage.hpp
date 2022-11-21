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
#include "../multithread_context/multithread_context.hpp"
#include <cpp_events/event.hpp>
#include "../logger/logger.hpp"

#pragma comment(lib, "Ws2_32.lib")

FARCONN_NAMESPACE_BEGIN(general)

#define BUILD_ERROR_MESSAGE(message) (std::ostringstream() << message << "\tКод: " << WSAGetLastError() << "\n").str()

class networking_storage : public multithread_context<networking_storage> {
public:
	static networking_storage& storage();

	void add_socket(SOCKET);
	void remove_socket(SOCKET);

	bool can_read(SOCKET);
	bool can_write(SOCKET);

protected:
	void setup_contexts() override;

private:
	networking_storage();

	bool can_do(SOCKET, std::map<UINT, WSAPOLLFD>&, SHORT);
	void update_descs(std::map<UINT, WSAPOLLFD>&);

	void working_context();

	~networking_storage();

private:
	std::map<UINT, WSAPOLLFD> descs_to_read;
	std::map<UINT, WSAPOLLFD> descs_to_write;
	std::shared_mutex descs_events_mutex;
};

FARCONN_NAMESPACE_END