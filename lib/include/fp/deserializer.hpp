#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

#include <glaze/glaze.hpp>

#include "fp/core.hpp"


namespace fp {
	template <typename T>
	auto deserialize(std::string_view str) noexcept -> std::optional<T> {
		T output {};
		if (glz::read<glz::opts{.minified = true}> (output, str))
			return std::nullopt;
		return output;
	}


	template <>
	FP_CORE auto deserialize<std::string> (std::string_view str) noexcept -> std::optional<std::string>;
	template <>
	FP_CORE auto deserialize<std::string_view> (std::string_view str) noexcept -> std::optional<std::string_view>;

	template <>
	FP_CORE auto deserialize<std::uint8_t> (std::string_view str) noexcept -> std::optional<std::uint8_t>;
	template <>
	FP_CORE auto deserialize<std::int8_t> (std::string_view str) noexcept -> std::optional<std::int8_t>;
	template <>
	FP_CORE auto deserialize<std::uint16_t> (std::string_view str) noexcept -> std::optional<std::uint16_t>;
	template <>
	FP_CORE auto deserialize<std::int16_t> (std::string_view str) noexcept -> std::optional<std::int16_t>;
	template <>
	FP_CORE auto deserialize<std::uint32_t> (std::string_view str) noexcept -> std::optional<std::uint32_t>;
	template <>
	FP_CORE auto deserialize<std::int32_t> (std::string_view str) noexcept -> std::optional<std::int32_t>;
	template <>
	FP_CORE auto deserialize<std::uint64_t> (std::string_view str) noexcept -> std::optional<std::uint64_t>;
	template <>
	FP_CORE auto deserialize<std::int64_t> (std::string_view str) noexcept -> std::optional<std::int64_t>;

	template <>
	FP_CORE auto deserialize<float> (std::string_view str) noexcept -> std::optional<float>;
	template <>
	FP_CORE auto deserialize<double> (std::string_view str) noexcept -> std::optional<double>;
	template <>
	FP_CORE auto deserialize<long double> (std::string_view str) noexcept -> std::optional<long double>;
}
