#include "../network/client_networking_proxy.hpp"
#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

client_networking_proxy::client_networking_proxy(server* serv, SOCKET sock) : farconn::general::client_networking_base(sock), main(serv)
{ }

void client_networking_proxy::store_message(const std::string& msg) {
	main->push_command(socket, msg);
}

client_networking_proxy::~client_networking_proxy() {
	if (socket != INVALID_SOCKET) {
		shutdown(socket, SD_BOTH);
	}
}

FARCONN_NAMESPACE_END