#pragma once

#include <iostream>
#include "include/general/protocol/command_analyzer.hpp"
#include "include/general/tools/utf8_encoder.hpp"
#include "include/general/network/networking.hpp"
#include "include/general/network/client_networking_base.hpp"
#include <mysql/jdbc.h>
#include "include/general/validators/format_validators.hpp"
#include <cpp_events/event.hpp>
#include <regex>
#include "include/general/protocol v2.0/protocol_interpreter.hpp"

using namespace farconn::general;

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	std::string command = "profile_set -l -p login password dafadsfasdf";
	const std::string& s = "login likromax qwerty123.";

	auto parsed = protocol_interpreter::interpret(s);
	auto derived =
		std::static_pointer_cast<command_request>(parsed);

	std::regex reg_com = std::regex(R"((^\s*(\S+)))");
	std::regex reg_opt = std::regex(R"((-(\S+)))");
	std::regex reg_param = std::regex(R"(^\s*\S+(\s+-\S+)*\s*.*(\b\w+\b).*)");

	std::vector<std::string> com{
		std::sregex_token_iterator{command.begin(), command.end(), reg_com , 2},
		std::sregex_token_iterator{} 
	};

	std::vector<std::string> opts{
		std::sregex_token_iterator{command.begin(), command.end(), reg_opt, 2},
		std::sregex_token_iterator{}
	};

	std::vector<std::string> params{
		std::sregex_token_iterator{command.begin(), command.end(), reg_param, 2},
		std::sregex_token_iterator{}
	};

	return 0;
}