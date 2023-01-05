#pragma once

#include "../general/multithread_context/multithread_context.hpp"
#include "network/client_networking.hpp"
#include "config/client_config.hpp"
#include "../general/protocol/command_entities.hpp"
#include "../general/protocol/entities/entities.hpp"
#include "../general/protocol/server_status_code.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(client)

class client : public farconn::general::multithread_context<client> {
public:
	client() = default;

	void setup(const client_config&);
	void run() override;
	void stop() override;

	bool is_run() const;
	bool is_setup() const;

	~client() = default;

protected:
	void setup_contexts() override;
	void working_context();

private:
	void on_message_received(SOCKET, std::string);
	void on_client_net_error_occured(void*);

private:
	void profile_get_handler(command_entity*) const;
	void invites_list_handler(command_entity*) const;
	void contacts_list_handler(command_entity*) const;
	void search_handler(command_entity*) const;
	void chats_list_handler(command_entity*) const;
	void chat_messages_list_handler(command_entity*) const;
	void chat_party_list_handler(command_entity*) const;
	void help_handler(command_entity*) const;

private:
	static const std::unordered_map<std::string, std::function<void(const client*, command_entity*)>> command_handlers;
	
	bool is_client_running = false;
	bool is_client_setup = false;

	client_config config;
	client_networking networking;
};

FARCONN_NAMESPACE_END