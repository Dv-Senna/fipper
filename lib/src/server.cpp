#include "fp/server.hpp"

#include <csignal>
#include <future>
#include <print>
#include <unistd.h>

#include "fp/errorStack.hpp"


namespace fp {
	auto Server::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		fp::Socket::CreateInfos socketCreateInfos {};
		socketCreateInfos.port = createInfos.port;
		if (m_serverSocket.create(socketCreateInfos) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't create server socket");

		return fp::Result::eSuccess;
	}


	auto Server::run() noexcept -> fp::Result {
		using namespace std::literals;
		(void)std::signal(SIGINT, Server::s_signalHandler);

		if (m_serverSocket.listen() != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't listen to server socket");

		std::optional<std::future<std::optional<fp::Socket>>> clientSocketPromise {};

		while (!s_endSignal) {
			if (!clientSocketPromise) {
				clientSocketPromise = std::async([this](){return this->m_serverSocket.accept();});
				continue;
			}

			if (clientSocketPromise->wait_for(1000ms) != std::future_status::ready)
				continue;

			auto clientSocketWithError {clientSocketPromise->get()};
			clientSocketPromise = std::nullopt;
			if (!clientSocketWithError)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't accept client socket");
			fp::Socket &clientSocket {*clientSocketWithError};

			auto hasDataToRecieveWithError {clientSocket.hasDataToRecieve(1000ms)};
			if (!hasDataToRecieveWithError)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't check read data for client socket");
			if (!*hasDataToRecieveWithError)
				continue;

			auto requestWithError {clientSocket.recieve()};
			if (!requestWithError)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't read data for client socket");
			std::string request {(char*)requestWithError->data(), (char*)requestWithError->data() + requestWithError->size()};

			auto split {std::views::split(request, ' ')};
			std::string_view methodString {*split.begin()};
			fp::HttpMethod method {};
			if (methodString == "GET")
				method = fp::HttpMethod::eGet;

			auto route {m_endpoints.find(method)};
			if (route == m_endpoints.end()) {
				std::string_view data {"HTTP/1.1 404 Not Found"};
				if (clientSocket.send({(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()}) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send 404 after invalid method");
					fp::ErrorStack::logAll();
				}
				continue;
			}

			std::string_view routeString {*++split.begin()};
			auto endpoint {route->second.find(routeString)};
			if (endpoint == route->second.end()) {
				std::string_view data {"HTTP/1.1 404 Not Found"};
				if (clientSocket.send({(const std::byte*)data.data(), (const std::byte*)data.data() + data.size()}) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send 404 after invalid route");
					fp::ErrorStack::logAll();
				}
				continue;
			}

			std::latch latch {1};
			endpoint->second->handleRequest(latch, std::move(clientSocket), request);
		}

		new (&clientSocketPromise) decltype(clientSocketPromise)(std::nullopt);
		return fp::Result::eSuccess;
	}


	auto Server::s_signalHandler(int) noexcept -> void {
		s_endSignal = true;
	}


	bool Server::s_endSignal {false};
} // namespace fp
