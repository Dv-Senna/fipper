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

	int connectionCount {};
	server.get<void, std::string> ("/", [&connectionCount](const fp::Request<void> &, fp::Response<std::string> &response) noexcept {
		std::println("ROUTE REQUEST !");
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<html><body><h1>Hello World for the {}th time !</h1></body></html>", ++connectionCount);
		return fp::HttpCode::e200;
	});

	if (server.run() != fp::Result::eSuccess) {
		std::println(stderr, "Can't run server");
		return 1;
	}

	return 0;
}
