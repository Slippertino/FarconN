#pragma once

#include "../../middleware/handlers/login_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

login_handler::login_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : handler(sm, ce, cr, false)
{ }

bool login_handler::is_command_valid() {
	return in->params.size() == 2;
}

void login_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	auto& login = params[0];
	auto& pass = params[1];

	SERVER_ASSERT_EX(
		out,
		!login_format_validator::is_valid(login) || !password_format_validator::is_valid(pass),
		server_status_code::LOGIN__INVALID_DATA_ERROR
	)

	client_session new_session;

	auto result = db->login_user(login, pass, new_session.native_token);
	out->status = status_code_interpreter::interpret(result);

	if (result == server_status_code::SYS__OKEY) {
		auto token = token_generator::generate([&](const std::string& str) {
			return main->sessions.find(str) == main->sessions.end();
		});

		out->params.push_back(token);

		new_session.session_token = token;
		new_session.login = login;

		main->sessions[token] = std::move(new_session);
	}
}

FARCONN_NAMESPACE_END