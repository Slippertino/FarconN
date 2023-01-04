#pragma once

#include "../../middleware/handlers/chats_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chats_list_handler::chats_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chats_list_handler::is_command_valid() {
	return !in->options.size() && in->params.size() == 3;
}

void chats_list_handler::execute() {
	auto& params = in->params;

	chats_selection selection;
	selection.user_id = session->native_token;

	SERVER_ASSERT_EX(
		out,
		try_convert_to_offset(params[1], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR
	)

	SERVER_ASSERT_EX(
		out,
		try_convert_to_limit(params[2], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR
	)

	server_status_code code;

	std::vector<std::string> chats_ids;
	SERVER_ASSERT(out, db->get_user_chats_tokens(selection, chats_ids) != SUCCESS)

	ex_chats_info result;
	for (auto i = 0; i < chats_ids.size(); ++i) {
		auto& id = chats_ids[i];

		chat_info chat;
		code = db->get_chat_info(selection.user_id, id, chat);
		SERVER_ASSERT_EX(out, code != SUCCESS, code)

		std::vector<message_info> last_message;
		code = db->get_messages_list(chat_messages_selection{
			0, 1, id, std::nullopt
		}, last_message);
		SERVER_ASSERT_EX(out, code != SUCCESS, code)

		auto chat_info = chat.to_external();
		if (!last_message.empty()) {
			chat_info.last_message = last_message[0].to_external(
				&chats_list_handler::convert_seconds_to_date,
				time_template
			);
		}

		result.data.insert({ 
			i, 
			std::move(chat_info) 
		});
	}

	out->params.push_back(
		result.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

const std::string chats_list_handler::time_template = "%d/%m/%Y %H:%M:%S";

FARCONN_NAMESPACE_END