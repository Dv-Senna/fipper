#pragma once

#include <optional>
#include <string_view>
#include <tuple>


#if !defined(__clang__) && !defined(__GNUC__)
	#error Your compiler is not support for enum manipulation
	#include <stop_compilation>
#endif


namespace fp::utils {
	template <typename Enum>
	concept IsEnum = std::is_scoped_enum_v<Enum>;

	template <IsEnum Enum>
	struct EnumTag {
		static constexpr std::string_view value {"e"};
	};

	template <IsEnum Enum>
	constexpr auto EnumTag_v {EnumTag<Enum>::value};

	template <typename Enum>
	concept HasEnumTag = IsEnum<Enum>
		&& !EnumTag_v<Enum>.empty();


	template <IsEnum Enum>
	struct EnumIsFlag {
		static constexpr bool value {false};
	};

	template <IsEnum Enum>
	constexpr auto EnumIsFlag_v {EnumIsFlag<Enum>::value};

	template <typename Enum>
	concept IsEnumFlag = IsEnum<Enum>
		&& EnumIsFlag_v<Enum>;


	template <IsEnum Enum, Enum value>
	consteval auto toString() noexcept -> std::optional<std::string_view>;

	template <IsEnum Enum>
	auto toString(Enum value) noexcept -> std::string_view;


	template <IsEnum Enum>
	struct EnumMaxIndex {
		static constexpr std::size_t value {15};
	};

	template <IsEnum Enum>
	constexpr std::size_t EnumMaxIndex_v {EnumMaxIndex<Enum>::value};


	template <IsEnum Enum, Enum val>
	struct EnumIsValueValid {
		static constexpr bool value {!!toString<Enum, val> ()};
	};

	template <IsEnum Enum, Enum value>
	constexpr auto EnumIsValueValid_v {EnumIsValueValid<Enum, value>::value};


	template <IsEnum Enum, std::size_t index>
	struct EnumValueConstructor {
		static constexpr Enum (*func)(std::size_t) {[](std::size_t index2) -> Enum {
			return static_cast<Enum> (index2);
		}};
	};

	template <IsEnumFlag Enum, std::size_t index>
	struct EnumValueConstructor<Enum, index> {
		static constexpr Enum (*func)(std::size_t) {[](std::size_t index2) -> Enum {
			if (index2 >= sizeof(Enum) * 8)
				return static_cast<Enum> (0);
			return static_cast<Enum> (1 << index2);
		}};
	};

	template <IsEnum Enum, std::size_t index>
	constexpr auto EnumValueConstructor_f = EnumValueConstructor<Enum, index>::func;

	template <IsEnum Enum, std::size_t index>
	constexpr Enum EnumValue_v {EnumValueConstructor_f<Enum, index> (index)};


	template <IsEnum Enum, std::size_t index, bool isIndexValid = EnumIsValueValid_v<Enum, EnumValue_v<Enum, index>>>
	struct EnumValueAsTuple {
		static constexpr std::tuple<> value {};
	};

	template <IsEnum Enum, std::size_t index>
	struct EnumValueAsTuple<Enum, index, true> {
		static constexpr auto value {std::make_tuple(EnumValue_v<Enum, index>)};
	};

	template <IsEnum Enum, std::size_t index>
	constexpr auto EnumValueAsTuple_v {EnumValueAsTuple<Enum, index>::value};


	template <IsEnum Enum, std::size_t index = 0, std::size_t maxIndex = EnumMaxIndex_v<Enum>>
	struct EnumValueFinder {
		static constexpr auto value {std::tuple_cat(EnumValueAsTuple_v<Enum, index>, EnumValueFinder<Enum, index + 1, maxIndex>::value)};
	};

	template <IsEnum Enum, std::size_t maxIndex>
	struct EnumValueFinder<Enum, maxIndex, maxIndex> {
		static constexpr auto value {EnumValueAsTuple_v<Enum, maxIndex>};
	};

	template <IsEnum Enum>
	constexpr auto EnumValueFinder_v {EnumValueFinder<Enum>::value};

} // namespace fp::utils

#include "fp/utils/enums.inl"

#undef FP_PRETTY_ENUM_NAME_SUPPORTED
