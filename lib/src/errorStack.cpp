#include "fp/errorStack.hpp"

#include <print>


namespace fp {
	ErrorStackIterator::ErrorStackIterator(bool isEnd) noexcept :
		m_isEnd {isEnd}
	{
		if (!m_isEnd)
			m_view = ErrorStack::s_stacks[std::this_thread::get_id()].top();
	}


	auto ErrorStackRange::begin() const noexcept -> iterator {
		if (ErrorStack::s_stacks[std::this_thread::get_id()].empty())
			return this->end();
		return ErrorStackIterator(false);
	}


	auto ErrorStackIterator::operator++() noexcept -> ErrorStackIterator& {
		ErrorStack::s_stacks[std::this_thread::get_id()].pop();
		if (ErrorStack::s_stacks[std::this_thread::get_id()].empty())
			m_isEnd = true;
		else
			m_view = ErrorStack::s_stacks[std::this_thread::get_id()].top();
		return *this;
	}


	auto ErrorStack::push(std::string_view str, std::source_location location) noexcept -> void {
		std::string formattedStr {std::format("in {} ({}:{}) > {}",
			location.function_name(),
			location.file_name(),
			location.line(),
			str
		)};

		auto stack {s_stacks.find(std::this_thread::get_id())};
		if (stack == s_stacks.end()) {
			s_stacks[std::this_thread::get_id()] = std::stack<std::string> ();
			s_stacks[std::this_thread::get_id()].push(formattedStr);
		}
		else
			stack->second.push(formattedStr);
	}


	auto ErrorStack::logAll() noexcept -> void {
		auto stack {s_stacks.find(std::this_thread::get_id())};
		if (stack == s_stacks.end())
			return;
		if (stack->second.empty())
			return;
		std::println(stderr, "Some error occured in thread {} :", std::this_thread::get_id());
		for (const auto &error : ErrorStack::iterate())
			std::println(stderr, "\t{}", error);
	}


	std::unordered_map<std::thread::id, std::stack<std::string>> ErrorStack::s_stacks {};
} // namespace fp
