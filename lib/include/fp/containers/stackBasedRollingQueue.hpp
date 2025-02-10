#pragma once

#include <cstddef>
#include <optional>
#include <type_traits>

#include "fp/result.hpp"


namespace fp::containers {
	template <typename T, std::size_t N>
	class StackBasedRollingQueue final {
		public:
			constexpr StackBasedRollingQueue() noexcept;
			constexpr ~StackBasedRollingQueue();

			template <typename Enable = std::enable_if_t<std::is_copy_constructible_v<T>, void>>
			constexpr StackBasedRollingQueue(const StackBasedRollingQueue<T, N> &queue) noexcept;
			template <typename Enable = std::enable_if_t<std::is_copy_constructible_v<T>, void>>
			constexpr auto operator=(const StackBasedRollingQueue<T, N> &queue) noexcept -> StackBasedRollingQueue<T, N>&;

			template <typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>, void>>
			constexpr StackBasedRollingQueue(StackBasedRollingQueue<T, N> &&queue) noexcept;
			template <typename Enable = std::enable_if_t<std::is_copy_constructible_v<T> || std::is_move_constructible_v<T>, void>>
			constexpr auto operator=(StackBasedRollingQueue<T, N> &&queue) noexcept -> StackBasedRollingQueue<T, N>&;

			[[nodiscard]] constexpr auto push(T &&value) noexcept -> fp::Result;
			[[nodiscard]] constexpr auto pop() noexcept -> std::optional<T>;

			constexpr auto size() const noexcept -> std::size_t {return m_size;}
			constexpr auto maxSize() const noexcept -> std::size_t {return N;}


		private:
			constexpr auto m_end() noexcept -> T*;
			constexpr auto m_end() const noexcept -> const T*;

			T *m_start;
			std::size_t m_size;
			// defined as overaligned array of bytes to avoid default construct / destruct
			alignas(T) std::byte m_datas[N * sizeof(T)];
	};

} // namespace fp::containers

#include "fp/containers/stackBasedRollingQueue.inl"
