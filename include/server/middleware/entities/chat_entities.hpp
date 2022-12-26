#pragma once

#include <string>
#include <vector>

FARCONN_NAMESPACE_BEGIN(server)

enum class chat_message_type {
	TEXT = 1,
	FILE = 2
}; 

enum class chat_type {
	PRIVATE = 1,
	PUBLIC = 2,
};

struct chat_info {
	std::string id;
	std::string title;
	std::string type;
	size_t size;
};

struct chat_message_info {
	std::string id;
	std::string sender_name;
	double time_s;
	std::string type;
	std::string content;
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