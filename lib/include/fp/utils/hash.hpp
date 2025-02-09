#pragma once

#include <cstdint>
#include <string>
#include <string_view>


namespace fp::utils {
	using Hash_t = std::uint64_t;

	class Hash {
		public:
			constexpr Hash() noexcept = default;
			constexpr Hash(Hash_t hash) noexcept : m_hash {hash} {}

			constexpr operator Hash_t() noexcept {return m_hash;}
			constexpr operator Hash_t() const noexcept {return m_hash;}

		private:
			Hash_t m_hash;
	};


	template <typename T>
	auto hash(const T &value) noexcept -> Hash;


	template <>
	constexpr auto hash<std::string_view> (const std::string_view &value) noexcept -> Hash;
	template <>
	constexpr auto hash<std::string> (const std::string &value) noexcept -> Hash;


	namespace literals {
		consteval auto operator""_hash(const char *ptr, std::size_t length) noexcept -> Hash {
			return hash(std::string_view{ptr, ptr + length});
		}

	} // namespace literals

} // namespace fp::utils

#include "fp/utils/hash.inl"
