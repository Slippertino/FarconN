#include "../middleware/entities/user_profile.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const std::unordered_map<users_relations_type, std::unordered_set<std::string>> user_profile::excluded_fields_map = {
	{ users_relations_type::SELF,     { }                         },
	{ users_relations_type::CONTACTS, { "pp" }                    },
	{ users_relations_type::NONE,     { "pp", "pph", "pe", "ps" } },
};

const std::map<size_t, std::string> user_profile::fields_order = {
	{ 0, "pl" },
	{ 1, "pp" },
	{ 2, "pn" },
	{ 3, "pph" },
	{ 4, "pe" },
	{ 5, "ps" },
};

void user_profile::apply_filter(users_relations_type type) {
	auto& excluded = excluded_fields_map.at(type);

	for (auto& cur : excluded) {
		auto& field = fields.at(cur);
		field.value.reset();
	}
}

FARCONN_NAMESPACE_END