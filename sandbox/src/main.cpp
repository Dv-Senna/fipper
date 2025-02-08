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
#include <fp/utils/macros.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


FP_REFLECTED_STRUCT(Person,
	((std::string) name)
	((std::string) surname)
	((int) age)
);


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};

	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't create server");

	server.get("/{name:not_empty}", [](const fp::Request<void, std::string> &request, fp::Response<std::string> &response) noexcept {
		auto name {**request.getParam<std::string> ("name")};
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<html>\
			<head>\
				<script src='scripts/script.js'></script>\
			</head>\
			<body style='background-color: #111; color: #fff;'>\
				<h1>Hello World {} !</h1>\
			</body>\
		</html>", name);
		return fp::HttpCode::e200;
	});

	server.get("/", [](const fp::Request<void>&, fp::Response<std::string> &response) noexcept {
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<html><body style='background-color: #111; color: #fff;'><h1>This is the root get</h1></body></html>");
		return fp::HttpCode::e200;
	});

	server.get("/scripts/script.js", [](const fp::Request<void>&, fp::Response<std::string> &response) noexcept {
		response.header.contentType = fp::ContentType::eJavascript;
		response.body = "console.log('Hello World !'); fetch('/api/data').then((res)=>res.json()).then((body)=>console.log(body))";
		return fp::HttpCode::e200;
	});

	server.get("/api/data", [](const fp::Request<void>&, fp::Response<Person> &response) noexcept {
		response.body.name = "Michel";
		response.body.surname = "Michel";
		response.body.age = 72;
		return fp::HttpCode::e200;
	});


	if (server.run() != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't run server");

	return EXIT_SUCCESS;
}
