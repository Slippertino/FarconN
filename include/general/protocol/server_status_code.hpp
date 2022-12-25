#pragma once

#include "../tools/macro.hpp"

FARCONN_NAMESPACE_BEGIN(general)

enum class server_status_code {
	SYS__OKEY										= 200,

	SYS__INVALID_COMMAND_ERROR						= 400,
	SYS__INVALID_TOKEN_ERROR						= 401,
	SYS__NONEXISTEN_USER_ERROR						= 402,
	SYS__INVALID_OFFSET_VALUE_ERROR					= 403,
	SYS__INVALID_LIMIT_VALUE_ERROR					= 404,

	SIGNUP__EXISTING_LOGIN_ERROR					= 405,
	SIGNUP__INVALID_LOGIN_ERROR						= 406,
	SIGNUP__INVALID_PASSWORD_ERROR					= 407,

	LOGIN__INVALID_DATA_ERROR						= 408,
	LOGIN__ALREADY_LOGGED_IN_ERROR					= 409,

	PROFILE__INVALID_DATA_TO_SET_ERROR				= 410,

	REQUEST__NONEXISTEN_REQUEST_ERROR				= 411,
	REQUEST__ALREADY_IN_CONTACTS_ERROR				= 412,
	REQUEST__ALREADY_EXIST_ERROR					= 413,

	CONTACT__NONEXISTEN_CONTACT_ERROR				= 414,

	CHAT__INVALID_NAME_ERROR						= 415,
	CHAT__JOINED_CHATS_LIMIT_EXCEEDED_ERROR			= 416,
	CHAT__NONEXISTEN_CHAT_ERROR						= 417,
	CHAT__USER_NOT_A_PARTICIPANT_ERROR				= 418,
	CHAT__USER_ALREADY_IN_CHAT						= 419,
	CHAT__IMPOSSIBLE_ADD_TO_PRIVATE_CHAT_ERROR		= 420,
	CHAT__CHAT_PARTICIPANTS_LIMIT_EXCEEDED_ERROR	= 421,
	CHAT__TEXT_MESSAGE_TOO_LARGE					= 422,
	CHAT__FILE_SIZE_TOO_LARGE						= 423,

	SYS__INTERNAL_SERVER_ERROR						= 500,
};

FARCONN_NAMESPACE_END