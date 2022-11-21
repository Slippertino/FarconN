#pragma once

#include "../../general/multithread_context/multithread_context.hpp"
#include "../../general/network/networking.hpp"
#include <cpp_events/event.hpp>

using namespace farconn::general;

FARCONN_NAMESPACE_BEGIN(server)

class server_networking : public multithread_context<server_networking>, 
						  public networking {
	event(connection_incoming, SOCKET);

public:
	server_networking() = default;

	void setup(const std::string&, uint16_t);

	void run() override;
	void stop() override;

	~server_networking();

private:
	std::string build_logging_prefix(const std::string&) const;

	void setup_contexts() override;

	void working_context();
};

FARCONN_NAMESPACE_END