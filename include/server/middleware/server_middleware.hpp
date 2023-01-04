#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../db/db_responder.hpp"
#include "../general/protocol/status_code_interpreter.hpp"
#include "../general/validators/format_validators.hpp"
#include "../tools/token_generator.hpp"
#include "entities/entities.hpp"
#include "protocol/command_entities.hpp"
#include "handlers/handlers.hpp"

FARCONN_NAMESPACE_BEGIN(server)

#define SERVER_ASSERT(out, condition) if(condition) { out->status = status_code_interpreter::interpret(server_status_code::SYS__INTERNAL_SERVER_ERROR); return; }
#define SERVER_ASSERT_EX(out, condition, error_t) if(condition) { out->status = status_code_interpreter::interpret(error_t); return; }

#define SUCCESS server_status_code::SYS__OKEY

#define DECLARE_HANDLER(handler) friend class handler;

class server_middleware {
	DECLARE_HANDLER(handler)
	DECLARE_HANDLER(signup_handler)
	DECLARE_HANDLER(login_handler)
	DECLARE_HANDLER(logout_handler)
	DECLARE_HANDLER(profile_get_handler)
	DECLARE_HANDLER(profile_set_handler)
	DECLARE_HANDLER(invite_handler)
	DECLARE_HANDLER(invites_list_handler)
	DECLARE_HANDLER(contacts_list_handler)
	DECLARE_HANDLER(contact_delete_handler)
	DECLARE_HANDLER(search_handler)
	DECLARE_HANDLER(chat_create_handler)
	DECLARE_HANDLER(chat_add_handler)
	DECLARE_HANDLER(chat_leave_handler)
	DECLARE_HANDLER(chat_post_handler)
	DECLARE_HANDLER(chat_messages_list_handler)
	DECLARE_HANDLER(chat_list_handler)
	DECLARE_HANDLER(chat_party_list_handler)

public:
	server_middleware() = default;

	void setup(const database_config&, std::string);
	void refresh();

	void handle(const command_entity*, command_response*);

private:
	static const std::unordered_map<std::string, std::function<void(server_middleware*, const command_entity*, command_response*)>> command_handlers;

	std::unordered_map<std::string, client_session> sessions;
	std::mutex sessions_mutex;
	db_responder database;

	std::mutex users_locker;
};

FARCONN_NAMESPACE_END