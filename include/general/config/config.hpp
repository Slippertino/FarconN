#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

template<class Derived>
class config {
public:
	config() = default;

	void load(const std::filesystem::path& config_path) {
		std::ifstream ifstr(config_path);
		nlohmann::json js;

		ifstr >> js;

		from_json(js, *static_cast<Derived*>(this));
	}

	std::string get_ip() const {
		return ipv4;
	}

	uint16_t get_port() const {
		return port;
	}

protected:
	std::string ipv4;
	uint16_t port;
};

FARCONN_NAMESPACE_END