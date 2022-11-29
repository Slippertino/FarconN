#include "../protocol v2.0/command_builder.hpp"

FARCONN_NAMESPACE_BEGIN(general)

command_builder::command_builder(command_type type) : type(type)
{ }

command_builder::command_builder(const command_request& req) : request(req), type(command_type::REQUEST)
{ }

command_builder::command_builder(const command_response& resp) : response(resp), type(command_type::RESPONSE)
{ }

command_builder& command_builder::set_command(const std::string& name) {
	switch (type)
	{
	case command_type::REQUEST:
		request.command = name;
		break;
	case command_type::RESPONSE:
		response.command = name;
		break;
	default:
		break;
	}

	return *this;
}

command_builder& command_builder::set_status(const std::pair<uint16_t, std::string>& st) {
	response.status = st;

	return *this;
}

command_builder& command_builder::add_option(const std::string& opt) {
	request.options.insert({ request.options.size(), opt });

	return *this;
}

void command_builder::clear_options() {
	request.options.clear();
}

command_builder& command_builder::add_parameter(const std::string& param) {
	switch (type)
	{
	case command_type::REQUEST:
		request.params.push_back(param);
		break;
	case command_type::RESPONSE:
		response.params.push_back(param);
		break;
	default:
		break;
	}

	return *this;
}

void command_builder::clear_parameters() {
	request.params.clear();
	response.params.clear();
}

void command_builder::build(std::string& result) {
	std::ostringstream stream;

	switch (type)
	{
	case command_type::REQUEST:
		stream << request.command;

		for (auto& opt : request.options) {
			stream << " -" << opt.second;
		}

		for (auto& param : request.params) {
			stream << " " << param;
		}

		break;
	case command_type::RESPONSE:
		stream 
			<< response.command << " "
			<< response.status.first << " "
			<< response.status.second;

		for (auto& param : response.params) {
			stream << " " << param;
		}

		break;
	default:
		break;
	}

	result = stream.str();
}

FARCONN_NAMESPACE_END