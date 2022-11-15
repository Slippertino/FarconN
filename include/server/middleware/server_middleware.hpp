#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "entities/client_session.hpp"
#include "../db/db_responder.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class server_middleware {
public:
	using arguments_t = std::vector<std::string>;

public:
	server_middleware() = delete;
	server_middleware(const database_config&, std::string);

	/* Handlers */

private:
	std::unordered_map<std::string, client_session> sessions;
	std::mutex sessions_mutex;
	db_responder database;
};

FARCONN_NAMESPACE_END