#pragma once

#include <algorithm>

#include "fp/utils/constexprMap.hpp"
#include "fp/utils/enums.hpp"


namespace fp::utils {
#ifdef __clang__
	constexpr char PRETTY_FUNC_TEMPLATE_SEPARATOR {','};
#elifdef __GNUC__
	constexpr char PRETTY_FUNC_TEMPLATE_SEPARATOR {';'};
#endif

	template <IsEnum Enum, Enum value>
	consteval auto toString() noexcept -> std::optional<std::string_view> {
		std::string_view name {__PRETTY_FUNCTION__};

		name = {std::ranges::find(name, PRETTY_FUNC_TEMPLATE_SEPARATOR) + 2, name.end()};
		auto column {std::ranges::find(name, ':')};
		auto endOfName {std::ranges::find(name, ']')};
		if (column >= endOfName)
			return std::nullopt;
		name = {column + 2 + EnumTag_v<Enum>.size(), endOfName};
		return name;
	}


	template <IsEnum Enum, std::size_t index = 0, std::size_t maxIndex = EnumValueCount_v<Enum>>
	consteval auto __foreachConstexprEnumValue() noexcept -> fp::utils::ConstexprMap<Enum, std::string_view, maxIndex> {
		if constexpr (index < maxIndex) {
			auto map {__foreachConstexprEnumValue<Enum, index + 1, maxIndex> ()};
			constexpr Enum value {std::get<index> (EnumValueFinder_v<Enum>)};
			map.insert(value, *toString<Enum, value> ());
			return map;
		}
		else
			return {};
	};


	template <IsEnum Enum>
	consteval auto __getEnumStringMap() noexcept {
		return __foreachConstexprEnumValue<Enum> ();
	};


	template <IsEnum Enum>
	constexpr auto toString(Enum value) noexcept -> std::string_view {
		constexpr std::string_view INVALID_VALUE {"fp::invalid"};
		constexpr auto map {__getEnumStringMap<Enum> ()};

		std::optional<std::string_view> str {map[value]};
		if (!str)
			return INVALID_VALUE;
		return *str;
	}

} // namespace fp::utils
