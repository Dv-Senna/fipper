#pragma once

#include "fp/coroutines/promise.hpp"


namespace fp::coroutines {
	template <typename T>
	concept IsTask = fp::coroutines::IsPromise<typename T::promise_type> && requires(T value) {
		{value.resume()} -> std::same_as<void>;
		{value.isDone()} -> std::same_as<bool>;
		{value.kill()} -> std::same_as<void>;
	};

} // namespace fp::coroutines
