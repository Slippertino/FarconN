#pragma once

#include "../../general/network/client_networking_base.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(client)

class client_networking : public farconn::general::client_networking_base<client_networking>{
public:
	client_networking() = default;

	bool connect(const std::string&, uint16_t);

	void run() override;

	~client_networking() = default;
};

FARCONN_NAMESPACE_END