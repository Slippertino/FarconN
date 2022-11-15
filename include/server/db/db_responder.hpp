#pragma once

#include <mutex>
#include <queue>
#include <filesystem>
#include <mysql/jdbc.h>
#include "../config/database_config.hpp"
#include "db_queries_generator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class db_responder {
public:
	db_responder() = delete;
	db_responder(const database_config&, std::string);

	void setup();

	~db_responder();

private:
	void create_connections();

private:
	static const size_t connections_count;
	std::queue<std::shared_ptr<sql::Connection>> connections;
	std::mutex connections_mutex;

	database_config config;
	std::filesystem::path files_storage_path;
};

FARCONN_NAMESPACE_END