#pragma once

#include <string>
#include <vector>
#include "../../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class query_ranking_estimator {
public:
	static double estimate_query_ranking(const std::string&, const std::string&);

private:
	static double get_mismatch_penalty_value(char, char);
	static double translate_ranking(const std::string&, double);

private:
	static const double penalty_for_mismatch;
	static const double penalty_for_gap;
};

FARCONN_NAMESPACE_END