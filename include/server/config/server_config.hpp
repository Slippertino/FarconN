#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "database_config.hpp"

FARCONN_NAMESPACE_BEGIN(server)

struct server_config {
	std::string ipv4;
	uint16_t port;
	std::string files_storage_path;
	database_config db_config;

	void load(const std::filesystem::path&);

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		server_config,
		ipv4,
		port,
		files_storage_path,
		db_config
	)
};

FARCONN_NAMESPACE_END