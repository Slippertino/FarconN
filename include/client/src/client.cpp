#include "../client.hpp"
#include "../../general/protocol/protocol_interpreter.hpp"

FARCONN_NAMESPACE_BEGIN(client)

#undef ERROR

client::client(const client_config& client_cfg) : config(client_cfg) {
	is_connected = networking.connect(config.get_ip(), config.get_port());

	if (is_connected) {
		networking.error_occured    += event_create(const_cast<client*>(this), client::on_client_net_error_occured);
		networking.message_received += event_create(const_cast<client*>(this), client::on_message_received);
	}
}

void client::run() {
	if (!is_connected) {
		return;
	}

	multithread_context<client>::run();

	networking.run();
}

void client::stop() {
	multithread_context<client>::stop();

	networking.stop();
}

void client::setup_contexts() {
	add_context(&client::working_context, mt_sleep_time(2000), mt_mode::ASYNCHRONIZED);
}

void client::working_context() {
	std::string message_to_send;
	LOG(CLIENT) << "Сообщение: ";
	std::getline(std::cin, message_to_send);

	LOG(CLIENT) << "В очередь на отправку: " << message_to_send << "\n";

	utf8_encoder::from_local_to_utf8(message_to_send);

	networking.push_message(message_to_send);
}

void client::on_message_received(SOCKET sock, std::string msg) { 
	utf8_encoder::from_utf8_to_local(msg);
	LOG(CLIENT) << "Получено сообщение: " << msg << "\n";

	try {
		auto resp = protocol_interpreter::interpret(msg);

		auto code = static_cast<server_status_code>(
			std::stoi(resp->params[0])
		);

		if (code != server_status_code::SYS__OKEY) {
			return;
		}

		if (command_handlers.find(resp->command) != command_handlers.end()) {
			command_handlers.at(resp->command)(this, resp.get());
		}
	}
	catch (const std::exception& ex) {
		LOG(CLIENT) << ex.what() << "\n";
	}
}

void client::on_client_net_error_occured(void* net) {
	stop();
}

void client::profile_get_handler(command_entity* ent) const {
	ex_user_profile info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(comp);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Данные профиля пользователя: \n";
	for (auto& req : info.data) {
		LOG(SESSION) << req.first << " " << req.second << "\n";
	}
}

void client::invites_list_handler(command_entity* ent) const {
	ex_invitations_info info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Исходящие заявки: \n";
	for (auto& req : info.data["outcoming"]) {
		auto& cur = req.second;

		LOG(SESSION) << cur.id << " " << cur.login << " " << cur.name << "\n";
	}

	LOG(SESSION) << "Входящие заявки: \n";
	for (auto& req : info.data["incoming"]) {
		auto& cur = req.second;

		LOG(SESSION) << cur.id << " " << cur.login << " " << cur.name << "\n";
	}
}

void client::contacts_list_handler(command_entity* ent) const {
	ex_contacts_info info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Список контактов: \n";
	for (auto& req : info.data) {
		auto& cur = req.second;

		LOG(SESSION) << cur.id << " " << cur.login << " " << cur.name << "\n";
	}
}

void client::search_handler(command_entity* ent) const {
	ex_searching_results info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Результаты поиска: \n";
	for (auto& req : info.data) {
		auto& cur = req.second;

		LOG(SESSION) << cur.id << " " << cur.login << " " << cur.name << "\n";
	}
}

void client::chats_list_handler(command_entity* ent) const {
	ex_chats_info info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Список чатов: \n";
	for (auto& req : info.data) {
		auto& cur = req.second;

		LOG(SESSION) << "Идентификатор: " << cur.id << "\n";
		LOG(SESSION) << "Название: " << cur.name << "\n";
		LOG(SESSION) << "Количество участников: " << cur.size << "\n";
		LOG(SESSION) << "Последнее сообщение: \n";
		LOG(SESSION) << cur.last_message.time << " " << cur.last_message.sender_name
			  << " [ " << cur.last_message.type << " ] : " << cur.last_message.content << "\n\n";
	}
}

void client::chat_messages_list_handler(command_entity* ent) const {
	ex_chat_messages_info info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Сообщения: \n";
	for (auto& req : info.data) {
		auto& cur = req.second;

		LOG(SESSION)
			<< cur.time << " " << cur.sender_name
			<< " [ " << cur.type << " ] : " << cur.content << "\n";
	}
}

void client::chat_party_list_handler(command_entity* ent) const {
	ex_chat_party_info info;
	auto& comp = ent->params[2];

	utf8_encoder::from_local_to_utf8(comp);
	auto js = nlohmann::json::parse(ent->params[2]);
	nlohmann::from_json(js, info.data);
	info.to_encoding(&utf8_encoder::from_utf8_to_local);

	LOG(SESSION) << "Участники чата: \n";
	for (auto& req : info.data) {
		auto& cur = req.second;

		LOG(SESSION) << cur.id << " " << cur.login << " " << cur.name << "\n";
	}
}

const std::unordered_map<std::string, std::function<void(const client*, command_entity*)>> client::command_handlers = {
	{ "profile_get",		&client::profile_get_handler		},
	{ "invites_list",		&client::invites_list_handler		},
	{ "contacts_list",		&client::contacts_list_handler		},
	{ "search",				&client::search_handler				},
	{ "chats_list",			&client::chats_list_handler			},
	{ "chat_messages_list", &client::chat_messages_list_handler },
	{ "chat_party_list",	&client::chat_party_list_handler	},
};

FARCONN_NAMESPACE_END