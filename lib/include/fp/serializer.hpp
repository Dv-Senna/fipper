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

	namespace __internals {
		template <typename T>
		auto jsonify(const T &value) noexcept -> std::optional<nlohmann::json>;

		template <typename S, std::size_t INDEX>
		auto jsonifyLoop(const S &value) noexcept -> std::optional<nlohmann::json> {
			if constexpr (INDEX >= S::MEMBERS_COUNT)
				return nlohmann::json{};
			else {
				auto json {jsonifyLoop<S, INDEX+1> (value)};
				if (!json)
					return std::nullopt;

				using T = std::tuple_element_t<INDEX, typename S::MembersTypes>;
				if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
					(*json)[S::MEMBERS_NAMES[INDEX]] = value.*(std::get<INDEX> (S::MEMBERS_PTRS));
				else {
					auto jsonified {jsonify(value.*(std::get<INDEX> (S::MEMBERS_PTRS)))};
					if (!jsonified)
						return std::nullopt;
					(*json)[S::MEMBERS_NAMES[INDEX]] = *jsonified;
				}
				return json;
			}
		}

		template <typename T>
		auto jsonify(const T &value) noexcept -> std::optional<nlohmann::json> {
			return jsonifyLoop<T, 0> (value);
		}

		template <>
		FP_CORE auto jsonify<nlohmann::json> (const nlohmann::json &value) noexcept -> std::optional<nlohmann::json>;

	} // namespace __internals

	template <typename T>
	auto serialize(const T &value) noexcept -> std::optional<Serialized> {
		auto json {__internals::jsonify(value)};
		if (!json)
			return std::nullopt;
		return serialize(*json);
	}

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

} // namespace fp
