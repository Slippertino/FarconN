#pragma once

#include "../../general/network/networking_execution.hpp"
#include "../../general/network/networking.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class server_networking : public networking_execution, 
						  public networking {
public:
	server_networking() = delete;
	server_networking(class server*);

	void setup(const std::string&, uint16_t);

	void run() override;
	void stop() override;
	void working_context() override;

	~server_networking();

private:
	class server* main;
};

FARCONN_NAMESPACE_END