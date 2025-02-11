#include "fp/jobScheduler.hpp"

#include <algorithm>
#include <print>
#include <ranges>

#include "fp/errorStack.hpp"


namespace fp {
	auto JobQueue::push(const JobMetadata &metadata, Job &&job) noexcept -> fp::Result {
		auto &queue {s_queues[static_cast<std::size_t> (metadata.priority)]};
		auto result {queue.push(std::move(job))};
		if (result != fp::Result::eSuccess)
			return result;
		++s_queuedJobsCount;
		return fp::Result::eSuccess;
	}


	auto JobQueue::pop() noexcept -> std::optional<Job> {
		for (auto &queue : s_queues | std::views::reverse) {
			if (queue.empty())
				continue;
			--s_queuedJobsCount;
			return queue.pop();
		}

		return std::nullopt;
	}


	std::mutex JobQueue::s_mutex {};
	std::condition_variable JobQueue::s_conditionVariable {};
	std::size_t JobQueue::s_queuedJobsCount {0};
	JobQueue::QueueType JobQueue::s_queues {};



	JobAcquisitionLockGuard::JobAcquisitionLockGuard() noexcept :
		m_lock {JobQueue::s_mutex}
	{

	}


	auto JobAcquisitionLockGuard::waitForJob(std::chrono::milliseconds timeout) noexcept -> bool {
		return JobQueue::s_conditionVariable.wait_for(m_lock, timeout, [](){return JobQueue::s_queuedJobsCount != 0;});
	}



	JobPushingLockGuard::JobPushingLockGuard() noexcept {
		JobQueue::s_mutex.lock();
	}


	JobPushingLockGuard::~JobPushingLockGuard() {
		JobQueue::s_mutex.unlock();
		JobQueue::s_conditionVariable.notify_one();
	}




	JobScheduler::~JobScheduler() noexcept {
		s_running = false;
		for (auto &thread : m_threads)
			thread.join();
	}


	auto JobScheduler::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		std::size_t threadCount {std::max<std::size_t> (createInfos.minThreadCount, std::thread::hardware_concurrency())};
		std::println("Created {} working thread", threadCount);

		for (std::size_t _ : std::views::iota(std::size_t{0}, threadCount))
			m_threads.push_back(std::thread{JobScheduler::s_threadMain});

		return fp::Result::eSuccess;
	}


	auto JobScheduler::s_threadMain() noexcept -> void {
		using namespace std::literals;

		while (s_running) {
			std::optional<Job> jobWithError {std::nullopt};
			{
				JobAcquisitionLockGuard acquisitionLockGuard {};
				if (!acquisitionLockGuard.waitForJob(1000ms))
					continue;
				jobWithError = JobQueue::pop();
			}

			if (!jobWithError) {
				fp::ErrorStack::push("No job actually retrieved from the job queue");
				fp::ErrorStack::logAll();
				continue;
			}

			Job job {std::move(*jobWithError)};
			if (job.work() != fp::Result::eSuccess) {
				fp::ErrorStack::push("Job failed");
				fp::ErrorStack::logAll();
			}
		}
	}


	std::atomic_bool JobScheduler::s_running {true};

} // namespace fp
