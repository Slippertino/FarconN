#pragma once

#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "external_user_info.hpp"
#include "external_message_info.hpp"
#include "external_chat_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

using ex_invitations_info   = std::unordered_map<std::string, std::vector<external_user_info>>;
using ex_contacts_info      = std::vector<external_user_info>;
using ex_searching_results  = std::unordered_map<size_t, external_user_info>;
using ex_chat_messages_info = std::vector<external_message_info>;
using ex_chats_info		    = std::vector<external_chat_info>;

FARCONN_NAMESPACE_END
