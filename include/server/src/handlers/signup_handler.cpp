#pragma once

#include "../../middleware/handlers/signup_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

signup_handler::signup_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : handler(sm, ce, cr, false)
{ }

bool signup_handler::is_command_valid() {
	return in->params.size() == 2;
}

void signup_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	auto& login = params[0];
	auto& pass = params[1];

	SERVER_ASSERT_EX(out, !user_login_format_validator::is_valid(login), server_status_code::SIGNUP__INVALID_LOGIN_ERROR)
	SERVER_ASSERT_EX(out, !user_password_format_validator::is_valid(pass), server_status_code::SIGNUP__INVALID_PASSWORD_ERROR)

	std::lock_guard<std::mutex> locker(main->users_locker);

	SERVER_ASSERT_EX(out, true, db->signup_user(login, pass))
}

FARCONN_NAMESPACE_END