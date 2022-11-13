#pragma once

#include <string>
#include <unordered_map>
#include "server_status_code.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class status_code_interpreter {
public:
	static std::string get_description(server_status_code);

private:
	static const std::string unknown_code_description;
	static const std::unordered_map<server_status_code, std::string> code_descriptions;
};

FARCONN_NAMESPACE_END