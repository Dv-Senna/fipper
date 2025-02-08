#pragma once

#include "fp/request.hpp"

#include <optional>

#include "fp/deserializer.hpp"


namespace fp {
	namespace __internals {
		template <std::size_t index, typename ...Params>
		auto __setParamAddress(
			const std::tuple<Params...> &tuple,
			std::pair<std::string_view, const void*> (&paramNames)[sizeof...(Params)]
		) -> void {
			if constexpr (index < sizeof...(Params)) {
				paramNames[index].second = &std::get<index> (tuple);
				__setParamAddress<index+1> (tuple, paramNames);
			}
		}

	} // namespace __internals


	template <typename Body, typename ...Params>
	auto Request<Body, Params...>::markRuntimeReady() noexcept -> void {
		__internals::__setParamAddress<0> (m_params, m_namesOfParams);
	}


	template <typename Body, typename ...Params>
	constexpr auto Request<Body, Params...>::setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void {
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_namesOfParams[i].first = names[i];
	}


	template <typename Body, typename ...Params>
	auto Request<Body, Params...>::deserialize(std::string_view str) noexcept -> fp::Result {
		std::optional<Body> bodyWithError {fp::deserialize<Body> (str)};
		if (!bodyWithError)
			return fp::Result::eFailure;
		m_body = std::move(*bodyWithError);
		return fp::Result::eSuccess;
	}


	template <typename Body, typename ...Params>
	template <typename T>
	requires (std::same_as<T, Params> || ...)
	auto Request<Body, Params...>::getParam(std::string_view name) const noexcept -> std::optional<const T*> {
		for (const auto &paramName : m_namesOfParams) {
			if (paramName.first != name)
				continue;
			return reinterpret_cast<const T*> (paramName.second);
		}
		return std::nullopt;
	}


	template <typename ...Params>
	auto Request<void, Params...>::markRuntimeReady() noexcept -> void {
		__internals::__setParamAddress<0> (m_params, m_namesOfParams);
	}


	template <typename ...Params>
	constexpr auto Request<void, Params...>::setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void {
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_namesOfParams[i].first = names[i];
	}


	template <typename ...Params>
	template <typename T>
	requires (std::same_as<T, Params> || ...)
	auto Request<void, Params...>::getParam(std::string_view name) const noexcept -> std::optional<const T*> {
		for (const auto &paramName : m_namesOfParams) {
			if (paramName.first != name)
				continue;
			return reinterpret_cast<const T*> (paramName.second);
		}
		return std::nullopt;
	}

} // namespace fp
