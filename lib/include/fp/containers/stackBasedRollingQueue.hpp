#pragma once

#include <cstddef>
#include <optional>
#include <type_traits>

#include "fp/result.hpp"


namespace fp::containers {
	template <typename T, std::size_t N>
	class StackBasedRollingQueue;


	template <typename T, std::size_t N>
	class StackBasedRollingQueueIterator final {
		friend class StackBasedRollingQueue<T, N>;

		public:
			using value_type = T;
			using iterator_category = std::input_iterator_tag;
			using reference = T&;
			using pointer = T*;
			using difference_type = std::ptrdiff_t;

			constexpr StackBasedRollingQueueIterator() noexcept = default;
			constexpr auto operator==(const StackBasedRollingQueueIterator<T, N> &iterator) const noexcept -> bool;

			constexpr auto operator++() noexcept -> StackBasedRollingQueueIterator<T, N>&;
			constexpr auto operator++(int) noexcept -> StackBasedRollingQueueIterator<T, N>& {auto tmp {*this}; ++(*this); return tmp;}

			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;


		protected:
			constexpr StackBasedRollingQueueIterator(StackBasedRollingQueue<T, N> *queue, bool isEnd) noexcept;

		private:
			StackBasedRollingQueue<T, N> *m_queue;
			bool m_isEnd;
	};

	static_assert(std::input_iterator<StackBasedRollingQueueIterator<int, 16>>);
	static_assert(std::input_iterator<StackBasedRollingQueueIterator<const int, 16>>);


	template <typename T, std::size_t N>
	class StackBasedRollingQueue final {
		friend class StackBasedRollingQueueIterator<T, N>;

		public:
			using iterator = StackBasedRollingQueueIterator<T, N>;
			using const_iterator = StackBasedRollingQueueIterator<const T, N>;

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

			constexpr auto begin() noexcept -> iterator;
			constexpr auto end() noexcept -> iterator;
			constexpr auto begin() const noexcept -> const_iterator;
			constexpr auto end() const noexcept -> const_iterator;


		protected:
			constexpr auto getTopRef() noexcept -> T* {return m_start;}
			constexpr auto getTopRef() const noexcept -> const T* {return m_start;}

		private:
			constexpr auto m_end() noexcept -> T*;
			constexpr auto m_end() const noexcept -> const T*;

			T *m_start;
			std::size_t m_size;
			// defined as overaligned array of bytes to avoid default construct / destruct
			alignas(T) std::byte m_datas[N * sizeof(T)];
	};

	static_assert(std::ranges::input_range<StackBasedRollingQueue<int, 16>>);

} // namespace fp::containers

#include "fp/containers/stackBasedRollingQueue.inl"
