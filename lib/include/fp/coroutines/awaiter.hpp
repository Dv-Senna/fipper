#pragma once

#include <concepts>

#include "fp/utils/traits.hpp"


namespace fp::coroutines {
	namespace __internals {
		template <typename T>
		concept IsAwaitSuspendReturn = std::is_void_v<T> || std::is_same_v<T, bool> || requires(T value) {
			{value.resume()} -> std::same_as<void>;
			{value.done()} -> std::same_as<bool>;
			{value.destroy()} -> std::same_as<void>;
		};

	} // namespace __internals

	template <typename T>
	concept IsAwaiter = requires(T value, fp::utils::AnyInitializer any) {
		{value.await_ready()} -> std::same_as<bool>;
		value.await_resume();
		{value.await_suspend(any)} -> __internals::IsAwaitSuspendReturn;
	};


} // namespace fp::coroutines

