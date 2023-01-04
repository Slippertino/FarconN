#pragma once

#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "ex_user_info.hpp"
#include "ex_message_info.hpp"
#include "ex_chat_info.hpp"
#include "../../tools/utf8_encoder.hpp"
#include "../../tools/encoders.hpp"

FARCONN_NAMESPACE_BEGIN(general)

template<class T>
struct ex_info {
	T data;

	void to_encoding(const encoding_func_t& enc_func) {
		encoder<decltype(data)>().encode(data, enc_func);
	}

	std::string to_string() {
		to_encoding(&utf8_encoder::from_local_to_utf8);

		nlohmann::json js;
		nlohmann::to_json(js, data);

		auto result = js.dump();
		utf8_encoder::from_utf8_to_local(result);

		to_encoding(&utf8_encoder::from_utf8_to_local);

		return result;
	}
};

template<class T>
using ex_selection			= std::unordered_map<std::string, T>;

using ex_user_profile		= ex_info<std::unordered_map<std::string, std::string>>;
using ex_invitations_info	= ex_info<std::unordered_map<std::string, std::map<size_t, ex_user_info>>>;
using ex_contacts_info		= ex_info<std::map<size_t, ex_user_info>>;
using ex_searching_results	= ex_info<std::map<size_t, ex_user_info>>;
using ex_chat_messages_info = ex_info<std::map<size_t, ex_message_info>>;
using ex_chats_info			= ex_info<std::map<size_t, ex_chat_info>>;
using ex_chat_party_info	= ex_info<std::map<size_t, ex_user_info>>;

FARCONN_NAMESPACE_END
