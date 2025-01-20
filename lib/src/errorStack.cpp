#include "fp/errorStack.hpp"


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


	std::stack<std::string> ErrorStack::s_stack {};
} // namespace fp
