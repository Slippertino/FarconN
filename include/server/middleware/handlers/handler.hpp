#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../entities/client_session.hpp"
#include "../../db/db_responder.hpp"
#include "../general/protocol/status_code_interpreter.hpp"
#include "../general/validators/format_validators.hpp"
#include "../../tools/token_generator.hpp"
#include "../entities/users_relations_type.hpp"
#include "../entities/user_profile.hpp"
#include "protocol/command_entities.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class handler {
public:
	handler() = delete;
	handler(class server_middleware*, const command_entity*, command_response*);

	virtual void handle();

protected:
	virtual bool is_command_valid() = 0;
	virtual void execute() = 0;

protected:
	server_middleware* main;
	command_entity* in;
	command_response* out;
};

FARCONN_NAMESPACE_END