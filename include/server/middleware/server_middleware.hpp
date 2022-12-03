#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "entities/client_session.hpp"
#include "../db/db_responder.hpp"
#include "../general/protocol/status_code_interpreter.hpp"
#include "../general/validators/format_validators.hpp"
#include "../tools/token_generator.hpp"
#include "entities/users_relations_type.hpp"
#include "entities/user_profile.hpp"
#include "protocol/command_entities.hpp"
#include "handlers/handlers.hpp"

FARCONN_NAMESPACE_BEGIN(server)

#define SERVER_ASSERT(out, condition, error_t) if(condition) { out->status = status_code_interpreter::interpret(error_t); return; }

#define DECLARE_HANDLER(handler) friend class handler;

class server_middleware {
	DECLARE_HANDLER(signup_handler)
	DECLARE_HANDLER(login_handler)
	DECLARE_HANDLER(logout_handler)
	DECLARE_HANDLER(profile_get_handler)
	DECLARE_HANDLER(profile_set_handler)

public:
	server_middleware() = delete;
	server_middleware(const database_config&, std::string);

	void setup();

	void handle(const command_entity*, command_response*);

private:
	static const std::unordered_map<std::string, std::function<void(server_middleware*, const command_entity*, command_response*)>> command_handlers;

	std::unordered_map<std::string, client_session> sessions;
	std::mutex sessions_mutex;
	db_responder database;

	std::mutex users_locker;
};

FARCONN_NAMESPACE_END