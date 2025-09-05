/**
 * @file ChdConstants.h
 * @brief Algorithm constants for ChdHashMap perfect hash implementation
 * @details FNV hash constants and CHD perfect hashing tuning parameters
 *          optimized for ChdHashMap performance
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace nfx::containers::constants::chd
{
	//=====================================================================
	// Hash algorithm constants
	//=====================================================================

	/** @brief FNV offset basis constant for hash calculations. */
	inline constexpr uint32_t FNV_OFFSET_BASIS{ 0x811C9DC5 };

	/** @brief FNV prime constant for hash calculations. */
	inline constexpr uint32_t FNV_PRIME{ 0x01000193 };

	//=====================================================================
	// CHD HashMap configuration
	//=====================================================================

	/** @brief Maximum multiplier for seed search iterations in CHD construction. */
	inline constexpr uint32_t MAX_SEED_SEARCH_MULTIPLIER = 100;
}
