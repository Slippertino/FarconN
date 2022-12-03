#include "../protocol/command_entities.hpp"

FARCONN_NAMESPACE_BEGIN(general)

void command_entity::clear() {
	options.clear();
	params.clear();
}

void command_response::to_entity(command_entity& result) {
	result.command = command;

	result.options = options;

	result.params.push_back(std::to_string(status.first));
	result.params.push_back(status.second);
	result.params.insert(result.params.end(), params.begin(), params.end());
}

FARCONN_NAMESPACE_END