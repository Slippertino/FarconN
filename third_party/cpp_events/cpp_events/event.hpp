#pragma once

#include "method_event_handler.hpp"
#include "ievent.hpp"

template<class...TParams>
class event : public ievent<TParams...> {
	using event_handler_ptr = std::unique_ptr<abstract_event_handler<TParams...>>;
	using event_handler_it = typename std::list<event_handler_ptr>::const_iterator;

public:
	event() : ievent<TParams...>()
	{ }

	void invoke(TParams... args, bool is_synchronized = true) {
		handler_list_mutex.lock_shared();

		is_current_it_removed = false;
		current_it = handlers.begin();

		while (current_it != handlers.end())
		{
			handler_list_mutex.unlock_shared();

			if (is_synchronized) {
				(*current_it)->call(args...);
			} else {
				std::thread executer(&abstract_event_handler<TParams...>::call, (*current_it).get(), args...);
				executer.detach();
			}

			handler_list_mutex.lock_shared();

			if (is_current_it_removed)
			{
				is_current_it_removed = false;
				event_handler_it removed_it = current_it;
				destroy_handler(removed_it);
			}

			++current_it;
		}

		handler_list_mutex.unlock_shared();
	}

protected:
	bool add_handler(event_handler_ptr& handler) override final {
		std::unique_lock<std::shared_mutex> locker(handler_list_mutex);

		if (find_event_handler(handler) == handlers.end()) {
			handlers.push_back(std::move(handler));
			return true;
		}

		return false;
	}

	bool remove_handler(event_handler_ptr& handler) override final {
		std::unique_lock<std::shared_mutex> locker(handler_list_mutex);

		auto it = find_event_handler(handler);

		if (it != handlers.end()) {

			if (current_it == it) {
				is_current_it_removed = true;
			}
			else {
				destroy_handler(it);
			}

			return true;
		}

		return false;
	}

private:
	inline event_handler_it find_event_handler(const event_handler_ptr& eh) const {
		return std::find_if(handlers.begin(), handlers.end(), [&eh](const event_handler_ptr& handler) {
			return (*handler == *eh);
		});
	}

	inline void destroy_handler(event_handler_it it) {
		handlers.erase(it);
	}

	std::list<event_handler_ptr> handlers;

	mutable event_handler_it current_it;
	mutable bool is_current_it_removed = false;

	mutable std::shared_mutex handler_list_mutex;
};

#define event(event_name, ...) \
protected: \
	event<__VA_ARGS__> event_name##_hidden; \
public: \
	ievent<__VA_ARGS__>& event_name = event_name##_hidden; \

#define event_invoke(event_name) event_name##_hidden.invoke
#define event_create( Object, Method ) std::move(create_method_event_handler( Object, &Method ))