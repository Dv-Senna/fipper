#pragma once

#include "fp/utils/hash.hpp"


namespace fp::utils {
	template <>
	constexpr auto hash<std::string_view> (const std::string_view &value) noexcept -> Hash {
		// implementation of djb2 with xor
		// hash(i) = (hash(i - 1) * 33) ^ value[i]

		Hash_t result {5381};
		for (const auto &c : value)
			result = (result * 33) ^ c;
		return Hash{result};
	}


	template <>
	constexpr auto hash<std::string> (const std::string &value) noexcept -> Hash {
		return hash(std::string_view{value});
	}

} // namespace fp::utils
