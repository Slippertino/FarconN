#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class utf8_encoder {
public:
	static void from_local_to_utf8(std::string&);
	static void from_utf8_to_local(std::string&);

	static void from_local_to_utf8_list(const std::initializer_list<std::string*>&);
	static void from_utf8_to_local_list(const std::initializer_list<std::string*>&);

private:
	static void to_unicode(const std::string&, std::wstring&, UINT);
	static void from_unicode(const std::wstring&, std::string&, UINT);
};

FARCONN_NAMESPACE_END