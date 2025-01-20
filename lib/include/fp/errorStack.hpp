#pragma once

#include <format>
#include <source_location>
#include <stack>
#include <string>
#include <string_view>

#include <print>

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

			template <typename ...Args>
			static auto push(
				std::format_string<Args...> str,
				std::source_location location = std::source_location::current(),
				Args &&...args
			) noexcept -> void {
				std::string formattedStr {std::format("in {} ({}:{}) > {}",
					location.function_name(),
					location.file_name(),
					location.line(),
					std::format(str, std::forward(args)...)
				)};
				s_stack.push(formattedStr);
			}

			template <typename Return, typename ...Args>
			[[nodiscard]] static auto push(
				Return &&ret,
				std::format_string<Args...> str,
				std::source_location location = std::source_location::current(),
				Args &&...args
			) noexcept {
				if constexpr (std::same_as<Return, fp::Result>)
					ErrorStack::push("{} : {}", location, fp::utils::toString(ret), std::format(str, std::forward(args)...));
				else
					ErrorStack::push(str, location, std::forward(args)...);
				return ret;
			}

			[[nodiscard]] static inline auto iterate() noexcept -> ErrorStackRange {return ErrorStackRange();}


		protected:
			static std::stack<std::string> s_stack;
	};

} // namespace fp

