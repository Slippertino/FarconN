#pragma once

#include "../../middleware/handlers/logout_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

logout_handler::logout_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : handler(sm, ce, cr, true)
{ }

bool logout_handler::is_command_valid() {
	return in->params.size() == 1;
}

void logout_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	auto& n_token = session->native_token;

	auto result = db->logout_user(n_token);
	out->status = status_code_interpreter::interpret(result);

	std::lock_guard<std::mutex> locker(main->sessions_mutex);

	main->sessions.erase(session->session_token);
}

FARCONN_NAMESPACE_END