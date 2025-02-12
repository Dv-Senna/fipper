#pragma once

#include "fp/coroutines/async.hpp"

#include <print>

#include "fp/jobScheduler.hpp"


namespace fp::coroutines {
	constexpr auto WrapperPromise::await_transform(EndpointTask &task) const noexcept -> EndpointAwaiter {
		return EndpointAwaiter{task};
	}


	template <IsAsyncAwaiterType T>
	constexpr AsyncAwaiter<T>::AsyncAwaiter(Callback &&callback) noexcept :
		m_callback {std::move(callback)},
		m_value {},
		m_handle {}
	{

	}

	template <IsAsyncAwaiterType T>
	constexpr auto AsyncAwaiter<T>::complete(T &&value) noexcept -> void {
		m_value = std::move(value);

		fp::Job job {};
		job.work = [this]() noexcept {
			this->m_handle.resume();
			return fp::Result::eSuccess;
		};

		fp::JobMetadata jobMetadata {};
		jobMetadata.priority = fp::JobPriority::__eResumingJob;

		fp::JobPushingLockGuard _ {};
		fp::JobQueue::push(jobMetadata, std::move(job));
	}

	template <IsAsyncAwaiterType T>
	constexpr auto AsyncAwaiter<T>::getPipelinedCallback() noexcept -> std::move_only_function<void(WrapperTask::Handle)> {
		return [this](WrapperTask::Handle handle) {
			this->m_handle = handle;
			this->m_callback(*this);
		};
	}


	template <typename T>
	constexpr auto EndpointPromise::await_transform(fp::coroutines::AsyncAwaiter<T> &&awaiter) noexcept -> fp::coroutines::AsyncAwaiter<T>&& {
		m_awaiterCallback = awaiter.getPipelinedCallback();
		return std::move(awaiter);
	}


	EndpointAwaiter::EndpointAwaiter(EndpointTask &task) noexcept :
		m_task {task},
		m_wrapperHandle {}
	{

	}

	constexpr auto EndpointAwaiter::await_suspend(WrapperTask::Handle handle) noexcept -> void {
		m_wrapperHandle = handle;
		m_task.getHandle().promise().getAwaiterCallback()(m_wrapperHandle);
	}

	constexpr auto EndpointAwaiter::await_resume() noexcept -> void {
		m_task.resume();
	}

} // namespace fp::coroutines
