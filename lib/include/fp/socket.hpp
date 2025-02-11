#pragma once

#ifndef FP_LINUX
	#error Socket are only supported on Linux for now
	#include <stop_compilation>
#endif

#include <chrono>
#include <cstdint>
#include <expected>
#include <optional>
#include <vector>

#include "fp/core.hpp"
#include "fp/result.hpp"


namespace fp {
	using namespace std::literals;

	class FP_CORE Socket {
		public:
			using NativeHandle = int;

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
			[[nodiscard]] auto accept(std::chrono::milliseconds timeout = 0ms) noexcept -> std::expected<Socket, fp::Result>;

			[[nodiscard]] auto hasDataToRecieve(std::chrono::milliseconds timeout = 0ms) noexcept -> std::optional<bool>;
			template <std::size_t BUFFER_SIZE = 1024>
			[[nodiscard]] inline auto recieve() noexcept -> std::expected<std::vector<std::byte>, fp::Result> {
				std::byte buffer[BUFFER_SIZE];
				return this->recieve(buffer, BUFFER_SIZE);
			}
			[[nodiscard]] auto recieve(std::byte *buffer, std::size_t bufferSize) noexcept -> std::expected<std::vector<std::byte>, fp::Result>;

			[[nodiscard]] auto send(const std::vector<std::byte> &data) noexcept -> fp::Result;

			inline auto getNativeHandle() const noexcept -> NativeHandle {return m_socket;}


		protected:
			int m_socket;
	};

} // namespace fp

