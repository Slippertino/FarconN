#pragma once

#include "../../middleware/handlers/selection_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

server_status_code selection_handler::try_convert_to_offset(const std::string& offsetStr, selection_base* obj) {
	return try_convert_to_selection_setting(offsetStr, obj->offset);
}

server_status_code selection_handler::try_convert_to_limit(const std::string& limitStr, selection_base* obj) {
	return try_convert_to_selection_setting(limitStr, obj->limit);
}

server_status_code selection_handler::try_convert_to_selection_setting(const std::string& stStr, uint64_t& st) {
	try {
		if (!stStr.empty()) {
			st = std::stoi(stStr);
		}

		return SUCCESS;
	}
	catch (...) {
		return server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}
}

FARCONN_NAMESPACE_END