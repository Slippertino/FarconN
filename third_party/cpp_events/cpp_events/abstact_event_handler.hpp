#pragma once

#include <list>
#include <mutex>
#include <assert.h>
#include <shared_mutex>

template<class...TParams>
class abstract_event_handler {
protected:
	using event_handler = abstract_event_handler<TParams...>;

public:
	bool operator==(const event_handler& other) const
	{
		return is_equal(other);
	}
	bool operator!=(const event_handler& other) const
	{
		return !(*this == other);
	}

	virtual bool is_equal(const event_handler&) const = 0;
	virtual void call(TParams...) = 0;

protected:
	abstract_event_handler() { }
};