#pragma once

#include "chat_handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class chat_post_handler final : public chat_handler {
public:
	chat_post_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	void post_message(chat_post_params&);
	void post_text_message(chat_post_params&);
	void post_file_message(chat_post_params&);

private:
	using option_handler = std::function<void(chat_post_handler*, chat_post_params&)>;

	static const std::unordered_map<std::string, std::pair<option_handler, size_t>> options_params_mapper;

	static const uint64_t max_file_message_size;
	static const uint64_t max_text_message_size;
};

FARCONN_NAMESPACE_END