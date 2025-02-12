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
#include <fp/containers/stackBasedRollingQueue.hpp>
#include <fp/jobScheduler.hpp>
#include <fp/coroutines/promise.hpp>
#include <fp/coroutines/awaiter.hpp>
#include <fp/coroutines/async.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


struct Address {
	std::string country;
	std::string city;
	std::string street;
	int postalCode;
	int number;
};

struct Person {
	std::string name;
	std::string surname;
	int age;
	Address address;
};


struct Country {
	std::string name;
	std::string id;
};



auto asyncProcess() -> fp::coroutines::AsyncAwaiter<int> {
	return fp::coroutines::AsyncAwaiter<int> {[](fp::coroutines::AsyncAwaiter<int> &awaiter) noexcept{
		std::thread{[&awaiter]() noexcept {
			awaiter.complete(12);
		}}.detach();
	}};
}


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
		response.body = std::format("<!DOCTYPE html><html>\
			<head>\
				<title>{0}</title>\
				<script src='scripts/script.js'></script>\
			</head>\
			<body style='background-color: #111; color: #fff;'>\
				<h1>Hello World {0} !</h1>\
			</body>\
		</html>", name);
	});

	server.get("/", [](const fp::Request<void>&, fp::Response<std::string> &response) noexcept {
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<!DOCTYPE html><html><body style='background-color: #111; color: #fff;'><h1>This is the root get</h1></body></html>");
	});

	server.get("/scripts/script.js", [](const fp::Request<void>&, fp::Response<std::string> &response) noexcept {
		response.header.contentType = fp::ContentType::eJavascript;
		response.body = "fetch('/api/data', {\
			method: 'POST',\
			body: JSON.stringify({\
				name: 'Switzerland',\
				id: 'CH'\
			})\
		}).then((res)=>res.json()).then((body)=>console.log(body))";
	});

	server.post("/api/data", [](const fp::Request<Country> &request, fp::Response<Person> &response) noexcept -> fp::Async {
		std::println("COUNTRY : {} ({})", request.body.name, request.body.id);

		for (std::size_t i {0}; i < 10; ++i)
			std::println("Result of async process {} : {}", i, co_await asyncProcess());

		response.body.name = "Albert";
		response.body.surname = "Einstein";
		response.body.age = 76;
		response.body.address.country = "Switzerland";
		response.body.address.city = "Bern";
		response.body.address.postalCode = 3000;
		response.body.address.street = "Kramgasse";
		response.body.address.number = 49;
		co_return fp::HttpCode::e200;
	});


	if (server.run() != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't run server");

	return EXIT_SUCCESS;
}
