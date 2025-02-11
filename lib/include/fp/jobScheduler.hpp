#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <optional>
#include <mutex>
#include <vector>

#include "fp/containers/stackBasedRollingQueue.hpp"
#include "fp/core.hpp"
#include "fp/result.hpp"
#include "fp/utils/enums.hpp"


namespace fp {
	/*
	 * @brief An enum representing the priority of job
	 * @value __eLowest The lowest priority. Reserved for fipper internal use
	 * @value eLow Low priority job. The lowest user-accessible priority
	 * @value eMedium Medium priority job
	 * @value eHight High priority job
	 * @value eCritical Critical priority job. Must be execute asap
	 * @value __eResumingJob Job that were paused but are resuming take the highest priority.
	 *                       Reserved for fipper internal use
	 * */
	enum class JobPriority : std::size_t {
		__eLowest = 0,
		eLow,
		eMedium,
		eHigh,
		eCritical,
		__eResumingJob
	};

	struct JobMetadata {
		JobPriority priority;
	};

	struct Job {
		std::move_only_function<fp::Result()> work;
	};



	class JobAcquisitionLockGuard;
	class JobPushingLockGuard;

	class FP_CORE JobQueue {
		using QueueType = std::array<fp::containers::StackBasedRollingQueue<Job, FP_JOB_QUEUE_SIZE>, fp::utils::EnumValueCount_v<JobPriority>>;

		friend class JobAcquisitionLockGuard;
		friend class JobPushingLockGuard;

		public:
			JobQueue() noexcept = delete;
			JobQueue(const JobQueue&) noexcept = delete;
			auto operator=(const JobQueue&) noexcept -> JobQueue& = delete;
			JobQueue(JobQueue&&) noexcept = delete;
			auto operator=(JobQueue&&) noexcept -> JobQueue& = delete;

			static auto push(const JobMetadata &metadata, Job &&job) noexcept -> fp::Result;
			static auto pop() noexcept -> std::optional<Job>;

		protected:
			static std::mutex s_mutex;
			static std::condition_variable s_conditionVariable;
			static std::size_t s_queuedJobsCount;

		private:
			static QueueType s_queues;
	};


	class FP_CORE JobAcquisitionLockGuard {
		public:
			JobAcquisitionLockGuard() noexcept;
			~JobAcquisitionLockGuard() = default;

			JobAcquisitionLockGuard(const JobAcquisitionLockGuard&) noexcept = delete;
			auto operator=(const JobAcquisitionLockGuard&) noexcept -> JobAcquisitionLockGuard& = delete;
			JobAcquisitionLockGuard(JobAcquisitionLockGuard&&) noexcept = delete;
			auto operator=(JobAcquisitionLockGuard&&) noexcept -> JobAcquisitionLockGuard& = delete;

			auto waitForJob(std::chrono::milliseconds timeout) noexcept -> bool;

		private:
			std::unique_lock<std::mutex> m_lock;
	};


	class FP_CORE JobPushingLockGuard {
		public:
			JobPushingLockGuard() noexcept;
			~JobPushingLockGuard();

			JobPushingLockGuard(const JobPushingLockGuard&) noexcept = delete;
			auto operator=(const JobPushingLockGuard&) noexcept -> JobPushingLockGuard& = delete;
			JobPushingLockGuard(JobPushingLockGuard&&) noexcept = delete;
			auto operator=(JobPushingLockGuard&&) noexcept -> JobPushingLockGuard& = delete;
	};


	class Server;
	
	class FP_CORE JobScheduler {
		friend class Server;

		public:
			struct CreateInfos {
				std::size_t minThreadCount {4};
			};

			~JobScheduler() noexcept;

			[[nodiscard]] auto create(const CreateInfos &createInfos) noexcept -> fp::Result;

		protected:
			JobScheduler() noexcept = default;

		private:
			static auto s_threadMain() noexcept -> void;

			static std::atomic_bool s_running;
			std::vector<std::thread> m_threads;
	};

} // namespace fp
