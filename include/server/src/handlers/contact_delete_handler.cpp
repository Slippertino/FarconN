#pragma once

#include "../../middleware/handlers/contact_delete_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

contact_delete_handler::contact_delete_handler(server_middleware* sm, const command_entity* ce, command_response* cr) : 
	handler(sm, ce, cr, true)
{ }

bool contact_delete_handler::is_command_valid() {
	return in->params.size() == 2;
}

void contact_delete_handler::execute() {
	bool is_contact_exist;
	
	auto& id = session->native_token;
	auto& contact_id = in->params[1];

	SERVER_ASSERT(out, db->check_contact_existence(id, contact_id, is_contact_exist) != SUCCESS)
	SERVER_ASSERT_EX(out, !is_contact_exist, server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR)
	SERVER_ASSERT(out, db->delete_contact(id, contact_id) != SUCCESS)
	SERVER_ASSERT_EX(out, true, SUCCESS)
}

FARCONN_NAMESPACE_END