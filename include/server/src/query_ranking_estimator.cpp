#include "../tools/query_ranking_estimator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const double query_ranking_estimator::penalty_for_mismatch = 20;
const double query_ranking_estimator::penalty_for_gap = 20;

double query_ranking_estimator::estimate_query_ranking(const std::string& sample, const std::string& query) {
	auto n = sample.size();
	auto m = query.size();

	std::vector<std::vector<double>> buffer(n, std::vector<double>(m));

	for (auto i = 0; i < buffer.size(); ++i) {
		buffer[i][0] = penalty_for_gap * i;
	}

	for (auto i = 0; i < buffer[0].size(); ++i) {
		buffer[0][i] = penalty_for_gap * i;
	}

	for (auto i = 1; i < buffer.size(); ++i) {
		for (auto j = 1; j < buffer[i].size(); ++j) {
			buffer[i][j] = std::min(
				buffer[i - 1][j - 1] + get_mismatch_penalty_value(sample[i], query[j]),
				std::min(
					buffer[i - 1][j] + penalty_for_gap, 
					buffer[i][j - 1] + penalty_for_gap
				)
			);
		}
	}

	auto raw_ranking = buffer[n - 1][m - 1];

	return translate_ranking(query, raw_ranking);
}

double query_ranking_estimator::get_mismatch_penalty_value(char ch1, char ch2) {
	return (ch1 == ch2) ? 0 : penalty_for_mismatch;
}

double query_ranking_estimator::translate_ranking(const std::string& query, double raw_ranking) {
	auto min_penalty = std::min(penalty_for_gap, penalty_for_mismatch);
	auto max_query_penalty = min_penalty * query.size();

	auto result = (max_query_penalty - raw_ranking) / max_query_penalty;

	if (result < 0) {
		result = 0;
	}

	return result;
}

FARCONN_NAMESPACE_END