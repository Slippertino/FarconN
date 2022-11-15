#pragma once

#include <string>
#include <vector>
#include "../../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class query_ranking_estimator {
public:
	static double estimate_query_ranking(const std::string&, const std::string&);

private:
	static const double penalty_for_mismatch;
	static const double penalty_for_gap;
};

FARCONN_NAMESPACE_END