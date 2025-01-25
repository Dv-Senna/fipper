#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>


namespace fp {
	template <typename T>
	class RouteStringExtractor;


	class RouteStringBase {
		public:
			virtual ~RouteStringBase() = default;

			virtual auto getString() const noexcept -> std::string_view = 0;
			virtual auto operator==(const RouteStringBase &) const noexcept -> bool = 0;
			virtual auto isInstance(std::string_view) const noexcept -> bool = 0;
	};

	template <typename ...Params>
	class RouteString final : public RouteStringBase {
		public:
			constexpr RouteString() noexcept = default;
			template <std::size_t N>
			consteval RouteString(const char (&str)[N]) noexcept;
			consteval RouteString(std::string_view str) noexcept;

			constexpr RouteString(const RouteString<Params...> &routeString) noexcept;
			constexpr auto operator=(const RouteString<Params...> &routeString) noexcept -> RouteString<Params...>&;
			constexpr RouteString(RouteString<Params...> &&routeString) noexcept;
			constexpr auto operator=(RouteString<Params...> &&routeString) noexcept -> RouteString<Params...>&;

			constexpr auto getParamNames() const noexcept {return m_paramNames;}
			constexpr auto getString() const noexcept -> std::string_view override {return m_str;}

			constexpr auto operator==(const RouteStringBase &routeString) const noexcept -> bool override;
			constexpr auto isInstance(std::string_view route) const noexcept -> bool override;
			constexpr auto extractParamsFromInstance(std::string_view route) const noexcept -> std::optional<std::tuple<Params...>>;


		private:
			std::string_view m_str;
			std::array<std::string_view, sizeof...(Params)> m_paramNames;
			std::tuple<RouteStringExtractor<Params>...> m_extractors;
	};


	template <>
	class RouteString<> final : public RouteStringBase {
		public:
			constexpr RouteString() noexcept = default;
			template <std::size_t N>
			consteval RouteString(const char (&str)[N]) noexcept;
			consteval RouteString(std::string_view str) noexcept;

			constexpr RouteString(const RouteString<> &routeString) noexcept = default;
			constexpr auto operator=(const RouteString<> &routeString) noexcept -> RouteString<>& = default;
			constexpr RouteString(RouteString<> &&routeString) noexcept = default;
			constexpr auto operator=(RouteString<> &&routeString) noexcept -> RouteString<>& = default;

			constexpr auto getString() const noexcept -> std::string_view override {return m_str;}

			constexpr auto operator==(const RouteStringBase &routeString) const noexcept -> bool override;
			constexpr auto isInstance(std::string_view route) const noexcept -> bool override;


		private:
			std::string_view m_str;
	};



	template <>
	class RouteStringExtractor<std::string> {
		public:
			bool notEmpty {false};

			constexpr auto instantiate(std::string_view str) -> void;
			inline auto extract(std::string_view str) const noexcept -> std::optional<std::string>;
	};

} // namespace fp

#include "fp/routeString.inl"
