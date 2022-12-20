#include "../validators/format_validators.hpp"

FARCONN_NAMESPACE_BEGIN(general)

//User Login
const std::pair<int, int> user_login_format_validator::length_range = { 5, 32 };
const std::regex user_login_format_validator::content_pattern = std::regex(R"(^[A-Za-z0-9]+$)");

//User Password
const std::pair<int, int> user_password_format_validator::length_range = { 7, 32 };
const std::regex user_password_format_validator::content_pattern = std::regex(R"(.*[\.,;!\?&\^@#\$%\*\(\)\[\]{}]+.*)");

//User Name
const std::pair<int, int> user_name_format_validator::length_range = { 1, 50 };
const std::regex user_name_format_validator::content_pattern = std::regex(R"(^[^\.,;!\?&\^@#\$%\*\(\)\[\]{}]+$)");

//User Phone
const std::pair<int, int> user_phone_format_validator::length_range = { 0, 12 };
const std::regex user_phone_format_validator::content_pattern = std::regex(R"(^()|(\+\d{11})$)");

//User Email
const std::pair<int, int> user_email_format_validator::length_range = { 0, 50 };
const std::regex user_email_format_validator::content_pattern = std::regex(R"(^()|(.+@.+\.\w+)$)");

//User Self
const std::pair<int, int> user_self_format_validator::length_range = { 0, 100 };
const std::regex user_self_format_validator::content_pattern = std::regex(R"(^.*$)");

//Chat Name
const std::pair<int, int> chat_name_format_validator::length_range = { 5, 32 };
const std::regex chat_name_format_validator::content_pattern = std::regex(R"(^[^\.,;!\?&\^@#\$%\*\(\)\[\]{}]+$)");

//Message Content
const std::pair<int, int> message_content_format_validator::length_range = { 1, 4096 };
const std::regex message_content_format_validator::content_pattern = std::regex(R"(^.*$)");

FARCONN_NAMESPACE_END