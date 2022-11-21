#include "../config/server_config.hpp"

FARCONN_NAMESPACE_BEGIN(server)

std::string server_config::get_files_storage_path() const {
	return files_storage_path;
}

database_config server_config::get_db_config() const {
	return db_config;
}

FARCONN_NAMESPACE_END