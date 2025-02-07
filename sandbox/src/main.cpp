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

#define REMOVE_PARENTHESIS(...) __VA_ARGS__

#define REFLECT_FOREACH_MEMBER_CREATION(seq) REFLECT_FOREACH_MEMBER_END(REFLECT_FOREACH_MEMBER_CREATION_A seq)
#define REFLECT_FOREACH_MEMBER_CREATION_BODY(value) REMOVE_PARENTHESIS value;
#define REFLECT_FOREACH_MEMBER_CREATION_A(x) REFLECT_FOREACH_MEMBER_CREATION_BODY(x) REFLECT_FOREACH_MEMBER_CREATION_B
#define REFLECT_FOREACH_MEMBER_CREATION_B(x) REFLECT_FOREACH_MEMBER_CREATION_BODY(x) REFLECT_FOREACH_MEMBER_CREATION_A
#define REFLECT_FOREACH_MEMBER_CREATION_A_END
#define REFLECT_FOREACH_MEMBER_CREATION_B_END
#define REFLECT_FOREACH_MEMBER_END(...) REFLECT_FOREACH_MEMBER_END_(__VA_ARGS__)
#define REFLECT_FOREACH_MEMBER_END_(...) __VA_ARGS__##_END

#define REFLECT(name, seq) struct name {\
	constexpr static bool hasReflection {true};\
	REFLECT_FOREACH_MEMBER_CREATION(seq)\
}

REFLECT(Foo,
	((int) a)((std::string) b)((nlohmann::json) c)
);

/*REFLECT(Bar,
	(std::uint64_t) value,
	(Foo) foo
);*/

/*struct Foo {
	constexpr static bool hasReflection {true};

	int a;
	std::string b;
	nlohmann::json c;

	using MembersTypes = std::tuple<int, std::string, nlohmann::json>;
	constexpr static std::tuple<int Foo::*, std::string Foo::*, nlohmann::json Foo::*> members {
		&Foo::a,
		&Foo::b,
		&Foo::c
	};
	constexpr static std::array<std::string_view, 3> names {"a", "b", "c"};
};*/


/*struct Bar {
	constexpr static bool hasReflection {true};

	std::uint64_t value;
	Foo foo;

	using MembersTypes = std::tuple<std::uint64_t, Foo>;
	constexpr static std::tuple<std::uint64_t Bar::*, Foo Bar::*> members {
		&Bar::value,
		&Bar::foo
	};
	constexpr static std::array<std::string_view, 2> names {"value", "foo"};
};


template <typename T>
auto jsonify(const T &t) noexcept -> std::optional<nlohmann::json>;

template <typename S, std::size_t index>
auto serializeLoop(const S &value) noexcept -> std::optional<nlohmann::json> {
	if constexpr (index >= std::tuple_size_v<typename S::MembersTypes>)
		return nlohmann::json{};
	else {
		auto json (serializeLoop<S, index + 1> (value));
		if (!json)
			return std::nullopt;
		using T = std::tuple_element_t<index, typename S::MembersTypes>;
		if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, std::string>)
			(*json)[S::names[index]] = value.*(std::get<index> (S::members));
		else {
			auto serialized {jsonify(value.*(std::get<index> (S::members)))};
			if (!serialized)
				return std::nullopt;
			(*json)[S::names[index]] = *serialized;
		}
		return json;
	}
}



template <typename T>
auto jsonify(const T &value) noexcept -> std::optional<nlohmann::json> {
	auto json = serializeLoop<T, 0> (value);
	if (!json)
		return std::nullopt;
	return *json;
}

template <>
auto jsonify<nlohmann::json> (const nlohmann::json &json) noexcept -> std::optional<nlohmann::json> {
	return json;
}


template <>
auto fp::serialize<Foo> (const Foo &foo) noexcept -> std::optional<fp::Serialized> {
	auto json = jsonify(foo);
	if (!json)
		return std::nullopt;
	return fp::serialize(*json);
}

template <>
auto fp::serialize<Bar> (const Bar &bar) noexcept -> std::optional<fp::Serialized> {
	auto json = jsonify(bar);
	if (!json)
		return std::nullopt;
	return fp::serialize(json->dump(4));
}*/


int main() {
	fp::utils::Janitor _ {[]() noexcept {
		fp::ErrorStack::logAll();
	}};

	Foo foo {};
	foo.a = 12;
	foo.b = "Hihi";
	foo.c["hello"] = "world";
/*	Bar bar {};
	bar.value = 42;
	bar.foo = foo;
	auto serialized {fp::serialize(bar)};
	if (!serialized)
		return EXIT_FAILURE;
	std::string str {(const char*)serialized->data.data(), (const char*)serialized->data.data() + serialized->data.size()};
	std::println("SER : {}", str);
*/
	return EXIT_SUCCESS;

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

	server.get("/api/data", [](const fp::Request<void>&, fp::Response<nlohmann::json> &response) noexcept {
		response.body["name"] = "Michel";
		response.body["surname"] = "Michel";
		response.body["age"] = 72;
		return fp::HttpCode::e200;
	});


	if (server.run() != fp::Result::eSuccess)
		return fp::ErrorStack::push(EXIT_FAILURE, "Can't run server");

	return EXIT_SUCCESS;
}
