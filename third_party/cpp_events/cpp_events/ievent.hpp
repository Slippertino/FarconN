#pragma once

#include <memory>
#include "abstact_event_handler.hpp"

template<class...TParams>
class ievent
{
protected:
	using event_handler_ptr = std::unique_ptr<abstract_event_handler<TParams...>>;

public:
	bool operator+=(event_handler_ptr&& eventHandler)
	{
		return add_handler(eventHandler);
	}
	bool operator-=(event_handler_ptr&& eventHandler)
	{
		return remove_handler(eventHandler);
	}

protected:
	ievent() = default;

	virtual bool add_handler(event_handler_ptr& event_handler) = 0;
	virtual bool remove_handler(event_handler_ptr& event_handler) = 0;
};
