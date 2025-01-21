#include <cerrno>
#include <cstring>
#include <print>

#include <fp/socket.hpp>
#include <fp/server.hpp>
#include <fp/utils/enums.hpp>
#include <fp/errorStack.hpp>
#include <fp/utils/janitor.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		std::println("JANITOR");
	}};

	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess) {
		fp::ErrorStack::logAll();
		std::println(stderr, "Can't create server");
		return 1;
	}


	if (server.run() != fp::Result::eSuccess) {
		fp::ErrorStack::logAll();
		std::println(stderr, "Can't run server");
		return 1;
	}

	return 0;
}
