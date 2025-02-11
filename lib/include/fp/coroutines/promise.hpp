#pragma once

#include <coroutine>
#include <utility>

#include "fp/coroutines/awaiter.hpp"
#include "fp/utils/traits.hpp"


namespace fp::coroutines {
	template <typename T, typename Enable = void>
	struct PromiseTask;

	template <typename T>
	struct PromiseTask<T, std::void_t<decltype(std::declval<T> ().get_return_object())>> {
		using Type = decltype(std::declval<T> ().get_return_object());
	};


	template <typename T>
	concept IsPromise = requires(T value) {
		typename PromiseTask<T>::Type;
		{value.unhandled_exception()} -> std::same_as<void>;
		{value.initial_suspend()} -> IsAwaiter;
		{value.final_suspend()} -> IsAwaiter;
	} && (requires(T value, fp::utils::AnyInitializer any) {
		{value.return_value(any)} -> std::same_as<void>;
	} || requires(T value) {
		{value.return_void()} -> std::same_as<void>;
	});


	template <typename T>
	concept IsNotYieldingPromise = IsPromise<T> && !requires(T value, fp::utils::AnyInitializer any) {
		{value.yield_value(any)} -> IsAwaiter;
	};


	template <typename T>
	class PromiseBase {
		public:
			auto return_value(T &&value) noexcept -> void {
				m_value = std::move(value);
			}

			auto getValue() noexcept -> T {
				return std::move(m_value);
			}

		protected:
			T m_value;
	};


	template <>
	class PromiseBase<void> {
		public:
			auto return_void() noexcept -> void {}
	};


	static_assert(IsAwaiter<std::suspend_never>);
	static_assert(IsAwaiter<std::suspend_always>);

} // namespace fp::coroutines
