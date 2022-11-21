#pragma once

#include <filesystem>
#include "../include/client/client.hpp"

using namespace farconn::client;

const std::filesystem::path cfg_path = std::filesystem::path(R"(..\\..\\..\\example\\configs\\client_cfg.json)");

int main() {
	setlocale(LC_ALL, "ru");

	client_config config;

	config.load(cfg_path);

	client client(config);

	client.run();
}