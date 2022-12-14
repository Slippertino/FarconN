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

std::vector<std::string> db_queries_generator::get_sys_db_init_queries(std::string db_name) {
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
		<< "index u_from_index(u_from(50)),"
		<< "index u_to_index(u_to(50)),"
		<< "foreign key (u_from) references " << users_name_tb << "(id) on delete set null on update no action,"
		<< "foreign key (u_to) references " << users_name_tb << "(id) on delete set null on update no action);";
	reset(ostr, res);
	
	ostr << "create table "
		<< "if not exists " << requests_name_tb << " ("
		<< "u_from varchar(50),"
		<< "u_to varchar(50),"
		<< "index u_from_index(u_from(50)),"
		<< "index u_to_index(u_to(50)),"
		<< "foreign key (u_from) references " << users_name_tb << "(id) on delete set null on update no action,"
		<< "foreign key (u_to) references " << users_name_tb << "(id) on delete set null on update no action);";
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
		<< "id varchar(50) primary key,"
		<< "chat_id varchar(50) not null,"
		<< "sender_id varchar(50) null,"
		<< "time datetime(3) not null,"
		<< "type enum(" << Q("text") << "," << Q("file") << ") not null,"
		<< "content text not null,"
		<< "unique index message_index(chat_id(50), sender_id(50), time),"
		<< "foreign key (chat_id) references " << chats_name_tb << "(id) on delete cascade on update no action,"
		<< "foreign key (sender_id) references " << users_name_tb << "(id) on delete set null on update no action);";
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

std::vector<std::string> db_queries_generator::get_sys_logout_users_query() {
	std::vector<std::string> res;
	std::ostringstream ostr;

	ostr << "update " << users_name_tb << " "
		<< "set online = false";
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

std::vector<std::string> db_queries_generator::get_users_searching_list_query(std::string user_id, std::vector<std::string> columns) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &user_id });
	for (auto& col : columns) {
		to_mysql_format({ &col });
	}

	ostr << "select " << columns[0];
	for (auto i = 1; i < columns.size(); ++i) {
		ostr << "," << columns[i];
	}
	ostr << " from " << users_name_tb << " where id <> " << Q(user_id) << " and id not in (";

	ostr 
		<< "select u_from from " << contacts_name_tb << " where u_to = " << Q(user_id) << " union "
		<< "select u_to from " << contacts_name_tb << " where u_from = " << Q(user_id) << " union "
		<< "select u_from from " << requests_name_tb << " where u_to = " << Q(user_id) << " union "
		<< "select u_to from " << requests_name_tb << " where u_from = " << Q(user_id) << ")";

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

std::vector<std::string> db_queries_generator::get_users_relations_query(std::string u1, std::string u2) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &u1, &u2 });

	ostr << "select * from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(u1) << " and u_to = " << Q(u2) << ") or "
		<< "(u_from = " << Q(u2) << " and u_to = " << Q(u1) << ");";
	reset(ostr, res);

	ostr << "select * from " << requests_name_tb << " where "
		<< "(u_from = " << Q(u1) << " and u_to = " << Q(u2) << ") or "
		<< "(u_from = " << Q(u2) << " and u_to = " << Q(u1) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_profile_data_query(std::string id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &id });

	ostr << "select * from " << users_name_tb << " where "
		 << "id = " << Q(id) << ";";
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

std::vector<std::string> db_queries_generator::get_check_request_existence_query(std::string ufrom, std::string uto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &ufrom, &uto });

	ostr << "select * from " << requests_name_tb << " where "
		 << "u_from = " << Q(ufrom) << " and "
		 << "u_to = " << Q(uto) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_create_request_query(std::string ufrom, std::string uto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &ufrom, &uto });

	ostr << "insert into " << requests_name_tb << "(u_from, u_to) values "
		<< "( " << Q(ufrom) << "," << Q(uto) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_delete_request_query(std::string ufrom, std::string uto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &ufrom, &uto });

	ostr << "delete from " << requests_name_tb << " where "
		 << "u_from = " << Q(ufrom) << " and "
		 << "u_to = " << Q(uto) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_check_contact_existence_query(std::string id, std::string contact_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &id, &contact_id });

	ostr << "select * from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(id) << " and " << "u_to = " << Q(contact_id) << ") or "
		<< "(u_from = " << Q(contact_id) << " and " << "u_to = " << Q(id) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_create_contact_query(std::string ufrom, std::string uto) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &ufrom, &uto });

	ostr << "insert into " << contacts_name_tb << "(u_from, u_to) values "
		<< "( " << Q(ufrom) << "," << Q(uto) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_delete_contact_query(std::string id, std::string contact_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &id, &contact_id });

	ostr << "delete from " << contacts_name_tb << " where "
		<< "(u_from = " << Q(id) << " and " << "u_to = " << Q(contact_id) << ") or "
		<< "(u_from = " << Q(contact_id) << " and " << "u_to = " << Q(id) << ");";

	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_invites_list_query(invites_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ 
		&selection.user_id, 
		&selection.filtration_column_name, 
		&selection.selection_column_name
	});

	ostr
		<< "select login, " << users_name_tb << ".name as name," << users_name_tb << ".id as id "
		<< "from " << requests_name_tb << " "
		<< "inner join " << users_name_tb << " on " 
		<< users_name_tb << ".id " << " = " << requests_name_tb << "." << selection.selection_column_name << " "
		<< "where " << selection.filtration_column_name << " = " << Q(selection.user_id) << " "
		<< "limit " << selection.offset << "," << selection.limit << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_contacts_list_query(contacts_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &selection.user_id });

	ostr
		<< "select id, login, name "
		<< "from " << contacts_name_tb << " "
		<< "inner join " << users_name_tb << " on "
		<< users_name_tb << ".id " << " = " << contacts_name_tb << "." << "u_from" << " or " 
		<< users_name_tb << ".id " << " = " << contacts_name_tb << "." << "u_to" << " "
		<< "where id <> " << Q(selection.user_id) << " "
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

std::vector<std::string> db_queries_generator::get_check_private_chat_existance_query(std::string uid1, std::string uid2) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &uid1, &uid2 });

	ostr << "select chat_id, count(user_id) as count "
		<< "from " << users_in_chats_name_tb << " "
		<< "inner join " << chats_name_tb << " on " << chats_name_tb << ".id = " << users_in_chats_name_tb << ".chat_id "
		<< "where " << chats_name_tb << ".type = " << Q("private") << " and ("
		<< "user_id = " << Q(uid1) << " or " << "user_id = " << Q(uid2) << ") "
		<< "group by chat_id "
		<< "having count(user_id) > 1";
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

std::vector<std::string> db_queries_generator::get_chat_info_queries(std::string user_id, std::string chat_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &user_id, &chat_id });

	ostr 
		<< "select "
		<< chats_name_tb << ".type as type,"
		<< "case "
		<< "	when " << chats_name_tb << ".type = " << Q("public")
		<< "	then " << chats_name_tb << ".title "
		<< "	when " << chats_name_tb << ".type = " << Q("private")
		<< "	then ("
		<< "		select " << users_name_tb << ".name as name"
		<< "		from " << users_in_chats_name_tb
		<< "		inner join " << users_name_tb << " on " << users_in_chats_name_tb << ".user_id = " << users_name_tb << ".id"
		<< "		where " << users_in_chats_name_tb << ".chat_id = " << chats_name_tb << ".id and " << users_in_chats_name_tb << ".user_id <> " << Q(user_id)
		<< "	)"
		<< "end as title,"
		<< chats_name_tb << ".size as size "
		<< "from " << users_in_chats_name_tb << " "
		<< "inner join " << chats_name_tb << " on " << chats_name_tb << ".id = " << users_in_chats_name_tb << ".chat_id "
		<< "where chat_id = " << Q(chat_id) << " and " << "user_id = " << Q(user_id) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_is_user_chat_participant_query(std::string user_id, std::string chat_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &chat_id });

	ostr << "select user_id "
		<< "from " << users_in_chats_name_tb << " "
		<< "where chat_id = " << Q(chat_id) << " and " << "user_id = " << Q(user_id) << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_add_user_to_chat_queries(std::string chat_id, std::string user_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &chat_id, &user_id });

	ostr << "insert into " << users_in_chats_name_tb << "(user_id, chat_id) values "
		<< "(" << Q(user_id) << "," << Q(chat_id) << ");";
	reset(ostr, res);

	ostr << "update " << chats_name_tb << " "
		<< "set size = size + 1 "
		<< "where id = " << Q(chat_id);
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_exclude_user_from_chat_queries(std::string user_id, std::string chat_id) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &chat_id, &user_id });

	ostr << "delete from " << users_in_chats_name_tb << " "
		<< "where user_id = " << Q(user_id) << " and " << "chat_id = " << Q(chat_id) << ";";
	reset(ostr, res);

	ostr << "update " << chats_name_tb << " "
		<< "set size = size - 1 "
		<< "where id = " << Q(chat_id);
	reset(ostr, res);

	ostr << "delete from " << chats_name_tb << " "
		<< "where id = " << Q(chat_id) << " and ("
		<< "type = " << Q("public") << " and " << "size = 0" << " or "
		<< "type = " << Q("private") << " and " << "size = 1)";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_post_message_query(chat_post_params params) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ 
		&params.id,
		&params.sender_id,
		&params.chat_id,
		&params.type,
		&params.content
	});

	ostr
		<< "insert into messages values("
		<< Q(params.id) << ","
		<< Q(params.chat_id) << ","
		<< Q(params.sender_id) << ","
		<< "sysdate(3)" << ","
		<< Q(params.type) << ","
		<< Q(params.content) << ");";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_messages_tokens_query() {
	std::vector<std::string> res;
	std::ostringstream ostr;

	ostr << "select id from " << messages_name_tb << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_messages_list_query(chat_messages_selection params) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({
		&params.chat_id
	});

	ostr
		<< "select "
		<< messages_name_tb << ".id,"
		<< "unix_timestamp(" << messages_name_tb << ".time) as time,"
		<< messages_name_tb << ".type,"
		<< messages_name_tb << ".content,"
		<< users_name_tb << ".name "
		<< "from " << messages_name_tb << " "
		<< "inner join " << users_name_tb << " on " << messages_name_tb << ".sender_id = " << users_name_tb << ".id "
		<< "where " << messages_name_tb << ".chat_id = " << Q(params.chat_id) << " ";

	if (params.last_actual_message_id.has_value()) {
		auto last = params.last_actual_message_id.value();
		to_mysql_format({ &last });

		ostr << " and unix_timestamp(" << messages_name_tb << ".time) > ("
			<< "select unix_timestamp(time) from " << messages_name_tb << " where id = " << Q(last) << ") ";
	}

	ostr << "order by time desc "
		<< "limit " << params.offset << "," << params.limit << ";";
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_user_chats_tokens_query(chats_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &selection.user_id });

	ostr << "select chat_id as id "
		<< "from " << users_in_chats_name_tb << " "
		<< "where user_id = " << Q(selection.user_id) << " "
		<< "limit " << selection.offset << "," << selection.limit;
	reset(ostr, res);

	return res;
}

std::vector<std::string> db_queries_generator::get_chat_party_query(chat_party_selection selection) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &selection.chat_id, &selection.user_id });

	ostr << "select id, login, name "
		<< "from " << users_in_chats_name_tb << " "
		<< "inner join " << users_name_tb << " on " << users_in_chats_name_tb << ".user_id = " << users_name_tb << ".id "
		<< "where chat_id = " << Q(selection.chat_id) << " and user_id <> " << Q(selection.user_id) << " "
		<< "limit " << selection.offset << "," << selection.limit;
	reset(ostr, res);

	return res;
}

FARCONN_NAMESPACE_END