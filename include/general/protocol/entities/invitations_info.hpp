#pragma once

#include <unordered_map>
#include <nlohmann/json.hpp>
#include "primitive_user_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

using invitations_info = std::unordered_map<std::string, std::vector<primitive_user_info>>;
using contacts_info    = std::vector<primitive_user_info>;

FARCONN_NAMESPACE_END