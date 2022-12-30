#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>
#include <nlohmann/json.hpp>
#include "../../../general/tools/macro.hpp"
#include "../general/protocol/entities/entities.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

enum class users_relations_type {
	NONE,
	CONTACTS,
	REQUESTS,
	SELF
};

struct field_description {
	std::string name;
	std::optional<std::string> value;
	std::function<bool(const std::string&)> validator;
};

using user_profile = std::map<std::string, field_description>;

struct user_info {
	std::string id;
	std::string login;
	std::string name;

	ex_user_info to_external() const {
		return ex_user_info{
			id,
			login,
			name
		};
	}
};

FARCONN_NAMESPACE_END