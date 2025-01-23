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
	using FunctionTraits = __internals::FunctionTraitsBase<decltype(std::function(T()))>;

} // namespace fp::utils
