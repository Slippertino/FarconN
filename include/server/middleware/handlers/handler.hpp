#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../../db/db_responder.hpp"
#include "../general/protocol/status_code_interpreter.hpp"
#include "../general/validators/format_validators.hpp"
#include "../../tools/token_generator.hpp"
#include "protocol/command_entities.hpp"
#include "../entities/entities.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class handler {
public:
	handler() = delete;
	handler(class server_middleware*, const command_entity*, command_response*, bool);

	virtual void handle();
	virtual void try_authenticate_user();

protected:
	virtual bool is_command_valid() = 0;
	virtual void execute() = 0;

protected:
	server_middleware* main;

	command_entity* in;
	command_response* out;

	bool is_auth_required;

	db_responder* db;
	client_session* session = nullptr;
};

FARCONN_NAMESPACE_END