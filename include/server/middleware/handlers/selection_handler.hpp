#pragma once

#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class selection_handler : public handler {
public:
	selection_handler(class server_middleware*, const command_entity*, command_response*, bool);

protected:
	server_status_code try_convert_to_offset(const std::string&, selection_base*);
	server_status_code try_convert_to_limit(const std::string&, selection_base*);

protected:
	virtual bool is_command_valid() override = 0;
	virtual void execute() override = 0;

private:
	server_status_code try_convert_to_selection_setting(const std::string&, uint64_t&);
};

FARCONN_NAMESPACE_END