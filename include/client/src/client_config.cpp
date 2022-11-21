#include "../config/client_config.hpp"

FARCONN_NAMESPACE_BEGIN(client)

std::string client_config::get_downloads_storage_path() const {
	return downloads_storage_path;
}

FARCONN_NAMESPACE_END