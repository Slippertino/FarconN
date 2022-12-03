#pragma once

#include "profile_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class profile_set_handler final : public profile_handler {
public:
	profile_set_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;
};

FARCONN_NAMESPACE_END