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

FP_SET_ENUM_START_INDEX(fp::HttpCode, 0);
FP_SET_ENUM_MAX_INDEX(fp::HttpCode, 100);

template <std::size_t index>
struct fp::utils::EnumValueConstructor<fp::HttpCode, index> {
	static constexpr fp::HttpCode (*func)(std::size_t) {[](std::size_t index2) -> fp::HttpCode {
		switch (index2) {
			case 0: return HttpCode::e100;
			case 1: return HttpCode::e101;
			case 2: return HttpCode::e102;
			case 3: return HttpCode::e200;
			case 4: return HttpCode::e201;
			case 5: return HttpCode::e202;
			case 6: return HttpCode::e203;
			case 7: return HttpCode::e204;
			case 8: return HttpCode::e205;
			case 9: return HttpCode::e206;
			case 10: return HttpCode::e207;
			case 11: return HttpCode::e208;
			case 12: return HttpCode::e226;
			case 13: return HttpCode::e300;
			case 14: return HttpCode::e301;
			case 15: return HttpCode::e302;
			case 16: return HttpCode::e303;
			case 17: return HttpCode::e304;
			case 18: return HttpCode::e307;
			case 19: return HttpCode::e308;
			case 20: return HttpCode::e400;
			case 21: return HttpCode::e401;
			case 22: return HttpCode::e402;
			case 23: return HttpCode::e403;
			case 24: return HttpCode::e404;
			case 25: return HttpCode::e405;
			case 26: return HttpCode::e406;
			case 27: return HttpCode::e407;
			case 28: return HttpCode::e408;
			case 29: return HttpCode::e409;
			case 30: return HttpCode::e410;
			case 31: return HttpCode::e411;
			case 32: return HttpCode::e412;
			case 33: return HttpCode::e413;
			case 34: return HttpCode::e414;
			case 35: return HttpCode::e415;
			case 36: return HttpCode::e416;
			case 37: return HttpCode::e417;
			case 38: return HttpCode::e418;
			case 39: return HttpCode::e421;
			case 40: return HttpCode::e422;
			case 41: return HttpCode::e423;
			case 42: return HttpCode::e424;
			case 43: return HttpCode::e426;
			case 44: return HttpCode::e428;
			case 45: return HttpCode::e429;
			case 46: return HttpCode::e431;
			case 47: return HttpCode::e444;
			case 48: return HttpCode::e451;
			case 49: return HttpCode::e499;
			case 50: return HttpCode::e500;
			case 51: return HttpCode::e501;
			case 52: return HttpCode::e502;
			case 53: return HttpCode::e503;
			case 54: return HttpCode::e504;
			case 55: return HttpCode::e505;
			case 56: return HttpCode::e506;
			case 57: return HttpCode::e507;
			case 58: return HttpCode::e508;
			case 59: return HttpCode::e510;
			case 60: return HttpCode::e511;
			case 61: return HttpCode::e599;
		}

		return (HttpCode)(600 + index2);
	}};
};
