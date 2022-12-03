#pragma once

#include <list>
#include <regex>
#include "command_entities.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class protocol_interpreter {
	using query_arguments = std::list<std::string>;

public:
	static std::shared_ptr<command_entity> interpret(const std::string&);

private:
	static void split_query(const std::string&, query_arguments&);
	static void extract_command(query_arguments&, command_entity*);
	static void extract_parameters(query_arguments&, command_entity*);
	static void extract_options(query_arguments&, command_entity*);

	static std::vector<std::string> find_submatches(const std::string&, const std::regex&, int);
};

FARCONN_NAMESPACE_END;