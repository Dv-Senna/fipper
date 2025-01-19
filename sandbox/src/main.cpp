#include <print>

#include <fp/utils/enums.hpp>


enum class Color {
	eRed = 0xff0000,
	eGreen = 0x00ff00,
	eBlue = 0x0000ff
};

enum class Color2 {
	red = 0xff0000,
	green = 0x00ff00,
	blue = 0x0000ff
};


template <>
struct fp::utils::EnumTag<Color2> {
	static constexpr std::string_view tag {""};
};


int main() {
	static_assert(fp::utils::HasEnumTag<Color>);
	static_assert(!fp::utils::HasEnumTag<Color2>);

	std::println("comp eRed : '{}'", fp::utils::toString<Color, Color::eRed> ().value_or("invalid"));
	std::println("comp invalid : '{}'", fp::utils::toString<Color, (Color)1> ().value_or("invalid"));

//	std::println("eRed : '{}'", fp::utils::toString<Color> (Color::eRed));
//	std::println("invalid : '{}'", fp::utils::toString<Color> ((Color)1));

	return 0;
}
