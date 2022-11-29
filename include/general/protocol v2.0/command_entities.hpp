#pragma once

#include <string>
#include <map>
#include <vector>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

enum class command_type {
	REQUEST,
	RESPONSE
};

struct command_entity { 
	std::string command;
	command_type type;
	std::vector<std::string> params;
};

struct command_request : public command_entity {
	std::map<size_t, std::string> options;
};

struct command_response : public command_entity {
	std::pair<uint16_t, std::string> status;
};

FARCONN_NAMESPACE_END