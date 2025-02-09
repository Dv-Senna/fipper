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
#include <fp/utils/generated/traitsAggregateReflection.hpp>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std::literals;


FP_REFLECTED_STRUCT(Address,
	(std::string) country,
	(std::string) city,
	(std::string) street,
	(int) postalCode,
	(int) number
);

FP_REFLECTED_STRUCT(Person,
	(std::string) name,
	(std::string) surname,
	(int) age,
	(Address) address
);

struct RandomStruct {
	std::string name;
	std::string surname;
	int age;
	Address address;
};

static_assert(std::is_same_v<std::tuple<std::string, std::string, std::string, int, int>, typename fp::utils::ReflectionTraits<Address>::MembersTypes>);
static_assert(fp::utils::IsReflectable<Address>);
static_assert(fp::utils::IsReflectable<Person>);

static_assert(fp::utils::AggregateMembersCount<Person>::value == 4);
static_assert(fp::utils::AggregateMembersCount<Address>::value == 5);
static_assert(std::is_same_v<fp::utils::AggregateMembers<Person>::Type, Person::MembersTypes>);
static_assert(std::is_same_v<fp::utils::AggregateMembers<Address>::Type, Address::MembersTypes>);

static_assert(fp::utils::IsReflectable<RandomStruct>);
static_assert(fp::utils::ReflectionTraits<RandomStruct>::MEMBERS_COUNT == 4);
static_assert(std::is_same_v<fp::utils::ReflectionTraits<RandomStruct>::MembersTypes, fp::utils::ReflectionTraits<Person>::MembersTypes>);

int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};


	RandomStruct randomStruct {};
	fp::utils::ReflectionTraits<RandomStruct>::getMember<0> (randomStruct) = "Hello";
	fp::utils::ReflectionTraits<RandomStruct>::getMember<1> (randomStruct) = "World";
	fp::utils::ReflectionTraits<RandomStruct>::getMember<2> (randomStruct) = 12;
	std::println("name : {}", randomStruct.name);
	std::println("surname : {}", randomStruct.surname);
	std::println("age : {}", randomStruct.age);


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
		response.body = "console.log('Hello World !'); fetch('/api/data').then((res)=>res.json()).then((body)=>console.log(body))";
	});

	server.get("/api/data", [](const fp::Request<void>&, fp::Response<Person> &response) noexcept {
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
