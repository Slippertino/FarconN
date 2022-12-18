#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct primitive_user_info {
	std::string login;
	std::string name;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		primitive_user_info,
		login,
		name
	)
};

FARCONN_NAMESPACE_END