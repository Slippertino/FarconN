#pragma once

#include "../../middleware/handlers/chat_messages_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_messages_list_handler::chat_messages_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_messages_list_handler::is_command_valid() {
	auto result = in->options.size() == 1;

	if (result) {
		auto& opt = in->options[0];

		result =
			result &&
			options_params_mapper.find(opt) != options_params_mapper.end() &&
			options_params_mapper.at(opt).second == in->params.size();
	}

	return result;
}

void chat_messages_list_handler::execute() {
	auto& opt = in->options[0];
	auto& params = in->params;

	ex_chat_messages_info info;
	chat_messages_selection selection;

	auto& user_id = session->native_token;
	auto& chat_id = in->params[1];

	auto is_part = db->is_user_chat_participant(user_id, chat_id);
	SERVER_ASSERT_EX(out, is_part != SUCCESS, is_part)

	selection.chat_id = chat_id;

	server_status_code code = options_params_mapper.at(opt).first(this, selection);
	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	std::vector<message_info> messages;
	code = db->get_messages_list(selection, messages);
	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	for(int i = messages.size() - 1; i >= 0; --i) {
		info.data.insert({
			info.data.size(),
			messages[i].to_external(
				&chat_messages_list_handler::convert_seconds_to_date,
				time_template
			)
		});
	}

	out->params.push_back(
		info.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

server_status_code chat_messages_list_handler::handle_any(chat_messages_selection& selection) {
	auto& params = in->params;

	if (try_convert_to_offset(params[2], &selection) != SUCCESS) {
		return server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR;
	}

	if (try_convert_to_limit(params[3], &selection) != SUCCESS) {
		return server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR;
	}

	return SUCCESS;
}

server_status_code chat_messages_list_handler::handle_new(chat_messages_selection& selection) {
	auto& params = in->params;

	selection.last_actual_message_id = params[2];

	return SUCCESS;
}

const std::unordered_map<std::string, std::pair<chat_messages_list_handler::option_handler, size_t>>
chat_messages_list_handler::options_params_mapper = {
	{ "any", { &chat_messages_list_handler::handle_any, 4 } },
	{ "new", { &chat_messages_list_handler::handle_new, 3 } },
};

const std::string chat_messages_list_handler::time_template = "%d/%m/%Y %H:%M:%S";

FARCONN_NAMESPACE_END