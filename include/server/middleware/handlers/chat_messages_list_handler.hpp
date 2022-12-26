#pragma once

#include "chat_handler.hpp"
#include "selection_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_messages_list_handler final : public chat_handler, 
										 public selection_handler {
public:
	chat_messages_list_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	time_t convert_seconds_to_time_t(double);
	std::string convert_seconds_to_date(double);

	server_status_code handle_any(chat_messages_selection&);
	server_status_code handle_new(chat_messages_selection&);

private:
	using option_handler = std::function<server_status_code(chat_messages_list_handler*, chat_messages_selection&)>;

	static const std::unordered_map<std::string, std::pair<option_handler, size_t>> options_params_mapper;
};

FARCONN_NAMESPACE_END