#include "../../middleware/handlers/chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_handler::chat_handler(
	class server_middleware* sm,
	const command_entity* ce,
	command_response* cr,
	bool auth_required
) : handler(sm, ce, cr, auth_required)
{ }

const std::unordered_map<std::string, chat_type> chat_handler::chat_name_type_mapper = {
	{ "private", chat_type::PRIVATE },
	{ "public",  chat_type::PUBLIC  },
};

const size_t chat_handler::max_chats_count_to_join = 512;

FARCONN_NAMESPACE_END