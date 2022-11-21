#include "../network/client_networking_proxy.hpp"
#include "../server.hpp"

FARCONN_NAMESPACE_BEGIN(server)

client_networking_proxy::client_networking_proxy(SOCKET sock) : farconn::general::client_networking_base<client_networking_proxy>(sock)
{ }

FARCONN_NAMESPACE_END