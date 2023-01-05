#pragma once

#include "../../middleware/handlers/help_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

help_handler::help_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	handler(sm, ce, cr, false)
{ }

bool help_handler::is_command_valid() {
	auto& params = in->params;

	return 
		params.empty() || 
		main->command_handlers.find(params[0]) != main->command_handlers.end();
}

void help_handler::execute() {
	auto& params = in->params;

	auto& section_name = (params.empty())
		? default_section_name
		: params[0];

	ex_help_info result;

	auto code = db->copy_docs_section(section_name, result);
	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	out->params.push_back(
		result.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

const std::string help_handler::default_section_name = "common";

FARCONN_NAMESPACE_END