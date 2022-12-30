#pragma once

#include "../../middleware/handlers/chat_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_list_handler::chat_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_list_handler::is_command_valid() {
	return !in->options.size() && in->params.size() == 1;
}

void chat_list_handler::execute() {
	auto& user_id = session->native_token;
	server_status_code code;

	std::vector<std::string> chats;
	SERVER_ASSERT(out, db->get_user_chats_tokens(user_id, chats) != SUCCESS)

	ex_chats_info result;
	for (auto& id : chats) {
		internal_chat_info chat;
		code = db->get_chat_info(user_id, id, chat);
		SERVER_ASSERT_EX(out, code != SUCCESS, code)

		std::vector<internal_message_info> last_message;
		code = db->get_messages_list(chat_messages_selection{
			0, 1, id, std::nullopt
		}, last_message);
		SERVER_ASSERT_EX(out, code != SUCCESS, code)

		auto chat_info = chat.to_external();
		if (!last_message.empty()) {
			chat_info.last_message = last_message[0].to_external(
				&chat_list_handler::convert_seconds_to_date,
				time_template
			);
		}

		result.push_back(std::move(chat_info));
	}

	nlohmann::json js;
	nlohmann::to_json(js, result);
	out->params.push_back(js.dump());

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

const std::string chat_list_handler::time_template = "%d/%m/%Y %H:%M:%S";

FARCONN_NAMESPACE_END