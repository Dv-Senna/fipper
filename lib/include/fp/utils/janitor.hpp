#pragma once

#include <concepts>
#include <utility>


namespace fp::utils {
	template <std::invocable<> Func>
	requires std::is_void_v<std::invoke_result_t<Func>>
	class Janitor {
		public:
			template <std::same_as<Func> Func2>
			constexpr Janitor(Func2 &&func) noexcept : m_func {std::move(func)} {}
			~Janitor() {m_func();}

			template <std::same_as<Func> Func2>
			requires (!std::is_nothrow_invocable_v<Func>)
			[[deprecated("It's recommended to pass a noexcept invocable to the Janitor to avoid terminating condition")]]
			constexpr Janitor(Func2 &&func) noexcept : m_func {std::move(func)} {}
		
		private:
			Func &&m_func;
	};

	
	template <std::invocable<> Func>
	Janitor(Func &&func) noexcept -> Janitor<Func>;

} // namespace fp::utils
