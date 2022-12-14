#include "../middleware/server_middleware.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

void server_middleware::setup(const server_config& config) {
	database.setup(config);
}

void server_middleware::refresh() {
	database.sys_logout_users();
}

void server_middleware::handle(const command_entity* in, command_response* out) {
	SERVER_ASSERT_EX(out, command_handlers.find(in->command) == command_handlers.end(), server_status_code::SYS__INVALID_COMMAND_ERROR)

	command_handlers.at(in->command)(this, in, out);
}

const std::unordered_map<std::string, std::function<void(server_middleware*, const command_entity*, command_response*)>> server_middleware::command_handlers =
{
	{ "signup",				[&](server_middleware* main, const command_entity* in, command_response* out) { signup_handler(main, in, out).handle();				} },
	{ "login",				[&](server_middleware* main, const command_entity* in, command_response* out) { login_handler(main, in, out).handle();				} },
	{ "logout",				[&](server_middleware* main, const command_entity* in, command_response* out) { logout_handler(main, in, out).handle();				} },
	{ "profile_get",		[&](server_middleware* main, const command_entity* in, command_response* out) { profile_get_handler(main, in, out).handle();		} },
	{ "profile_set",		[&](server_middleware* main, const command_entity* in, command_response* out) { profile_set_handler(main, in, out).handle();		} },
	{ "invite",				[&](server_middleware* main, const command_entity* in, command_response* out) { invite_handler(main, in, out).handle();				} },
	{ "invites_list",		[&](server_middleware* main, const command_entity* in, command_response* out) { invites_list_handler(main, in, out).handle();		} },
	{ "contacts_list",		[&](server_middleware* main, const command_entity* in, command_response* out) { contacts_list_handler(main, in, out).handle();		} },
	{ "contact_delete",		[&](server_middleware* main, const command_entity* in, command_response* out) { contact_delete_handler(main, in, out).handle();		} },
	{ "search",				[&](server_middleware* main, const command_entity* in, command_response* out) { search_handler(main, in, out).handle();				} },
	{ "chat_create",		[&](server_middleware* main, const command_entity* in, command_response* out) { chat_create_handler(main, in, out).handle();		} },
	{ "chat_add",			[&](server_middleware* main, const command_entity* in, command_response* out) { chat_add_handler(main, in, out).handle();			} },
	{ "chat_leave",			[&](server_middleware* main, const command_entity* in, command_response* out) { chat_leave_handler(main, in, out).handle();			} },
	{ "chat_post",			[&](server_middleware* main, const command_entity* in, command_response* out) { chat_post_handler(main, in, out).handle();			} },
	{ "chat_messages_list",	[&](server_middleware* main, const command_entity* in, command_response* out) { chat_messages_list_handler(main, in, out).handle();	} },
	{ "chats_list",			[&](server_middleware* main, const command_entity* in, command_response* out) { chats_list_handler(main, in, out).handle();			} },
	{ "chat_party_list",	[&](server_middleware* main, const command_entity* in, command_response* out) { chat_party_list_handler(main, in, out).handle();	} },
	{ "help",				[&](server_middleware* main, const command_entity* in, command_response* out) { help_handler(main, in, out).handle();				} },
};

FARCONN_NAMESPACE_END