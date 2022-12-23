#pragma once

#include "../../middleware/handlers/contacts_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

contacts_list_handler::contacts_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	selection_handler(sm, ce, cr, true)
{ }

bool contacts_list_handler::is_command_valid() {
	return in->options.empty() && in->params.size() == 3;
}

void contacts_list_handler::execute() {
	auto& params = in->params;

	contacts_selection selection;
	selection.login = session->login;

	contacts_info info;

	SERVER_ASSERT_EX(
		out, 
		try_convert_to_offset(params[1], &selection) != SUCCESS, 
		server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR
	)

	SERVER_ASSERT_EX(
		out, 
		try_convert_to_limit(params[2], &selection) != SUCCESS, 
		server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR
	)

	auto code = db->get_contacts_list(
		selection,
		info
	);

	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	nlohmann::json js;
	nlohmann::to_json(js, info);
	out->params.push_back(js.dump());

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END