#pragma once

#include "searching_cache.hpp"

FARCONN_NAMESPACE_BEGIN(server)

struct client_session {
	std::string native_token;
	searching_cache searching;
};

FARCONN_NAMESPACE_END