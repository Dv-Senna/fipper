#pragma once

#include "fp/routeString.hpp"

#include <algorithm>


namespace fp {
	namespace __internals {
		constexpr auto __checkRouteStringForNoParam(std::string_view::iterator begin, std::string_view::iterator end) -> void {
			if (std::find(begin, end, '{') != end || std::find(begin, end, '}') != end)
				throw "Invalid route string : more argument than necessary provided";
		}


		template <std::size_t index, typename ...Params>
		constexpr auto __generateRouteString(
			std::string_view::iterator begin,
			std::string_view::iterator end,
			std::array<std::string_view, sizeof...(Params)> &paramNames,
			std::tuple<RouteStringExtractor<Params>...> &extractors
		) -> void {
			if constexpr (index < sizeof...(Params)) {
				auto it {std::find(begin, end, '{')};
				if (it == end)
					throw "Invalid route string : it has not enough parameter";
				if (it == begin)
					throw "Invalid route string : it starts with the parameter specifier and not '/'";
				begin = it;
				if (*--it != '/')
					throw "Invalid route string : it doesn't have a '/' before parameter specifier";

				it = std::min(std::find(begin, end, ':'), std::find(begin, end, '}'));
				if (it == end)
					throw "Invalid route string : { was never closed";

				paramNames[index] = {begin + 1, it};
				begin = it;
				if (*begin == ':')
					++begin;
				it = std::find(begin, end, '}');
				std::get<index> (extractors).instantiate(std::string_view{begin, it});
				begin = ++it;

				if (begin != end && *begin != '/')
					throw "Invalid route string : parameter specifier is not at end of string or followed by '/'";

				__generateRouteString<index+1> (begin, end, paramNames, extractors);
			}
			if constexpr (index == sizeof...(Params) - 1)
				__checkRouteStringForNoParam(begin, end);
		}

	} // namespace __internals



	template <typename ...Params>
	template <std::size_t N>
	consteval RouteString<Params...>::RouteString(const char (&str)[N]) noexcept :
		RouteString({str, str + N - 1})
	{

	}


	template <typename ...Params>
	consteval RouteString<Params...>::RouteString(std::string_view str) noexcept :
		m_str {str},
		m_paramNames {},
		m_extractors {}
	{
		__internals::__generateRouteString<0> (str.begin(), str.end(), m_paramNames, m_extractors);
	}


	template <typename ...Params>
	constexpr RouteString<Params...>::RouteString(const RouteString<Params...> &routeString) noexcept :
		m_str {routeString.m_str},
		m_paramNames {},
		m_extractors {routeString.m_extractors}
	{
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_paramNames[i] = routeString.m_paramNames[i];
	}


	template <typename ...Params>
	constexpr auto RouteString<Params...>::operator=(const RouteString<Params...> &routeString) noexcept -> RouteString<Params...>& {
		m_str = routeString.m_str;
		m_extractors = routeString.m_extractors;
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_paramNames[i] = routeString.m_paramNames[i];
		return *this;
	}


	template <typename ...Params>
	constexpr RouteString<Params...>::RouteString(RouteString<Params...> &&routeString) noexcept :
		m_str {routeString.m_str},
		m_paramNames {},
		m_extractors {std::move(routeString.m_extractors)}
	{
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_paramNames[i] = routeString.m_paramNames[i];
	}


	template <typename ...Params>
	constexpr auto RouteString<Params...>::operator=(RouteString<Params...> &&routeString) noexcept -> RouteString<Params...>& {
		m_str = routeString.m_str;
		m_extractors = std::move(routeString.m_extractors);
		for (std::size_t i {0}; i < sizeof...(Params); ++i)
			m_paramNames[i] = routeString.m_paramNames[i];
		return *this;
	}


	template <typename ...Params>
	constexpr auto RouteString<Params...>::operator==(const RouteStringBase &routeString) const noexcept -> bool {
		return m_str == routeString.getString();
	}


	template <typename ...Params>
	constexpr auto RouteString<Params...>::isInstance(std::string_view route) const noexcept -> bool{
		auto beginStr {m_str.begin()};
		auto beginRoute {route.begin()};
		while (beginStr != m_str.end() && beginRoute != route.end()) {
			auto it {std::find(beginStr, m_str.end(), '{')};
			std::ptrdiff_t length {std::distance(beginStr, it)};
			std::ptrdiff_t availableLength {std::distance(beginRoute, route.end())};
			if (length > availableLength)
				return false;
			if (std::string_view{beginStr, it} != std::string_view{beginRoute, beginRoute + length})
				return false;

			beginStr = std::find(it, m_str.end(), '/');
			beginRoute = std::find(beginRoute + length, route.end(), '/');
			if ((beginStr == m_str.end()) xor (beginRoute == route.end()))
				return false;
		}

		return true;
	}


	namespace __internals {
		template <std::size_t index = 0, typename First, typename ...Params, typename ...Extractors>
		constexpr auto __extractParamFromRouteString(
			std::string_view::iterator beginTemplate, std::string_view endTemplate,
			std::string_view::iterator beginInstance, std::string_view endInstance,
			std::tuple<RouteStringExtractor<Extractors>...> &m_extractors
		) -> std::optional<std::tuple<First, Params...>> {
			auto it {std::find(beginTemplate, endTemplate, '{')};
			std::ptrdiff_t length {std::distance(beginTemplate, it)};
			beginInstance += length;
		}

	} // namespace __internals


	template <typename ...Params>
	constexpr auto RouteString<Params...>::extractParamsFromInstance(std::string_view route) const noexcept -> std::optional<std::tuple<Params...>> {
		return __internals::__extractParamFromRouteString(
			m_str.begin(), m_str.end(),
			route.begin(), route.end(),
			m_extractors
		);
	}




	template <std::size_t N>
	consteval RouteString<>::RouteString(const char (&str)[N]) noexcept :
		RouteString(std::string_view{str, str + N - 1})
	{

	}

	consteval RouteString<>::RouteString(std::string_view str) noexcept :
		m_str {str}
	{
		__internals::__checkRouteStringForNoParam(str.begin(), str.end());
	}


	constexpr auto RouteString<>::operator==(const RouteStringBase &routeString) const noexcept -> bool {
		return m_str == routeString.getString();
	}


	constexpr auto RouteString<>::isInstance(std::string_view route) const noexcept -> bool {
		return m_str == route;
	}




	constexpr auto RouteStringExtractor<std::string>::instantiate(std::string_view str) -> void {
		if (str.empty())
			return;
		if (str != "not_empty")
			throw "Invalid route string parameter<std::string> specifier : the specifier must be either empty of 'not_empty'";
		notEmpty = true;
	}


	auto RouteStringExtractor<std::string>::extract(std::string_view str) noexcept -> std::optional<std::string> {
		if (notEmpty && str.empty())
			return std::nullopt;
		return std::string{str};
	}

} // namespace fp
