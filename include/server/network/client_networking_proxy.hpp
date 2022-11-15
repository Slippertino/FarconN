#pragma once

#include "../../general/network/client_networking_base.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class client_networking_proxy : public client_networking_base {
public:
	client_networking_proxy() = delete;
	client_networking_proxy(class server*, SOCKET);

	void store_message(const std::string&) override;

	~client_networking_proxy();

private:
	class server* main;
};

FARCONN_NAMESPACE_END