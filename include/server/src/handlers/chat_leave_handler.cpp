#pragma once

#include "../../middleware/handlers/chat_leave_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_leave_handler::chat_leave_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_leave_handler::is_command_valid() {
	return in->params.size() == 2;
}

void chat_leave_handler::execute() {
	auto& user_id = session->native_token;
	auto& chat_id = in->params[1];

	auto is_part = db->is_user_chat_participant(user_id, chat_id);
	SERVER_ASSERT_EX(out, is_part != SUCCESS, is_part)

	SERVER_ASSERT_EX(out, true, db->exclude_user_from_chat(user_id, chat_id))
}

FARCONN_NAMESPACE_END