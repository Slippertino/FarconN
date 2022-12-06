#pragma once

#include <unordered_map>
#include "handler.hpp"

FARCONN_NAMESPACE_BEGIN(server)

class invite_handler final : public handler {
public:
	invite_handler(class server_middleware*, const command_entity*, command_response*);

protected:
	bool is_command_valid() override final;
	void execute() override final;

private:
	void handle_accept(const std::string& lufrom, const std::string& luto);
	void handle_reject(const std::string& lufrom, const std::string& luto);
	void handle_create(const std::string& lufrom, const std::string& luto);
	void handle_cancel(const std::string& lufrom, const std::string& luto);

private:
	using option_handler = std::function<void(invite_handler*, const std::string&, const std::string&)>;

	static const std::unordered_map<std::string, option_handler> option_handlers_map;

	db_responder& database;
};

FARCONN_NAMESPACE_END