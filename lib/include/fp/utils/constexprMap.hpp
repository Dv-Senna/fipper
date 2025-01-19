#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>


namespace fp::utils {
	template <typename Key, typename Value, std::size_t N>
	class ConstexprMap {
		public:
			constexpr ConstexprMap() noexcept = default;
			constexpr ConstexprMap(const ConstexprMap &) noexcept = default;

			constexpr void insert(const Key &key, const Value &value) noexcept {
				if (this->getValue(key))
					return;
				m_values[m_keyCount] = std::make_pair(key, value);
				++m_keyCount;
			}

			constexpr std::optional<Value> getValue(const Key &key) const noexcept {
				auto it {std::find_if(m_values, m_values + m_keyCount, [&key](const std::pair<Key, Value> &value){
					return value.first == key;
				})};
				if (it != m_values + m_keyCount)
					return it->second;
				return std::nullopt;
			}
			constexpr std::optional<Value> operator[](const Key &key) const noexcept {return this->getValue(key);}

//		private:
			std::pair<Key, Value> m_values[N];
			std::size_t m_keyCount;
	};

} // namespace fp::utils
