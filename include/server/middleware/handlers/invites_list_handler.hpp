#pragma once

#include <unordered_map>
#include "selection_handler.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class invites_list_handler final : public handler, 
								   public selection_handler {
public:
	invites_list_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	static const std::unordered_map<std::string, std::pair<std::string, std::string>> available_options_descs;
};

FARCONN_NAMESPACE_END