#pragma once

#include <string>
#include <map>
#include <vector>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

struct command_entity { 
	std::string command;
	std::map<size_t, std::string> options;
	std::vector<std::string> params;

	void clear();
};

struct command_response : public command_entity {
	std::pair<uint16_t, std::string> status;

	void to_entity(command_entity&);
};

FARCONN_NAMESPACE_END