#pragma once

#include <sstream>
#include <regex>
#include "command_entities.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class command_builder {
public:
	command_builder() = default;
	command_builder(const command_entity&);

	command_builder& set_command(std::string);

	command_builder& add_option(std::string);
	void clear_options();

	command_builder& add_parameter(std::string);
	void clear_parameters();

	void build(std::string&);

private:
	void try_to_wrap(std::string&);

	void clear();

private:
	command_entity command;
};

FARCONN_NAMESPACE_END
