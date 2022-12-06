#pragma once

#include "../../middleware/handlers/logout_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

logout_handler::logout_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : handler(sm, ce, cr)
{ }

bool logout_handler::is_command_valid() {
	return in->params.size() == 1;
}

void logout_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;
	auto& database = main->database;
	auto& sessions = main->sessions;

	auto& s_token = params[0];

	SERVER_ASSERT_EX(out, sessions.find(s_token) == sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)

	auto& n_token = sessions[s_token].native_token;

	auto result = database.logout_user(n_token);
	out->status = status_code_interpreter::interpret(result);

	sessions.erase(s_token);
}

FARCONN_NAMESPACE_END