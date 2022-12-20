#include "../db/db_queries_generator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

std::string db_queries_generator::users_name_tb			 = "users";
std::string db_queries_generator::contacts_name_tb		 = "contacts";
std::string db_queries_generator::requests_name_tb       = "requests";
std::string db_queries_generator::chats_name_tb          = "chats";
std::string db_queries_generator::messages_name_tb       = "messages";
std::string db_queries_generator::users_in_chats_name_tb = "users_in_chats";

void db_queries_generator::to_mysql_format(const std::initializer_list<std::string*>& args) {
	static const std::string special_symbols = "\"\'\\";

	std::for_each(args.begin(), args.end(), [&](std::string* val) {
		utf8_encoder::from_local_to_utf8(*val);

		for (auto i = 0; i < val->size(); ++i) {
			if (special_symbols.find((*val)[i]) != std::string::npos) {
				val->insert(i++, "\\");
			}
		}
	});
}

void db_queries_generator::reset(std::ostringstream& ostr, std::vector<std::string>& buffer) {
	buffer.push_back(ostr.str());
	ostr.str("");
}

std::vector<std::string> db_queries_generator::get_db_init_queries(std::string db_name) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &db_name });

	ostr << "set global max_connections = 10000;";
	reset(ostr, res);

	ostr << "create database if not exists " << db_name << ";";
	reset(ostr, res);

	ostr << "use " << db_name << ";";
	reset(ostr, res);

	ostr << "create table "
		<< "if not exists " << users_name_tb << " ("
		<< "id varchar(50) primary key,"
		<< "login varchar(32) not null,"
		<< "password varchar(32) not null,"
		<< "name varchar(50),"
		<< "phone varchar(16),"
		<< "email varchar(50),"
		<< "self varchar(100),"
		<< "online bool not null,"
		<< "unique index login_index(login(32)),"
		<< "index name_index(name(50)));";
	reset(ostr, res);

	ostr << "create table "
		<< "if not exists " << contacts_name_tb << " ("
		<< "u_from varchar(50),"
		<< "u_to varchar(50),"
		<< "primary key(u_from, u_to),"
		<< "foreign key (u_from) references " << users_name_tb << "(login) on delete cascade on update cascade,"
		<< "foreign key (u_to) references " << users_name_tb << "(login) on delete cascade on update cascade);";
	reset(ostr, res);
	
	ostr << "create table "
		<< "if not exists " << requests_name_tb << " ("
		<< "u_from varchar(50),"
		<< "u_to varchar(50),"
		<< "primary key(u_from, u_to),"
		<< "foreign key (u_from) references " << users_name_tb << "(login) on delete cascade on update cascade,"
		<< "foreign key (u_to) references " << users_name_tb << "(login) on delete cascade on update cascade);";
	reset(ostr, res);

	ostr << "create table "
		<< "if not exists " << chats_name_tb << " ("
		<< "id varchar(50) primary key,"
		<< "type enum(" << Q("private") << "," << Q("public") << ") not null,"
		<< "title varchar(32) null,"
		<< "size int not null);";
	reset(ostr, res);

	ostr << "create table "
		<< "if not exists " << messages_name_tb << " ("
		<< "chat_id varchar(50),"
		<< "sender_id varchar(50),"
		<< "time datetime,"
		<< "type enum(" << Q("text") << "," << Q("file") << ") not null,"
		<< "content text not null,"
		<< "primary key(chat_id, sender_id, time),"
		<< "foreign key (chat_id) references " << chats_name_tb << "(id) on delete cascade on update no action,"
		<< "foreign key (sender_id) references " << users_name_tb << "(id) on delete cascade on update no action);";
	reset(ostr, res);

	ostr << "create table "
		<< "if not exists " << users_in_chats_name_tb << " ("
		<< "user_id varchar(50),"
		<< "chat_id varchar(50),"
		<< "primary key(user_id, chat_id),"
		<< "foreign key (user_id) references " << users_name_tb << "(id) on delete cascade on update no action,"
		<< "foreign key (chat_id) references " << chats_name_tb << "(id) on delete cascade on update no action);";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_users_tokens_query() {
	std::vector<std::string> res;
	std::ostringstream ostr;

	ostr << "select id from " << users_name_tb << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_id_by_login_query(std::string login) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login });

	ostr << "select id "
		<< "from " << users_name_tb << " "
		<< "where login = " << Q(login) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_users_searching_list_query(std::string login, std::vector<std::string> columns) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login });
	for (auto& col : columns) {
		to_mysql_format({ &col });
	}

	ostr << "select " << columns[0];
	for (auto i = 1; i < columns.size(); ++i) {
		ostr << "," << columns[i];
	}
	ostr << " from " << users_name_tb << " where login <> " << Q(login) << " and login not in (";

	ostr 
		<< "select u_from from " << contacts_name_tb << " where u_to = " << Q(login) << " union "
		<< "select u_to from " << contacts_name_tb << " where u_from = " << Q(login) << " union "
		<< "select u_from from " << requests_name_tb << " where u_to = " << Q(login) << " union "
		<< "select u_to from " << requests_name_tb << " where u_from = " << Q(login) << ")";

	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_add_user_query(std::string id, std::string login, std::string password) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &id, &login, &password });

	ostr << "insert into " << users_name_tb << "(id, login, password, name, phone, email, self, online) values ("
		 << Q(id) << "," << Q(login) << "," << Q(password) << "," << Q(login) << "," << Q("") << "," << Q("") << "," << Q("") << "," << "false" << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_login_user_queries(std::string login, std::string password) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login, &password });

	ostr << "select id from " << users_name_tb << " where "
		 << "login = " << Q(login) << " and " << "password = " << Q(password) << ";";
	reset(ostr, res);

	ostr << "update " << users_name_tb << " "
		 << "set online = true where "
		 << "login = " << Q(login) << " and " << "password = " << Q(password) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_logout_user_query(std::string token) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &token });

	ostr << "update " << users_name_tb << " "
		 << "set online = false where "
		 << "id = " << Q(token) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_users_relations_query(std::string lu1, std::string lu2) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lu1, &lu2 });

	ostr << "select * from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(lu1) << " and u_to = " << Q(lu2) << ") or "
		<< "(u_from = " << Q(lu2) << " and u_to = " << Q(lu1) << ");";
	reset(ostr, res);

	ostr << "select * from " << requests_name_tb << " where "
		<< "(u_from = " << Q(lu1) << " and u_to = " << Q(lu2) << ") or "
		<< "(u_from = " << Q(lu2) << " and u_to = " << Q(lu1) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_profile_data_query(std::string login) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login });

	ostr << "select * from " << users_name_tb << " where "
		 << "login = " << Q(login) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_profile_to_update_query(std::string token, user_profile profile) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &token });

	ostr << "update " << users_name_tb << " set ";

	bool is_first = true;
	for (auto param : profile) {
		if (param.second.value.has_value()) {
			auto value = param.second.value.value();

			to_mysql_format({ &value });

			if (!is_first) {
				ostr << ",";
			}
			else {
				is_first = false;
			}

			ostr << param.second.name << " = " << Q(value) << " ";
		}
	}

	ostr << "where id = " << Q(token) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_check_request_existence_query(std::string lufrom, std::string luto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lufrom, &luto });

	ostr << "select * from " << requests_name_tb << " where "
		 << "u_from = " << Q(lufrom) << " and "
		 << "u_to = " << Q(luto) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_create_request_query(std::string lufrom, std::string luto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lufrom, &luto });

	ostr << "insert into " << requests_name_tb << "(u_from, u_to) values "
		<< "( " << Q(lufrom) << "," << Q(luto) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_delete_request_query(std::string lufrom, std::string luto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lufrom, &luto });

	ostr << "delete from " << requests_name_tb << " where "
		 << "u_from = " << Q(lufrom) << " and "
		 << "u_to = " << Q(luto) << ";";
	reset(ostr, res);

	return res;
}


std::vector<std::string> db_queries_generator::get_check_contact_existence_query(std::string login, std::string contact_login) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login, &contact_login });

	ostr << "select * from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(login) << " and " << "u_to = " << Q(contact_login) << ") or "
		<< "(u_from = " << Q(contact_login) << " and " << "u_to = " << Q(login) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_create_contact_query(std::string lufrom, std::string luto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lufrom, &luto });

	ostr << "insert into " << contacts_name_tb << "(u_from, u_to) values "
		<< "( " << Q(lufrom) << "," << Q(luto) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_delete_contact_query(std::string login, std::string contact_login) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &login, &contact_login });

	ostr << "delete from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(login) << " and " << "u_to = " << Q(contact_login) << ") or "
		<< "(u_from = " << Q(contact_login) << " and " << "u_to = " << Q(login) << ");";

	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_invites_list_query(invites_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ 
		&selection.login, 
		&selection.filtration_column_name, 
		&selection.selection_column_name
	});

	ostr
		<< "select login, " << users_name_tb << ".name as name "
		<< "from " << requests_name_tb << " "
		<< "inner join " << users_name_tb << " on " 
		<< users_name_tb << ".login " << " = " << requests_name_tb << "." << selection.selection_column_name << " "
		<< "where " << selection.filtration_column_name << " = " << Q(selection.login) << " "
		<< "limit " << selection.offset << "," << selection.limit << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_contacts_list_query(contacts_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &selection.login });

	ostr
		<< "select login" << "," << "name "
		<< "from " << contacts_name_tb << " "
		<< "inner join " << users_name_tb << " on "
		<< users_name_tb << ".login " << " = " << contacts_name_tb << "." << "u_from" << " or " 
		<< users_name_tb << ".login " << " = " << contacts_name_tb << "." << "u_to" << " "
		<< "where login <> " << Q(selection.login) << " "
		<< "order by name asc" << " "
		<< "limit " << selection.offset << "," << selection.limit << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_chats_tokens_query() {
	std::vector<std::string> res;
	std::ostringstream ostr;

	ostr << "select id from " << chats_name_tb << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_chat_creation_query(chat_creation_params params) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({
		&params.id,
		&params.type,
		&params.title
	});

	for (auto& id : params.default_users_ids) {
		to_mysql_format({ &id });
	}

	ostr << "insert into " << chats_name_tb << "(id, type, size, title) values("
		<< Q(params.id) << "," << Q(params.type) << "," << params.size << ",";

	if (params.title.empty()) {
		ostr << "null";
	}
	else {
		ostr << Q(params.title);
	}

	ostr << ");";

	reset(ostr, res);

	ostr << "insert into " << users_in_chats_name_tb << "(user_id, chat_id) values";

	for (auto i = 0; i < params.default_users_ids.size(); ++i) {
		if (i) {
			ostr << ",";
		}

		ostr << "(" << Q(params.default_users_ids[i]) << "," << Q(params.id) << ")";
	}
	
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_chats_count_query(std::string id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &id });

	ostr << "select count(*) as count "
		<< "from " << users_in_chats_name_tb << " "
		<< "where user_id = " << Q(id) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_is_chat_id_valid(std::string chat_id, std::string inviter_id, std::string user_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &chat_id, &inviter_id });

	ostr << "select id "
		<< "from " << chats_name_tb << " "
		<< "where type = " << Q("public") << " and " << "id = " << Q(chat_id) << ";";
	reset(ostr, res);

	ostr << "select user_id "
		<< "from " << users_in_chats_name_tb << " "
		<< "where (chat_id = " << Q(chat_id) << " and " << "user_id = " << Q(inviter_id) << ");";
	reset(ostr, res);

	ostr << "select user_id "
		<< "from " << users_in_chats_name_tb << " "
		<< "where chat_id = " << Q(chat_id) << " and " << "user_id = " << Q(user_id) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_add_user_to_chat(std::string chat_id, std::string user_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &chat_id, &user_id });

	ostr << "insert into " << users_in_chats_name_tb << "(user_id, chat_id) values "
		<< "(" << Q(user_id) << "," << Q(chat_id) << ");";
	reset(ostr, res);

	return res;
}

FARCONN_NAMESPACE_END