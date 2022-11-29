#pragma once

#include <regex>
#include <string>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

template<class Derived>
class format_validator {
public:
	static bool is_valid(const std::string& src) {
		return is_valid_length(src) && is_valid_content(src);
	}

protected:
	static bool is_valid_length(const std::string& src) {
		auto sz = src.size();

		return length_range.first <= sz && length_range.second >= sz;
	}

	static bool is_valid_content(const std::string& src) {
		return std::regex_match(src, content_pattern);
	}

protected:
	static const std::pair<int, int> length_range;
	static const std::regex content_pattern;
};

FARCONN_NAMESPACE_END