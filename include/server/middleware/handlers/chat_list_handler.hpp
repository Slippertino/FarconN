#pragma once

#include "chat_handler.hpp"
#include "selection_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_list_handler final : public chat_handler, 
								public selection_handler {
public:
	chat_list_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	static const std::string time_template;
};

FARCONN_NAMESPACE_END