#pragma once

#include "../../middleware/handlers/chat_party_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_party_list_handler::chat_party_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_party_list_handler::is_command_valid() {
	return in->params.size() == 4;
}

void chat_party_list_handler::execute() {
	auto& params = in->params;

	chat_party_selection selection;
	selection.chat_id = params[1];
	selection.user_id = session->native_token;

	SERVER_ASSERT_EX(
		out,
		try_convert_to_offset(params[2], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR
	)

	SERVER_ASSERT_EX(
		out,
		try_convert_to_limit(params[3], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR
	)

	ex_chat_party_info info;
	SERVER_ASSERT(out, db->get_chat_party(selection, info) != SUCCESS)

	out->params.push_back(
		info.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END