#pragma once

#include <chrono>
#include <functional>
#include <tuple>
#include <utility>

#include "fp/utils/hash.hpp"


namespace fp::utils {
	template <fp::utils::Hash_t HASH>
	struct BenchmarkEnabled : std::true_type {};

	template <fp::utils::Hash_t HASH, typename ...Args>
	class Benchmark {
		public:
			using Callback = std::function<void(std::chrono::microseconds, Args&&...)>;

			constexpr Benchmark() noexcept {
				if constexpr (BenchmarkEnabled<HASH>::value) {
					m_startTimePoint = std::chrono::high_resolution_clock::now();
				}
			}

			constexpr ~Benchmark() {
				if constexpr (BenchmarkEnabled<HASH>::value) {
					auto duration {std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - m_startTimePoint)};
					(void)std::apply(s_callback, std::tuple_cat(std::make_tuple(duration), m_args));
				}
			}

			constexpr auto setArgs(const Args &...args) noexcept {
				if constexpr (BenchmarkEnabled<HASH>::value) {
					m_args = std::make_tuple(args...);
				}
			}

			static constexpr auto setCallback(Callback callback) noexcept -> void {
				if constexpr (BenchmarkEnabled<HASH>::value) {
					s_callback = std::move(callback);
				}
			}

		private:
			static Callback s_callback;

			std::chrono::high_resolution_clock::time_point m_startTimePoint;
			std::tuple<Args...> m_args;
	};


	template <fp::utils::Hash_t HASH, typename ...Args>
	Benchmark<HASH, Args...>::Callback Benchmark<HASH, Args...>::s_callback {};

} // namespace fp::utils
