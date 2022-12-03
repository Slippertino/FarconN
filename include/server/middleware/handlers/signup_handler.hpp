#pragma once

#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class signup_handler final : public handler {
public:
	signup_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;
};

FARCONN_NAMESPACE_END