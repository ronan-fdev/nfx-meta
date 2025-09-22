/**
 * @file Int128Constants.h
 * @brief Mathematical and formatting constants for 128-bit signed integer arithmetic
 * @details Compile-time constants for Int128 operations, string conversion,
 *          and arithmetic bounds validation
 */

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace nfx::datatypes::constants::int128
{
	//=====================================================================
	// Int128 arithmetic constants
	//=====================================================================

	/** @brief High 64 bits of maximum positive 128-bit signed integer (2^127 - 1) */
	inline constexpr std::uint64_t MAX_POSITIVE_HIGH{ 0x7FFFFFFFFFFFFFFFULL };
	/** @brief Low 64 bits of maximum positive 128-bit signed integer (2^127 - 1) */
	inline constexpr std::uint64_t MAX_POSITIVE_LOW{ 0xFFFFFFFFFFFFFFFFULL };

	/** @brief High 64 bits of minimum negative 128-bit signed integer (-2^127) */
	inline constexpr std::uint64_t MIN_NEGATIVE_HIGH{ 0x8000000000000000ULL };
	/** @brief Low 64 bits of minimum negative 128-bit signed integer (-2^127) */
	inline constexpr std::uint64_t MIN_NEGATIVE_LOW{ 0x0000000000000000ULL };

	//=====================================================================
	// String conversion constants
	//=====================================================================

	/** @brief Base for decimal digit conversion. */
	inline constexpr int BASE{ 10 };

	/** @brief Maximum expected length for Int128 string representation. */
	inline constexpr size_t MAX_STRING_LENGTH{ 64UL };

	/** @brief Maximum positive value as decimal string (2^127 - 1). */
	inline constexpr std::string_view MAX_POSITIVE_STRING{ "170141183460469231731687303715884105727" };

	/** @brief Maximum negative value as decimal string (absolute value of -2^127). */
	inline constexpr std::string_view MAX_NEGATIVE_STRING{ "170141183460469231731687303715884105728" };
}
