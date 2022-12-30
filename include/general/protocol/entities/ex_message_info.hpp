#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct ex_message_info {
	std::string id;
	std::string sender_name;
	std::string time;
	std::string type;
	std::string content;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		ex_message_info,
		id,
		sender_name,
		time,
		type,
		content
	)
};

FARCONN_NAMESPACE_END