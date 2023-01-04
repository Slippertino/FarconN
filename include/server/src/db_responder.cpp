#include "../db/db_responder.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t db_responder::connections_count = 1;

void db_responder::setup(const database_config& db_config, std::string files_storage_path) {
	this->config = db_config;
	this->files_storage_path = files_storage_path;

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

		auto queries = db_queries_generator::get_sys_db_init_queries(config.database_name);

		try {
			for (auto& cur : queries) {
				exec->execute(cur);
			}

		}
		catch (const sql::SQLException& ex) {
			LOG(DB) << ex.what() << " ; Code: " << ex.getErrorCode() << "\n";
			throw ex;
		}
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

server_status_code db_responder::sys_logout_users() {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_sys_logout_users_query();

	auto code = server_status_code::SYS__OKEY;

	try {
		comps.exec->executeUpdate(queries[0]);
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_users_tokens(std::unordered_set<std::string>& tokens) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_users_tokens_query();
	std::string user_token;
	
	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();
		while (!results->isAfterLast()) {
			std::string id = results->getString("id");
			utf8_encoder::from_utf8_to_local(id);
			tokens.insert(id);
			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		free_query_components(comps);
		return server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::signup_user(
	const std::string& id,
	const std::string& login, 
	const std::string& password
) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_add_user_query(id, login, password);
	
	auto code = server_status_code::SYS__OKEY;
	
	try {
		auto count = comps.exec->executeUpdate(queries[0]);

		LOG(DB) << "�������� ����� ������������: " << login << "\n";
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::SIGNUP__EXISTING_LOGIN_ERROR
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG(DB) << ex.what() << " ; Code : " << ex.getErrorCode() << "\n";
	}
	catch (const std::exception& ex) {
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG(DB) << ex.what() << "\n";
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::login_user(const std::string& login, const std::string& password, std::string& token) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_login_user_queries(login, password);

	auto code = server_status_code::SYS__OKEY;

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
				utf8_encoder::from_utf8_to_local(token);
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
		LOG(DB) << ex.what() << "\n";
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
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_id_by_login(const std::string& login, std::string& token) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_id_by_login_query(login);

	auto code = server_status_code::SYS__OKEY;

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
			utf8_encoder::from_utf8_to_local(token);
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_users_relations(const std::string& user1_id, const std::string& user2_id, users_relations_type& rels) {
	if (user1_id == user2_id) {
		rels = users_relations_type::SELF;
		return server_status_code::SYS__OKEY;
	}

	auto comps = create_query_components();
	auto queries = db_queries_generator::get_users_relations_query(
		user1_id,
		user2_id
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

	auto code = server_status_code::SYS__OKEY;

	try {
		check_for_relation(users_relations_type::CONTACTS, 0);
		check_for_relation(users_relations_type::REQUESTS, 1);
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_profile_data(const std::string& id, user_profile& data) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_profile_data_query(id);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		if (result->rowsCount()) {
			result->beforeFirst(); 
			result->next();

			for (auto& cur : data) {
				cur.second.value = result->getString(cur.second.name);
				utf8_encoder::from_utf8_to_local(cur.second.value.value());
			}
		} 
		else {
			code = server_status_code::SYS__NONEXISTEN_USER_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::update_user_profile(const std::string& token, const user_profile& profile) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_profile_to_update_query(token, profile);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::SIGNUP__EXISTING_LOGIN_ERROR
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG(DB) << ex.what() << " ; Code : " << ex.getErrorCode() << "\n";
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::check_request_existence(const std::string& ufrom, const std::string& uto, bool& res) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_check_request_existence_query(ufrom, uto);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res = result->rowsCount();
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_request(const std::string& ufrom, const std::string& uto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_create_request_query(ufrom, uto);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);
	}
	catch (const sql::SQLException& ex) {
		code = (ex.getErrorCode() == 1062)
			? server_status_code::REQUEST__ALREADY_EXIST_ERROR
			: server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG(DB) << ex.what() << " ; Code : " << ex.getErrorCode() << "\n";
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::delete_request(const std::string& ufrom, const std::string& uto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_delete_request_query(ufrom, uto);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::check_contact_existence(const std::string& id, const std::string& contact_id, bool& res) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_check_contact_existence_query(
		id,
		contact_id
	);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto result = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res = result->rowsCount();
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_contact(const std::string& ufrom, const std::string& uto) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_create_contact_query(ufrom, uto);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::delete_contact(const std::string& id, const std::string& contact_id) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_delete_contact_query(
		id,
		contact_id
	);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto count = comps.exec->executeUpdate(queries[0]);
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_invites_list(const invites_selection& selection, ex_invitations_info& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_invites_list_query(selection);

	auto& list_type = selection.invitation_type_name;

	auto code = server_status_code::SYS__OKEY;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		auto& data = info.data[list_type];

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			std::string id = res->getString("id");
			std::string login = res->getString("login");
			std::string name = res->getString("name");

			utf8_encoder::from_utf8_to_local_list({
				&id, &login, &name
			});

			data.insert({ 
				data.size(), 
				{ id, login, name } 
			});

			res->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_contacts_list(const contacts_selection& selection, ex_contacts_info& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_contacts_list_query(selection);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			std::string id = res->getString("id");
			std::string login = res->getString("login");
			std::string name = res->getString("name");

			utf8_encoder::from_utf8_to_local_list({
				&id, &login, &name
			});

			info.data.insert({ 
				info.data.size(), 
				{ id, login, name } 
			});

			res->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_users_searching_list(const std::string& user_id, std::list<user_info>& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_users_searching_list_query(
		user_id, 
		{"id", "login", "name"}
	);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			std::string id = res->getString("id");
			std::string login = res->getString("login");
			std::string name = res->getString("name");

			utf8_encoder::from_utf8_to_local_list({
				&id, &login, &name
			});

			info.push_back({ id, login, name });

			res->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::create_chat(const chat_creation_params& params) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_chat_creation_query(params);

	auto code = server_status_code::SYS__OKEY;
	
	try {
		comps.exec->executeUpdate(queries[0]);
		comps.exec->executeUpdate(queries[1]);

		LOG(DB) << "�������� ����� ���!\n";
	}
	catch (const std::exception& ex) {
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;

		LOG(DB) << ex.what() << "\n";
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
			std::string id = results->getString("id");
			utf8_encoder::from_utf8_to_local(id);
			tokens.insert(id);
			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::check_private_chat_existance(const std::string& uid1, const std::string& uid2, bool& res) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_check_private_chat_existance_query(uid1, uid2);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res = results->rowsCount();
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
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
		LOG(DB) << ex.what() << "\n";
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

		LOG(DB) << ex.what() << "\n";
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

			utf8_encoder::from_utf8_to_local_list({
				&info.id,
				&info.type,
				&info.title
			});
		}
		else {
			code = server_status_code::CHAT__NONEXISTEN_CHAT_ERROR;
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
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
		LOG(DB) << ex.what() << "\n";
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

		LOG(DB) << ex.what() << "\n";
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
			std::string id = results->getString("id");
			utf8_encoder::from_utf8_to_local(id);
			tokens.insert(id);
			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
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

		LOG(DB) << ex.what() << "\n";
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
		LOG(DB) << "������ ��� ������� ������ � ����: " << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}
	
	if (output.is_open()) {
		output.close();
	}

	return code;
}

server_status_code db_responder::get_messages_list(const chat_messages_selection& params, std::vector<message_info>& info) {
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
			message_info cur;

			cur.id = results->getString("id");
			cur.sender_name = results->getString("name");
			cur.time_s = results->getDouble("time");
			cur.type = results->getString("type");
			cur.content = results->getString("content");

			utf8_encoder::from_utf8_to_local_list({
				&cur.id,
				&cur.sender_name,
				&cur.type,
				&cur.content
			});

			info.push_back(cur);

			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_user_chats_tokens(const chats_selection& selection, std::vector<std::string>& chats) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_user_chats_tokens_query(selection);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto results = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		results->beforeFirst();
		results->next();

		while (!results->isAfterLast()) {
			std::string id = results->getString("id");
			utf8_encoder::from_utf8_to_local(id);

			chats.push_back(id);

			results->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
		code = server_status_code::SYS__INTERNAL_SERVER_ERROR;
	}

	free_query_components(comps);

	return code;
}

server_status_code db_responder::get_chat_party(const chat_party_selection& selection, ex_chat_party_info& info) {
	auto comps = create_query_components();
	auto queries = db_queries_generator::get_chat_party_query(selection);

	auto code = server_status_code::SYS__OKEY;

	try {
		auto res = std::unique_ptr<sql::ResultSet>(
			comps.exec->executeQuery(queries[0])
		);

		res->beforeFirst(); res->next();
		while (!res->isAfterLast()) {
			std::string id = res->getString("id");
			std::string login = res->getString("login");
			std::string name = res->getString("name");

			utf8_encoder::from_utf8_to_local_list({
				&id, &login, &name
			});

			info.data.insert({ 
				info.data.size(), 
				{ id, login, name }
			});

			res->next();
		}
	}
	catch (const std::exception& ex) {
		LOG(DB) << ex.what() << "\n";
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