#pragma once

#include <latch>
#include <memory>
#include <string_view>

#include "fp/httpCode.hpp"
#include "fp/httpMethod.hpp"
#include "fp/request.hpp"
#include "fp/response.hpp"
#include "fp/socket.hpp"
#include "fp/utils/traits.hpp"


namespace fp {
	template <typename Func, typename Request, typename Response>
	concept IsEndpointCallbackWithReqRes = fp::IsRequest<Request>
		&& fp::IsResponse<Response>
		&& std::is_nothrow_invocable_v<Func, Request, Response&>
		&& std::same_as<std::invoke_result_t<Func, Request, Response&>, fp::HttpCode>;

	template <typename Func>
	concept IsEndpointCallback = fp::IsEndpointCallbackWithReqRes<
		Func,
		fp::utils::SanitizedFunctionParameter_t<Func, 0>,
		fp::utils::SanitizedFunctionParameter_t<Func, 1>
	>;


	class EndpointBase {
		public:
			EndpointBase() noexcept = delete;
			constexpr EndpointBase(fp::HttpMethod method) noexcept : m_method {method} {}
			virtual ~EndpointBase() = default;

			virtual auto handleRequest(std::shared_ptr<std::latch> latch, fp::Socket &&connection, std::string_view requestString) const noexcept -> void = 0;

		protected:
			fp::HttpMethod m_method;
	};


	template <fp::IsEndpointCallback Func>
	class Endpoint final : public EndpointBase {
		public:
			using Request = fp::utils::SanitizedFunctionParameter_t<Func, 0>;
			using Response = fp::utils::SanitizedFunctionParameter_t<Func, 1>;
			using Route = typename Request::Route;

			Endpoint() noexcept = delete;
			constexpr Endpoint(fp::HttpMethod method, Route route, Func &&callback) noexcept;
			~Endpoint() override = default;

			auto handleRequest(std::shared_ptr<std::latch> latch, fp::Socket &&connection, std::string_view requestString) const noexcept -> void override;


		private:
			Func &&m_callback;
			Route m_route;
	};

} // namespace fp

#include "fp/endpoint.inl"
