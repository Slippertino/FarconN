#include "../protocol/command_builder.hpp"

FARCONN_NAMESPACE_BEGIN(general)

command_builder::command_builder(const command_entity& com)
{ 
	set_command(com.command);

	for (auto& opt : com.options) {
		add_option(opt.second);
	}

	for (auto& param : com.params) {
		add_parameter(param);
	}
}

command_builder& command_builder::set_command(std::string name) {
	try_to_wrap(name);
	command.command = name;
	return *this;
}

command_builder& command_builder::add_option(std::string opt) {
	auto& options = command.options;
	
	try_to_wrap(opt);
	options.insert({ options.size(), opt });

	return *this;
}

void command_builder::clear_options() {
	command.options.clear();
}

command_builder& command_builder::add_parameter(std::string param) {
	try_to_wrap(param);
	command.params.push_back(param);

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

void command_builder::try_to_wrap(std::string& param) {
	static const std::regex whitespace_pattern = std::regex(R"(\s)");

	if (std::regex_search(param, whitespace_pattern)) {
		param = "<" + param + ">";
	}
}

FARCONN_NAMESPACE_END