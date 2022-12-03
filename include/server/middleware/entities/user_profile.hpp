#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include "../../../general/validators/format_validators.hpp"
#include "../../../general/tools/macro.hpp"
#include "users_relations_type.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

struct user_profile {
private:
	struct field_description {
		std::string name;
		std::optional<std::string> value;
		std::function<bool(const std::string&)> validator;
	};

public:
	static const std::map<size_t, std::string> fields_order;

	std::map<std::string, field_description> fields = {
		{ "pl",  { "login",    std::nullopt, &login_format_validator::is_valid    } },
		{ "pp",  { "password", std::nullopt, &password_format_validator::is_valid } },
		{ "pn",  { "name",     std::nullopt, &name_format_validator::is_valid     } },
		{ "pph", { "phone",    std::nullopt, &phone_format_validator::is_valid    } },
		{ "pe",  { "email",    std::nullopt, &email_format_validator::is_valid    } },
		{ "ps",  { "self",     std::nullopt, &self_format_validator::is_valid     } },
	};

public:
	void apply_filter(users_relations_type);

private:
	static const std::unordered_map<users_relations_type, std::unordered_set<std::string>> excluded_fields_map;
};

FARCONN_NAMESPACE_END