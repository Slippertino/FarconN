#pragma once

#include "chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_add_handler final : public chat_handler {
public:
	chat_add_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	static const uint32_t max_chat_participants_count;
};

FARCONN_NAMESPACE_END