#include "../middleware/server_middleware.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

server_middleware::server_middleware(const database_config& db_config, std::string files_storage_path) :
	database(db_config, files_storage_path) 
{ }

void server_middleware::setup() {
	database.setup();
}

void server_middleware::handle_echo(const command_request* in, command_response* out) {
	out->params = std::move(in->params);
}

void server_middleware::handle_signup(const command_request* in, command_response* out) {
	auto& params = in->params;

	if (params.size() != 2) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND);
		return;
	}

	auto& login = params[0];
	auto& pass = params[1];

	if (!login_format_validator::is_valid(login)) {
		out->status = status_code_interpreter::interpret(server_status_code::SIGNUP__INVALID_LOGIN_ERROR);
	}
	else if (!password_format_validator::is_valid(pass)) {
		out->status = status_code_interpreter::interpret(server_status_code::SIGNUP__INVALID_PASSWORD_ERROR);
	}
	else {
		std::lock_guard<std::mutex> locker(users_locker);

		out->status = status_code_interpreter::interpret(
			database.signup_user(login, pass)
		);
	}
}

void server_middleware::handle_login(const command_request* in, command_response* out) {
	auto& params = in->params;

	if (params.size() != 2) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND);
		return;
	}

	auto& login = params[0];
	auto& pass  = params[1];

	if (!login_format_validator::is_valid(login) || 
		!password_format_validator::is_valid(pass)) {
		out->status = status_code_interpreter::interpret(server_status_code::LOGIN__INVALID_DATA_ERROR);
	}
	else {
		client_session new_session;

		auto result = database.login_user(login, pass, new_session.native_token);
		out->status = status_code_interpreter::interpret(result);

		if (result == server_status_code::SYS__OKEY) {
			auto token = token_generator::generate([&](const std::string& str) {
				return sessions.find(str) == sessions.end();
			});

			out->params.push_back(token);

			new_session.login = login;

			sessions[token] = std::move(new_session);
		}
	}
}

void server_middleware::handle_logout(const command_request* in, command_response* out) {
	auto& params = in->params;

	if (params.size() != 1) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND);
		return;
	}

	auto& s_token = params[0];

	if (sessions.find(s_token) == sessions.end()) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_TOKEN);
		return;
	}

	auto& n_token = sessions[s_token].native_token;

	auto result = database.logout_user(n_token);
	out->status = status_code_interpreter::interpret(result);

	sessions.erase(s_token);
}

void server_middleware::handle_profile_get(const command_request* in, command_response* out) {
	auto& params = in->params;

	if (params.size() != 2) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_COMMAND);
		return;
	}

	auto& session_token = params[0];
	auto& user_login = params[1];

	if (sessions.find(session_token) == sessions.end()) {
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INVALID_TOKEN);
		return;
	}

	auto& session = sessions[session_token];

	server_status_code code;
	users_relations_type rels;

	code = database.get_users_relations(session.login, user_login, rels);

	if (code != server_status_code::SYS__OKEY) {
		out->status = status_code_interpreter::interpret(code);
		return;
	}

	std::vector<std::string> profile_data;
	code = database.get_user_profile_data(user_login, profile_data);

	if (code != server_status_code::SYS__OKEY) {
		out->status = status_code_interpreter::interpret(code);
		return;
	}

	out->status = status_code_interpreter::interpret(server_status_code::SYS__OKEY);

	switch (rels) {
	case users_relations_type::SELF:
		break;
	case users_relations_type::CONTACTS:
		profile_data[1].clear();
		break;
	case users_relations_type::NONE:
		profile_data[1].clear();
		profile_data[3].clear();
		profile_data[4].clear();
		profile_data[5].clear();
		break;
	default:
		out->status = status_code_interpreter::interpret(server_status_code::SYS__INTERNAL_SERVER_ERROR);
		return;
	}

	out->params.insert(out->params.end(), profile_data.begin(), profile_data.end());
}

void server_middleware::handle_profile_set(const command_request* in, command_response* out) {
	out->status = status_code_interpreter::interpret(server_status_code::SYS__INTERNAL_SERVER_ERROR);
}

FARCONN_NAMESPACE_END