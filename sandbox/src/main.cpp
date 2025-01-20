#include <cerrno>
#include <cstring>
#include <print>

#include <fp/socket.hpp>
#include <fp/utils/enums.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


int main() {
	std::println("Result::eSuccess : {}", fp::utils::toString(fp::Result::eSuccess));
	std::println("Result::eFailure : {}", fp::utils::toString(fp::Result::eFailure));
	std::println("Result::eNothingToRecieve : {}", fp::utils::toString(fp::Result::eNothingToRecieve));

	fp::Socket socket {};
	if (socket.create({.port = 1242}) != fp::Result::eSuccess) {
		std::println(stderr, "Can't create socket : {}", errno);
		return 1;
	}

	std::println("Listening...");
	if (socket.listen(5) != fp::Result::eSuccess) {
		std::println(stderr, "Can't listen : {}", errno);
		return 1;
	}

	while (true) {
		auto clientSocketWithError {socket.accept()};
		if (!clientSocketWithError) {
			std::println(stderr, "Can't accept connection : {}", errno);
			return 1;
		}
		fp::Socket clientSocket {std::move(*clientSocketWithError)};

		auto requestWithError {clientSocket.recieve()};
		if (!requestWithError) {
			if (requestWithError.error() > fp::Result::eSuccess)
				continue;

			std::println(stderr, "Can't recieve data : {}", errno);
			return 1;
		}

		std::string request {(char*)requestWithError->data(), (char*)requestWithError->data() + requestWithError->size()};
		std::println("Request : {}", request);

		std::string html {"<html><body><h1>Hello World !</h1></body></html>"};
		std::string response {
			"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.size()) + "\r\n\r\n" + html
		};
		if (clientSocket.send({(std::byte*)response.data(), (std::byte*)response.data() + response.size()}) != fp::Result::eSuccess) {
			std::println(stderr, "Can't send data : {}", errno);
			return 1;
		}
		std::println("Response sent");
	}

	return 0;
}
