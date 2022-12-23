#pragma once

#include "chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_leave_handler final : public chat_handler {
public:
	chat_leave_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;
};

FARCONN_NAMESPACE_END