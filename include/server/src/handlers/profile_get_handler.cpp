#pragma once

#include "../../middleware/handlers/profile_get_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

profile_get_handler::profile_get_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : 
	profile_handler(sm, ce, cr, true)
{ }

bool profile_get_handler::is_command_valid() {
	auto size = in->params.size();

	return size == 1 || size == 2;
}

void profile_get_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	ex_user_profile result;

	auto& user_id = (params.size() == 2) 
		? params[1] 
		: session->native_token;

	server_status_code code;
	users_relations_type rels;

	code = db->get_users_relations(session->native_token, user_id, rels);

	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	code = db->get_user_profile_data(user_id, profile_fields);

	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	apply_filter(rels);

	for (auto& cur : profile_fields_order) {
		auto& field = profile_fields[cur.second];

		if (field.value.has_value()) {
			result.data.insert({ field.name, field.value.value() });
		}
	}

	out->params.push_back(
		result.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END