#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct ex_user_info {
	std::string id;
	std::string login;
	std::string name;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		ex_user_info,
		id,
		login,
		name
	)
};

FARCONN_NAMESPACE_END