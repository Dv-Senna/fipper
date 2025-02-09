#pragma once

#include <optional>
#include <string_view>

#include <glaze/glaze.hpp>


namespace fp {
	template <typename T>
	auto deserialize(std::string_view str) noexcept -> std::optional<T> {
		T output {};
		if (glz::read<glz::opts{.minified = true}> (output, str))
			return std::nullopt;
		return output;
	}

}
