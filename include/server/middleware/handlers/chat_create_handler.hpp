#pragma once

#include "chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_create_handler final : public chat_handler {
public:
	chat_create_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	void handle_private(chat_creation_params&);
	void handle_public(chat_creation_params&);

private:
	static const std::unordered_map<std::string, std::function<void(chat_create_handler*, chat_creation_params&)>> options_handlers;
};

FARCONN_NAMESPACE_END