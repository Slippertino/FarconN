#include "../middleware/server_middleware.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

server_middleware::server_middleware(const database_config& db_config, std::string files_storage_path) :
	database(db_config, files_storage_path) 
{ }

void server_middleware::setup() {
	database.setup();
}

void server_middleware::handle_echo(const command_entity* in, command_response* out) {
	out->params = std::move(in->params);
}

void server_middleware::handle_signup(const command_entity* in, command_response* out) {
	auto& params = in->params;

	SERVER_ASSERT(out, params.size() != 2, server_status_code::SYS__INVALID_COMMAND_ERROR)

	auto& login = params[0];
	auto& pass = params[1];

	SERVER_ASSERT(out, !login_format_validator::is_valid(login), server_status_code::SIGNUP__INVALID_LOGIN_ERROR)
	SERVER_ASSERT(out, !password_format_validator::is_valid(pass), server_status_code::SIGNUP__INVALID_PASSWORD_ERROR)

	std::lock_guard<std::mutex> locker(users_locker);

	out->status = status_code_interpreter::interpret(
		database.signup_user(login, pass)
	);
}

void server_middleware::handle_login(const command_entity* in, command_response* out) {
	auto& params = in->params;

	SERVER_ASSERT(out, params.size() != 2, server_status_code::SYS__INVALID_COMMAND_ERROR)

	auto& login = params[0];
	auto& pass  = params[1];

	SERVER_ASSERT(
		out, 
		!login_format_validator::is_valid(login) || !password_format_validator::is_valid(pass), 
		server_status_code::LOGIN__INVALID_DATA_ERROR
	)

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

void server_middleware::handle_logout(const command_entity* in, command_response* out) {
	auto& params = in->params;

	SERVER_ASSERT(out, params.size() != 1, server_status_code::SYS__INVALID_COMMAND_ERROR)

	auto& s_token = params[0];

	SERVER_ASSERT(out, sessions.find(s_token) == sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)

	auto& n_token = sessions[s_token].native_token;

	auto result = database.logout_user(n_token);
	out->status = status_code_interpreter::interpret(result);

	sessions.erase(s_token);
}

void server_middleware::handle_profile_get(const command_entity* in, command_response* out) {
	auto& params = in->params;

	SERVER_ASSERT(out, params.size() != 2, server_status_code::SYS__INVALID_COMMAND_ERROR)

	auto& session_token = params[0];

	SERVER_ASSERT(out, sessions.find(session_token) == sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)

	auto& user_login = params[1];

	auto& session = sessions[session_token];

	server_status_code code;
	users_relations_type rels;

	code = database.get_users_relations(session.login, user_login, rels);

	SERVER_ASSERT(out, code != server_status_code::SYS__OKEY, code)

	user_profile profile_data;
	code = database.get_user_profile_data(user_login, profile_data);

	SERVER_ASSERT(out, code != server_status_code::SYS__OKEY, code)

	out->status = status_code_interpreter::interpret(server_status_code::SYS__OKEY);

	profile_data.apply_filter(rels);

	for (auto& cur : profile_data.fields_order) {
		auto& field = profile_data.fields[cur.second];

		if (field.value.has_value()) {
			out->params.push_back(field.value.value());
		}
	}
}

void server_middleware::handle_profile_set(const command_entity* in, command_response* out) {
	auto& options = in->options;
	auto& params = in->params;

	SERVER_ASSERT(out, options.size() + 1 != params.size(), server_status_code::SYS__INVALID_COMMAND_ERROR)

	auto& session_token = params[0];

	SERVER_ASSERT(out, sessions.find(session_token) == sessions.end(), server_status_code::SYS__INVALID_TOKEN_ERROR)

	auto& session = sessions[session_token];

	SERVER_ASSERT(out, options.empty(), server_status_code::SYS__OKEY)

	user_profile profile;
	for (auto& opt : options) {
		SERVER_ASSERT(out, profile.fields.find(opt.second) == profile.fields.end(), server_status_code::SYS__INVALID_COMMAND_ERROR)
		SERVER_ASSERT(out, !profile.fields[opt.second].validator(params[opt.first + 1]), server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR)

		profile.fields[opt.second].value = in->params[opt.first + 1];
	}

	out->status = status_code_interpreter::interpret(
		database.update_user_profile(session.native_token, profile)
	);
}

FARCONN_NAMESPACE_END