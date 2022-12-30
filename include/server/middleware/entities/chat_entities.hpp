#pragma once

#include <string>
#include <vector>
#include "../general/protocol/entities/entities.hpp"

FARCONN_NAMESPACE_BEGIN(server)

enum class chat_message_type {
	TEXT = 1,
	FILE = 2
}; 

enum class chat_type {
	PRIVATE = 1,
	PUBLIC = 2,
};

struct internal_chat_info {
	std::string id;
	std::string title;
	std::string type;
	size_t size;

	external_chat_info to_external() const {
		return external_chat_info{
			id,
			title,
			size,
			external_message_info{}
		};
	}
};

struct internal_message_info {
	std::string id;
	std::string sender_name;
	double time_s;
	std::string type;
	std::string content;

	external_message_info to_external(
		const std::function<std::string(double, const std::string&)>& time_converter,
		const std::string& time_tmpl
	) const {
		return external_message_info{
			id,
			sender_name,
			time_converter(time_s, time_tmpl),
			type,
			content
		};
	}
};

struct chat_creation_params {
	std::string id;
	std::string title = "";
	std::string type;
	size_t size;
	std::vector<std::string> default_users_ids;
};

struct chat_post_params {
	std::string id;
	std::string sender_id;
	std::string chat_id;
	std::string type;
	std::string content;
};

FARCONN_NAMESPACE_END