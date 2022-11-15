#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Rpc.h>
#include <string>
#include <functional>
#include "../../general/tools/macro.hpp"

#pragma comment(lib, "Rpcrt4.lib")

FARCONN_NAMESPACE_BEGIN(server)

class token_generator {
public:
	static std::string generate(std::function<bool(const std::string&)>);
};

FARCONN_NAMESPACE_END