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

	fs::create_directories(files_storage_path);
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

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		if (!count) {
			code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_id_by_login(const std::string& login, std::string& token) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_id_by_login_query(login);

	server_status_code code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		if (!results->rowsCount()) {
			code = server_status_code::SYS__NONEXISTEN_USER_ERROR;
		} else {
			results->beforeFirst();
			results->next();
			token = results->getString("id");
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
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
	auto queries = db_queries_generator::get_users_relations_query(
		login_user1,
		login_user2
	);

	auto check_for_relation = [&](users_relations_type type, size_t query_id) {
		if (rels == users_relations_type::NONE) {
			auto result_contacts = std::unique_ptr<sql::ResultSet>(
				comps.exec->executeQuery(queries[query_id])
			);

			if (result_contacts->rowsCount()) {
				rels = type;
			}
		}
	};

	rels = users_relations_type::NONE;

	server_status_code code;

	try {
		check_for_relation(users_relations_type::CONTACTS, 0);
		check_for_relation(users_relations_type::REQUESTS, 1);

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

server_status_code db_responder::check_request_existence(const std::string& lufrom, const std::string& luto, bool& res) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_check_request_existence_query(lufrom, luto);

	server_status_code code;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res = result->rowsCount();

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_request(const std::string& lufrom, const std::string& luto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_create_request_query(lufrom, luto);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = server_status_code::SYS__OKEY;
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::REQUEST__ALREADY_EXIST_ERROR
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

server_status_code db_responder::delete_request(const std::string& lufrom, const std::string& luto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_delete_request_query(lufrom, luto);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::check_contact_existence(const std::string& login, const std::string& contact_login, bool& res) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_check_contact_existence_query(
		login,
		contact_login
	);

	server_status_code code;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res = result->rowsCount();

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_contact(const std::string& lufrom, const std::string& luto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_create_contact_query(lufrom, luto);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::delete_contact(const std::string& login, const std::string& contact_login) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_delete_contact_query(
		login,
		contact_login
	);

	server_status_code code;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_invites_list(const invites_selection& selection, invitations_info& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_invites_list_query(selection);

	auto& list_type = selection.invitation_type_name;

	server_status_code code;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			auto login = res->getString("login");
			auto name = res->getString("name");

			info[list_type].push_back({ login, name });

			res->next();
		}

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_contacts_list(const contacts_selection& selection, contacts_info& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_contacts_list_query(selection);

	server_status_code code;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			auto login = res->getString("login");
			auto name = res->getString("name");

			info.push_back({ login, name });

			res->next();
		}

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_users_searching_list(const std::string& login, std::list<basic_user_info>& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_users_searching_list_query(login, {"login", "name"});

	server_status_code code;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			auto login = res->getString("login");
			auto name = res->getString("name");

			info.push_back({ login, name });

			res->next();
		}

		code = server_status_code::SYS__OKEY;
	}
	catch (const std::exception& ex) {
		LOG() << "Database : " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_chat(const chat_creation_params& params) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_chat_creation_query(params);

	server_status_code code;
	
	try {
		comps.exec->executeUpdate(queries[0]);
		comps.exec->executeUpdate(queries[1]);

		code = server_status_code::SYS__OKEY;

		LOG() << "Database: " << "Добавлен новый чат!\n";
	}
	catch (const std::exception& ex) {
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG() << ex.what() << "\n";
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_chats_tokens(std::unordered_set<std::string>& tokens) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_chats_tokens_query();

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();
		while (!results->isAfterLast()) {
			auto id = results->getString("id");
			tokens.insert(id);
			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_chats_count(const std::string& id, uint32_t& count) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_chats_count_query(id);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();
		count = results->getUInt("count");
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::add_user_to_chat(const std::string& chat_id, const std::string& user_id) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_add_user_to_chat_queries(chat_id, user_id);

	auto code = server_status_code::SYS__OKEY;

	comps.connection->setAutoCommit(false);

	try {
		for (auto& query : queries) {
			comps.exec->executeUpdate(query);
		}

		comps.connection->commit();
	}
	catch (const std::exception& ex) {
		comps.connection->rollback();

		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	comps.connection->setAutoCommit(true);

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_chat_info(
	const std::string& user_id, 
	const std::string& chat_id, 
	chat_info& info
) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_chat_info_queries(
		user_id,
		chat_id
	);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		if (results->rowsCount()) {
			results->beforeFirst();
			results->next();

			info.id = chat_id;
			info.type = results->getString("type");
			info.title = results->getString("title");
			info.size = results->getInt("size");
		}
		else {
			code = server_status_code::CHAT__NONEXISTEN_CHAT_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::is_user_chat_participant(const std::string& user_id, const std::string& chat_id) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_is_user_chat_participant_query(user_id, chat_id);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		if (!results->rowsCount()) {
			code = server_status_code::CHAT__USER_NOT_A_PARTICIPANT_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::exclude_user_from_chat(const std::string& user_id, const std::string& chat_id) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_exclude_user_from_chat_queries(user_id, chat_id);

	auto code = server_status_code::SYS__OKEY;

	comps.connection->setAutoCommit(false);

	try {
		auto affected = comps.exec->executeUpdate(queries[0]);

		if (!affected) {
			code = server_status_code::CHAT__USER_NOT_A_PARTICIPANT_ERROR;
		}

		if (code == server_status_code::SYS__OKEY) {
			comps.exec->executeUpdate(queries[1]);
			comps.exec->executeUpdate(queries[2]);
		}

		comps.connection->commit();
	}
	catch (const std::exception& ex) {
		comps.connection->rollback();

		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	comps.connection->setAutoCommit(true);

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_messages_tokens(std::unordered_set<std::string>& tokens) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_messages_tokens_query();

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();
		while (!results->isAfterLast()) {
			auto id = results->getString("id");
			tokens.insert(id);
			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::post_message(const chat_post_params& params) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_post_message_query(params);

	auto code = server_status_code::SYS__OKEY;

	comps.connection->setAutoCommit(false);

	try {
		comps.exec->executeUpdate(queries[0]);
		comps.connection->commit();
	}
	catch (const std::exception& ex) {
		comps.connection->rollback();

		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	comps.connection->setAutoCommit(true);

	free_query_components(comps);

	return code;
}

server_status_code db_responder::save_file(
	const std::string& chat_id,
	const std::string& name, 
	const std::string& content
) {
	auto code = server_status_code::SYS__OKEY;

	auto chat_path = files_storage_path / fs::path(chat_id);
	fs::create_directories(chat_path);

	std::ofstream output;

	try {
		output.open(chat_path / std::filesystem::path(name));

		output << content;
	}
	catch (const std::exception& ex) {
		LOG() << "Ошибка при попытке записи в файл: " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}
	
	if (output.is_open()) {
		output.close();
	}

	return code;
}

server_status_code db_responder::get_messages_list(const chat_messages_selection& params, std::vector<chat_message_info>& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_messages_list_query(params);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();

		while (!results->isAfterLast()) {
			chat_message_info cur;

			cur.id = results->getString("id");
			cur.sender_name = results->getString("name");
			cur.time_s = results->getDouble("time");
			cur.type = results->getString("type");
			cur.content = results->getString("content");

			info.push_back(cur);

			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_chats_tokens(const std::string& user_id, std::vector<std::string>& chats) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_chats_tokens_query(user_id);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();

		while (!results->isAfterLast()) {
			auto id = results->getString("id");

			chats.push_back(id);

			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG() << ex.what() << "\n";
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