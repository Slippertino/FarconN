#pragma once

#include "format_validator.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class user_login_format_validator		: public format_validator<user_login_format_validator>{ };

class user_password_format_validator	: public format_validator<user_password_format_validator>{ };

class user_name_format_validator		: public format_validator<user_name_format_validator> { };

class user_phone_format_validator		: public format_validator<user_phone_format_validator> { };

class user_email_format_validator		: public format_validator<user_email_format_validator> { };

class user_self_format_validator		: public format_validator<user_self_format_validator> { };

class chat_name_format_validator        : public format_validator<chat_name_format_validator> { };

class message_content_format_validator  : public format_validator<message_content_format_validator> { };

FARCONN_NAMESPACE_END