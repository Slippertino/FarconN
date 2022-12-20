#pragma once

#include <optional>
#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class profile_handler : public handler {
public:
	profile_handler(class server_middleware*, const command_entity*, command_response*, bool);

protected:
	void apply_filter(users_relations_type);

	virtual bool is_command_valid() override = 0;
	virtual void execute() override = 0;

protected:
	static const std::unordered_map<users_relations_type, std::unordered_set<std::string>> excluded_profile_fields_map;
	static const std::map<size_t, std::string> profile_fields_order;

	user_profile profile_fields = {
		{ "pl",  { "login",    std::nullopt, &user_login_format_validator::is_valid    } },
		{ "pp",  { "password", std::nullopt, &user_password_format_validator::is_valid } },
		{ "pn",  { "name",     std::nullopt, &user_name_format_validator::is_valid     } },
		{ "pph", { "phone",    std::nullopt, &user_phone_format_validator::is_valid    } },
		{ "pe",  { "email",    std::nullopt, &user_email_format_validator::is_valid    } },
		{ "ps",  { "self",     std::nullopt, &user_self_format_validator::is_valid     } },
	};
};

FARCONN_NAMESPACE_END