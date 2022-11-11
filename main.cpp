#include <iostream>
#include "include/general/protocol/command_analyzer.hpp"
#include "include/general/protocol/composite_parameter_builder.hpp"
#include "include/general/tools/utf8_encoder.hpp"
#include "include/general/network/networking.hpp"

int main() {
	setlocale(LC_ALL, "rus");

	std::string msg = "Привет!";
	std::cout << msg << std::endl;

	farconn::general::utf8_encoder::from_local_to_utf8(msg);
	std::cout << msg << std::endl;

	farconn::general::utf8_encoder::from_utf8_to_local(msg);
	std::cout << msg << std::endl;
	
	farconn::general::command_analyzer anal;
	std::cout <<
		anal
		.set_id("sdfsdfsdf")
		.set_command("sdfsdf")
		.push_parameter(
			farconn::general::composite_parameter_builder({"age", "name"}).build(18, "Ilya")
		).to_string()
		<< std::endl;
	return 0;
}