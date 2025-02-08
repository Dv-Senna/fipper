#pragma once

#include <optional>
#include <string_view>


namespace fp {
	template <typename T>
	auto deserialize(std::string_view str) noexcept -> std::optional<T>;
}
