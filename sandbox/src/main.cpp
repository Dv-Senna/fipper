#include <print>
#include <ranges>

#include <fp/utils/enums.hpp>


enum class Color {
	eRed = 0xff0000,
	eGreen = 0x00ff00,
	eBlue = 0x0000ff
};

enum class Component {
	cpu = 1 << 0,
	gpu = 1 << 1,
	psu = 1 << 6,
	mobo = 1 << 10,
	wifi = 1 << 16
};

template <>
struct fp::utils::EnumTag<Component> {
	static constexpr std::string_view value {""};
};

template <>
struct fp::utils::EnumIsFlag<Component> {
	static constexpr bool value {true};
};

template <std::size_t index>
struct fp::utils::EnumValueConstructor<Color, index> {
	static constexpr auto func {[](size_t index2) -> Color {
		std::underlying_type_t<Color> color {};
		std::underlying_type_t<Color> mask {0xf};
		for (const auto i : std::views::iota(std::size_t{0}, std::size_t{8})) {
			std::size_t current {std::size_t{0b10} << i};
			std::size_t last {current >> 1};
			if ((index2 % current) >= last)
				color |= mask;
			mask <<= 4;
		}
		return static_cast<Color> (color);
	}};
};


int main() {
	static_assert(fp::utils::HasEnumTag<Color>);
	static_assert(!fp::utils::HasEnumTag<Component>);

	constexpr auto valueOfColor {fp::utils::EnumValueFinder_v<Color>};
	std::println("SIZE OF TUPLE : {}", std::tuple_size_v<decltype(valueOfColor)>);
	std::println("Color <0> : {}", fp::utils::toString<Color, std::get<0> (valueOfColor)> ().value_or("invalid"));
	std::println("Color <1> : {}", fp::utils::toString<Color, std::get<1> (valueOfColor)> ().value_or("invalid"));
	std::println("Color <2> : {}", fp::utils::toString<Color, std::get<2> (valueOfColor)> ().value_or("invalid"));

	constexpr auto valueOfComponent {fp::utils::EnumValueFinder_v<Component>};
	std::println("SIZE OF TUPLE : {}", std::tuple_size_v<decltype(valueOfComponent)>);
	std::println("Component <0> : {}", fp::utils::toString<Component, std::get<0> (valueOfComponent)> ().value_or("invalid"));
	std::println("Component <1> : {}", fp::utils::toString<Component, std::get<1> (valueOfComponent)> ().value_or("invalid"));
	std::println("Component <2> : {}", fp::utils::toString<Component, std::get<2> (valueOfComponent)> ().value_or("invalid"));
	std::println("Component <3> : {}", fp::utils::toString<Component, std::get<3> (valueOfComponent)> ().value_or("invalid"));
	std::println("Component <4> : {}", fp::utils::toString<Component, std::get<4> (valueOfComponent)> ().value_or("invalid"));

	std::println("comp cpu : '{}'", fp::utils::toString<Component, Component::cpu> ().value_or("invalid"));
	std::println("comp invalid : '{}'", fp::utils::toString<Color, (Color)1> ().value_or("invalid"));

//	std::println("eRed : '{}'", fp::utils::toString<Color> (Color::eRed));
//	std::println("invalid : '{}'", fp::utils::toString<Color> ((Color)1));

	return 0;
}
