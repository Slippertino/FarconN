#include "../middleware/server_middleware.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

server_middleware::server_middleware(const database_config& db_config, std::string files_storage_path) :
	database(db_config, files_storage_path) 
{ }

void server_middleware::setup() {
	database.setup();
}

void server_middleware::handle(const command_entity* in, command_response* out) {
	SERVER_ASSERT_EX(out, command_handlers.find(in->command) == command_handlers.end(), server_status_code::SYS__INVALID_COMMAND_ERROR)

	command_handlers.at(in->command)(this, in, out);
}

const std::unordered_map<std::string, std::function<void(server_middleware*, const command_entity*, command_response*)>> server_middleware::command_handlers =
{
	{ "signup",         [&](server_middleware* main, const command_entity* in, command_response* out) { signup_handler(main, in, out).handle();         } },
	{ "login",          [&](server_middleware* main, const command_entity* in, command_response* out) { login_handler(main, in, out).handle();          } },
	{ "logout",         [&](server_middleware* main, const command_entity* in, command_response* out) { logout_handler(main, in, out).handle();         } },
	{ "profile_get",    [&](server_middleware* main, const command_entity* in, command_response* out) { profile_get_handler(main, in, out).handle();    } },
	{ "profile_set",    [&](server_middleware* main, const command_entity* in, command_response* out) { profile_set_handler(main, in, out).handle();    } },
	{ "invite",         [&](server_middleware* main, const command_entity* in, command_response* out) { invite_handler(main, in, out).handle();         } },
	{ "invites_list",   [&](server_middleware* main, const command_entity* in, command_response* out) { invites_list_handler(main, in, out).handle();   } },
	{ "contacts_list",  [&](server_middleware* main, const command_entity* in, command_response* out) { contacts_list_handler(main, in, out).handle();  } },
	{ "contact_delete", [&](server_middleware* main, const command_entity* in, command_response* out) { contact_delete_handler(main, in, out).handle(); } },
	{ "search",			[&](server_middleware* main, const command_entity* in, command_response* out) { search_handler(main, in, out).handle();			} },
};

FARCONN_NAMESPACE_END