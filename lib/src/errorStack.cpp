#include "fp/errorStack.hpp"

#include <print>


namespace fp {
	ErrorStackIterator::ErrorStackIterator(bool isEnd) noexcept :
		m_isEnd {isEnd}
	{
		if (!m_isEnd)
			m_view = ErrorStack::s_stack.top();
	}


	auto ErrorStackRange::begin() const noexcept -> iterator {
		if (ErrorStack::s_stack.empty())
			return this->end();
		return ErrorStackIterator(false);
	}


	auto ErrorStackIterator::operator++() noexcept -> ErrorStackIterator& {
		ErrorStack::s_stack.pop();
		if (ErrorStack::s_stack.empty())
			m_isEnd = true;
		else
			m_view = ErrorStack::s_stack.top();
		return *this;
	}


	auto ErrorStack::push(std::string_view str, std::source_location location) noexcept -> void {
		std::string formattedStr {std::format("in {} ({}:{}) > {}",
			location.function_name(),
			location.file_name(),
			location.line(),
			str
		)};
		s_stack.push(formattedStr);
	}


	auto ErrorStack::logAll() noexcept -> void {
		if (s_stack.empty())
			return;
		std::println(stderr, "Some error occured :");
		for (const auto &error : ErrorStack::iterate())
			std::println(stderr, "\t{}", error);
	}


	std::stack<std::string> ErrorStack::s_stack {};
} // namespace fp
