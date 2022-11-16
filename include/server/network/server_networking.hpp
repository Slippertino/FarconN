#pragma once

#include "../../general/multithread_context/multithread_context.hpp"
#include "../../general/network/networking.hpp"

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class server_networking : public multithread_context<server_networking>, 
						  public networking {
public:
	server_networking() = delete;
	server_networking(class server*);

	void setup(const std::string&, uint16_t);

	void setup_contexts() override;
	void run() override;
	void stop() override;

	void working_context();

	~server_networking();

private:
	class server* main;
};

FARCONN_NAMESPACE_END