/**
 * @file StringSet.h
 * @brief Heterogeneous string set container with zero-copy lookup optimization
 * @details Provides std::unordered_set with StringViewHash and StringViewEqual functors,
 *          enabling zero-copy string_view lookups without temporary string allocations
 */

#pragma once

#include <string>
#include <unordered_set>

#include "StringFunctors.h"

namespace nfx::containers
{
	//=====================================================================
	// Type aliases for heterogeneous containers
	//=====================================================================

	/** @brief Unordered set with heterogeneous lookup support */
	using StringSet = std::unordered_set<std::string, StringViewHash, StringViewEqual>;
}
