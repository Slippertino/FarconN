#pragma once

#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "ex_user_info.hpp"
#include "ex_message_info.hpp"
#include "ex_chat_info.hpp"

FARCONN_NAMESPACE_BEGIN(general)

template<class T>
using ex_selection			= std::unordered_map<std::string, T>;

using ex_invitations_info   = std::unordered_map<std::string, ex_selection<ex_user_info>>;
using ex_contacts_info      = ex_selection<ex_user_info>;
using ex_searching_results  = std::unordered_map<size_t, ex_user_info>;
using ex_chat_messages_info = ex_selection<ex_message_info>;
using ex_chats_info		    = ex_selection<ex_chat_info>;
using ex_chat_party_info	= ex_selection<ex_user_info>;

FARCONN_NAMESPACE_END
