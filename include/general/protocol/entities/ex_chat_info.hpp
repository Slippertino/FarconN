#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../tools/macro.hpp"
#include "ex_message_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct ex_chat_info {
	std::string id;
	std::string name;
	size_t size;
	ex_message_info last_message;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		ex_chat_info,
		id,
		name,
		size,
		last_message
	)
};

FARCONN_NAMESPACE_END