#pragma once

#include "../../middleware/handlers/profile_set_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

profile_set_handler::profile_set_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : profile_handler(sm, ce, cr, true)
{ }

bool profile_set_handler::is_command_valid() {
	auto& options = in->options;

	bool result = (options.size() + 1) == in->params.size();

	for (auto& opt : options) {
		result &= profile_fields.find(opt.second) == profile_fields.end();

		if (!result) {
			break;
		}
	}

	return result;
}

void profile_set_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	SERVER_ASSERT_EX(out, options.empty(), server_status_code::SYS__OKEY)

	for (auto& opt : options) {
		SERVER_ASSERT_EX(out, !profile_fields[opt.second].validator(params[opt.first + 1]), server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR)

		profile_fields[opt.second].value = in->params[opt.first + 1];
	}

	out->status = status_code_interpreter::interpret(
		db->update_user_profile(session->native_token, profile_fields)
	);

	auto& login = profile_fields["pl"].value;
	if (login != std::nullopt) {
		session->login = login.value();
	}
}

FARCONN_NAMESPACE_END