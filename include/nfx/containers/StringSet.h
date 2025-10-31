/**
 * @file StringSet.h
 * @brief Enhanced unordered_set with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 *
 * ## Memory Layout & std::unordered_set Structure:
 *
 * ```
 * StringSet Internal Structure:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                    StringSet Wrapper                        │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Base: std::unordered_set<std::string,                      │
 * │                           StringViewHash,                   │ ← Custom functors
 * │                           StringViewEqual>                  │
 * │ ┌─────────────────────────────────────────────────────────┐ │
 * │ │              Standard Hash Table Buckets                │ │ ← STL implementation
 * │ │ ┌─────────────────────────────────────────────────────┐ │ │
 * │ │ │ Bucket[0] → ["hello"] → ["world"] → next...         │ │ │ ← Chain/bucket structure
 * │ │ │ Bucket[1] → ["test"] → nullptr                      │ │ │
 * │ │ │ Bucket[2] → nullptr                                 │ │ │
 * │ │ │ Bucket[3] → ["data"] → ["value"] → next...          │ │ │
 * │ │ │ ...                                                 │ │ │
 * │ │ │ Bucket[n] → ["key"] → nullptr                       │ │ │
 * │ │ └─────────────────────────────────────────────────────┘ │ │
 * │ └─────────────────────────────────────────────────────────┘ │
 * └─────────────────────────────────────────────────────────────┘
 *                              ↓
 *                 Heterogeneous Lookup Process
 *                              ↓
 * ┌─────────────────────────────────────────────────────────────┐
 * │              Zero-Copy String Lookup Resolution             │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Input: string_view /const char* / char* key                │
 * │                            ↓                                │
 * │  1. Hash: StringViewHash{}(key) → std::hash<string_view>    │
 * │                            ↓                                │
 * │  2. Bucket: hash % bucket_count                             │
 * │                            ↓                                │
 * │  3. Chain Walk: for each string in bucket                   │
 * │     - Compare: StringViewEqual{}(stored_string, input_key)  │
 * │     - Zero-copy: No temporary string creation               │
 * │     - Match: Return iterator to stored string               │
 * │                            ↓                                │
 * │  4. Insert: Convert to std::string for storage              │
 * │                            ↓                                │
 * │  Result: O(1) average case, heterogeneous lookup support    │
 * └─────────────────────────────────────────────────────────────┘
 * ```
 *
 * ## Heterogeneous Set Operations:
 *
 * ```
 * Zero-Copy Set Operations:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                  StringSet Heterogeneous API                │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Key Types: const char*, char*, std::string_view            │
 * │                            ↓                                │
 * │  1. insert(): Add string with zero-copy duplicate check     │
 * │     - Lookup: Zero-copy hash and comparison                 │
 * │     - Insert: Convert to std::string only when new          │
 * │                            ↓                                │
 * │  2. contains(): C++20-style existence check                 │
 * │     - Lookup: No temporary allocation                       │
 * │     - Return: Boolean result without iterator overhead      │
 * │                            ↓                                │
 * │  3. emplace(): In-place construction with efficiency        │
 * │     - Check: Zero-copy existence test                       │
 * │     - Insert: std::string construction only on new keys     │
 * │                            ↓                                │
 * │  4. find(): STL iterator-based lookup (inherited)           │
 * │     - Access: Zero-copy heterogeneous search                │
 * │     - Return: Iterator for advanced operations              │
 * │                            ↓                                │
 * │  Result: Full std::unordered_set API + zero-copy lookups    │
 * └─────────────────────────────────────────────────────────────┘
 * ```
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
		NFX_META_INLINE std::pair<typename Base::iterator, bool> insert( const char* key );

		/**
		 * @brief Heterogeneous insert for char*
		 * @param key Mutable C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_META_INLINE std::pair<typename Base::iterator, bool> insert( char* key );

		/**
		 * @brief Heterogeneous insert for string_view
		 * @param key String view key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_META_INLINE std::pair<typename Base::iterator, bool> insert( std::string_view key );

		//----------------------------------------------
		// Heterogeneous emplace overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous emplace for const char*
		 * @param key C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_META_INLINE std::pair<typename Base::iterator, bool> emplace( const char* key );

		/**
		 * @brief Heterogeneous emplace for char*
		 * @param key Mutable C-string key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_META_INLINE std::pair<typename Base::iterator, bool> emplace( char* key );

		/**
		 * @brief Heterogeneous emplace for string_view
		 * @param key String view key
		 * @return Pair of iterator and bool indicating insertion
		 */
		NFX_META_INLINE std::pair<typename Base::iterator, bool> emplace( std::string_view key );

		//----------------------------------------------
		// C++20-style contains() method
		//----------------------------------------------

		/**
		 * @brief Check if set contains key (const char*)
		 * @param key C-string key
		 * @return True if key exists
		 */
		NFX_META_INLINE bool contains( const char* key ) const noexcept;

		/**
		 * @brief Check if set contains key (char*)
		 * @param key Mutable C-string key
		 * @return True if key exists
		 */
		NFX_META_INLINE bool contains( char* key ) const noexcept;

		/**
		 * @brief Check if set contains key (std::string)
		 * @param key String key
		 * @return True if key exists
		 */
		NFX_META_INLINE bool contains( const std::string& key ) const noexcept;

		/**
		 * @brief Check if set contains key (string_view)
		 * @param key String view key
		 * @return True if key exists
		 */
		NFX_META_INLINE bool contains( std::string_view key ) const noexcept;
	};
} // namespace nfx::containers

#include "nfx/detail/containers/StringSet.inl"
