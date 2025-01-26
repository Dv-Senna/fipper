#pragma once

#include "fp/header.hpp"
#include "fp/serializer.hpp"


namespace fp {
	template <typename Response>
	concept IsResponse = (std::same_as<typename Response::HasBody, std::true_type> || std::same_as<typename Response::HasBody, std::false_type>)
		&& requires (Response res, typename Response::Header) {
		{res.header} -> std::same_as<typename Response::Header&>;
	};

	template <typename Body>
	struct Response {
		using Header = ResponseHeader<Body>;
		using HasBody = std::true_type;
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
		using HasBody = std::false_type;
		ResponseHeader<void> header;

		constexpr auto serialize() noexcept -> void {}
	};

} // namespace fp
