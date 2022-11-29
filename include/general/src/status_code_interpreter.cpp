#include "../protocol/status_code_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(general)

const std::string status_code_interpreter::unknown_code_description = "����������� ��� ���������!";

const std::unordered_map<server_status_code, std::string> status_code_interpreter::code_descriptions =
{
	{ server_status_code::SYS__OKEY,				      "��"										    },

	{ server_status_code::SYS__INVALID_COMMAND,           "�������� �������!"						    },
	{ server_status_code::SYS__INVALID_TOKEN,             "������������ ���������� ����� ������������!" },
	{ server_status_code::SYS__NONEXISTEN_USER,           "����������� ������������!"                   },

	{ server_status_code::SIGNUP__EXISTING_LOGIN_ERROR,   "������ ����� ��� ����������!"			    },
	{ server_status_code::SIGNUP__INVALID_LOGIN_ERROR,    "�������� ������ ������!"					    },
	{ server_status_code::SIGNUP__INVALID_PASSWORD_ERROR, "�������� ������ ������!"					    },

	{ server_status_code::LOGIN__INVALID_DATA_ERROR,      "�������� ����� ��� ������!"				    },
	{ server_status_code::LOGIN__ALREADY_LOGGED_IN,       "������������ ��� �����������!"			    },



	{ server_status_code::SYS__INTERNAL_SERVER_ERROR,     "������ �� ������� �������!"					},
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