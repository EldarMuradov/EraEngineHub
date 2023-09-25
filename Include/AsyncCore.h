#pragma once
#include <thread>
#include <future>

template<typename T>
auto __await_async(T lambda)->decltype(lambda())
{
	auto func = std::async(std::launch::async, [=]()
		{
			auto r = lambda();
			return r;
		});
	return func.get();
}

#define _async_t(block) std::async(std::launch::deferred, [=](){block})

#define _await_t(block) __await_async([&] {block})