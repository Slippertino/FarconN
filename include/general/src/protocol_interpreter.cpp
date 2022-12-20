#include "../protocol/protocol_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

std::shared_ptr<command_entity> protocol_interpreter::interpret(const std::string& query) {
	auto result = new command_entity();

	query_arguments args;

	split_query(query, args);

	if (!args.empty()) {
		extract_command(args.front(), result);
		extract_options(args.front(), result);
		args.pop_front();
	}

	if (!args.empty()) {
		extract_parameters(args.front(), result);
	}

	return std::shared_ptr<command_entity>(result);
}

void protocol_interpreter::split_query(const std::string& query, query_arguments& args) {
	static const std::regex split_pattern = std::regex(R"((.*?)([\"{}].*)$)");

	auto command_and_opts = find_submatches(query, split_pattern, 1);
	auto params = find_submatches(query, split_pattern, 2);

	if (!command_and_opts.empty()) {
		args.push_back(command_and_opts[0]);
	}

	if (!params.empty()) {
		args.push_back(params[0]);
	}
}

void protocol_interpreter::extract_command(std::string& query, command_entity* command) {
	static const std::regex split_pattern = std::regex(R"(\s*(\S+)\s*)");

	if (query.empty()) {
		throw std::runtime_error("Недопустимый запрос!");
	}

	auto matches = find_submatches(query, split_pattern, 1);

	if (!matches.empty()) {
		command->command = find_submatches(query, split_pattern, 1)[0];
	}
}

void protocol_interpreter::extract_options(std::string& query, command_entity* command) {
	static const std::regex options_pattern = std::regex(R"(-(\S+))");
	
	auto& options = command->options;

	for (auto& opt : find_submatches(query, options_pattern, 1)) {
		options.insert({ options.size(), opt });
	}
}

void protocol_interpreter::extract_parameters(std::string& query, command_entity* command) {
	std::regex params_pattern = std::regex(R"(\".*?\"|\{.*\}|\[.*\])");
	static const std::regex quotes_pattern = std::regex(R"(^\"(.*)\"$)");

	auto& params = command->params;

	for (auto& param : find_submatches(query, params_pattern, 0)) {
		auto quotes_cont = find_submatches(param, quotes_pattern, 1);

		quotes_cont.empty() 
			? params.push_back(param) 
			: params.push_back(quotes_cont[0]);
	}
}

std::vector<std::string> protocol_interpreter::find_submatches(
	const std::string& query, 
	const std::regex& pattern, 
	int group_id, 
	std::regex_constants::match_flag_type match_flag
) {
	return std::vector<std::string> {
		std::sregex_token_iterator{ query.cbegin(), query.cend(), pattern, group_id, match_flag },
		std::sregex_token_iterator{}
	};
}

FARCONN_NAMESPACE_END