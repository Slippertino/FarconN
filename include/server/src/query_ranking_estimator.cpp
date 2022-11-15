#include "../tools/query_ranking_estimator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const double query_ranking_estimator::penalty_for_mismatch = 20;
const double query_ranking_estimator::penalty_for_gap = 40;

double query_ranking_estimator::estimate_query_ranking(const std::string& sample, const std::string& query) {
	std::vector<std::vector<double>> buffer(sample.size(), std::vector<double>(query.size()));

	for (auto i = 0; i < buffer.size(); ++i) {
		buffer[i][0] = penalty_for_gap * i;
	}

	for (auto i = 0; i < buffer[0].size(); ++i) {
		buffer[0][i] = penalty_for_gap * i;
	}

	for (auto i = 1; i < buffer.size(); ++i) {
		for (auto j = 1; j < buffer[i].size(); ++j) {
			buffer[i][j] = std::max(
				buffer[i - 1][j - 1] + penalty_for_mismatch, 
				std::max(
					buffer[i - 1][j] + penalty_for_gap, 
					buffer[i][j - 1] + penalty_for_gap
				)
			);
		}
	}

	return buffer[sample.size() - 1][query.size() - 1];
}

FARCONN_NAMESPACE_END