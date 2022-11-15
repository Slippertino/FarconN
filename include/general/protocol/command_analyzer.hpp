#pragma once

#include <sstream>
#include <nlohmann/json.hpp>
#include "command.hpp"
#include "../tools/utf8_encoder.hpp"
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class command_analyzer {
public:
	command_analyzer();
	command_analyzer& set_id(const std::string&);
	command_analyzer& set_command(const std::string&);

	template<class TParam>
	command_analyzer& push_parameter(const TParam& param) {
		model["params"].push_back(convert_to_string<TParam>(param));
		return *this;
	}

	std::string pop_parameter();
	void clear_parameters();

	std::string to_string() const;
	void to_command(command&) const;

	command_analyzer& parse(std::string&);

private:
	template<class T>
	std::string convert_to_string(const T& arg) {
		return (std::ostringstream() << arg).str();
	}

private:
	nlohmann::json model;
};

FARCONN_NAMESPACE_END