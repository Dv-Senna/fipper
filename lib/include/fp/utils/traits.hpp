#pragma once

#include <functional>
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
	using FunctionTraits = __internals::FunctionTraitsBase<decltype(std::function(*(T*)(nullptr)))>;

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

} // namespace fp::utils
