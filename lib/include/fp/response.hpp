#pragma once

#include "fp/header.hpp"
#include "fp/serializer.hpp"


namespace fp {
	template <typename Response>
	concept IsResponse = requires (Response res, typename Response::Header) {
		{Response::HAS_BODY} -> std::convertible_to<bool>;
		{res.header} -> std::same_as<typename Response::Header&>;
	};

	template <typename Body>
	struct Response {
		using Header = ResponseHeader<Body>;
		static constexpr bool HAS_BODY {true};
		ResponseHeader<Body> header;
		Body body;
		std::vector<std::byte> serialized;

		constexpr auto serialize() noexcept -> void {
			auto data {*fp::serialize<Body> (body)};
			if (header.contentType == fp::ContentType::eUnknown)
				header.contentType = data.type;
			header.contentSize = data.data.size();
			serialized = data.data;
		}
	};

	template <>
	struct Response<void> {
		using Header = ResponseHeader<void>;
		static constexpr bool HAS_BODY {false};
		ResponseHeader<void> header;

		constexpr auto serialize() noexcept -> void {}
	};

} // namespace fp
