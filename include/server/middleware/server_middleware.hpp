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
//#include "../../general/protocol/composite_parameter_builder.hpp"
#include "protocol/command_entities.hpp"

FARCONN_NAMESPACE_BEGIN(server)

#define SERVER_ASSERT(out, condition, error_t) if(condition) { out->status = status_code_interpreter::interpret(error_t); return; }

class server_middleware {
public:
	using arguments_t = std::vector<std::string>;

public:
	server_middleware() = delete;
	server_middleware(const database_config&, std::string);

	void setup();

	/* Handlers */

	void handle_echo(const command_entity*, command_response*);

	void handle_signup(const command_entity*, command_response*);

	void handle_login(const command_entity*, command_response*);

	void handle_logout(const command_entity*, command_response*);

	void handle_profile_get(const command_entity*, command_response*);

	void handle_profile_set(const command_entity*, command_response*);

private:
	std::unordered_map<std::string, client_session> sessions;
	std::mutex sessions_mutex;
	db_responder database;

	std::mutex users_locker;
};

FARCONN_NAMESPACE_END