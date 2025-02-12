#pragma once

#include "fp/endpoint.hpp"

#include <format>

#include "fp/errorStack.hpp"
#include "fp/utils/benchmark.hpp"


#define FP_REQUEST_HANDLING_BENCHMARK fp::utils::Benchmark<"request_handling"_hash, std::string>

namespace fp {
	template <fp::IsEndpointCallback Func>
	constexpr Endpoint<Func>::Endpoint(fp::HttpMethod method, Route route, Func &&callback) noexcept :
		EndpointBase(method),
		m_callback {std::move(callback)},
		m_route {route}
	{

	}


	template <fp::IsEndpointCallback Func>
	auto Endpoint<Func>::handleRequest(fp::Socket &&connection, std::string_view requestString) const noexcept -> fp::coroutines::WrapperTask {
		using namespace fp::utils::literals;
		FP_REQUEST_HANDLING_BENCHMARK benchmark {};
		fp::Socket clientConnection {std::move(connection)};

		auto split {std::views::split(requestString, ' ')};
		std::string_view requestRoute {*++split.begin()};
		benchmark.setArgs(std::string{requestRoute});

		Request request {};
		if constexpr (Request::HAS_PARAMS) {
			std::optional params {m_route.extractParamsFromInstance(requestRoute)};
			if (!params) {
				if (clientConnection.send(fp::serialize("HTTP/1.1 400 Bad Request"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send response of invalid parameter of route");
					fp::ErrorStack::logAll();
				}
				co_return;
			}
			request.setParamNames(m_route.getParamNames());
			request.markRuntimeReady();
			request.setParams(std::move(*params));
		}

		if constexpr (Request::HAS_BODY) {
			std::string_view requestBody {requestString.begin() + requestString.find("\r\n\r\n") + 4, requestString.end()};

			if (request.deserialize(requestBody) != fp::Result::eSuccess) {
				if (clientConnection.send(fp::serialize("HTTP/1.1 400 Bad Request"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send response of invalid request body of route");
					fp::ErrorStack::logAll();
				}
				co_return;
			}
		}


		Response response {};
		fp::HttpCode code {fp::HttpCode::e200};
		if constexpr (fp::IsHttpReturningEndpointCallback<Func>)
			code = co_await this->m_callback(request, response);
		else
			this->m_callback(request, response);
		response.serialize();

		std::string responseData {};
		if constexpr (Response::HAS_BODY) {
			std::string data {(const char*)response.serialized.data(), (const char*)response.serialized.data() + response.serialized.size()};
			responseData = std::format("HTTP/1.1 {} {}\r\nContent-Type: {}\r\nContent-Length: {}\r\n\r\n{}",
				std::to_string(static_cast<std::int32_t> (code)),
				fp::getHttpCodeString(code).value_or(""),
				fp::getContentTypeString(response.header.contentType),
				data.size(),
				data
			);
		}
		else
			responseData = std::format("HTTP/1.1 {} {}", std::to_string(static_cast<std::int32_t> (code)), fp::getHttpCodeString(code).value_or(""));

		if (clientConnection.send({(std::byte*)responseData.data(), (std::byte*)responseData.data() + responseData.size()}) != fp::Result::eSuccess) {
			fp::ErrorStack::push("Can't send response of route");
			fp::ErrorStack::logAll();
		}
	}

} // namespace fp

#undef FP_REQUEST_HANDLING_BENCHMARK
