#pragma once

#include <string>
#include <list>
#include "../general/tools/macro.hpp"
#include "../../../general/protocol/entities/entities.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

struct searching_cache {
	std::string query;
	std::unordered_map<size_t, internal_user_info> results;
};

FARCONN_NAMESPACE_END