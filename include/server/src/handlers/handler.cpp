#include "../../middleware/handlers/handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

handler::handler(server_middleware* sm, const command_entity* ce, command_response* cr, bool auth_required) :
	main(sm), 
	in(const_cast<command_entity*>(ce)), 
	out(cr),
	is_auth_required(auth_required),
	db(&sm->database)
{ }

server_status_code handler::try_authenticate_user() {
	auto& session_token = in->params[0];

	auto result = main->sessions.find(session_token) == main->sessions.end()
		? server_status_code::SYS__INVALID_TOKEN_ERROR
		: SUCCESS;

	session = &main->sessions[session_token];

	return result;
}

void handler::handle() {
	SERVER_ASSERT_EX(out, !is_command_valid(), server_status_code::SYS__INVALID_COMMAND_ERROR)

	if (is_auth_required) {
		auto code = try_authenticate_user();
		SERVER_ASSERT_EX(out, code != SUCCESS, code)
	}

	execute();
}

FARCONN_NAMESPACE_END
