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
			std::string_view (&paramNames)[sizeof...(Params)],
			std::tuple<RouteStringExtractor<Params>...> &extractors
		) -> void {
			if constexpr (index < sizeof...(Params)) {
				begin = std::find(begin, end, '{');
				if (begin == end)
					throw "Invalid route string : it has not enough parameter";

				auto it {std::min(std::find(begin, end, ':'), std::find(begin, end, '}'))};
				if (it == end)
					throw "Invalid route string : { was never closed";

				paramNames[index] = {begin + 1, it};
				begin = it;
				if (*begin == ':')
					++begin;
				it = std::find(begin, end, '}');
				std::get<index> (extractors).instantiate(std::string_view{begin, it});
				begin = ++it;

				__generateRouteString<index+1> (begin, end, paramNames, extractors);
			}
			if constexpr (index == sizeof...(Params) - 1)
				__checkRouteStringForNoParam(begin, end);
		}

	} // namespace __internals



	template <typename ...Params>
	template <std::size_t N>
	consteval RouteString<Params...>::RouteString(const char (&str)[N]) noexcept :
		RouteString({str, str + N})
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



	template <std::size_t N>
	consteval RouteString<>::RouteString(const char (&str)[N]) noexcept :
		RouteString(std::string_view{str, str + N})
	{

	}

	consteval RouteString<>::RouteString(std::string_view str) noexcept :
		m_str {str}
	{
		__internals::__checkRouteStringForNoParam(str.begin(), str.end());
	}



	constexpr auto RouteStringExtractor<std::string>::instantiate(std::string_view str) -> void {
		if (!str.empty())
			throw "Route string doesn't take any format argument";
	}


	auto RouteStringExtractor<std::string>::extract(std::string_view str) noexcept -> std::string {
		return std::string{str};
	}

} // namespace fp
