#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "../../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class db_queries_generator {
public:

private:
	static void to_mysql_format(const std::initializer_list<std::string*>&);
};

FARCONN_NAMESPACE_END