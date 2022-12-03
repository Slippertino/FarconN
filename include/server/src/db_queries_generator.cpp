#include "../db/db_queries_generator.hpp"

FARCONN_NAMESPACE_BEGIN(server)

std::string db_queries_generator::users_name_tb    = "users";
std::string db_queries_generator::contacts_name_tb = "contacts";
std::string db_queries_generator::requests_name_tb = "requests";

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

	return res;
}

std::vector<std::string> db_queries_generator::get_users_tokens_query() {
	std::vector<std::string> res;
	std::ostringstream ostr;

	ostr << "select id from " << users_name_tb << ";";
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

std::vector<std::string> db_queries_generator::get_are_users_in_contacts_query(std::string lu1, std::string lu2) {
	std::vector<std::string> res;
	std::ostringstream ostr;

	to_mysql_format({ &lu1, &lu2 });

	ostr << "select * from " << contacts_name_tb << " where "
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

FARCONN_NAMESPACE_END