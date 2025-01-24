#pragma once

#include "fp/endpoint.hpp"

#include <format>
#include <thread>

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
	auto Endpoint<Func>::handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view) noexcept -> void {
		std::thread([&, this](fp::Socket &&connection){
			fp::Socket clientConnection {std::move(connection)};
			Request request {};
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
			latch.count_down();
		}, std::move(connection)).detach();
	}

} // namespace fp
