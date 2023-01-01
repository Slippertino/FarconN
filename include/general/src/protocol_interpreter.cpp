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
	static const char composite_delimeter_opened = '<';
	static const char composite_delimeter_closed = '>';
	static const std::regex whitespace_pattern = std::regex(R"(\s)");

	bool is_word = false;
	auto pos = 0;
	auto word_pos_begin = 0;
	auto cd_count = 0;

	while (pos < query.size()) {
		if (!is_word) {
			while (
				pos < query.size() && 
				std::regex_match(std::string(1, query[pos]), whitespace_pattern)
			) ++pos;

			is_word = true;
			word_pos_begin = pos;

			continue;
		}

		if (std::regex_match(std::string(1, query[pos]), whitespace_pattern) && !cd_count) {
			is_word = false;
			args.push_back(query.substr(word_pos_begin, pos - word_pos_begin));
		}

		if (query[pos] == composite_delimeter_opened) {
			cd_count++;
		}

		if (query[pos] == composite_delimeter_closed) {
			cd_count--;
		}

		++pos;
	}

	if (is_word) {
		args.push_back(query.substr(word_pos_begin, pos - word_pos_begin));
	}
}

void protocol_interpreter::extract_command(query_arguments& args, command_entity* command) {
	if (args.empty()) {
		throw std::runtime_error("Недопустимый запрос!");
	}

	command->command = free_from_composite(args.front());
	args.pop_front();
}

void protocol_interpreter::extract_options(query_arguments& args, command_entity* command) {
	static const std::regex options_pattern = std::regex(R"(^-(.*)$)");
	
	auto& options = command->options;

	bool is_continued = true;
	do {
		if (args.empty()) {
			break;
		}

		auto& word = *args.begin();
		auto matches = find_submatches(word, options_pattern, 1);

		is_continued = !matches.empty();

		if (is_continued) {
			options.insert({ 
				options.size(), 
				free_from_composite(matches[0]) 
			});

			args.pop_front();
		}

	} while (is_continued);
}

void protocol_interpreter::extract_parameters(query_arguments& args, command_entity* command) {
	auto& params = command->params;

	for (auto& cur : args) {
		auto& word = free_from_composite(cur);
		params.push_back(word);
	}
}

std::string& protocol_interpreter::free_from_composite(std::string& word) {
	static const std::regex composite_pattern = std::regex(R"(<(.*)>)");

	auto matches = find_submatches(word, composite_pattern, 1);

	if (!matches.empty()) {
		word = matches[0];
	}

	return word;
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