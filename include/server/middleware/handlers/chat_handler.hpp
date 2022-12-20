#pragma once

#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_handler : public handler {
public:
	chat_handler(class server_middleware*, const command_entity*, command_response*, bool);

protected:
	virtual bool is_command_valid() override = 0;
	virtual void execute() override = 0;

protected:
	static const std::unordered_map<std::string, chat_type> chat_name_type_mapper;
	static const size_t max_chats_count_to_join;
};

FARCONN_NAMESPACE_END