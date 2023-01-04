#pragma once

#include <filesystem>
#include "../include/client/client.hpp"

using namespace farconn::client;

const std::filesystem::path cfg_path = std::filesystem::path(R"(..\\..\\..\\example\\configs\\client_cfg.json)");

client fcnn_client;

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (CTRL_CLOSE_EVENT == dwCtrlType) {
		fcnn_client.stop();
		return TRUE;
	}

	return FALSE;
}

int main() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	if (!SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE)) {
		LOG(SERVER) 
			<< "Ошибка при попытке регистрации обработчика закрытия консоли! "
			<< "Code : " << GetLastError() << "\n";
	}

	client_config config;

	config.load(cfg_path);
	fcnn_client.setup(config);

	fcnn_client.run();

	while(fcnn_client.is_run()) { 
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}