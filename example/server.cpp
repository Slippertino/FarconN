#pragma once

#include <filesystem>
#include "../include/server/server.hpp"

using namespace farconn::server;

const std::filesystem::path cfg_path = std::filesystem::path(R"(..\\..\\..\\example\\configs\\server_cfg.json)");

server fcnn_server;

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (CTRL_CLOSE_EVENT == dwCtrlType) {
		fcnn_server.stop();
		return TRUE;
	}

	return FALSE;
}

int main() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	if (!SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE)) {
		LOG(SERVER) << "Ошибка при попытке регистрации обработчика закрытия консоли! "
			<< "Code : " << GetLastError() << "\n";
	}

	server_config config;

	config.load(cfg_path);
	fcnn_server.setup(config);

	fcnn_server.run();

	while (fcnn_server.is_run()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100000));
	}
}