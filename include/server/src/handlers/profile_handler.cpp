#include "../../middleware/handlers/profile_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

profile_handler::profile_handler(class server_middleware* sm, const command_entity* ce, command_response* cr) : handler(sm, ce, cr)
{ }

void profile_handler::apply_filter(users_relations_type type) {
	auto& excluded = excluded_profile_fields_map.at(type);

	for (auto& cur : excluded) {
		auto& field = profile_fields.at(cur);
		field.value.reset();
	}
}

const std::unordered_map<users_relations_type, std::unordered_set<std::string>> profile_handler::excluded_profile_fields_map = {
	{ users_relations_type::SELF,     { }                         },
	{ users_relations_type::CONTACTS, { "pp" }                    },
	{ users_relations_type::NONE,     { "pp", "pph", "pe", "ps" } },
};

const std::map<size_t, std::string> profile_handler::profile_fields_order = {
	{ 0, "pl"  },
	{ 1, "pp"  },
	{ 2, "pn"  },
	{ 3, "pph" },
	{ 4, "pe"  },
	{ 5, "ps"  },
};

FARCONN_NAMESPACE_END