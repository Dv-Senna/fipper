#pragma once

#include <array>
#include <concepts>
#include <optional>
#include <string_view>
#include <tuple>

#include "fp/header.hpp"
#include "fp/result.hpp"
#include "fp/routeString.hpp"


namespace fp {
	template <typename Request>
	concept IsRequest = requires (
		Request req,
		const Request constReq,
		std::string_view strView
	) {
		{Request::HAS_PARAMS} -> std::convertible_to<bool>;
		typename Request::Header;
		typename Request::Route;
		{req.markRuntimeReady()} -> std::same_as<void>;
		{req.getHeader()} -> std::same_as<typename Request::Header&>;
		{req.deserialize(strView)} -> std::same_as<fp::Result>;
		{constReq.getHeader()} -> std::same_as<const typename Request::Header&>;
	};

	template <typename Body, typename ...Params>
	class Request {
		public:
			using Header = RequestHeader<Body>;
			using Route = RouteString<Params...>;
			static constexpr bool HAS_PARAMS {true};
			static constexpr bool HAS_BODY {true};

			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void;
			constexpr auto setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void;
			auto deserialize(std::string_view str) noexcept -> fp::Result;

			constexpr auto getHeader() noexcept -> RequestHeader<Body>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<Body>& {return m_header;}
			constexpr auto getBody() noexcept -> std::enable_if_t<!std::is_void_v<Body>, Body&> {return m_body;}
			constexpr auto getBody() const noexcept -> std::enable_if_t<!std::is_void_v<Body>, const Body&> {return m_body;}

			constexpr auto setParams(std::tuple<Params...> &&params) noexcept -> void {m_params = std::move(params);}

			template <std::size_t index>
			requires (index < sizeof...(Params))
			constexpr auto getParam() noexcept -> std::tuple_element_t<index, std::tuple<Params...>>& {return std::get<index> (m_params);}
			template <std::size_t index>
			requires (index < sizeof...(Params))
			constexpr auto getParam() const noexcept -> const std::tuple_element_t<index, std::tuple<Params...>>&{return std::get<index> (m_params);}

			template <typename T>
			requires (std::same_as<T, Params> || ...)
			auto getParam(std::string_view name) const noexcept -> std::optional<const T*>;


		private:
			std::pair<std::string_view, const void*> m_namesOfParams[sizeof...(Params)];
			RequestHeader<Body> m_header;
			std::tuple<Params...> m_params;
			Body m_body;
	};


	template <typename ...Params>
	class Request<void, Params...> {
		public:
			using Header = RequestHeader<void>;
			using Route = RouteString<Params...>;
			static constexpr bool HAS_PARAMS {true};
			static constexpr bool HAS_BODY {false};

			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void;
			constexpr auto setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void;
			inline auto deserialize(std::string_view) noexcept -> fp::Result {return fp::Result::eSuccess;}

			constexpr auto getHeader() noexcept -> RequestHeader<void>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<void>& {return m_header;}

			constexpr auto setParams(std::tuple<Params...> &&params) noexcept -> void {m_params = std::move(params);}

			template <std::size_t index>
			requires (index < sizeof...(Params))
			constexpr auto getParam() noexcept -> std::tuple_element_t<index, std::tuple<Params...>>& {return std::get<index> (m_params);}
			template <std::size_t index>
			requires (index < sizeof...(Params))
			constexpr auto getParam() const noexcept -> const std::tuple_element_t<index, std::tuple<Params...>>& {return std::get<index> (m_params);}

			template <typename T>
			requires (std::same_as<T, Params> || ...)
			auto getParam(std::string_view name) const noexcept -> std::optional<const T*>;


		private:
			std::pair<std::string_view, const void*> m_namesOfParams[sizeof...(Params)];
			RequestHeader<void> m_header;
			std::tuple<Params...> m_params;
	};


	template <typename Body>
	class Request<Body> {
		public:
			using Header = RequestHeader<Body>;
			using Route = RouteString<>;
			static constexpr bool HAS_PARAMS {false};
			static constexpr bool HAS_BODY {true};

			constexpr Request() noexcept = default;

			inline auto markRuntimeReady() noexcept -> void {}
			auto deserialize(std::string_view str) noexcept -> fp::Result;

			constexpr auto getHeader() noexcept -> RequestHeader<Body>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<Body>& {return m_header;}
			constexpr auto getBody() noexcept -> std::enable_if_t<!std::is_void_v<Body>, Body&> {return m_body;}
			constexpr auto getBody() const noexcept -> std::enable_if_t<!std::is_void_v<Body>, const Body&> {return m_body;}

		private:
			RequestHeader<Body> m_header;
			Body m_body;
	};


	template <>
	class Request<void> {
		public:
			using Header = RequestHeader<void>;
			using Route = RouteString<>;
			static constexpr bool HAS_PARAMS {false};
			static constexpr bool HAS_BODY {false};

			constexpr Request() noexcept = default;

			inline auto markRuntimeReady() noexcept -> void {}
			inline auto deserialize(std::string_view) noexcept -> fp::Result {return fp::Result::eSuccess;}

			constexpr auto getHeader() noexcept -> RequestHeader<void>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<void>& {return m_header;}

		private:
			RequestHeader<void> m_header;
	};

} // namespace fp

#include "fp/request.inl"
