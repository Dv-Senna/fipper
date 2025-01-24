#pragma once

#include <optional>
#include <string_view>

#include "fp/core.hpp"
#include "fp/utils/enums.hpp"


namespace fp {
	enum class HttpCode {
		e100 = 100, e101 = 101, e102 = 102,
		e200 = 200, e201 = 201, e202 = 202, e203 = 203, e204 = 204, e205 = 205, e206 = 206, e207 = 207, e208 = 208,
		e226 = 226,
		e300 = 300, e301 = 301, e302 = 302, e303 = 303, e304 = 304, e307 = 307, e308 = 308,
		e400 = 400, e401 = 401, e402 = 402, e403 = 403, e404 = 404, e405 = 405, e406 = 406, e407 = 407, e408 = 408,
		e409 = 409, e410 = 410, e411 = 411, e412 = 412, e413 = 413, e414 = 414, e415 = 415, e416 = 416, e417 = 417,
		e418 = 418, e421 = 421, e422 = 422, e423 = 423, e424 = 424, e426 = 426, e428 = 428, e429 = 429, e431 = 431,
		e444 = 444, e451 = 451, e499 = 499,
		e500 = 500, e501 = 501, e502 = 502, e503 = 503, e504 = 504, e505 = 505, e506 = 506, e507 = 507, e508 = 508,
		e510 = 510, e511 = 511, e599 = 599
	};

	FP_CORE std::optional<std::string_view> getHttpCodeString(HttpCode code);

} // namespace fp

