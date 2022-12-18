#pragma once

#include <string>
#include <optional>
#include <limits>
#include "../general/tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(server)

#undef max

struct selection_base {
	uint64_t offset = 0;
	uint64_t limit  = std::numeric_limits<uint64_t>::max();
};

struct invites_selection : selection_base {
	std::string invitation_type_name;
	std::string login;
	std::string selection_column_name;
	std::string filtration_column_name;
};

struct contacts_selection : selection_base { 
	std::string login;
};

FARCONN_NAMESPACE_END