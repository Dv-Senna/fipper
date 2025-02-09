#pragma once

#include <functional>
#include <string_view>
#include <tuple>


namespace fp::utils {
	namespace __internals {
		template <typename T>
		struct FunctionTraitsBase;

		template <typename Ret, typename ...Args>
		struct FunctionTraitsBase<std::function<Ret(Args...)>> {
			using ReturnType = Ret;
			using ArgumentsTypes = std::tuple<Args...>;
		};
	};

	template <typename T>
	using FunctionTraits = __internals::FunctionTraitsBase<decltype(std::function(std::declval<T> ()))>;

	template <typename T, std::size_t index>
	using FunctionParameter_t = std::tuple_element_t<index, typename FunctionTraits<T>::ArgumentsTypes>;


	template <typename T>
	struct SanitizeParameter {
		using Type = std::remove_const_t<std::remove_reference_t<T>>;
	};

	template <typename T>
	using SanitizeParameter_t = SanitizeParameter<T>::Type;

	template <typename T, std::size_t index>
	using SanitizedFunctionParameter_t = SanitizeParameter_t<FunctionParameter_t<T, index>>;



	template <typename T>
	concept IsMacroReflected = (T::HAS_REFLECTION) && requires {
		{T::MEMBERS_COUNT} -> std::convertible_to<std::size_t>;
		T::MEMBERS_PTRS;
		std::get<0> (T::MEMBERS_PTRS);
		T::MEMBERS_NAMES;
		{T::MEMBERS_NAMES[0]} -> std::convertible_to<std::string_view>;
		typename T::MembersTypes;
		std::get<0> (std::declval<typename T::MembersTypes> ());
	};


	template <typename T>
	concept IsReflectable = std::is_aggregate_v<T> || IsMacroReflected<T>;


	template <IsReflectable T>
	struct ReflectionTraits {
		static constexpr bool IS_MACRO_REFLECTED {false};
		static constexpr std::size_t MEMBERS_COUNT {0};
		static constexpr std::array<std::ptrdiff_t, MEMBERS_COUNT> MEMBERS_PTRS {std::make_index_sequence<MEMBERS_COUNT> (-1)};
		static constexpr std::array<std::string_view, MEMBERS_COUNT> MEMBERS_NAMES {};
//		using MembersTypes = ;

		template <std::size_t INDEX>
		requires (INDEX < MEMBERS_COUNT)
		static constexpr auto getMember(T &instance) noexcept;

		template <std::size_t INDEX>
		requires (INDEX < MEMBERS_COUNT)
		static constexpr auto getMember(const T &instance) noexcept;
	};


	template <IsMacroReflected T>
	struct ReflectionTraits<T> {
		static constexpr bool IS_MACRO_REFLECTED {true};
		static constexpr std::size_t MEMBERS_COUNT {T::MEMBERS_COUNT};
		static constexpr auto MEMBERS_PTRS {T::MEMBERS_PTRS};
		static constexpr auto MEMBERS_NAMES {T::MEMBERS_NAMES};
		using MembersTypes = T::MembersTypes;

		template <std::size_t INDEX>
		requires (INDEX < MEMBERS_COUNT)
		static constexpr auto getMember(T &instance) noexcept -> std::tuple_element_t<INDEX, MembersTypes>& {
			return instance.*(std::get<INDEX> (MEMBERS_PTRS));
		}

		template <std::size_t INDEX>
		requires (INDEX < MEMBERS_COUNT)
		static constexpr auto getMember(const T &instance) noexcept -> const std::tuple_element_t<INDEX, MembersTypes>& {
			return instance.*(std::get<INDEX> (MEMBERS_PTRS));
		}
	};

} // namespace fp::utils
