#pragma once

#include <latch>
#include <string_view>

#include "fp/httpMethod.hpp"
#include "fp/request.hpp"
#include "fp/response.hpp"
#include "fp/result.hpp"
#include "fp/socket.hpp"


namespace fp {
	template <typename Func, typename Request, typename Response>
	concept IsEndpointCallback = fp::IsRequest<Request>
		&& fp::IsResponse<Response>
		&& std::is_nothrow_invocable_v<Func, Request, Response&>
		&& std::is_void_v<std::invoke_result_t<Func, Request, Response&>>;


	class EndpointBase {
		public:
			EndpointBase() noexcept = delete;
			constexpr EndpointBase(fp::HttpMethod method, std::string_view route) noexcept : m_method {method}, m_route {route} {}
			~EndpointBase() = default;

			virtual auto handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view requestString) noexcept -> fp::Result = 0;

		protected:
			fp::HttpMethod m_method;
			std::string_view m_route;
	};


	template <fp::IsRequest Request, fp::IsResponse Response, fp::IsEndpointCallback<Request, Response> Func>
	class Endpoint final : public EndpointBase {
		public:
			Endpoint() noexcept = delete;
			constexpr Endpoint(fp::HttpMethod method, std::string_view route, Func &&callback) noexcept;
			~Endpoint() = default;

			auto handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view requestString) noexcept -> fp::Result override;


		private:
			Func &&m_callback;
	};

} // namespace fp

#include "fp/endpoint.inl"
