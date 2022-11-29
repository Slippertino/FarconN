#include "../protocol/command_analyzer.hpp"

FARCONN_NAMESPACE_BEGIN(general)

command_analyzer::command_analyzer() = default;

command_analyzer& command_analyzer::set_command(std::string cmd) {
	utf8_encoder::from_local_to_utf8(cmd);
	model["cmd_name"] = cmd;
	return *this;
}

std::string command_analyzer::pop_parameter() {
	auto& params = model["params"];
	auto params_count = params.size();

	std::string param = params.back();

	if (params_count) {
		params.erase(params_count - 1);
	}

	return param;
}

void command_analyzer::clear_parameters() {
	model["params"].clear();
}

std::string command_analyzer::to_string(int indent) const {
	auto res = model.dump(indent);
	utf8_encoder::from_utf8_to_local(res);
	return res;
}

void command_analyzer::to_command(command& cmd) const {
	nlohmann::from_json(model, cmd);
	utf8_encoder::from_utf8_to_local(cmd.cmd_name);
	for (auto& p : cmd.params) {
		utf8_encoder::from_utf8_to_local(p);
	}
}

command_analyzer& command_analyzer::parse(std::string command) {
	utf8_encoder::from_local_to_utf8(command);
	model = nlohmann::json::parse(command);
	return *this;
}

FARCONN_NAMESPACE_END