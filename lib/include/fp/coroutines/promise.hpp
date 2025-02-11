#pragma once

#include <coroutine>
#include <utility>


namespace fp::coroutines {
	namespace __internals {
		template <typename T>
		concept IsAwaitSuspendReturn = std::is_void_v<T> || std::is_same_v<T, bool> || requires(T value) {
			{value.resume()} -> std::same_as<void>;
			{value.done()} -> std::same_as<bool>;
			{value.destroy()} -> std::same_as<void>;
		};

		struct AnyInitializer {
			template <typename T>
			operator T() const noexcept;
		};

		template <typename T>
		struct Void {
			using Type = void;
		};
	} // namespace __internals

	template <typename T>
	concept IsAwaiter = requires(T value, __internals::AnyInitializer any) {
		{value.await_ready()} -> std::same_as<bool>;
		value.await_resume();
		{value.await_suspend(any)} -> __internals::IsAwaitSuspendReturn;
	};


	template <typename T, typename Enable = void>
	struct PromiseTask;

	template <typename T>
	struct PromiseTask<T, typename __internals::Void<decltype(std::declval<T> ().get_return_object())>::Type> {
		using Type = decltype(std::declval<T> ().get_return_object());
	};


	template <typename T>
	concept IsPromise = requires(T value) {
		typename PromiseTask<T>::Type;
		{value.unhandled_exception()} -> std::same_as<void>;
		{value.initial_suspend()} -> IsAwaiter;
		{value.final_suspend()} -> IsAwaiter;
	} && (requires(T value, __internals::AnyInitializer any) {
		{value.return_value(any)} -> std::same_as<void>;
	} || requires(T value) {
		{value.return_void()} -> std::same_as<void>;
	});


	template <typename T>
	concept IsNotYieldingPromise = IsPromise<T> && !requires(T value, __internals::AnyInitializer any) {
		{value.yield_value(any)} -> IsAwaiter;
	};


	template <typename T>
	class PromiseBase {
		public:
			void return_value(T &&value) noexcept {
				m_value = std::move(value);
			}

			T getValue() noexcept {
				return std::move(m_value);
			}

		protected:
			T m_value;
	};


	template <>
	class PromiseBase<void> {
		public:
			void return_void() noexcept {}
	};


	static_assert(IsAwaiter<std::suspend_never>);
	static_assert(IsAwaiter<std::suspend_always>);

} // namespace fp::coroutines
