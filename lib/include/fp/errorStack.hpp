#pragma once

#include <format>
#include <source_location>
#include <stack>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "fp/core.hpp"
#include "fp/result.hpp"
#include "fp/utils/enums.hpp"


namespace fp {
	class ErrorStack;
	class ErrorStackRange;

	class FP_CORE ErrorStackIterator final {
		friend class ErrorStackRange;

		public:
			using value_type = std::string_view;
			using iterator_category = std::input_iterator_tag;
			using reference = const value_type&;
			using pointer = const value_type*;
			using difference_type = std::ptrdiff_t;

			ErrorStackIterator() noexcept = default;

			constexpr auto operator==(const ErrorStackIterator &iterator) const noexcept {return m_isEnd == iterator.m_isEnd;}

			auto operator++() noexcept -> ErrorStackIterator&;
			inline auto operator++(int) noexcept -> ErrorStackIterator {auto tmp {*this}; ++(*this); return tmp;}

			constexpr auto operator*() const noexcept -> reference {return m_view;}
			constexpr auto operator->() const noexcept -> pointer {return &m_view;}


		protected:
			ErrorStackIterator(bool isEnd) noexcept;

		private:
			std::string_view m_view;
			bool m_isEnd;
	};

	static_assert(std::input_iterator<ErrorStackIterator>);


	class FP_CORE ErrorStackRange final {
		friend class ErrorStack;

		public:
			using iterator = ErrorStackIterator;

			ErrorStackRange(const ErrorStackRange &) noexcept = delete;
			ErrorStackRange(ErrorStackRange &&) noexcept = default;

			auto begin() const noexcept -> iterator;
			inline auto end() const noexcept -> iterator {return ErrorStackIterator(true);}


		protected:
			ErrorStackRange() noexcept = default;
	};

	static_assert(std::ranges::input_range<ErrorStackRange>);


	class FP_CORE ErrorStack final {
		friend class ErrorStackIterator;
		friend class ErrorStackRange;

		public:
			ErrorStack() = delete;

			static auto push(std::string_view str, std::source_location location = std::source_location::current()) noexcept -> void;

			template <typename Return>
			[[nodiscard]] static auto push(
				Return &&ret,
				std::string_view str,
				std::source_location location = std::source_location::current()
			) noexcept {
				if constexpr (std::same_as<Return, fp::Result>)
					ErrorStack::push(std::format("{} : {}", fp::utils::toString(ret), str), location);
				else
					ErrorStack::push(str, location);
				return ret;
			}

			[[nodiscard]] static inline auto iterate() noexcept -> ErrorStackRange {return ErrorStackRange();}
			
			static auto logAll() noexcept -> void;
			template <typename Return>
			[[nodiscard]] static auto logAll(Return &&ret) {
				ErrorStack::logAll();
				return ret;
			}


		protected:
			static std::unordered_map<std::thread::id, std::stack<std::string>> s_stacks;
	};

} // namespace fp

