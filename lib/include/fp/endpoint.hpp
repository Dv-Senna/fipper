#pragma once

#include <string_view>

#include "fp/coroutines/async.hpp"
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
		&& (std::is_same_v<std::invoke_result_t<Func, Request, Response&>, fp::HttpCode>
			|| std::is_void_v<std::invoke_result_t<Func, Request, Response&>>
			|| std::is_same_v<std::invoke_result_t<Func, Request, Response&>, fp::Async>
		);

	template <typename Func>
	concept IsEndpointCallback = fp::IsEndpointCallbackWithReqRes<
		Func,
		fp::utils::SanitizedFunctionParameter_t<Func, 0>,
		fp::utils::SanitizedFunctionParameter_t<Func, 1>
	>;

	template <typename Func>
	concept IsHttpReturningEndpointCallback = fp::IsEndpointCallback<Func>
		&& std::same_as<std::invoke_result_t<
			Func,
			fp::utils::SanitizedFunctionParameter_t<Func, 0>,
			fp::utils::SanitizedFunctionParameter_t<Func, 1>&>,
		fp::HttpCode>;
	
	template <typename Func>
	concept IsHttpCoroutineEndpointCallback = fp::IsEndpointCallback<Func>
		&& std::same_as<std::invoke_result_t<
			Func,
			fp::utils::SanitizedFunctionParameter_t<Func, 0>,
			fp::utils::SanitizedFunctionParameter_t<Func, 1>&>,
		fp::Async>;


	class EndpointBase {
		public:
			EndpointBase() noexcept = delete;
			constexpr EndpointBase(fp::HttpMethod method) noexcept : m_method {method} {}
			virtual ~EndpointBase() = default;

			virtual auto handleRequest(fp::Socket &&connection, std::string_view requestString) const noexcept -> fp::coroutines::WrapperTask = 0;

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

			auto handleRequest(fp::Socket &&connection, std::string_view requestString) const noexcept -> fp::coroutines::WrapperTask override;


		private:
			Func &&m_callback;
			Route m_route;
	};

} // namespace fp

#include "fp/endpoint.inl"
