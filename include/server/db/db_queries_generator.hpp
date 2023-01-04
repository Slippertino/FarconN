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
	static std::vector<std::string> get_sys_db_init_queries(std::string);
	static std::vector<std::string> get_sys_logout_users_query();

	static std::vector<std::string> get_users_tokens_query();
	static std::vector<std::string> get_user_id_by_login_query(std::string);
	static std::vector<std::string> get_users_searching_list_query(std::string, std::vector<std::string>);
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

	static std::vector<std::string> get_chats_tokens_query();
	static std::vector<std::string> get_check_private_chat_existance_query(std::string, std::string);
	static std::vector<std::string> get_chat_creation_query(chat_creation_params);
	static std::vector<std::string> get_user_chats_count_query(std::string);
	static std::vector<std::string> get_chat_info_queries(std::string, std::string);
	static std::vector<std::string> get_is_user_chat_participant_query(std::string, std::string);
	static std::vector<std::string> get_exclude_user_from_chat_queries(std::string, std::string);
	static std::vector<std::string> get_add_user_to_chat_queries(std::string, std::string);
	static std::vector<std::string> get_post_message_query(chat_post_params);
	static std::vector<std::string> get_messages_tokens_query();
	static std::vector<std::string> get_messages_list_query(chat_messages_selection);
	static std::vector<std::string> get_user_chats_tokens_query(chats_selection);
	static std::vector<std::string> get_chat_party_query(chat_party_selection);

private:
	static void to_mysql_format(const std::initializer_list<std::string*>&);
	static void reset(std::ostringstream&, std::vector<std::string>&);

private:
	static std::string users_name_tb;
	static std::string contacts_name_tb;
	static std::string requests_name_tb;
	static std::string chats_name_tb;
	static std::string messages_name_tb;
	static std::string users_in_chats_name_tb;
};

FARCONN_NAMESPACE_END