#pragma once

#include "fp/endpoint.hpp"

#include <format>
#include <thread>

#include "fp/errorStack.hpp"
#include "fp/utils/janitor.hpp"


namespace fp {
	template <fp::IsEndpointCallback Func>
	constexpr Endpoint<Func>::Endpoint(fp::HttpMethod method, Route route, Func &&callback) noexcept :
		EndpointBase(method),
		m_callback {std::move(callback)},
		m_route {route}
	{

	}


	template <fp::IsEndpointCallback Func>
	auto Endpoint<Func>::handleRequest(std::shared_ptr<std::latch> latch, fp::Socket &&connection, std::string_view requestString) noexcept -> void {
		std::string requestStringOwnership {requestString};
		std::thread([this](std::shared_ptr<std::latch> &&latchParam, fp::Socket &&connection, std::string &&requestStringParam){
			std::shared_ptr<std::latch> latch {std::move(latchParam)};
			fp::Socket clientConnection {std::move(connection)};
			std::string requestString {std::move(requestStringParam)};
			fp::utils::Janitor _ {[&]() noexcept {latch->count_down();}};

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

			std::string html {(const char*)response.serialized.data(), (const char*)response.serialized.data() + response.serialized.size()};
			std::string responseData {std::format("HTTP/1.1 {} {}\r\nContent-Type: text/html\r\nContent-Lenght: {}\r\n\r\n{}",
				std::to_string(static_cast<std::int32_t> (code)),
				fp::getHttpCodeString(code).value_or(""),
				html.size(),
				html
			)};
			if (clientConnection.send({(std::byte*)responseData.data(), (std::byte*)responseData.data() + responseData.size()}) != fp::Result::eSuccess) {
				fp::ErrorStack::push("Can't send response of route");
				fp::ErrorStack::logAll();
			}
		}, std::move(latch), std::move(connection), std::move(requestStringOwnership)).detach();
	}

} // namespace fp
