#include "../protocol/status_code_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::string status_code_interpreter::unknown_code_description = "����������� ��� ���������!";

const std::unordered_map<server_status_code, std::string> status_code_interpreter::code_descriptions =
{
	{ server_status_code::SYS__OKEY,									"��"																	},

	{ server_status_code::SYS__INVALID_COMMAND_ERROR,					"�������� �������!"														},
	{ server_status_code::SYS__INVALID_TOKEN_ERROR,						"������������ ���������� ����� ������������!"							},
	{ server_status_code::SYS__NONEXISTEN_USER_ERROR,					"����������� ������������!"												},
	{ server_status_code::SYS__INVALID_OFFSET_VALUE_ERROR,				"������������ �������� �������� � �������!"								},
	{ server_status_code::SYS__INVALID_LIMIT_VALUE_ERROR ,				"������������ �������� ���������� ������� � �������!"					},

	{ server_status_code::SIGNUP__EXISTING_LOGIN_ERROR,					"������ ����� ��� ����������!"											},
	{ server_status_code::SIGNUP__INVALID_LOGIN_ERROR,					"�������� ������ ������!"												},
	{ server_status_code::SIGNUP__INVALID_PASSWORD_ERROR,				"�������� ������ ������!"												},

	{ server_status_code::LOGIN__INVALID_DATA_ERROR,					"�������� ����� ��� ������!"											},
	{ server_status_code::LOGIN__ALREADY_LOGGED_IN_ERROR,				"������������ ��� �����������!"											},

	{ server_status_code::PROFILE__INVALID_DATA_TO_SET_ERROR,			"������ ������� ��������� �������!"										},

	{ server_status_code::REQUEST__NONEXISTEN_REQUEST_ERROR,			"�������������� ������!"												},
	{ server_status_code::REQUEST__ALREADY_IN_CONTACTS_ERROR,			"���������� ������������ ���� � ����� ���������, ���� ��!"				},
	{ server_status_code::REQUEST__ALREADY_EXIST_ERROR,					"������ � �������� ��� ����������!"										},

	{ server_status_code::CONTACT__NONEXISTEN_CONTACT_ERROR,			"�������������� �������!"												},

	{ server_status_code::CHAT__INVALID_NAME_ERROR,						"�������� ������ �������� ����!"										},
	{ server_status_code::CHAT__JOINED_CHATS_LIMIT_EXCEEDED_ERROR,		"�������� ����� ������� � �����!"										},
	{ server_status_code::CHAT__NONEXISTEN_CHAT_ERROR,					"�������������� ���!"													},
	{ server_status_code::CHAT__USER_NOT_A_PARTICIPANT_ERROR,			"�� �� ��������� ���������� ����!"										},
	{ server_status_code::CHAT__USER_ALREADY_IN_CHAT,					"������������ ��� ��������� � ����!"									},
	{ server_status_code::CHAT__IMPOSSIBLE_ADD_TO_PRIVATE_CHAT_ERROR,	"���������� �������� ������������ � ��������� ���!"						},
	{ server_status_code::CHAT__CHAT_PARTICIPANTS_LIMIT_EXCEEDED_ERROR,	"�������� ����� ���������� � ����!"										},
	{ server_status_code::CHAT__TEXT_MESSAGE_TOO_LARGE,					"��������� ��������� ������� �������!"									},
	{ server_status_code::CHAT__FILE_SIZE_TOO_LARGE,					"���� ������� �������!"													},
	
	{ server_status_code::SYS__INTERNAL_SERVER_ERROR,					"������ �� ������� �������!"											},
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

