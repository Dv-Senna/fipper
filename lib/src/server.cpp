#include "fp/server.hpp"

#include <csignal>
#include <map>
#include <print>
#include <unistd.h>

#include "fp/errorStack.hpp"


namespace fp {
	auto Server::create(const CreateInfos &createInfos) noexcept -> fp::Result {
		m_port = createInfos.port;
		fp::Socket::CreateInfos socketCreateInfos {};
		socketCreateInfos.port = m_port;
		if (m_serverSocket.create(socketCreateInfos) != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't create server socket");

		return fp::Result::eSuccess;
	}


	auto Server::run() noexcept -> fp::Result {
		using namespace std::literals;
		(void)std::signal(SIGINT, Server::s_signalHandler);

		std::map<std::string_view, fp::HttpMethod> methodMap {
			{"GET", fp::HttpMethod::eGet},
			{"PUT", fp::HttpMethod::ePut},
			{"POST", fp::HttpMethod::ePost},
			{"PATCH", fp::HttpMethod::ePatch},
			{"DELETE", fp::HttpMethod::eDelete}
		};

		if (m_serverSocket.listen() != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't listen to server socket");


		std::println("Server listening");
		std::println("\tFor local use : \033]8;;localhost:{0}\033\\localhost:{0}\033]8;;\033\\", m_port);

		while (!s_endSignal) {
			auto clientSocketWithError {m_serverSocket.accept(1000ms)};
			if (!clientSocketWithError) {
				if (clientSocketWithError.error() == fp::Result::eNothingToAccept)
					continue;
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't accept client socket");
			}

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
			auto method {methodMap.find(methodString)};
			if (method == methodMap.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 400 Bad Request"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send 400 after invalid method name");
				}
				continue;
			}

			std::string_view routeString {*++split.begin()};
			auto route {std::ranges::find_if(m_endpoints, [&routeString](const auto &endpoint){return endpoint.first->isInstance(routeString);})};
			if (route == m_endpoints.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 404 Not Found"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send 404 after invalid route");
					fp::ErrorStack::logAll();
				}
				continue;
			}

			auto endpoint {route->second.find(method->second)};
			if (endpoint == route->second.end()) {
				if (clientSocket.send(fp::serialize("HTTP/1.1 405 Method Not Allowed"sv)->data) != fp::Result::eSuccess) {
					fp::ErrorStack::push("Can't send 405 after invalid method");
					fp::ErrorStack::logAll();
				}
				continue;
			}

			auto latch {std::make_shared<std::latch> (1)};
			endpoint->second->handleRequest(latch, std::move(clientSocket), request);
		}

		return fp::Result::eSuccess;
	}


	auto Server::s_signalHandler(int) noexcept -> void {
		s_endSignal = true;
	}


	std::atomic_bool Server::s_endSignal {false};
} // namespace fp
