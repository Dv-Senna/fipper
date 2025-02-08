#include "fp/serializer.hpp"


namespace fp {
	namespace __internals {
		template <>
		FP_CORE auto jsonify<nlohmann::json> (const nlohmann::json &value) noexcept -> std::optional<nlohmann::json> {
			return value;
		}

	} // namespace __internals

	template <>
	auto serialize<std::string> (const std::string &data) noexcept -> std::optional<Serialized> {
		return Serialized{
			.data = {(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()},
			.type = fp::ContentType::ePlain
		};
	}

	template <>
	auto serialize<std::string_view> (const std::string_view &data) noexcept -> std::optional<Serialized> {
		return Serialized{
			.data = {(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()},
			.type = fp::ContentType::ePlain
		};
	}


	template <>
	auto serialize<std::uint8_t> (const std::uint8_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::int8_t> (const std::int8_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::uint16_t> (const std::uint16_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::int16_t> (const std::int16_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::uint32_t> (const std::uint32_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::int32_t> (const std::int32_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::uint64_t> (const std::uint64_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<std::int64_t> (const std::int64_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}


	template <>
	auto serialize<float> (const float &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<double> (const double &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	auto serialize<long double> (const long double &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}


	template <>
	FP_CORE auto serialize<nlohmann::json> (const nlohmann::json &data) noexcept -> std::optional<Serialized>{
		auto serialized {serialize(std::string{data.dump()})};
		if (!!serialized)
			serialized->type = fp::ContentType::eJson;
		return serialized;
	}

} // namespace fp
