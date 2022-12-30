#include "../../middleware/handlers/chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_handler::chat_handler(
	class server_middleware* sm,
	const command_entity* ce,
	command_response* cr,
	bool auth_required
) : handler(sm, ce, cr, auth_required)
{ }

time_t chat_handler::convert_seconds_to_time_t(double time) {
	return std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::time_point(
			std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(time))
		)
	);
}

std::string chat_handler::convert_seconds_to_date(double time, const std::string& tmpl) {
	auto t = convert_seconds_to_time_t(time);
	auto tm = *std::localtime(&t);
	return (std::ostringstream() << std::put_time(&tm, tmpl.c_str())).str();
}

const std::unordered_map<std::string, chat_type> chat_handler::chat_name_type_mapper = {
	{ "private", chat_type::PRIVATE },
	{ "public",  chat_type::PUBLIC  },
};

const size_t chat_handler::max_chats_count_to_join = 512;

FARCONN_NAMESPACE_END