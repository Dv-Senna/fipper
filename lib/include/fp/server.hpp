#pragma once

#include "fp/core.hpp"
#include "fp/result.hpp"
#include "fp/socket.hpp"


namespace fp {
	class FP_CORE Server {
		public:
			struct CreateInfos {
				std::uint16_t port;
			};

			Server() noexcept = default;
			~Server() = default;

			[[nodiscard]] auto create(const CreateInfos &createInfos) noexcept -> fp::Result;
			[[nodiscard]] auto run() noexcept -> fp::Result;


		private:
			fp::Socket m_serverSocket;
	};

} // namespace fp
