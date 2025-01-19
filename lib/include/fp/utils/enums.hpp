#pragma once

#include <optional>
#include <string_view>


#if defined(__clang__) || defined(__GNUC__)
	#define FP_PRETTY_ENUM_NAME_SUPPORTED
#endif


namespace fp::utils {
	template <typename Enum>
	concept IsEnum = std::is_scoped_enum_v<Enum>;

	template <IsEnum Enum>
	struct EnumTag {
		static constexpr std::string_view tag {"e"};
	};

	template <typename Enum>
	concept HasEnumTag = IsEnum<Enum>
		&& !EnumTag<Enum>::tag.empty();

	template <IsEnum Enum, Enum value>
	consteval auto toString() noexcept -> std::optional<std::string_view>;

	template <IsEnum Enum>
	auto toString(Enum value) noexcept -> std::string_view;

} // namespace fp::utils

#include "fp/utils/enums.inl"

#undef FP_PRETTY_ENUM_NAME_SUPPORTED
