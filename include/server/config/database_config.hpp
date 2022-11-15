#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

struct database_config {
	std::string url;
	std::string user_name;
	std::string password;
	std::string database_name;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		database_config,
		url,
		user_name,
		password,
		database_name
	)
};

FARCONN_NAMESPACE_END