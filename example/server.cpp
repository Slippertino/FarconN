#pragma once

#include <filesystem>
#include "../include/server/server.hpp"

using namespace farconn::server;

const std::filesystem::path cfg_path = std::filesystem::path(R"(..\\..\\..\\example\\configs\\server_cfg.json)");

int main() {
	setlocale(LC_ALL, "ru");

	server_config config;

	config.load(cfg_path);

	server server(config);

	server.run();
}