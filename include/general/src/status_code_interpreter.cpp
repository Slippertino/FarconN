#include "../protocol/status_code_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::string status_code_interpreter::unknown_code_description = "Неизвестный код состояния!";

const std::unordered_map<server_status_code, std::string> status_code_interpreter::code_descriptions =
{
	{ server_status_code::OKEY,				              "ОК"			                  },

	{ server_status_code::SIGNUP__EXISTING_LOGIN_ERROR,   "Данный логин уже существует!"  },
	{ server_status_code::SIGNUP__INVALID_LOGIN_ERROR,    "Неверный формат логина!"       },
	{ server_status_code::SIGNUP__INVALID_PASSWORD_ERROR, "Неверный формат пароля!"       },

	{ server_status_code::LOGIN__NOT_EXISTEN_LOGIN_ERROR, "Данного логина не существует!" },
	{ server_status_code::LOGIN__INVALID_PASSWORD_ERROR,  "Неверный пароль!"              },

	{ server_status_code::INTERNAL_SERVER_ERROR,          "Ошибка сервера!"               },
};

std::string status_code_interpreter::get_description(server_status_code code) {
	if (code_descriptions.find(code) != code_descriptions.end()) {
		return code_descriptions.at(code);
	} else {
		return unknown_code_description;
	}
}

FARCONN_NAMESPACE_END