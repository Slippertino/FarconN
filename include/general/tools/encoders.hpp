#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "utf8_encoder.hpp"
#include "../protocol/entities/ex_chat_info.hpp"
#include "../protocol/entities/ex_message_info.hpp"
#include "../protocol/entities/ex_user_info.hpp"
#include "macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

using encoding_func_t = std::function<void(std::string&)>;

template<class T>
struct encoder {
	void encode(T& obj, const encoding_func_t& func) { }
};

template<>
struct encoder<std::string> {
	void encode(std::string& obj, const encoding_func_t& func) {
		func(obj);
	}
};

template<class TKey, class TValue>
struct encoder<std::unordered_map<TKey, TValue>> {
	void encode(std::unordered_map<TKey, TValue>& obj, const encoding_func_t& func) {
		std::vector<std::pair<TKey, TValue>> temp;

		std::transform(
			std::move_iterator(obj.begin()),
			std::move_iterator(obj.end()),
			std::back_inserter(temp),
			[](auto&& val) { return val; }
		);

		obj.clear();

		for (auto& cur : temp) {
			encoder<TKey>().encode(cur.first, func);
			encoder<TValue>().encode(cur.second, func);
		}

		std::transform(
			std::move_iterator(temp.begin()),
			std::move_iterator(temp.end()),
			std::inserter(obj, obj.begin()),
			[](auto&& val) { return val; }
		);
	}
};

template<class TKey, class TValue>
struct encoder<std::map<TKey, TValue>> {
	void encode(std::map<TKey, TValue>& obj, const encoding_func_t& func) {
		std::vector<std::pair<TKey, TValue>> temp;

		std::transform(
			std::move_iterator(obj.begin()),
			std::move_iterator(obj.end()),
			std::back_inserter(temp),
			[](auto&& val) { return val; }
		);

		obj.clear();

		for (auto& cur : temp) {
			encoder<TKey>().encode(cur.first, func);
			encoder<TValue>().encode(cur.second, func);
		}

		std::transform(
			std::move_iterator(temp.begin()),
			std::move_iterator(temp.end()),
			std::inserter(obj, obj.begin()),
			[](auto&& val) { return val; }
		);
	}
};

template<>
struct encoder<ex_message_info> {
	void encode(ex_message_info& obj, const encoding_func_t& func) {
		encoder<decltype(obj.id)>().encode(obj.id, func);
		encoder<decltype(obj.content)>().encode(obj.content, func);
		encoder<decltype(obj.sender_name)>().encode(obj.sender_name, func);
		encoder<decltype(obj.time)>().encode(obj.time, func);
		encoder<decltype(obj.type)>().encode(obj.type, func);
	}
};

template<>
struct encoder<ex_chat_info> {
	void encode(ex_chat_info& obj, const encoding_func_t& func) {
		encoder<decltype(obj.id)>().encode(obj.id, func);
		encoder<decltype(obj.last_message)>().encode(obj.last_message, func);
		encoder<decltype(obj.name)>().encode(obj.name, func);
		encoder<decltype(obj.size)>().encode(obj.size, func);
	}
};

template<>
struct encoder<ex_user_info> {
	void encode(ex_user_info& obj, const encoding_func_t& func) {
		encoder<decltype(obj.id)>().encode(obj.id, func);
		encoder<decltype(obj.login)>().encode(obj.login, func);
		encoder<decltype(obj.name)>().encode(obj.name, func);
	}
};

FARCONN_NAMESPACE_END