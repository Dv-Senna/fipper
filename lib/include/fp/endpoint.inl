#pragma once

#include "fp/endpoint.hpp"

#include <thread>

#include "fp/errorStack.hpp"


namespace fp {
	template <fp::IsRequest Request, fp::IsResponse Response, fp::IsEndpointCallback<Request, Response> Func>
	constexpr Endpoint<Request, Response, Func>::Endpoint(fp::HttpMethod method, std::string_view route, Func &&callback) noexcept :
		EndpointBase(method, route),
		m_callback {std::move(callback)}
	{

	}


	template <fp::IsRequest Request, fp::IsResponse Response, fp::IsEndpointCallback<Request, Response> Func>
	auto Endpoint<Request, Response, Func>::handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view) noexcept -> fp::Result {
		std::thread([&, this](fp::Socket &&connection){
			fp::Socket clientConnection {std::move(connection)};
			Response response {};
			this->m_callback({}, response);
			response.serialize();

			std::string html {(const char*)response.serialized.data(), (const char*)response.serialized.data() + response.serialized.size()};
			std::string responseData {
				"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html
			};
			if (clientConnection.send({(std::byte*)responseData.data(), (std::byte*)responseData.data() + responseData.size()}) != fp::Result::eSuccess) {
				fp::ErrorStack::push("Can't send response of route");
				fp::ErrorStack::logAll();
			}
			latch.count_down();
		}, std::move(connection)).detach();

		return fp::Result::eSuccess;
	}

} // namespace fp
