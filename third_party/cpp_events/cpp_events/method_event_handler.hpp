#pragma once

#include "abstact_event_handler.hpp"

template<class TObject, class ...TParams>
class method_event_handler : public abstract_event_handler<TParams...> {
	using event_handler = abstract_event_handler<TParams...>;
	using method_t = void(TObject::*)(TParams...);

public:
	method_event_handler(TObject* obj, method_t method) :
		abstract_event_handler<TParams...>(),
		object(obj),
		method(method)
	{
		assert(obj != nullptr);
	}

	bool is_equal(const event_handler& handler) const override final {
		const method_event_handler<TObject, TParams...>* other
			= dynamic_cast<const method_event_handler<TObject, TParams...>*>(&handler);

		return (other != nullptr && object == other->object && method == other->method);
	}

	void call(TParams... args) override final {
		(object->*method)(args...);
	}

private:
	TObject* object;
	method_t method;
};

template<class TObject, class...TParams>
std::unique_ptr<abstract_event_handler<TParams...>> create_method_event_handler(TObject* object, void(TObject::* method)(TParams...)) {
	return std::make_unique<method_event_handler<TObject, TParams...>>(object, method);
}