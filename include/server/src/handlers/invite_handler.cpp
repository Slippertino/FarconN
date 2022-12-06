#pragma once

#include "../../middleware/handlers/invite_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

invite_handler::invite_handler(class server_middleware* sm, const command_entity* ce, command_response* cr) : 
	handler(sm, ce, cr), 
	database(sm->database)
{ }

bool invite_handler::is_command_valid() {
	auto& opts = in->options;

	return
		opts.size() == 1 &&
		option_handlers_map.find(opts[0]) != option_handlers_map.end() &&
		in->params.size() == 2;
}

void invite_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	auto& session_token = params[0];

	SERVER_ASSERT_EX(out, main->sessions.find(session_token) == main->sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)
	
	auto& session = main->sessions[session_token];

	option_handlers_map.at(options[0])(this, params[1], session.login);
}

void invite_handler::handle_accept(const std::string& lufrom, const std::string& luto) {
	bool is_request_exist = false;
	SERVER_ASSERT(out, database.is_request_exist(lufrom, luto, is_request_exist) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, !is_request_exist, server_status_code::REQUEST__NONEXISTEN_REQUEST_ERROR);

	SERVER_ASSERT(out, database.delete_request(lufrom, luto) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, true, database.create_contact(lufrom, luto))
}

void invite_handler::handle_reject(const std::string& lufrom, const std::string& luto) {
	bool is_request_exist = false;
	SERVER_ASSERT(out, database.is_request_exist(lufrom, luto, is_request_exist) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, !is_request_exist, server_status_code::REQUEST__NONEXISTEN_REQUEST_ERROR)

	SERVER_ASSERT_EX(out, true, database.delete_request(lufrom, luto))
}

void invite_handler::handle_create(const std::string& luto, const std::string& lufrom) {
	users_relations_type rels;
	SERVER_ASSERT(out, database.get_users_relations(lufrom, luto, rels) != server_status_code::SYS__OKEY)
	SERVER_ASSERT_EX(out, rels != users_relations_type::NONE, server_status_code::REQUEST__ALREADY_IN_CONTACTS_ERROR)

	SERVER_ASSERT_EX(out, true, database.create_request(lufrom, luto))
}

void invite_handler::handle_cancel(const std::string& luto, const std::string& lufrom) {
	handle_reject(luto, lufrom);
}

const std::unordered_map<std::string, invite_handler::option_handler> invite_handler::option_handlers_map = {
	{ "accept", &invite_handler::handle_accept },
	{ "reject", &invite_handler::handle_reject },
	{ "create", &invite_handler::handle_create },
	{ "cancel", &invite_handler::handle_cancel },
};

FARCONN_NAMESPACE_END