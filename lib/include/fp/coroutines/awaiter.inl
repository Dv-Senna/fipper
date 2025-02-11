#pragma once

#include "fp/coroutines/awaiter.hpp"

#include <chrono>

#include "fp/jobScheduler.hpp"


namespace fp::coroutines {
	template <typename T>
	constexpr AsyncAwaiter<T>::AsyncAwaiter(Callback &&callback) :
		m_value {},
		m_callback {std::move(callback)},
		m_resume {}
	{

	}


	template <typename T>
	constexpr AsyncAwaiter<T>::AsyncAwaiter(T &&value) requires (!std::is_void_v<T>) :
		m_value {std::move(value)},
		m_callback {},
		m_resume {}
	{

	}


	template <typename T>
	constexpr auto AsyncAwaiter<T>::complete(T &&value) -> void {
		using namespace std::literals;

		m_value = std::move(value);

		fp::Job job {};
		job.work = [this]() noexcept {
			this->m_resume();
			return fp::Result::eSuccess;
		};

		fp::JobMetadata jobMetadata {};
		jobMetadata.priority = fp::JobPriority::__eResumingJob;

		fp::JobPushingLockGuard _ {};
		fp::JobQueue::push(jobMetadata, std::move(job));
	}


	template <typename T>
	constexpr auto AsyncAwaiter<T>::await_resume() noexcept -> T {
		return std::move(m_value);
	}


	template <typename T>
	template <__internals::IsAwaitSuspendReturn CoroutineHandle>
	constexpr auto AsyncAwaiter<T>::await_suspend(CoroutineHandle handle) noexcept -> void {
		m_resume = [handle]() noexcept {
			handle.resume();
		};
		m_callback(*this);
	}

} // namespace fp::coroutines
