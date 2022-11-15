#include "../config/server_config.hpp"

FARCONN_NAMESPACE_BEGIN(server)

void server_config::load(const std::filesystem::path& config_path) {
	std::ifstream ifstr(config_path);
	nlohmann::json js;

	ifstr >> js;

	from_json(js, *this);
}

FARCONN_NAMESPACE_END