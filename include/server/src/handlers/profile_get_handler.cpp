#pragma once

#include "../../middleware/handlers/profile_get_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

profile_get_handler::profile_get_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : profile_handler(sm, ce, cr, true)
{ }

bool profile_get_handler::is_command_valid() {
	return in->params.size() == 2;
}

void profile_get_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	auto& user_login = params[1];

	server_status_code code;
	users_relations_type rels;

	code = db->get_users_relations(session->login, user_login, rels);

	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	code = db->get_user_profile_data(user_login, profile_fields);

	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	apply_filter(rels);

	for (auto& cur : profile_fields_order) {
		auto& field = profile_fields[cur.second];

		if (field.value.has_value()) {
			out->params.push_back(field.value.value());
		}
	}

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END