#pragma once

#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class help_handler final : public handler {
public:
	help_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	static const std::string default_section_name;
};

FARCONN_NAMESPACE_END