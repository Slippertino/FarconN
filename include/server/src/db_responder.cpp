#include "../db/db_responder.hpp"

FARCONN_NAMESPACE_BEGIN(server)

const size_t db_responder::connections_count = 1;

db_responder::db_responder(const database_config& db_config, std::string files_storage_path) : 
	config(db_config), 
	files_storage_path(files_storage_path) 
{ }

db_responder::~db_responder() {
	while (!connections.empty()) {
		auto con = connections.front();
		if (!con->isClosed()) {
			con->close();
		}

		connections.pop();
	}
}

void db_responder::setup() {
	if (!connections.empty()) {
		return;
	}

	for (auto i = 0; i < connections_count; ++i) {
		auto con = std::shared_ptr<sql::Connection>(
			sql::mysql::get_mysql_driver_instance()->connect(
				config.url,
				config.user_name,
				config.password
			)
		);

		con->setSchema(config.database_name);

		connections.push(con);
	}
}

FARCONN_NAMESPACE_END