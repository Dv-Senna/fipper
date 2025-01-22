#include "fp/server.hpp"

#include <csignal>
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
		//(void)std::signal(SIGINT, Server::s_signalHandler);

		if (m_serverSocket.listen() != fp::Result::eSuccess)
			return fp::ErrorStack::push(fp::Result::eFailure, "Can't listen to server socket");

		int a {};

		while (!s_endSignal) {
			auto clientSocketWithError {m_serverSocket.accept()};
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
			std::println("-------------");
			std::println("REQ : {}", request);
			std::println("-------------");

			{
				std::string_view methodString {request.begin(), std::ranges::find(request, ' ')};
				fp::HttpMethod method {};
				if (methodString == "GET")
					method = fp::HttpMethod::eGet;

				auto route {m_endpoints.find(method)};
				if (route == m_endpoints.end())
					continue;
				std::latch latch {1};
				(void)route->second["/"]->handleRequest(latch, std::move(clientSocket), request);
			}

			continue;
			std::string html {std::format("<html><body><h1>Hello World {} !</h1></body></html>", a++)};
			std::string response {
				"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html
			};
			if (clientSocket.send({(std::byte*)response.data(), (std::byte*)response.data() + response.size()}) != fp::Result::eSuccess)
				return fp::ErrorStack::push(fp::Result::eFailure, "Can't send data for client socket");
		}

		return fp::Result::eSuccess;
	}


	auto Server::s_signalHandler(int) noexcept -> void {
		s_endSignal = true;
	}


	bool Server::s_endSignal {false};
} // namespace fp
