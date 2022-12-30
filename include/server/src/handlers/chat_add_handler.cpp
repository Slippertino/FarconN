#pragma once

#include "../../middleware/handlers/chat_add_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_add_handler::chat_add_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_add_handler::is_command_valid() {
	return !in->options.size() && in->params.size() == 3;
}

void chat_add_handler::execute() {
	auto& chat_id = in->params[1];
	auto& user_login = in->params[2];

	auto is_part = db->is_user_chat_participant(session->native_token, chat_id);
	SERVER_ASSERT_EX(out, is_part != SUCCESS, is_part)

	chat_info info;
	SERVER_ASSERT(out, db->get_chat_info(session->native_token, chat_id, info) != SUCCESS)
	SERVER_ASSERT_EX(
		out,
		chat_name_type_mapper.at(info.type) == chat_type::PRIVATE,
		server_status_code::CHAT__IMPOSSIBLE_ADD_TO_PRIVATE_CHAT_ERROR
	)
	SERVER_ASSERT_EX(
		out, 
		info.size >= max_chat_participants_count, 
		server_status_code::CHAT__CHAT_PARTICIPANTS_LIMIT_EXCEEDED_ERROR
	)

	users_relations_type rels;
	SERVER_ASSERT(out, db->get_users_relations(session->login, user_login, rels) != SUCCESS)
	SERVER_ASSERT_EX(out, rels != users_relations_type::CONTACTS, server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR)

	std::string user_id;
	SERVER_ASSERT(out, db->get_user_id_by_login(user_login, user_id) != SUCCESS)

	is_part = db->is_user_chat_participant(user_id, chat_id);
	SERVER_ASSERT_EX(out, is_part == SUCCESS, server_status_code::CHAT__USER_ALREADY_IN_CHAT)

	SERVER_ASSERT_EX(out, true, db->add_user_to_chat(chat_id, user_id))
}

const uint32_t chat_add_handler::max_chat_participants_count = 65536;

FARCONN_NAMESPACE_END