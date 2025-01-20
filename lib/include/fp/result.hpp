#pragma once

#include <cstdint>

#include "fp/utils/enums.hpp"


namespace fp {
	enum class Result : std::int32_t {
		eSuccess = 0,
		eNothingToRecieve = 1,
		eFailure = -1
	};

} // namespace fp

FP_SET_ENUM_START_INDEX(fp::Result, -1);
