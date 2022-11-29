#pragma once

#include <filesystem>
#include "../include/client/client.hpp"

using namespace farconn::client;

const std::filesystem::path cfg_path = std::filesystem::path(R"(..\\..\\..\\example\\configs\\client_cfg.json)");

int main() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	client_config config;

	config.load(cfg_path);

	client client(config);

	client.run();
}