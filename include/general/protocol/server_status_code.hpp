#pragma once

#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

enum class server_status_code {
	OKEY							  = 200,

	SIGNUP__EXISTING_LOGIN_ERROR	  = 400,
	SIGNUP__INVALID_LOGIN_ERROR		  = 401,
	SIGNUP__INVALID_PASSWORD_ERROR    = 402,

	LOGIN__NOT_EXISTEN_LOGIN_ERROR	  = 403,
	LOGIN__INVALID_PASSWORD_ERROR	  = 404,

	INTERNAL_SERVER_ERROR			  = 500,
};

FARCONN_NAMESPACE_END