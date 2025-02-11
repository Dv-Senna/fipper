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



	/*
	 * @brief Wrapper to help convert async process to co_await and resume coroutine by pushing them to the
	 *        job queue when the process is over
	 * */
	template <typename T>
	class AsyncAwaiter {
		public:
			using Callback = std::move_only_function<void(AsyncAwaiter<T>&)>;

			AsyncAwaiter() noexcept = delete;

			constexpr AsyncAwaiter(Callback &&callback);
			constexpr AsyncAwaiter(T &&value) requires (!std::is_void_v<T>);

			/*
			 * @brief Resume the coroutine and set the result of the co_await
			 * @warning After calling this method, the awaiter is not guaranty to exist anymore
			 * */
			constexpr auto complete(T &&value) -> void;

			constexpr auto await_ready() const noexcept -> bool {return false;}
			constexpr auto await_resume() noexcept -> T;

			template <__internals::IsAwaitSuspendReturn CoroutineHandle>
			constexpr auto await_suspend(CoroutineHandle handle) noexcept -> void;


		private:
			T m_value;
			Callback m_callback;
			std::move_only_function<void()> m_resume;
	};

} // namespace fp::coroutines

#include "fp/coroutines/awaiter.inl"
