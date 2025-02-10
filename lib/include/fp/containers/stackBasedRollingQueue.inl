#pragma once

#include "fp/containers/stackBasedRollingQueue.hpp"


namespace fp::containers {
	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueueIterator<T, N>::operator==(const StackBasedRollingQueueIterator<T, N> &iterator) const noexcept -> bool {
		return m_queue == iterator.m_queue && m_isEnd == iterator.m_isEnd;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueueIterator<T, N>::operator++() noexcept -> StackBasedRollingQueueIterator<T, N>& {
		std::optional result {m_queue->pop()};
		m_isEnd = !result;
		return *this;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueueIterator<T, N>::operator*() const noexcept -> reference {
		return *m_queue->getTopRef();
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueueIterator<T, N>::operator->() const noexcept -> pointer {
		return m_queue->getTopRef();
	}


	template <typename T, std::size_t N>
	constexpr StackBasedRollingQueueIterator<T, N>::StackBasedRollingQueueIterator(StackBasedRollingQueue<T, N> *queue, bool isEnd) noexcept :
		m_queue {queue},
		m_isEnd {isEnd}
	{

	}




	template <typename T, std::size_t N>
	constexpr StackBasedRollingQueue<T, N>::StackBasedRollingQueue() noexcept :
		m_start {reinterpret_cast<T*> (&m_datas)},
		m_size {0},
		m_datas {}
	{

	}


	template <typename T, std::size_t N>
	constexpr StackBasedRollingQueue<T, N>::~StackBasedRollingQueue() {
		for (; m_size-- != 0; ++m_start) {
			if (m_start >= reinterpret_cast<T*> (&m_datas) + N)
				m_start -= N;
			m_start->~T();
		}
	}


	template <typename T, std::size_t N>
	template <typename Enable>
	constexpr StackBasedRollingQueue<T, N>::StackBasedRollingQueue(const StackBasedRollingQueue<T, N> &queue) noexcept :
		m_start {reinterpret_cast<T*> (&m_datas)},
		m_size {queue.m_size},
		m_datas {}
	{
		T *current {m_start};
		const T *queueCurrent {queue.m_start};
		for (std::size_t i {0}; i < m_size; ++i) {
			current->~T();
			new (current) T(*queueCurrent);

			++current;
			if (++queueCurrent >= reinterpret_cast<const T*> (&queue.m_datas) + N)
				queueCurrent -= N;
		}
	}


	template <typename T, std::size_t N>
	template <typename Enable>
	constexpr auto StackBasedRollingQueue<T, N>::operator=(const StackBasedRollingQueue<T, N> &queue) noexcept -> StackBasedRollingQueue<T, N>& {
		this->~StackBasedRollingQueue<T, N> ();
		new (this) StackBasedRollingQueue<T, N> (queue);
		return *this;
	}


	template <typename T, std::size_t N>
	template <typename Enable>
	constexpr StackBasedRollingQueue<T, N>::StackBasedRollingQueue(StackBasedRollingQueue<T, N> &&queue) noexcept :
		m_start {reinterpret_cast<T*> (&m_datas)},
		m_size {queue.m_size},
		m_datas {}
	{
		T *current {m_start};
		const T *queueCurrent {queue.m_start};
		for (std::size_t i {0}; i < m_size; ++i) {
			current->~T();
			if constexpr (std::is_move_constructible_v<T>)
				new (current) T(std::move(*queueCurrent));
			else
				new (current) T(*queueCurrent);

			++current;
			if (++queueCurrent >= reinterpret_cast<const T*> (&queue.m_datas) + N)
				queueCurrent -= N;
		}
	}


	template <typename T, std::size_t N>
	template <typename Enable>
	constexpr auto StackBasedRollingQueue<T, N>::operator=(StackBasedRollingQueue<T, N> &&queue) noexcept -> StackBasedRollingQueue<T, N>& {
		this->~StackBasedRollingQueue<T, N> ();
		new (this) StackBasedRollingQueue<T, N> (std::move(queue));
		return *this;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::push(T &&value) noexcept -> fp::Result {
		if (m_size == N)
			return fp::Result::eSaturated;

		T *end {this->m_end()};
		++m_size;
		new (end) T(std::move(value));
		return fp::Result::eSuccess;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::pop() noexcept -> std::optional<T> {
		if (m_size == 0)
			return std::nullopt;

		--m_size;
		T object {std::move(*m_start)};
		m_start->~T();
		if (++m_start >= reinterpret_cast<T*> (&m_datas) + N)
			m_start -= N;
		return object;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::begin() noexcept -> iterator {
		return iterator(this, false);
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::end() noexcept -> iterator {
		return iterator(this, true);

	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::begin() const noexcept -> const_iterator {
		return const_iterator(this, false);
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::end() const noexcept -> const_iterator {
		return const_iterator(this, true);
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::m_end() noexcept -> T* {
		T *end {reinterpret_cast<T*> (m_start) + m_size};
		if (end >= reinterpret_cast<T*> (&m_datas) + N)
			end -= N;
		return end;
	}


	template <typename T, std::size_t N>
	constexpr auto StackBasedRollingQueue<T, N>::m_end() const noexcept -> const T* {
		const T *end {reinterpret_cast<const T*> (m_start) + m_size};
		if (end >= reinterpret_cast<const T*> (&m_datas) + N)
			end -= N;
		return end;
	}

} // namespace fp::containers
