#pragma once

#include <regex>
#include "command_entities.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class protocol_interpreter {
public:
	static std::shared_ptr<command_entity> interpret(std::string);

private:
	static void interpret_request(std::string, command_entity*);
	static void interpret_response(std::string, command_entity*);

	static void extract_command(std::string&, command_entity*);
	static void extract_parameters(std::string&, command_entity*);
	static void extract_options(std::string&, command_request*);
	static void extract_status(std::string&, command_response*);

	static std::vector<std::string> find_submatches(const std::string&, const std::regex&, int);

private:
	static const std::regex request_pattern;
	static const std::regex response_pattern;
};

FARCONN_NAMESPACE_END;