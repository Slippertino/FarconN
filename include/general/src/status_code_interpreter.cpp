#include "../protocol/status_code_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::string status_code_interpreter::unknown_code_description = "Неизвестный код состояния!";

const std::unordered_map<server_status_code, std::string> status_code_interpreter::code_descriptions =
{
	{ server_status_code::SYS__OKEY,						  "ОК"														},

	{ server_status_code::SYS__INVALID_COMMAND_ERROR,		  "Неверная команда!"										},
	{ server_status_code::SYS__INVALID_TOKEN_ERROR,			  "Недопустимый сессионный токен пользователя!"				},
	{ server_status_code::SYS__NONEXISTEN_USER_ERROR,		  "Неизвестный пользователь!"								},

	{ server_status_code::SIGNUP__EXISTING_LOGIN_ERROR,		  "Данный логин уже существует!"							},
	{ server_status_code::SIGNUP__INVALID_LOGIN_ERROR,		  "Неверный формат логина!"									},
	{ server_status_code::SIGNUP__INVALID_PASSWORD_ERROR,	  "Неверный формат пароля!"									},

	{ server_status_code::LOGIN__INVALID_DATA_ERROR,          "Неверный логин или пароль!"								},
	{ server_status_code::LOGIN__ALREADY_LOGGED_IN_ERROR,     "Пользователь уже авторизован!"							},

	{ server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR, "Данные профиля неверного формата!"						},

	{ server_status_code::REQUEST__NONEXISTEN_REQUEST_ERROR,  "Несуществующая заявка!"                                  },
	{ server_status_code::REQUEST__ALREADY_IN_CONTACTS_ERROR, "Заявленный пользователь либо в ваших контактах, либо Вы!" },
	{ server_status_code::REQUEST__ALREADY_EXIST,             "Заявка в контакты уже существует!" },

	{ server_status_code::SYS__INTERNAL_SERVER_ERROR,         "Ошибка на стороне сервера!"					            },
};

std::pair<uint16_t,std::string> status_code_interpreter::interpret(server_status_code code) {
	return {
		static_cast<uint16_t>(code),
		get_description(code)
	};
}

std::string status_code_interpreter::get_description(server_status_code code) {
	if (code_descriptions.find(code) != code_descriptions.end()) {
		return code_descriptions.at(code);
	} else {
		return unknown_code_description;
	}
}

FARCONN_NAMESPACE_END