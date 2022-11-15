#include "../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

server_middleware::server_middleware(const database_config& db_config, std::string files_storage_path) :
	database(db_config, files_storage_path) 
{ }

FARCONN_NAMESPACE_END