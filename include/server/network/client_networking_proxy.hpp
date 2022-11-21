#pragma once

#include "../../general/network/client_networking_base.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class client_networking_proxy : public client_networking_base<client_networking_proxy> {
public:
	client_networking_proxy() = delete;
	client_networking_proxy(SOCKET);

	~client_networking_proxy() = default;
};

FARCONN_NAMESPACE_END