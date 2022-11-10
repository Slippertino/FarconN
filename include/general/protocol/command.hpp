#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct command {
	std::string id;
	std::string cmd_name;
	std::vector<std::string> params;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		command,
		id,
		cmd_name,
		params
	)
};