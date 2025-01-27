#pragma once

#include "fp/endpoint.hpp"

#include <format>

#include "fp/errorStack.hpp"


namespace fp {
	template <fp::IsEndpointCallback Func>
	constexpr Endpoint<Func>::Endpoint(fp::HttpMethod method, Route route, Func &&callback) noexcept :
		EndpointBase(method),
		m_callback {std::move(callback)},
		m_route {route}
	{

	}


	template <fp::IsEndpointCallback Func>
	auto Endpoint<Func>::handleRequest(fp::Socket &&connection, std::string_view requestString) const noexcept -> void {
		fp::Socket clientConnection {std::move(connection)};

		auto split {std::views::split(requestString, ' ')};
		std::string_view requestRoute {*++split.begin()};

		Request request {};
		if constexpr (std::same_as<typename Request::HasParams, std::true_type>) {
			std::optional params {m_route.extractParamsFromInstance(requestRoute)};
			if (!params) {
				if (clientConnection.send(fp::serialize("HTTP/1.1 400 Bad Request"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send response of invalid parameter of route");
					fp::ErrorStack::logAll();
				}
				return;
			}
			request.setParamNames(m_route.getParamNames());
			request.markRuntimeReady();
			request.setParams(std::move(*params));
		}

		Response response {};
		fp::HttpCode code {this->m_callback(request, response)};
		response.serialize();

		std::string responseData {};
		if constexpr (std::same_as<typename Response::HasBody, std::true_type>) {
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
