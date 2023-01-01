#pragma once

#include <fstream>
#include <iostream>
#include "include/general/tools/utf8_encoder.hpp"
#include "include/general/network/networking.hpp"
#include "include/general/network/client_networking_base.hpp"
#include <mysql/jdbc.h>
#include "include/general/validators/format_validators.hpp"
#include <cpp_events/event.hpp>
#include <regex>
#include "include/general/protocol/protocol_interpreter.hpp"
#include "include/server/middleware/entities/entities.hpp"
#include "include/server/tools/query_ranking_estimator.hpp"

using namespace farconn::general;
using namespace farconn::server;
using namespace std;

std::filesystem::path p = std::filesystem::path(R"(D:\\farconn_files_storage\\folder)");
int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*std::filesystem::create_directories(p);



	auto input = 1447431619.012;

	auto r = chrono::system_clock::to_time_t(
		chrono::system_clock::time_point(
			chrono::duration_cast<chrono::seconds>(
				chrono::duration<double>(input)
				)));
	auto tm = *std::localtime(&r);
	auto d = p.c_str();
	std::ofstream of;
	of.open(p / std::filesystem::path("input.txt"), std::ios_base::app);

	of << "sdafsdfasdf";

	of.close();*/

	std::string q = "chat_create -public 24-24-asfs-324 <��� ���>";
	std::string r = "login likromax qwerty123.";
	std::string r1 = "chats_list -any 400 <�������� �������!>";
	auto res = protocol_interpreter::interpret(r1);
	std::cout << query_ranking_estimator::estimate_query_ranking("slaip", "slippert") << std::endl;
	/*farconn::server::user_profile p;

	std::string name = "{[\"dfksdajf;kasdjfdsa adsf kasdj fasd\"]}";

	//nlohmann::json js = name;

	nlohmann::json js;
	//js["value"] = "dfksdajf;kasdjfdsa adsf kasdj fasd";

	//auto name_copy = js.dump();
	js.parse(name);

	//js.parse(js.dump());

	//std::string r;
	//std::cout << js.dump() << std::endl;

	std::string v = js.get<std::string>();
	std::cout << js["value"] << std::endl;

	std::string command = "profile_set -l -p login password dafadsfasdf";
	const std::string& s = "login likromax qwerty123.";

	auto parsed = protocol_interpreter::interpret(s);

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
	};*/

	return 0;
}