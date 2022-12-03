#include "../db/db_responder.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t db_responder::connections_count = 1;

db_responder::db_responder(const database_config& db_config, std::string files_storage_path) : 
	config(db_config), 
	files_storage_path(files_storage_path) 
{ }

void db_responder::setup() {
	if (!connections.empty()) {
		return;
	}

	{
		auto con = std::unique_ptr<sql::Connection>(
			sql::mysql::get_mysql_driver_instance()->connect(
				config.url,
				config.user_name,
				config.password
			)
		);

		auto exec = std::unique_ptr<sql::Statement>(con->createStatement());

		auto queries = db_queries_generator::get_db_init_queries(config.database_name);

		try {
			for (auto& cur : queries) {
				exec->execute(cur);
			}
		}
		catch (const sql::SQLException& ex) {
			LOG() << "Database: " << ex.what() << " ; Code: " << ex.getErrorCode() << "\n";
			throw ex;
		}

		exec->close();
	}

	for (auto i = 0; i < connections_count; ++i) {
		auto con = std::shared_ptr<sql::Connection>(
			sql::mysql::get_mysql_driver_instance()->connect(
				config.url,
				config.user_name,
				config.password
			)
		);

		con->setSchema(config.database_name);

		connections.add(con);
	}
}

server_status_code db_responder::signup_user(const std::string& login, const std::string& password) {
	auto comps = create_query_components();

	std::string user_token;
	std::vector<std::string> queries;
	{
		queries = db_queries_generator::get_users_tokens_query();
		std::unordered_set<std::string> existing_tokens;

		try {
			auto results = std::unique_ptr<sql::ResultSet>(
				comps.exec->executeQuery(queries[0])
			);

			results->beforeFirst();
			results->next();
			while (!results->isAfterLast()) {
				auto id = results->getString("id");
				existing_tokens.insert(id);
				results->next();
			}
		}
		catch (const std::exception& ex) {
			LOG() << ex.what() << "\n";
			free_query_components(comps);
			return server_status_code::SYS__INTERNAL_SERVER_ERROR;
		}

		user_token = token_generator::generate([&existing_tokens](const std::string& cur) {
			return existing_tokens.find(cur) == existing_tokens.end();
		});
	}

	server_status_code code;
	queries = db_queries_generator::get_add_user_query(user_token, login, password);
	
	try {
		auto count = comps.exec->executeUpdate(queries[0]);
		code = server_status_code::SYS__OKEY;

		LOG() << "Database: " << "Добавлен новый пользователь!\n";
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::SIGNUP__EXISTING_LOGIN_ERROR
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG() << ex.what() << " ; Code : " << ex.getErrorCode() << "\n";
	}
	catch (const std::exception& ex) {
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG() << ex.what() << "\n";
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::login_user(const std::string& login, const std::string& password, std::string& token) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_login_user_queries(login, password);

	server_status_code code;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();
		if (!results->isAfterLast()) {
			auto count = comps.exec->executeUpdate(queries[1]);

			if (count) {
				token = results->getString("id");
				code = server_status_code::SYS__OKEY;
			}
			else {
				code = server_status_code::LOGIN__ALREADY_LOGGED_IN_ERROR;
			}
		}
		else {
			code = server_status_code::LOGIN__INVALID_DATA_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::logout_user(const std::string& token) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_logout_user_query(token);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = (count)
			? server_status_code::SYS__OKEY
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_users_relations(const std::string& login_user1, const std::string& login_user2, users_relations_type& rels) {
	if (login_user1 == login_user2) {
		rels = users_relations_type::SELF;
		return server_status_code::SYS__OKEY;
	}

	auto comps = create_query_components();
	auto queries = db_queries_generator::get_are_users_in_contacts_query(
		login_user1, 
		login_user2
	);

	server_status_code code;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		rels = (result->rowsCount())
			? users_relations_type::CONTACTS
			: users_relations_type::NONE;

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_profile_data(const std::string& login, user_profile& data) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_profile_data_query(login);

	server_status_code code;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		if (result->rowsCount()) {
			result->beforeFirst(); 
			result->next();

			for (auto& cur : data) {
				std::string temp = result->getString(cur.second.name);
				utf8_encoder::from_utf8_to_local(temp);
				cur.second.value = temp;
			}

			code = server_status_code::SYS__OKEY;
		} 
		else {
			code = server_status_code::SYS__NONEXISTEN_USER_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::update_user_profile(const std::string& token, const user_profile& profile) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_profile_to_update_query(token, profile);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = server_status_code::SYS__OKEY;
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::SIGNUP__EXISTING_LOGIN_ERROR
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG() << ex.what() << " ; Code : " << ex.getErrorCode() << "\n";
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

db_responder::~db_responder() {
	while (!connections.empty()) {
		auto con = connections.wait_and_erase();

		if (!con->isClosed()) {
			con->close();
		}
	}
}

db_responder::query_components db_responder::create_query_components() {
	query_components comps;

	comps.connection = connections.wait_and_erase();
	comps.exec = std::unique_ptr<sql::Statement>(comps.connection->createStatement());

	return std::move(comps);
}

void db_responder::free_query_components(query_components& comps) {
	comps.exec->close();
	connections.add(comps.connection);
}

FARCONN_NAMESPACE_END