#include "../protocol v2.0/protocol_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::regex protocol_interpreter::request_pattern = std::regex(R"(^\s*\S+(\s+-(\S+))*(\s+\S+)*)");//std::regex(R"(.*)");
const std::regex protocol_interpreter::response_pattern = std::regex(R"(^\s*\S+\s*\d{3}\s*".*?"(\s*\S*)*)");

std::shared_ptr<command_entity> protocol_interpreter::interpret(std::string query) {
	auto result = new command_entity();

	if (std::regex_match(query, request_pattern)) {
		result->type = command_type::REQUEST;
		result = new command_request();
		interpret_request(query, result);
	}
	else if (std::regex_match(query, response_pattern)) {
		result->type = command_type::RESPONSE;
		result = new command_response();
		interpret_response(query, result);
	}
	else {
		throw std::runtime_error("Некорректная команда!");
	}

	return std::shared_ptr<command_entity>(result);
}

void protocol_interpreter::interpret_request(std::string query, command_entity* command) {
	auto request = static_cast<command_request*>(command);

	extract_command(query, request);
	extract_options(query, request);
	extract_parameters(query, request);
}

void protocol_interpreter::interpret_response(std::string query, command_entity* command) {
	auto response = static_cast<command_response*>(command);

	extract_command(query, response);
	extract_status(query, response);
	extract_parameters(query, response);
}

void protocol_interpreter::extract_command(std::string& query, command_entity* command) {
	static const std::regex command_pattern = std::regex(R"((^\s*(\S+))\s*(.*))");
	command->command = find_submatches(query, command_pattern, 2)[0];
	query = find_submatches(query, command_pattern, 3)[0];
}

void protocol_interpreter::extract_parameters(std::string& query, command_entity* command) {
	static const std::regex params_pattern = std::regex(R"((\S+))");

	for (auto& param : find_submatches(query, params_pattern, 1)) {
		command->params.push_back(param);
	}

	query.clear();
}

void protocol_interpreter::extract_options(std::string& query, command_request* command) {
	static const std::regex options_pattern = std::regex(R"((\s+-(\S+)))");
	static const std::regex options_cut_pattern = std::regex(R"((\s(\w.*)))");

	for (auto& opt : find_submatches(query, options_pattern, 2)) {
		command->options.insert({ command->options.size(), opt });
	}

	if (command->options.size()) {
		query = find_submatches(query, options_cut_pattern, 2)[0];
	}
}

void protocol_interpreter::extract_status(std::string& query, command_response* command) {
	static const std::regex status_pattern = std::regex(R"((\d{3})\s*(\"(.*?)\")\s*(.*))");

	command->status.first = std::atoi(find_submatches(query, status_pattern, 1)[0].c_str());
	command->status.second = find_submatches(query, status_pattern, 3)[0];

	query = find_submatches(query, status_pattern, 4)[0];
}

std::vector<std::string> protocol_interpreter::find_submatches(const std::string& query, const std::regex& pattern, int group_id) {
	return std::vector<std::string> {
		std::sregex_token_iterator{ query.cbegin(), query.cend(), pattern, group_id },
		std::sregex_token_iterator{}
	};
}

FARCONN_NAMESPACE_END