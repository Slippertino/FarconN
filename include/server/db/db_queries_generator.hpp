#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include "../general/tools/utf8_encoder.hpp"
#include "../middleware/entities/entities.hpp"
#include "../../general/tools/macro.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

#define Q(name) "\"" << name << "\""

class db_queries_generator {
public:
	static std::vector<std::string> get_db_init_queries(std::string);
	static std::vector<std::string> get_users_tokens_query();
	static std::vector<std::string> get_users_searching_list_query(const std::string&, std::vector<std::string>);
	static std::vector<std::string> get_add_user_query(std::string, std::string, std::string);
	static std::vector<std::string> get_login_user_queries(std::string, std::string);
	static std::vector<std::string> get_logout_user_query(std::string);
	static std::vector<std::string> get_users_relations_query(std::string, std::string);
	static std::vector<std::string> get_user_profile_data_query(std::string);
	static std::vector<std::string> get_user_profile_to_update_query(std::string, user_profile);

	static std::vector<std::string> get_check_request_existence_query(std::string, std::string);
	static std::vector<std::string> get_create_request_query(std::string, std::string);
	static std::vector<std::string> get_delete_request_query(std::string, std::string);

	static std::vector<std::string> get_check_contact_existence_query(std::string, std::string);
	static std::vector<std::string> get_create_contact_query(std::string, std::string);
	static std::vector<std::string> get_delete_contact_query(std::string, std::string);

	static std::vector<std::string> get_invites_list_query(invites_selection);
	static std::vector<std::string> get_contacts_list_query(contacts_selection);

private:
	static void to_mysql_format(const std::initializer_list<std::string*>&);
	static void reset(std::ostringstream&, std::vector<std::string>&);

private:
	static std::string users_name_tb;
	static std::string contacts_name_tb;
	static std::string requests_name_tb;
};

FARCONN_NAMESPACE_END