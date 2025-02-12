#pragma once

#include "fp/coroutines/awaiter.hpp"
#include "fp/coroutines/promise.hpp"
#include "fp/coroutines/task.hpp"
#include "fp/httpCode.hpp"


namespace fp::coroutines {
	class WrapperTask;
	class EndpointTask;
	class EndpointAwaiter;

	class WrapperPromise : public fp::coroutines::PromiseBase<void> {
		struct Awaiter {
			fp::HttpCode value;
			constexpr auto await_ready() const noexcept -> bool {return true;}
			constexpr auto await_resume() const noexcept -> fp::HttpCode {return value;}
			constexpr auto await_suspend(auto) noexcept -> void {}
		};
		static_assert(fp::coroutines::IsAwaiter<Awaiter>);

		public:
			constexpr auto get_return_object() noexcept -> WrapperTask;
			constexpr auto initial_suspend() const noexcept -> std::suspend_never {return {};}
			constexpr auto final_suspend() const noexcept -> std::suspend_never {return {};}

			constexpr auto await_transform(EndpointTask &&task) const noexcept -> EndpointAwaiter;
			constexpr auto await_transform(fp::HttpCode code) const noexcept -> Awaiter;
	};

	static_assert(fp::coroutines::IsNotYieldingPromise<WrapperPromise>);

	
	class WrapperTask {
		public:
			using promise_type = WrapperPromise;
			using Handle = std::coroutine_handle<promise_type>;

			WrapperTask() noexcept = delete;
			constexpr WrapperTask(Handle handle) noexcept : m_handle {handle} {}

			constexpr auto resume() const noexcept -> void {m_handle.resume();}
			constexpr auto isDone() const noexcept -> bool {return m_handle.done();}
			constexpr auto kill() const noexcept -> void {m_handle.destroy();}
		
		private:
			Handle m_handle;
	};

	static_assert(fp::coroutines::IsTask<WrapperTask>);

	constexpr auto WrapperPromise::get_return_object() noexcept -> WrapperTask {
		return WrapperTask{WrapperTask::Handle::from_promise(*this)};
	}





	template <typename T>
	concept IsAsyncAwaiterType = !std::is_void_v<T>;

	/*
	 * @brief Wrapper to help convert async process to co_await and resume coroutine by pushing them to the
	 *        job queue when the process is over
	 * */
	template <IsAsyncAwaiterType T>
	class AsyncAwaiter {
		public:
			using Callback = std::move_only_function<void(AsyncAwaiter<T>&)>;

			constexpr AsyncAwaiter(Callback &&callback) noexcept;

			constexpr auto complete(T &&value) noexcept -> void;
			constexpr auto await_ready() const noexcept -> bool {return false;}
			constexpr auto await_resume() const noexcept -> T {return std::move(m_value);}
			constexpr auto await_suspend(auto) noexcept -> void {}

			constexpr auto getPipelinedCallback() noexcept -> std::move_only_function<void(WrapperTask::Handle)>;

		private:
			Callback m_callback;
			T m_value;
			WrapperTask::Handle m_handle;
	};

	static_assert(fp::coroutines::IsAwaiter<AsyncAwaiter<int>>);





	class EndpointPromise : public fp::coroutines::PromiseBase<fp::HttpCode> {
		public:
			constexpr auto get_return_object() noexcept -> EndpointTask;
			constexpr auto initial_suspend() const noexcept -> std::suspend_never {return {};}
			constexpr auto final_suspend() noexcept -> std::suspend_always {m_done = true; return {};}

			template <typename T>
			constexpr auto await_transform(fp::coroutines::AsyncAwaiter<T> &&awaiter) noexcept -> fp::coroutines::AsyncAwaiter<T>&&;

			constexpr auto getAwaiterCallback() noexcept -> std::move_only_function<void(WrapperTask::Handle)> {return std::move(m_awaiterCallback);}
			constexpr auto isDone() const noexcept -> bool {return m_done;}

		private:
			bool m_done;
			std::move_only_function<void(WrapperTask::Handle)> m_awaiterCallback;
	};

	static_assert(fp::coroutines::IsNotYieldingPromise<EndpointPromise>);


	class EndpointTask {
		public:
			using promise_type = EndpointPromise;
			using Handle = std::coroutine_handle<promise_type>;

			EndpointTask() noexcept = delete;
			constexpr EndpointTask(Handle handle) noexcept : m_done {false}, m_handle {handle} {}

			constexpr auto resume() noexcept -> void {m_handle.resume(); m_done = m_handle.promise().isDone();}
			constexpr auto isDone() const noexcept -> bool {return m_done;}
			constexpr auto kill() const noexcept -> void {m_handle.destroy();}
			constexpr auto getValue() const noexcept -> fp::HttpCode {
				fp::HttpCode value {m_handle.promise().getValue()};
				this->kill();
				return value;
			}
			constexpr auto getHandle() const noexcept -> Handle {return m_handle;}

		private:
			bool m_done;
			Handle m_handle;
	};

	static_assert(fp::coroutines::IsTask<EndpointTask>);

	constexpr auto EndpointPromise::get_return_object() noexcept -> EndpointTask {
		return EndpointTask{EndpointTask::Handle::from_promise(*this)};
	}





	class EndpointAwaiter {
		public:
			EndpointAwaiter(EndpointTask &&task) noexcept;

			constexpr auto await_ready() const noexcept -> bool {return false;}
			constexpr auto await_suspend(WrapperTask::Handle handle) noexcept -> void;
			constexpr auto await_resume() noexcept -> fp::HttpCode;

		private:
			EndpointTask m_task;
			WrapperTask::Handle m_wrapperHandle;
	};

	static_assert(fp::coroutines::IsAwaiter<EndpointAwaiter>);

} // namespace fp::coroutines

namespace fp {
	using Async = fp::coroutines::EndpointTask;
} // namespace


#include "fp/coroutines/async.inl"
