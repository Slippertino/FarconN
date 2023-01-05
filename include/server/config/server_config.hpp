#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../../general/config/config.hpp"
#include "database_config.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class server_config : public farconn::general::config<server_config> {
public:
	server_config() = default;

	std::string get_files_storage_path() const;
	std::string get_docs_storage_path() const;
	database_config get_db_config() const;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		server_config,
		ipv4,
		port,
		files_storage_path,
		docs_storage_path,
		db_config
	)

private:
	std::string files_storage_path;
	std::string docs_storage_path;
	database_config db_config;
};

FARCONN_NAMESPACE_END