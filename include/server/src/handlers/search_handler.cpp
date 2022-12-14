#pragma once

#include "../../middleware/handlers/search_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

#undef min

const double search_handler::min_relevance_value = 0.3;

search_handler::search_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	handler(sm, ce, cr, true)
{ }

bool search_handler::is_command_valid() {
	return in->params.size() == 4;
}

server_status_code search_handler::update_searching_cache(const std::string& query) {
	auto& searching = session->searching;

	if (searching.query == query) {
		return SUCCESS;
	}

	searching.query = query;
	searching.results.clear();

	std::list<user_info> info;
	auto code = db->get_users_searching_list(session->native_token, info);

	if (code != SUCCESS) {
		return code;
	}

	std::unordered_map<std::string, double> info_relevance;
	for (auto& usr : info) {
		auto rel = std::max(
			query_ranking_estimator::estimate_query_ranking(usr.login, query),
			query_ranking_estimator::estimate_query_ranking(usr.name, query)
		);

		info_relevance.insert({ usr.id, rel });
	}

	auto it = std::remove_if(info.begin(), info.end(), [&](auto& usr) {
		return info_relevance[usr.id] < min_relevance_value;
	});

	info.erase(it, info.end());
	info.sort([&](const auto& usr1, const auto& usr2) {
		return info_relevance[usr1.id] > info_relevance[usr2.id];
	});

	auto i = 0;
	for (auto it = info.begin(); it != info.end(); ++it, ++i) {
		searching.results.insert({ i, *it });
	}

	return SUCCESS;
}

void search_handler::execute() {
	auto& params = in->params;
	searching_selection selection;

	ex_searching_results info;

	selection.query = params[1];

	SERVER_ASSERT_EX(
		out,
		try_convert_to_offset(params[2], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR
	)

	SERVER_ASSERT_EX(
		out,
		try_convert_to_limit(params[3], &selection) != SUCCESS,
		server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR
	)

	auto update_status = update_searching_cache(selection.query);

	SERVER_ASSERT_EX(out, update_status != SUCCESS, update_status)

	auto& searching = session->searching;

	auto i = selection.offset;
	while (i < searching.results.size() && (i - selection.offset) < selection.limit) {
		info.data.insert({ i, searching.results.at(i).to_external()});
		++i;
	}

	out->params.push_back(
		info.to_string()
	);

	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END