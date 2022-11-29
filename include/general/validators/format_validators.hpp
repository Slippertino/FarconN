#pragma once

#include "format_validator.hpp"

FARCONN_NAMESPACE_BEGIN(general)

class login_format_validator    : public format_validator<login_format_validator>{ };

class password_format_validator : public format_validator<password_format_validator>{ };

class name_format_validator     : public format_validator<name_format_validator> { };

class phone_format_validator    : public format_validator<phone_format_validator> { };

class email_format_validator    : public format_validator<email_format_validator> { };

class self_format_validator     : public format_validator<self_format_validator> { };

FARCONN_NAMESPACE_END