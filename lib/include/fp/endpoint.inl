#pragma once

#include "fp/endpoint.hpp"

#include <thread>

#include "fp/errorStack.hpp"


namespace fp {
	template <typename Request, typename Response, typename Func>
	constexpr Endpoint<Request, Response, Func>::Endpoint(fp::HttpMethod method, std::string_view route, Func &&callback) noexcept :
		EndpointBase(method, route),
		m_callback {callback}
	{

	}


	template <typename Request, typename Response, typename Func>
	auto Endpoint<Request, Response, Func>::handleRequest(std::latch &latch, fp::Socket &&connection, std::string_view) noexcept -> fp::Result {
		std::thread([&, this](fp::Socket &&connection){
			fp::Socket clientConnection {std::move(connection)};
			(void)this->m_callback();
			std::string html {"<html><body><h1>Hello World {} !</h1></body></html>"};
			std::string response {
				"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html
			};
			if (clientConnection.send({(std::byte*)response.data(), (std::byte*)response.data() + response.size()}) != fp::Result::eSuccess) {
				fp::ErrorStack::push("Can't send response of route");
				fp::ErrorStack::logAll();
			}
			latch.count_down();
		}, std::move(connection)).detach();

		return fp::Result::eSuccess;
	}

} // namespace fp
