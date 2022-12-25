#pragma once

#include "../../middleware/handlers/chat_post_handler.hpp"
#include "../../middleware/server_middleware.hpp"

FARCONN_NAMESPACE_BEGIN(server)

chat_post_handler::chat_post_handler(server_middleware* sm, const command_entity* ce, command_response* cr) :
	chat_handler(sm, ce, cr, true)
{ }

bool chat_post_handler::is_command_valid() {
	auto result = in->options.size() == 1;

	if (result) {
		auto& opt = in->options[0];

		result =
			result &&
			options_params_mapper.find(opt) != options_params_mapper.end() &&
			options_params_mapper.at(opt).second == in->params.size();
	}

	return result;
}

void chat_post_handler::post_text_message(chat_post_params& params) {
	params.content = in->params[2];

	SERVER_ASSERT_EX(out, params.content.size() > max_text_message_size, server_status_code::CHAT__TEXT_MESSAGE_TOO_LARGE)
	SERVER_ASSERT_EX(out, true, db->post_message(params))
}

void chat_post_handler::post_file_message(chat_post_params& params) {
	auto& file_name = in->params[2];
	auto& file_content = in->params[3];

	SERVER_ASSERT_EX(out, file_content.size() > max_file_message_size, server_status_code::CHAT__FILE_SIZE_TOO_LARGE)

	auto code = db->save_file(params.chat_id, params.id, file_content);
	SERVER_ASSERT_EX(out, code != SUCCESS, code)

	params.content = file_name;

	SERVER_ASSERT_EX(out, true, db->post_message(params))
}

void chat_post_handler::execute() {
	auto& opt = in->options[0];

	chat_post_params params;

	auto& user_id = session->native_token;
	auto& chat_id = in->params[1];

	auto is_part = db->is_user_chat_participant(user_id, chat_id);
	SERVER_ASSERT_EX(out, is_part != SUCCESS, is_part)

	params.chat_id = chat_id;
	params.sender_id = session->native_token;
	params.type = opt;

	std::unordered_set<std::string> messages_tokens;
	SERVER_ASSERT(out, db->get_messages_tokens(messages_tokens) != SUCCESS)

	params.id = token_generator::generate([&messages_tokens](const auto& cur) {
		return messages_tokens.find(cur) == messages_tokens.end();
	});

	options_params_mapper.at(opt).first(this, params);
}

const std::unordered_map<std::string, std::pair<chat_post_handler::option_handler, size_t>>
chat_post_handler::options_params_mapper = {
	{ "text", { &chat_post_handler::post_text_message, 3 } },
	{ "file", { &chat_post_handler::post_file_message, 4 } },
};

const uint64_t chat_post_handler::max_file_message_size = 256 * (1 << 20);
const uint64_t chat_post_handler::max_text_message_size = 1 << 12;

FARCONN_NAMESPACE_END