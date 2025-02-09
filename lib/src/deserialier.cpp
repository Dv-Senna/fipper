#include "fp/deserializer.hpp"


namespace fp {
	template <>
	auto deserialize<std::string> (std::string_view str) noexcept -> std::optional<std::string> {
		return std::string{str};
	}

	template <>
	auto deserialize<std::string_view> (std::string_view str) noexcept -> std::optional<std::string_view> {
		return str;
	}


	template <>
	auto deserialize<std::uint8_t> (std::string_view str) noexcept -> std::optional<std::uint8_t> {
		try {
			return static_cast<std::uint8_t> (std::stoul(std::string{str}));
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::int8_t> (std::string_view str) noexcept -> std::optional<std::int8_t> {
		try {
			return static_cast<std::int8_t> (std::stol(std::string{str}));
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::uint16_t> (std::string_view str) noexcept -> std::optional<std::uint16_t> {
		try {
			return static_cast<std::uint16_t> (std::stoul(std::string{str}));
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::int16_t> (std::string_view str) noexcept -> std::optional<std::int16_t> {
		try {
			return static_cast<std::int16_t> (std::stol(std::string{str}));
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::uint32_t> (std::string_view str) noexcept -> std::optional<std::uint32_t> {
		try {
			return std::stoul(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::int32_t> (std::string_view str) noexcept -> std::optional<std::int32_t> {
		try {
			return std::stol(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::uint64_t> (std::string_view str) noexcept -> std::optional<std::uint64_t> {
		try {
			return std::stoull(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<std::int64_t> (std::string_view str) noexcept -> std::optional<std::int64_t> {
		try {
			return std::stoll(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	
	template <>
	auto deserialize<float> (std::string_view str) noexcept -> std::optional<float> {
		try {
			return std::stof(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<double> (std::string_view str) noexcept -> std::optional<double> {
		try {
			return std::stod(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

	template <>
	auto deserialize<long double> (std::string_view str) noexcept -> std::optional<long double> {
		try {
			return std::stold(std::string{str});
		}
		catch (...) {
			return std::nullopt;
		}
	}

} // namespace fp
