#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <tuple>

#include "fp/header.hpp"


namespace fp {
	template <typename Body, typename ...Params>
	class Request {
		public:
			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void;
			constexpr auto setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void;

			constexpr auto getHeader() noexcept -> RequestHeader<Body>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<Body>& {return m_header;}
			constexpr auto getBody() noexcept -> std::enable_if_t<!std::is_void_v<Body>, Body&> {return m_body;}
			constexpr auto getBody() const noexcept -> std::enable_if_t<!std::is_void_v<Body>, const Body&> {return m_body;}

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
			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void;
			constexpr auto setParamNames(const std::array<std::string_view, sizeof...(Params)> &names) noexcept -> void;

			constexpr auto getHeader() noexcept -> RequestHeader<void>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<void>& {return m_header;}

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
			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void {}

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
			constexpr Request() noexcept = default;

			auto markRuntimeReady() noexcept -> void {}

			constexpr auto getHeader() noexcept -> RequestHeader<void>& {return m_header;}
			constexpr auto getHeader() const noexcept -> const RequestHeader<void>& {return m_header;}

		private:
			RequestHeader<void> m_header;
	};

} // namespace fp

#include "fp/request.inl"
