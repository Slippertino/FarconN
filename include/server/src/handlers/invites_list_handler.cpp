#pragma once

#include "../../middleware/handlers/invites_list_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

invites_list_handler::invites_list_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	handler(sm, ce, cr, true)
{ }

bool invites_list_handler::is_command_valid() {
	auto& opts = in->options;
	auto opts_count = opts.size();

	bool result = true;

	result &= in->params.size() == (1 + opts_count * 2);
	result &= opts_count == 1 || opts_count == 2;

	for (auto& opt : opts) {
		result &= available_options_descs.find(opt.second) != available_options_descs.end();

		if (!result) {
			break;
		}
	}

	return result;
}

void invites_list_handler::execute() {
	auto& options = in->options;
	auto& params = in->params;

	invites_selection selection;
	selection.login = session->login;

	ex_invitations_info info;
	for (auto& opt : options) {
		auto desc = available_options_descs.at(opt.second);
		auto& offsetStr = params[2 * opt.first + 1];
		auto& limitStr = params[2 * opt.first + 2];

		selection.invitation_type_name = opt.second;
		selection.filtration_column_name = desc.first;
		selection.selection_column_name = desc.second;

		SERVER_ASSERT_EX(
			out,
			try_convert_to_offset(params[2 * opt.first + 1], &selection) != SUCCESS,
			server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR
		)

		SERVER_ASSERT_EX(
			out,
			try_convert_to_limit(params[2 * opt.first + 2], &selection) != SUCCESS,
			server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR
		)

		auto code = db->get_invites_list(
			selection,
			info
		);

		SERVER_ASSERT_EX(out, code != SUCCESS, code)
	}

	nlohmann::json js;
	nlohmann::to_json(js, info);
	out->params.push_back(js.dump());

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

const std::unordered_map<std::string, std::pair<std::string, std::string>> invites_list_handler::available_options_descs = {
	{ "incoming",  { "u_to",   "u_from" } },
	{ "outcoming", { "u_from", "u_to"   } },
};

FARCONN_NAMESPACE_END