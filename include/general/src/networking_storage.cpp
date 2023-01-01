#include "../network/networking_storage.hpp"

#include <iostream>

FARCONN_NAMESPACE_BEGIN(general)

networking_storage& networking_storage::storage() {
	static networking_storage storage;

	return storage;
}

networking_storage::networking_storage() {
	WSADATA wsData;
	auto erState = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erState) {
		LOG(NETWORK) << BUILD_ERROR_MESSAGE("Ошибка инициализации сокетных интерфейсов!");
	}
	else {
		LOG(NETWORK) << "Успешная инициализация сокетных интерфейсов!\n";
	}

	run();
}

void networking_storage::add_socket(SOCKET sock) {
	std::lock_guard<std::shared_mutex> locker(descs_events_mutex);

	WSAPOLLFD new_poll_fd;
	new_poll_fd.fd = sock;

	new_poll_fd.events = POLLIN;
	descs_to_read.insert({ sock, new_poll_fd });

	new_poll_fd.events = POLLOUT;
	descs_to_write.insert({ sock, new_poll_fd });
}

void networking_storage::remove_socket(SOCKET sock) {
	std::lock_guard<std::shared_mutex> locker(descs_events_mutex);

	if (descs_to_read.find(sock) != descs_to_read.end()) {
		descs_to_read.erase(sock);
	}

	if (descs_to_write.find(sock) != descs_to_write.end()) {
		descs_to_write.erase(sock);
	}
}

bool networking_storage::can_read(SOCKET sock) {
	return can_do(sock, descs_to_read, POLLRDNORM);
}

bool networking_storage::can_write(SOCKET sock) {
	return can_do(sock, descs_to_write, POLLWRNORM);
}

networking_storage::~networking_storage() {
	WSACleanup();
}

bool networking_storage::can_do(SOCKET sock, std::map<UINT, WSAPOLLFD>& descs, SHORT flag) {
	descs_events_mutex.lock_shared();

	auto& poll_fd = descs[sock];

	bool result = poll_fd.revents & flag;
	poll_fd.revents = 0;

	descs_events_mutex.unlock_shared();

	return result;
}

void networking_storage::update_descs(std::map<UINT, WSAPOLLFD>& descs) {
	descs_events_mutex.lock_shared();

	std::vector<WSAPOLLFD> transformed_descs;
	for (auto& cur : descs) {
		transformed_descs.push_back(cur.second);
	}

	descs_events_mutex.unlock_shared();

	if (!transformed_descs.empty()) {
		std::lock_guard<std::shared_mutex> locker(descs_events_mutex);

		auto result = WSAPoll(transformed_descs.data(), transformed_descs.size(), NULL);

		if (result == SOCKET_ERROR) {
			LOG(NETWORK) << BUILD_ERROR_MESSAGE("Ошибка при выполнении инструкции WSAPoll!");
		}

		for (auto& desc : transformed_descs) {
			descs[desc.fd].revents = desc.revents;
		}
	}
}

void networking_storage::working_context() {
	update_descs(descs_to_read);
	update_descs(descs_to_write);
}

void networking_storage::update() {
	working_context();
}

void networking_storage::setup_contexts() {
	add_context(&networking_storage::working_context, mt_sleep_time(1));
}

FARCONN_NAMESPACE_END