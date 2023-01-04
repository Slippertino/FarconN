#include "../protocol/status_code_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::string status_code_interpreter::unknown_code_description = "Неизвестный код состояния!";

const std::unordered_map<server_status_code, std::string> status_code_interpreter::code_descriptions =
{
	{ server_status_code::SYS__OKEY,									"ОК"																	},

	{ server_status_code::SYS__INVALID_COMMAND_ERROR,					"Неверная команда!"														},
	{ server_status_code::SYS__INVALID_TOKEN_ERROR,						"Недопустимый сессионный токен пользователя!"							},
	{ server_status_code::SYS__NONEXISTEN_USER_ERROR,					"Неизвестный пользователь!"												},
	{ server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR,				"Недопустимое значение смещения в выборке!"								},
	{ server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR ,				"Недопустимое значение количества записей в выборке!"					},

	{ server_status_code::SIGNUP__EXISTING_LOGIN_ERROR,					"Данный логин уже существует!"											},
	{ server_status_code::SIGNUP__INVALID_LOGIN_ERROR,					"Неверный формат логина!"												},
	{ server_status_code::SIGNUP__INVALID_PASSWORD_ERROR,				"Неверный формат пароля!"												},

	{ server_status_code::LOGIN__INVALID_DATA_ERROR,					"Неверный логин или пароль!"											},
	{ server_status_code::LOGIN__ALREADY_LOGGED_IN_ERROR,				"Пользователь уже авторизован!"											},

	{ server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR,			"Данные профиля неверного формата!"										},

	{ server_status_code::REQUEST__NONEXISTEN_REQUEST_ERROR,			"Несуществующая заявка!"												},
	{ server_status_code::REQUEST__IMPOSSIBLE_TO_CREATE_ERROR,			"Заявленному пользователю невозможно отправить заявку!"					},
	{ server_status_code::REQUEST__ALREADY_EXIST_ERROR,					"Заявка в контакты уже существует!"										},

	{ server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR,			"Несуществующий контакт!"												},

	{ server_status_code::CHAT__INVALID_NAME_ERROR,						"Неверный формат названия чата!"										},
	{ server_status_code::CHAT__JOINED_CHATS_LIMIT_EXCEEDED_ERROR,		"Превышен лимит участий в чатах!"										},
	{ server_status_code::CHAT__NONEXISTEN_CHAT_ERROR,					"Несуществующий чат!"													},
	{ server_status_code::CHAT__USER_NOT_A_PARTICIPANT_ERROR,			"Вы не являетесь участником чата!"										},
	{ server_status_code::CHAT__USER_ALREADY_IN_CHAT,					"Пользователь уже находится в чате!"									},
	{ server_status_code::CHAT__IMPOSSIBLE_ADD_TO_PRIVATE_CHAT_ERROR,	"Невозможно добавить пользователя в приватный чат!"						},
	{ server_status_code::CHAT__CHAT_PARTICIPANTS_LIMIT_EXCEEDED_ERROR,	"Превышен лимит участников в чате!"										},
	{ server_status_code::CHAT__TEXT_MESSAGE_TOO_LARGE,					"Текстовое сообщение слишком длинное!"									},
	{ server_status_code::CHAT__FILE_SIZE_TOO_LARGE,					"Файл слишком большой!"													},
	
	{ server_status_code::SYS__INTERNAL_SERVER_ERROR,					"Ошибка на стороне сервера!"											},
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

