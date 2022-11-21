#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include "../tools/macro.hpp"
#include "context_mode_type.hpp"
#include <cpp_events/event.hpp>

FARCONN_NAMESPACE_BEGIN(general)

template<class Derived>
class multithread_context {
	event(error_occured, void*);

public:
	multithread_context() = default;
	
	virtual void run() {
		if (!is_setup) {
			setup_contexts();
			is_setup = true;
		}

		cancellation_flag = false;

		for (auto& cont : contexts) {
			auto th = std::thread(
				&Derived::run_context_loop, 
				get_derived(),
				std::get<0>(cont), 
				std::get<1>(cont)
			);

			if (std::get<2>(cont) == mt_mode::ASYNCHRONIZED) {
				th.detach();
				continue;
			}

			flows.push_back(std::move(th));
		}
	}

	virtual void stop() {
		cancellation_flag = true;

		for (auto& cur : flows) {
			if (cur.joinable()) {
				cur.join();
			}
		}
	}

	~multithread_context() {
		stop();
	}

protected:
	using mt_context = std::function<void(Derived*)>;
	using mt_sleep_time = std::chrono::milliseconds;
	using mt_mode = context_mode_type;

protected:
	void add_context(const mt_context& context, mt_sleep_time sleep_time, mt_mode mode = mt_mode::SYNCHRONIZED) {
		contexts.push_back({ context, sleep_time, mode});
	}

	void run_context_loop(const mt_context& context, mt_sleep_time sleep_time) {
		while (!cancellation_flag) {
			context(get_derived());
			std::this_thread::sleep_for(sleep_time);
		}
	}

	Derived* get_derived() {
		return dynamic_cast<Derived*>(this);
	}

	virtual void setup_contexts() = 0;

protected:
	bool is_setup = false;

	std::vector<std::tuple<mt_context, mt_sleep_time, mt_mode>> contexts;

	std::vector<std::thread> flows;
	std::atomic<bool> cancellation_flag = true;
};

FARCONN_NAMESPACE_END