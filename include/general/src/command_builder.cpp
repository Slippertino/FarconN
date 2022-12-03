#include "../protocol/command_builder.hpp"

FARCONN_NAMESPACE_BEGIN(general)

command_builder::command_builder(const command_entity& com)
{ 
	command.command = com.command;

	for (auto& opt : com.options) {
		add_option(opt.second);
	}

	for (auto& param : com.params) {
		add_parameter(param);
	}
}

command_builder& command_builder::set_command(const std::string& name) {
	command.command = name;
	return *this;
}

command_builder& command_builder::add_option(const std::string& opt) {
	auto& options = command.options;
	options.insert({ options.size(), opt });

	return *this;
}

void command_builder::clear_options() {
	command.options.clear();
}

command_builder& command_builder::add_parameter(const std::string& param) {
	static const std::regex quotes_pattern = std::regex(R"((.*\s.*|))");

	std::regex_match(param, quotes_pattern) 
		? command.params.push_back("\"" + param + "\"")
		: command.params.push_back(param);

	return *this;
}

void command_builder::clear_parameters() {
	command.params.clear();
}

void command_builder::build(std::string& result) {
	std::ostringstream stream;
	stream << command.command;

	for (auto& opt : command.options) {
		stream << " -" << opt.second;
	}

	for (auto& param : command.params) {
		stream << " " << param;
	}

	result = stream.str();
}

FARCONN_NAMESPACE_END