#pragma once

#include "../../../general/protocol/entities/invitations_info.hpp"
#include "handler.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class contacts_list_handler final : public handler {
public:
	contacts_list_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;
};

FARCONN_NAMESPACE_END