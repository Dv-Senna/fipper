#pragma once

#include <string>
#include <string_view>
#include <tuple>


namespace fp {
	template <typename T>
	class RouteStringExtractor;


	template <typename ...Params>
	class RouteString {
		public:
			constexpr RouteString() noexcept = default;
			template <std::size_t N>
			consteval RouteString(const char (&str)[N]) noexcept;
			consteval RouteString(std::string_view str) noexcept;

			constexpr RouteString(const RouteString<Params...> &routeString) noexcept;
			constexpr auto operator=(const RouteString<Params...> &routeString) noexcept -> RouteString<Params...>&;
			constexpr RouteString(RouteString<Params...> &&routeString) noexcept;
			constexpr auto operator=(RouteString<Params...> &&routeString) noexcept -> RouteString<Params...>&;

			constexpr auto getParamNames() const noexcept -> std::string_view (&)[sizeof...(Params)] {return m_paramNames;}
			constexpr auto getString() const noexcept -> std::string_view {return m_str;}


		private:
			std::string_view m_str;
			std::string_view m_paramNames[sizeof...(Params)];
			std::tuple<RouteStringExtractor<Params>...> m_extractors;
	};


	template <>
	class RouteString<> {
		public:
			constexpr RouteString() noexcept = default;
			template <std::size_t N>
			consteval RouteString(const char (&str)[N]) noexcept;
			consteval RouteString(std::string_view str) noexcept;

			constexpr RouteString(const RouteString<> &routeString) noexcept = default;
			constexpr auto operator=(const RouteString<> &routeString) noexcept -> RouteString<>& = default;
			constexpr RouteString(RouteString<> &&routeString) noexcept = default;
			constexpr auto operator=(RouteString<> &&routeString) noexcept -> RouteString<>& = default;

			constexpr auto getString() const noexcept -> std::string_view {return m_str;}


		private:
			std::string_view m_str;
	};



	template <>
	class RouteStringExtractor<std::string> {
		public:
			constexpr auto instantiate(std::string_view str) -> void;
			inline auto extract(std::string_view str) noexcept -> std::string;
	};

} // namespace fp

#include "fp/routeString.inl"
