#include "../protocol/protocol_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

std::shared_ptr<command_entity> protocol_interpreter::interpret(const std::string& query) {
	auto result = new command_entity();

	query_arguments args;
	split_query(query, args);

	extract_command(args, result);
	extract_options(args, result);
	extract_parameters(args, result);

	return std::shared_ptr<command_entity>(result);
}

void protocol_interpreter::split_query(const std::string& query, query_arguments& args) {
	static const std::regex split_pattern = std::regex(R"((\b\".*\"|\S+))");

	for (auto& param : find_submatches(query, split_pattern, 1)) {
		args.push_back(param);
	}
}

void protocol_interpreter::extract_command(query_arguments& args, command_entity* command) {
	if (args.empty()) {
		throw std::runtime_error("Недопустимый запрос!");
	}

	command->command = args.front();
	args.pop_front();
}

void protocol_interpreter::extract_options(query_arguments& args, command_entity* command) {
	static const std::regex option_pattern = std::regex(R"(-\S+)");
	
	auto& options = command->options;

	while (!args.empty() && std::regex_match(args.front(), option_pattern)) {
		auto opt = args.front();
		opt = opt.substr(1);
		options.insert({ options.size(), opt });
		args.pop_front();
	}
}

void protocol_interpreter::extract_parameters(query_arguments& args, command_entity* command) {
	static const std::regex composite_param_pattern = std::regex(R"(\"(.*)\")");

	for (auto& param : args) {
		auto content = find_submatches(param, composite_param_pattern, 1);

		content.empty()
			? command->params.push_back(param)
			: command->params.push_back(content[0]);
	}

	args.clear();
}

std::vector<std::string> protocol_interpreter::find_submatches(const std::string& query, const std::regex& pattern, int group_id) {
	return std::vector<std::string> {
		std::sregex_token_iterator{ query.cbegin(), query.cend(), pattern, group_id },
		std::sregex_token_iterator{}
	};
}

FARCONN_NAMESPACE_END