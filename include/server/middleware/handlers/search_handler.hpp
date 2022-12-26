#pragma once

#include <algorithm>
#include "selection_handler.hpp"
#include "../../tools/query_ranking_estimator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class search_handler final : public handler, 
							 public selection_handler {
public:
	search_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	server_status_code update_searching_cache(const std::string&);

private:
	static const double min_relevance_value;
};

FARCONN_NAMESPACE_END