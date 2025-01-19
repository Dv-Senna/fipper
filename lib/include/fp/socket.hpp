#pragma once

#ifndef FP_LINUX
	#error Socket are only supported on Linux for now
	#include <stop_compilation>
#endif

#include <cstdint>

#include "fp/core.hpp"
#include "fp/result.hpp"


namespace fp {
	class FP_CORE Socket {
		public:
			struct CreateInfos {
				std::uint16_t port;
			};

			Socket() noexcept = default;
			~Socket();

			[[nodiscard]]
			auto create(const CreateInfos &createInfos) noexcept -> fp::Result;

			inline auto getSocket() const noexcept -> int {return m_socket;}

		private:
			int m_socket;
	};

} // namespace fp
