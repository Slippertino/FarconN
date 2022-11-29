#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct command {
	std::string cmd_name;
	std::vector<std::string> params;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		command,
		cmd_name,
		params
	)
};

FARCONN_NAMESPACE_END