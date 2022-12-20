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

	users_relations_type rels;
	std::string user_id;

	SERVER_ASSERT(out, db->get_users_relations(session->login, user_login, rels) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, rels != users_relations_type::CONTACTS, server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR)
	SERVER_ASSERT(out, db->get_user_id_by_login(user_login, user_id) != server_status_code::SYS__OKEY)

	auto code = db->is_chat_id_valid(chat_id, session->native_token, user_id);
	SERVER_ASSERT_EX(out, code != server_status_code::SYS__OKEY, code)

	SERVER_ASSERT_EX(out, true, db->add_user_to_chat(chat_id, user_id))
}

FARCONN_NAMESPACE_END