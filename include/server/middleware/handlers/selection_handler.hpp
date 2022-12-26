#pragma once

#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class selection_handler {
public:
	selection_handler() = default;

protected:
	server_status_code try_convert_to_offset(const std::string&, selection_base*);
	server_status_code try_convert_to_limit(const std::string&, selection_base*);

private:
	server_status_code try_convert_to_selection_setting(const std::string&, uint64_t&);
};

FARCONN_NAMESPACE_END