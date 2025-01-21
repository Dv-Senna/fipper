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


	fp::Request<int, std::string, float> request {};
	request.setParamNames({"param1", "param2"});
	request.markRuntimeReady();

	static_assert(std::same_as<decltype(request.getParam<0> ()), std::string&>);
	request.getParam<0> () = "Hello World !";
	std::println("PARAM1 : {}", **request.getParam<std::string> ("param1"));

	fp::Request<void> request2 {};
	request2.markRuntimeReady();

	fp::Request<void, std::string> request3 {};
	request3.getParam<0> () = "Hello World !";
	std::println("3 PARAM1 : {}", request3.getParam<0> ());

	fp::Request<int> request4 {};

	return 0;

	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess) {
		std::println(stderr, "Can't create server");
		return 1;
	}


	if (server.run() != fp::Result::eSuccess) {
		std::println(stderr, "Can't run server");
		return 1;
	}

	return 0;
}
