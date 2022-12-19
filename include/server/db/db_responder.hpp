#pragma once

#include <mutex>
#include <queue>
#include <unordered_set>
#include <filesystem>
#include <mysql/jdbc.h>
#include "../tools/token_generator.hpp"
#include "../config/database_config.hpp"
#include "../general/tools/thread_safe_containers/thread_safe_queue.hpp"
#include "../../general/protocol/server_status_code.hpp"
#include "../../general/logger/logger.hpp"
#include "db_queries_generator.hpp"
#include "../../general/protocol/entities/entities.hpp"
#include "../middleware/entities/entities.hpp"

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

	server_status_code check_request_existence(const std::string&, const std::string&, bool&);
	server_status_code create_request(const std::string&, const std::string&);
	server_status_code delete_request(const std::string&, const std::string&);

	server_status_code check_contact_existence(const std::string&, const std::string&, bool&);
	server_status_code create_contact(const std::string&, const std::string&);
	server_status_code delete_contact(const std::string&, const std::string&);

	server_status_code get_invites_list(const invites_selection&, invitations_info&);

	server_status_code get_contacts_list(const contacts_selection&, contacts_info&);

	server_status_code get_users_searching_list(const std::string&, std::list<primitive_user_info>&);

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