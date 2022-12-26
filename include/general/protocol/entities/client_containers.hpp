#pragma once

#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "primitive_user_info.hpp"
#include "basic_message_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

using invitations_info   = std::unordered_map<std::string, std::vector<primitive_user_info>>;
using contacts_info      = std::vector<primitive_user_info>;
using searching_results  = std::unordered_map<size_t, primitive_user_info>;
using chat_messages_info = std::vector<basic_message_info>;

FARCONN_NAMESPACE_END
