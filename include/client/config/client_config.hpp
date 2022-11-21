#pragma once

#include "../../general/config/config.hpp"

FARCONN_NAMESPACE_BEGIN(client)

class client_config : public farconn::general::config<client_config>{
public:
	client_config() = default;

	std::string get_downloads_storage_path() const;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		client_config,
		ipv4,
		port,
		downloads_storage_path
	)

private:
	std::string downloads_storage_path;
};

FARCONN_NAMESPACE_END