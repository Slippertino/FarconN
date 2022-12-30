#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../../tools/macro.hpp"
#include "external_message_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct external_chat_info {
	std::string id;
	std::string name;
	size_t size;
	external_message_info last_message;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		external_chat_info,
		id,
		name,
		size,
		last_message
	)
};

FARCONN_NAMESPACE_END