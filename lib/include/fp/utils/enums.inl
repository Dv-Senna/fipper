#pragma once

#include <algorithm>
#include <ranges>

#include "fp/utils/constexprMap.hpp"
#include "fp/utils/enums.hpp"


namespace fp::utils {
	template <IsEnum Enum, Enum value>
	consteval auto toString() noexcept -> std::optional<std::string_view> {
		std::string_view name {__PRETTY_FUNCTION__};
		std::string_view lookupString {"value = "};

		name = {name.begin() + name.find(lookupString) + lookupString.size(), name.end() - 1};
		if (name[0] == '(')
			return std::nullopt;

		auto column {std::ranges::find_last(name, ':').begin()};
		if (column != name.end())
			name = {column + 1, name.end()};

		if (name.size() <= EnumTag_v<Enum>.size())
			return std::nullopt;
		name = {name.begin() + EnumTag_v<Enum>.size(), name.end()};

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
		constexpr std::string_view INVALID_VALUE {"<invalid>"};
		constexpr auto map {__getEnumStringMap<Enum> ()};

		std::optional<std::string_view> str {map[value]};
		if (!str)
			return INVALID_VALUE;
		return *str;
	}

} // namespace fp::utils
