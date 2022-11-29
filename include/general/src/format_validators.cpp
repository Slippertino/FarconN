#include "../validators/format_validators.hpp"

FARCONN_NAMESPACE_BEGIN(general)

//Login
const std::pair<int, int> login_format_validator::length_range = { 5, 32 };
const std::regex login_format_validator::content_pattern = std::regex(R"(^[A-Za-z0-9]+$)");

//Password
const std::pair<int, int> password_format_validator::length_range = { 7, 32 };
const std::regex password_format_validator::content_pattern = std::regex(R"(.*[\.,;!\?&\^@#\$%\*\(\)\[\]{}]+.*)");

//Name
const std::pair<int, int> name_format_validator::length_range = { 1, 50 };
const std::regex name_format_validator::content_pattern = std::regex(R"(^[^\.,;!\?&\^@#\$%\*\(\)\[\]{}]+$)");

//Phone
const std::pair<int, int> phone_format_validator::length_range = { 0, 12 };
const std::regex phone_format_validator::content_pattern = std::regex(R"(^()|(\+\d{11})$)");

//Email
const std::pair<int, int> email_format_validator::length_range = { 0, 50 };
const std::regex email_format_validator::content_pattern = std::regex(R"(^()|(.+@.+\.\w+)$)");

//Self
const std::pair<int, int> self_format_validator::length_range = { 0, 100 };
const std::regex self_format_validator::content_pattern = std::regex(R"(^.*$)");

FARCONN_NAMESPACE_END