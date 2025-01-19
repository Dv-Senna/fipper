#include <cerrno>
#include <cstring>
#include <print>
#include <ranges>

#include <fp/socket.hpp>
#include <fp/utils/enums.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


int main() {
	fp::Socket socket {};
	if (socket.create({.port = 1242}) != fp::Result::eSuccess) {
		std::println("Can't create socket : {}", errno);
		return 1;
	}

	if (listen(socket.getSocket(), 5) != 0) {
		std::println("Can't listen : {}", errno);
		return 1;
	}

	int clientSocket {accept(socket.getSocket(), nullptr, nullptr)};

	const char *msg {"Hello from fipper !"};
	send(clientSocket, msg, strlen(msg), 0);

	return 0;
}
