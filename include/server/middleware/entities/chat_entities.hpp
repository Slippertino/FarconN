#pragma once

enum class chat_message_type {
	TEXT = 1,
	FILE = 2
}; 

enum class chat_type {
	PRIVATE = 1,
	PUBLIC = 2,
};

struct chat_creation_params {
	std::string id;
	std::string title = "";
	std::string type;
	size_t size;
	std::vector<std::string> default_users_ids;
};