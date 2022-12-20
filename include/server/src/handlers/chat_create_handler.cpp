#pragma once

#include "../../middleware/handlers/chat_create_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_create_handler::chat_create_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_create_handler::is_command_valid() {
	auto res = in->options.size() == 1 && in->params.size() == 2;

	if (res) {
		res &= options_handlers.find(in->options[0]) != options_handlers.end();
	}

	return res;
}

void chat_create_handler::execute() {
	chat_creation_params params;
	params.type = in->options[0];

	std::unordered_set<std::string> chat_tokens;
	SERVER_ASSERT(out, db->get_chats_tokens(chat_tokens) != server_status_code::SYS__OKEY)

	params.id = token_generator::generate([&chat_tokens](const std::string& cur) {
		return chat_tokens.find(cur) == chat_tokens.end();
	});

	auto& id = session->native_token;

	uint32_t chats_count;
	SERVER_ASSERT(out, db->get_user_chats_count(id, chats_count) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, chats_count == max_chats_count_to_join, server_status_code::CHAT__JOINED_CHATS_LIMIT_EXCEEDED_ERROR)

	params.default_users_ids.push_back(id);

	options_handlers.at(params.type)(this, params);
}

void chat_create_handler::handle_private(chat_creation_params& params) {
	auto& user_login = in->params[1];

	params.size = 2;

	users_relations_type rels;
	std::string user_id;

	SERVER_ASSERT(out, db->get_users_relations(session->login, user_login, rels) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, rels != users_relations_type::CONTACTS, server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR)
	SERVER_ASSERT(out, db->get_user_id_by_login(user_login, user_id) != server_status_code::SYS__OKEY)

	uint32_t chats_count;
	SERVER_ASSERT(out, db->get_user_chats_count(user_id, chats_count) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, chats_count == max_chats_count_to_join, server_status_code::CHAT__JOINED_CHATS_LIMIT_EXCEEDED_ERROR)

	params.default_users_ids.push_back(user_id);

	SERVER_ASSERT_EX(out, true, db->create_chat(params))
}

void chat_create_handler::handle_public(chat_creation_params& params) {
	params.size = 1;

	auto& title = in->params[1];
	SERVER_ASSERT_EX(out, !chat_name_format_validator::is_valid(title), server_status_code::CHAT__INVALID_NAME_ERROR)

	params.title = title;

	SERVER_ASSERT_EX(out, true, db->create_chat(params))
}

const std::unordered_map<std::string, std::function<void(chat_create_handler*, chat_creation_params&)>> chat_create_handler::options_handlers = {
	{ "private", &chat_create_handler::handle_private },
	{ "public",  &chat_create_handler::handle_public  },
};

FARCONN_NAMESPACE_END