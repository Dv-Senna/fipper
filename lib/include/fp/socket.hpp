#pragma once

#ifndef FP_LINUX
	#error Socket are only supported on Linux for now
	#include <stop_compilation>
#endif

#include <cstdint>
#include <expected>
#include <optional>
#include <vector>

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

			Socket(const Socket &) noexcept = delete;
			Socket &operator=(const Socket &) noexcept = delete;

			Socket(Socket &&socket) noexcept;
			Socket &operator=(Socket &&socket) noexcept;

			[[nodiscard]] auto create(const CreateInfos &createInfos) noexcept -> fp::Result;

			[[nodiscard]] auto listen(std::uint32_t queueSizeHint = 1) noexcept -> fp::Result;
			[[nodiscard]] auto accept() noexcept -> std::optional<Socket>;
			[[nodiscard]] auto recieve() noexcept -> std::expected<std::vector<std::byte>, fp::Result>;
			[[nodiscard]] auto send(const std::vector<std::byte> &data) noexcept -> fp::Result;

			inline auto getSocket() const noexcept -> int {return m_socket;}


		private:
			int m_socket;
	};

} // namespace fp
