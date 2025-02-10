#pragma once

#include <functional>

#include "fp/core.hpp"
#include "fp/result.hpp"


namespace fp {
	enum class JobPriority {
		eLow,
		eMedium,
		eHigh,
		eCritical,
		__eResumingJob
	};

	struct Job {
		JobPriority priority;
		std::function<fp::Result()> work;
	};



	class FP_CORE JobQueue {
		public:
			JobQueue() noexcept = delete;
			JobQueue(const JobQueue&) noexcept = delete;
			auto operator=(const JobQueue&) noexcept -> JobQueue& = delete;
			JobQueue(JobQueue&&) noexcept = delete;
			auto operator=(JobQueue&&) noexcept -> JobQueue& = delete;


		private:

	};

} // namespace fp
