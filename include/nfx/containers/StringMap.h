/**
 * @file StringMap.h
 * @brief Enhanced unordered_map with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 */

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "nfx/config.h"
#include "StringFunctors.h"

namespace nfx::containers
{
	//=====================================================================
	// StringMap class
	//=====================================================================

	/**
	 * @brief Enhanced unordered map with full heterogeneous support
	 * @tparam T Value type
	 */
	template <typename T>
	class StringMap final : public std::unordered_map<std::string, T, StringViewHash, StringViewEqual>
	{
		using Base = std::unordered_map<std::string, T, StringViewHash, StringViewEqual>;

	public:
		using Base::Base;
		using Base::operator[];
		using Base::try_emplace;

		//----------------------------------------------
		// Heterogeneous operator[] overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous operator[] for const char*
		 * @param key C-string key
		 * @return Reference to the mapped value
		 */
		inline T& operator[]( const char* key );

		/**
		 * @brief Heterogeneous operator[] for string_view
		 * @param key String view key
		 * @return Reference to the mapped value
		 */
		inline T& operator[]( std::string_view key );

		//----------------------------------------------
		// Heterogeneous try_emplace overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous try_emplace for const char*
		 * @param key C-string key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		inline std::pair<typename Base::iterator, bool> try_emplace( const char* key, Args&&... args );

		/**
		 * @brief Heterogeneous try_emplace for string_view
		 * @param key String view key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		inline std::pair<typename Base::iterator, bool> try_emplace( std::string_view key, Args&&... args );
	};
}

#include "StringMap.inl"
