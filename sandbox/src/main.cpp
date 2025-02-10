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


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};


	fp::containers::StackBasedRollingQueue<int, 16> queue {};

	for (std::size_t i {0}; i < 20; ++i) {
		if (queue.push(i) != fp::Result::eSuccess)
			std::println("Can't push {} in the rolling queue", i);
	}

	for (std::size_t i {0}; i < 9; ++i) {
		std::optional element {queue.pop()};
		if (!element)
			std::println("No more element to pop : {}", i);
		else
			std::println("Element {} : {}", i, (int)*element);
	}

	for (std::size_t i {20}; i < 30; ++i) {
		(void)queue.push(i);
	}

	return EXIT_SUCCESS;

	fp::Server server {};
	if (server.create({.port = 1242}) != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't create server");

	server.get("/{name:not_empty}", [](const fp::Request<void, std::string> &request, fp::Response<std::string> &response) noexcept {
		auto name {**request.getParam<std::string> ("name")};
		response.header.contentType = fp::ContentType::eHtml;
		response.body = std::format("<html>\
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
		response.body = std::format("<html><body style='background-color: #111; color: #fff;'><h1>This is the root get</h1></body></html>");
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

	server.post("/api/data", [](const fp::Request<Country> &request, fp::Response<Person> &response) noexcept {
		std::println("COUNTRY : {} ({})", request.body.name, request.body.id);

		response.body.name = "Albert";
		response.body.surname = "Einstein";
		response.body.age = 76;
		response.body.address.country = "Switzerland";
		response.body.address.city = "Bern";
		response.body.address.postalCode = 3000;
		response.body.address.street = "Kramgasse";
		response.body.address.number = 49;
	});


	if (server.run() != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't run server");

	return EXIT_SUCCESS;
}
