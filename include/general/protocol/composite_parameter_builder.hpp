#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class composite_parameter_builder {
public:
	composite_parameter_builder() = delete;
    composite_parameter_builder(const std::vector<std::string>& keys) : keys(keys) 
	{ }

	template<class...Args>
	std::string build(const Args&... args) {
		current_key_it = keys.cbegin();
		set_value(args...);
		return model.dump(-1);
	}

private:
	template<class T, class...Args>
	void set_value(const T& value, const Args&... args) {
		if (current_key_it == keys.cend()) {
			current_key_it = keys.cbegin();
		}

		model[*current_key_it++] = value;

		if constexpr (sizeof...(args) > 0) {
			set_value(args...);
		}
	}

private:
	nlohmann::json model;
	std::vector<std::string> keys;
	std::vector<std::string>::const_iterator current_key_it;
};

FARCONN_NAMESPACE_END