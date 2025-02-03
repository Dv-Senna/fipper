#pragma once

#include <any>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

#include "fp/core.hpp"
#include "fp/header.hpp"


namespace fp {
	struct Serialized {
		std::vector<std::byte> data;
		fp::ContentType type {fp::ContentType::eJson};
	};

	template <typename T>
	auto serialize(const T &) noexcept -> std::optional<Serialized>;

	template <>
	FP_CORE auto serialize<std::string> (const std::string &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::string_view> (const std::string_view &) noexcept -> std::optional<Serialized>;

	template <>
	FP_CORE auto serialize<std::uint8_t> (const std::uint8_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::int8_t> (const std::int8_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::uint16_t> (const std::uint16_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::int16_t> (const std::int16_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::uint32_t> (const std::uint32_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::int32_t> (const std::int32_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::uint64_t> (const std::uint64_t &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<std::int64_t> (const std::int64_t &) noexcept -> std::optional<Serialized>;

	template <>
	FP_CORE auto serialize<float> (const float &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<double> (const double &) noexcept -> std::optional<Serialized>;
	template <>
	FP_CORE auto serialize<long double> (const long double &) noexcept -> std::optional<Serialized>;

	template <>
	FP_CORE auto serialize<nlohmann::json> (const nlohmann::json &) noexcept -> std::optional<Serialized>;


	class FP_CORE SerializerBase {
		public:
			virtual auto serialize(std::any) const noexcept -> std::optional<Serialized> = 0;
	};

	template <typename T>
	class Serializer final : public SerializerBase {
		public:
			auto serialize(std::any data) const noexcept -> std::optional<Serialized> override {
				return fp::serialize(std::any_cast<T> (data));
			}

			static Serializer<T> instance;

		private:
			Serializer() noexcept = default;
	};

	template <typename T>
	Serializer<T> Serializer<T>::instance {};

} // namespace fp
