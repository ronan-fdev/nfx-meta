/**
 * @file DecimalConstants.h
 * @brief Mathematical and formatting constants for high-precision decimal arithmetic
 * @details Compile-time constants for decimal operations, string conversion,
 *          and arithmetic optimization
 */

#pragma once

#include <array>
#include <cstdint>

namespace nfx::datatypes::constants::decimal
{
	//=====================================================================
	// Decimal arithmetic constants
	//=====================================================================

	/** @brief Maximum number of decimal places supported. */
	inline constexpr std::uint8_t MAXIMUM_PLACES{ 28U };

	/** @brief Bit mask for extracting scale from flags field (bits 16-23). */
	inline constexpr std::uint32_t SCALE_MASK{ 0x00FF0000U };

	/** @brief Bit mask for sign detection (bit 31). */
	inline constexpr std::uint32_t SIGN_MASK{ 0x80000000U };

	/** @brief Bit position for scale field in flags. */
	inline constexpr std::uint8_t SCALE_SHIFT{ 16U };

	//=====================================================================
	// String conversion constants
	//=====================================================================

	/** @brief Base for decimal digit conversion. */
	inline constexpr std::uint64_t BASE{ 10ULL };

	/** @brief Maximum expected length for decimal string representation. */
	inline constexpr size_t MAX_STRING_LENGTH{ 64UL };

	//=====================================================================
	// Performance optimization constants
	//=====================================================================

	/** @brief Power of 10 lookup table size for fast scaling operations (64-bit range). */
	inline constexpr std::uint8_t POWER_TABLE_SIZE{ 20U };

	//=====================================================================
	// Pre-computed power of 10 lookup table
	//=====================================================================

	/** @brief Powers of 10 lookup table for efficient scaling operations (64-bit range: 10^0 to 10^19). */
	inline constexpr std::array<std::uint64_t, POWER_TABLE_SIZE> POWERS_OF_10{ {
		1ULL,					 // 10^0
		10ULL,					 // 10^1
		100ULL,					 // 10^2
		1000ULL,				 // 10^3
		10000ULL,				 // 10^4
		100000ULL,				 // 10^5
		1000000ULL,				 // 10^6
		10000000ULL,			 // 10^7
		100000000ULL,			 // 10^8
		1000000000ULL,			 // 10^9
		10000000000ULL,			 // 10^10
		100000000000ULL,		 // 10^11
		1000000000000ULL,		 // 10^12
		10000000000000ULL,		 // 10^13
		100000000000000ULL,		 // 10^14
		1000000000000000ULL,	 // 10^15
		10000000000000000ULL,	 // 10^16
		100000000000000000ULL,	 // 10^17
		1000000000000000000ULL,	 // 10^18
		10000000000000000000ULL, // 10^19 (max uint64_t support)
	} };

	//=====================================================================
	// Extended 128-bit power calculation helpers
	//=====================================================================

	/**
	 * @brief Power-of-10 calculation constants for 128-bit arithmetic
	 * @details Pre-computed constants for powers that exceed 64-bit range.
	 *          These are the low and high 64-bit components of 10^n where n > 19.
	 */
	inline constexpr std::array<std::pair<std::uint64_t, std::uint64_t>, 9> EXTENDED_POWERS_OF_10{ {
		{ 0x6BC75E2D630EB4E0ULL, 0x0000000000000005ULL }, // 10^20
		{ 0x35C9ADC5DEA00000ULL, 0x0000000000000036ULL }, // 10^21
		{ 0x19E0C9BAB2400000ULL, 0x000000000000021EULL }, // 10^22
		{ 0x02C7E14AF6800000ULL, 0x000000000000152DULL }, // 10^23
		{ 0x1BCECCEDA1000000ULL, 0x000000000000D3C2ULL }, // 10^24
		{ 0x161401484A000000ULL, 0x0000000000084595ULL }, // 10^25
		{ 0xDCC80CD2E4000000ULL, 0x0000000000052B7DULL }, // 10^26
		{ 0x9FD0803CE8000000ULL, 0x00000000003336A9ULL }, // 10^27
		{ 0x3E25026110000000ULL, 0x0000000000204FCEULL }  // 10^28
	} };
}
