/**
 * @file StringSet.h
 * @brief Enhanced unordered_set with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 */

#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>

#include "nfx/config.h"
#include "functors/StringFunctors.h"

namespace nfx::containers
{
	//=====================================================================
	// StringSet class
	//=====================================================================

	/**
	 * @brief Enhanced unordered set with full heterogeneous support for string types
	 */
	class StringSet final : public std::unordered_set<std::string, StringViewHash, StringViewEqual>
	{
		using Base = std::unordered_set<std::string, StringViewHash, StringViewEqual>;

	public:
		//----------------------------------------------
		// Inherited Constructors & Core Methods
		//----------------------------------------------

		/**
		 * @brief Inherits all std::unordered_set constructors
		 *
		 * Available constructors:
		 * - Default:           StringSet set;
		 * - Initializer list:  StringSet set{"key1", "key2", "key3"};
		 * - Range:             StringSet set(other.begin(), other.end());
		 * - Copy:              StringSet set2(set1);
		 * - Move:              StringSet set2(std::move(set1));
		 * - Bucket count:      StringSet set(100);  // Pre-allocate buckets
		 * - Custom allocator:  StringSet set(alloc);
		 */
		using Base::Base;

		/**
		 * @brief Inherited insert for std::string keys
		 * @note Use heterogeneous overloads below for zero-copy operations with other string types
		 */
		using Base::insert;

		//----------------------------------------------
		// Heterogeneous insert overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous insert for const char*
		 * @param key C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert( const char* key );

		/**
		 * @brief Heterogeneous insert for char*
		 * @param key Mutable C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert( char* key );

		/**
		 * @brief Heterogeneous insert for string_view
		 * @param key String view key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert( std::string_view key );

		//----------------------------------------------
		// Heterogeneous emplace overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous emplace for const char*
		 * @param key C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> emplace( const char* key );

		/**
		 * @brief Heterogeneous emplace for char*
		 * @param key Mutable C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> emplace( char* key );

		/**
		 * @brief Heterogeneous emplace for string_view
		 * @param key String view key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> emplace( std::string_view key );

		//----------------------------------------------
		// C++20-style contains() method
		//----------------------------------------------

		/**
		 * @brief Check if set contains key (const char*)
		 * @param key C-string key
		 * @return True if key exists
		 */
		NFX_CORE_INLINE bool contains( const char* key ) const noexcept;

		/**
		 * @brief Check if set contains key (char*)
		 * @param key Mutable C-string key
		 * @return True if key exists
		 */
		NFX_CORE_INLINE bool contains( char* key ) const noexcept;

		/**
		 * @brief Check if set contains key (std::string)
		 * @param key String key
		 * @return True if key exists
		 */
		NFX_CORE_INLINE bool contains( const std::string& key ) const noexcept;

		/**
		 * @brief Check if set contains key (string_view)
		 * @param key String view key
		 * @return True if key exists
		 */
		NFX_CORE_INLINE bool contains( std::string_view key ) const noexcept;
	};
}

#include "StringSet.inl"
