#pragma once

#include <sstream>
#include "command_entities.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class command_builder {
public:
	command_builder() = delete;
	command_builder(command_type);
	command_builder(const command_request&);
	command_builder(const command_response&);

	command_builder& set_command(const std::string&);
	command_builder& set_status(const std::pair<uint16_t, std::string>&);

	command_builder& add_option(const std::string&);
	void clear_options();

	command_builder& add_parameter(const std::string&);
	void clear_parameters();

	void build(std::string&);

private:
	void clear();

private:
	command_type type;
	command_request request;
	command_response response;
};

FARCONN_NAMESPACE_END
