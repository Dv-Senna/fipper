#pragma once

#include "header.hpp"


namespace fp {
	template <typename Body>
	struct Response {
		ResponseHeader<Body> header;
		Body body;
	};

	template <>
	struct Response<void> {
		ResponseHeader<void> header;
	};

} // namespace fp
