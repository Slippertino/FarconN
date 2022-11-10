#include <iostream>
#include "include/general/protocol/command_analyzer.hpp"
#include "include/general/protocol/composite_parameter_builder.hpp"

int main() {
	std::cout << "Hello world!" << std::endl;
	
	command_analyzer anal;
	std::cout <<
		anal
		.set_id("sdfsdfsdf")
		.set_command("sdfsdf")
		.push_parameter(
			composite_parameter_builder({"age", "name"}).build(18, "Ilya")
		).to_string()
		<< std::endl;
	return 0;
}