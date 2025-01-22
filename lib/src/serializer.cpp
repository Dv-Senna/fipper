#include "fp/serializer.hpp"


namespace fp {
	template <>
	FP_CORE auto serialize<std::string> (const std::string &data) noexcept -> std::optional<Serialized> {
		return Serialized{
			.data = {(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()},
			.type = fp::ContentType::ePlain
		};
	}

	template <>
	FP_CORE auto serialize<std::string_view> (const std::string_view &data) noexcept -> std::optional<Serialized> {
		return Serialized{
			.data = {(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()},
			.type = fp::ContentType::ePlain
		};
	}


	template <>
	FP_CORE auto serialize<std::uint8_t> (const std::uint8_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::int8_t> (const std::int8_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::uint16_t> (const std::uint16_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::int16_t> (const std::int16_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::uint32_t> (const std::uint32_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::int32_t> (const std::int32_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::uint64_t> (const std::uint64_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<std::int64_t> (const std::int64_t &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}


	template <>
	FP_CORE auto serialize<float> (const float &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<double> (const double &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}
	template <>
	FP_CORE auto serialize<long double> (const long double &data) noexcept -> std::optional<Serialized> {
		return serialize(std::to_string(data));
	}

} // namespace fp
