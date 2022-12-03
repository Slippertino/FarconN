#pragma once

#include <mutex>
#include <queue>
#include <filesystem>
#include <mysql/jdbc.h>
#include "../config/database_config.hpp"
#include "../general/tools/thread_safe_containers/thread_safe_queue.hpp"
#include "../../general/protocol/server_status_code.hpp"
#include "../tools/token_generator.hpp"
#include <unordered_set>
#include "middleware/entities/user_profile.hpp"
#include "../../general/logger/logger.hpp"
#include "db_queries_generator.hpp"
#include "../middleware/entities/users_relations_type.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class db_responder {
public:
	db_responder() = delete;
	db_responder(const database_config&, std::string);

	void setup();

	server_status_code signup_user(const std::string&, const std::string&);

	server_status_code login_user(const std::string&, const std::string&, std::string&);
	server_status_code logout_user(const std::string&);

	server_status_code get_users_relations(const std::string&, const std::string&, users_relations_type&);
	server_status_code get_user_profile_data(const std::string&, user_profile&);

	server_status_code update_user_profile(const std::string&, const user_profile&);

	~db_responder();

private:
	struct query_components {
		std::shared_ptr<sql::Connection> connection;
		std::unique_ptr<sql::Statement> exec;
	};

private:
	query_components create_query_components();
	void free_query_components(query_components&);

	void create_connections();

private:
	static const size_t connections_count;
	thread_safe_queue<std::shared_ptr<sql::Connection>> connections;
	std::mutex connections_mutex;

	database_config config;
	std::filesystem::path files_storage_path;
};

FARCONN_NAMESPACE_END