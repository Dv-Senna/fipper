#include <cerrno>
#include <cstring>
#include <print>

#include <fp/socket.hpp>
#include <fp/server.hpp>
#include <fp/utils/enums.hpp>
#include <fp/errorStack.hpp>
#include <fp/utils/janitor.hpp>
#include <fp/request.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};


	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess) {
		std::println(stderr, "Can't create server");
		return 1;
	}

	server.get("/", [](){
		std::println("ROUTE REQUEST !");
	});

	if (server.run() != fp::Result::eSuccess) {
		std::println(stderr, "Can't run server");
		return 1;
	}

	return 0;
}
