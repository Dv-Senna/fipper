#pragma once

#include <algorithm>

#include "fp/utils/enums.hpp"


namespace fp::utils {
#ifdef __clang__
	constexpr char PRETTY_FUNC_TEMPLATE_SEPARATOR {','};
#elifdef __GNUC__
	constexpr char PRETTY_FUNC_TEMPLATE_SEPARATOR {';'};
#endif

	template <IsEnum Enum, Enum value>
	consteval auto toString() noexcept -> std::optional<std::string_view> {
	#ifndef FP_PRETTY_ENUM_NAME_SUPPORTED
		return std::nullopt;
	#else
	#if defined(__clang__) || defined(__GNUC__)
		std::string_view name {__PRETTY_FUNCTION__};
	#endif

		name = {std::ranges::find(name, PRETTY_FUNC_TEMPLATE_SEPARATOR) + 2, name.end()};
		auto column {std::ranges::find(name, ':')};
		auto endOfName {std::ranges::find(name, ']')};
		if (column >= endOfName)
			return std::nullopt;
		name = {column + 2, endOfName};
		return name;
	#endif
	}


	template <IsEnum Enum>
	auto toString(Enum value) noexcept -> std::string_view {
		constexpr std::string_view INVALID_VALUE {"fp::invalid"};


		return INVALID_VALUE;
	}

} // namespace fp::utils
