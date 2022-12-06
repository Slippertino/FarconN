#pragma once

#include "../../middleware/handlers/profile_set_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

profile_set_handler::profile_set_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : profile_handler(sm, ce, cr)
{ }

bool profile_set_handler::is_command_valid() {
	return in->options.size() + 1 == in->params.size();
}

void profile_set_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;
	auto& database = main->database;

	auto& session_token = params[0];

	SERVER_ASSERT_EX(out, main->sessions.find(session_token) == main->sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)

	auto& session = main->sessions[session_token];

	SERVER_ASSERT_EX(out, options.empty(), server_status_code::SYS__OKEY)

	for (auto& opt : options) {
		SERVER_ASSERT_EX(out, profile_fields.find(opt.second) == profile_fields.end(), server_status_code::SYS__INVALID_COMMAND_ERROR)
		SERVER_ASSERT_EX(out, !profile_fields[opt.second].validator(params[opt.first + 1]), server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR)

		profile_fields[opt.second].value = in->params[opt.first + 1];
	}

	out->status = status_code_interpreter::interpret(
		database.update_user_profile(session.native_token, profile_fields)
	);
}

FARCONN_NAMESPACE_END