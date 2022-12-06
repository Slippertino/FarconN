#include "../../middleware/handlers/handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

handler::handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	main(sm), 
	in(const_cast<command_entity*>(ce)), 
	out(cr)
{ }

void handler::handle() {
	if (in->command == "invite") {
		int b;
	}

	SERVER_ASSERT_EX(out, !is_command_valid(), server_status_code::SYS__INVALID_COMMAND_ERROR)

	execute();
}

FARCONN_NAMESPACE_END
