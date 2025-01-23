#include <cerrno>
#include <cstring>
#include <print>

#include <fp/socket.hpp>
#include <fp/server.hpp>
#include <fp/utils/enums.hpp>
#include <fp/errorStack.hpp>
#include <fp/utils/janitor.hpp>
#include <fp/request.hpp>
#include <fp/utils/traits.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};


	using Lambda = decltype([](int) -> char {return 'a';});
	using Funct = std::function<char(int)>;
	static_assert(std::same_as<fp::utils::FunctionTraits<Funct>::ReturnType, char>);
	static_assert(std::same_as<fp::utils::FunctionTraits<Lambda>::ReturnType, char>);
	static_assert(std::same_as<std::tuple_element_t<0, fp::utils::FunctionTraits<Lambda>::ArgumentsTypes>, int>);


	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess)
		return fp::ErrorStack::push(1, "Can't create server");

	std::atomic_int connectionCount {};
	server.get<void, std::string> ("/", [&connectionCount](const auto &, auto &response) noexcept {
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<html><body style='background-color: #111; color: #fff;'><h1>Hello World for the {}th time !</h1></body></html>", ++connectionCount);
		return fp::HttpCode::e200;
	});

	if (server.run() != fp::Result::eSuccess)
		return fp::ErrorStack::push(1, "Can't run server");

	return 0;
}
