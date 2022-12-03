#pragma once

#include <string>
#include <list>
#include "../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

struct searching_cache {
	std::string query;
	std::list<std::string> results;
};

FARCONN_NAMESPACE_END