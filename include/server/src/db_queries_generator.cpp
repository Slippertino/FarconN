#include "../db/db_queries_generator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

void db_queries_generator::to_mysql_format(const std::initializer_list<std::string*>& args) {
	static const std::string special_symbols = "\"\'\\";

	std::for_each(args.begin(), args.end(), [&](std::string* val) {
		for (auto i = 0; i < val->size(); ++i) {
			if (special_symbols.find((*val)[i]) != std::string::npos) {
				val->insert(i++, "\\");
			}
		}
	});
}

FARCONN_NAMESPACE_END