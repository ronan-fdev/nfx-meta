/**
 * @file StringMap.h
 * @brief Enhanced unordered_map with heterogeneous string lookup optimization
 * @details Provides zero-copy string_view lookups while maintaining std::string storage,
 *          eliminating temporary string allocations during key operations
 *
 * ## Memory Layout & std::unordered_map Structure:
 *
 * ```
 * StringMap Internal Structure:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                    StringMap<T> Wrapper                     │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Base: std::unordered_map<std::string, T,                   │
 * │                           StringViewHash,                   │ ← Custom functors
 * │                           StringViewEqual>                  │
 * │ ┌─────────────────────────────────────────────────────────┐ │
 * │ │              Standard Hash Table Buckets                │ │ ← STL implementation
 * │ │ ┌─────────────────────────────────────────────────────┐ │ │
 * │ │ │ Bucket[0] → [key: "hello"] → [value: T] → next...   │ │ │ ← Chain/bucket structure
 * │ │ │ Bucket[1] → [key: "world"] → [value: T] → nullptr   │ │ │
 * │ │ │ Bucket[2] → nullptr                                 │ │ │
 * │ │ │ Bucket[3] → [key: "test"] → [value: T] → next...    │ │ │
 * │ │ │ ...                                                 │ │ │
 * │ │ │ Bucket[n] → [key: "data"] → [value: T] → nullptr    │ │ │
 * │ │ └─────────────────────────────────────────────────────┘ │ │
 * │ └─────────────────────────────────────────────────────────┘ │
 * └─────────────────────────────────────────────────────────────┘
 *                              ↓
 *                   Heterogeneous Lookup Process
 *                              ↓
 * ┌─────────────────────────────────────────────────────────────┐
 * │              Zero-Copy String Lookup Resolution             │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Input: string_view / const char* / char* key               │
 * │                            ↓                                │
 * │  1. Hash: StringViewHash{}(key) → std::hash<string_view>    │
 * │                            ↓                                │
 * │  2. Bucket: hash % bucket_count                             │
 * │                            ↓                                │
 * │  3. Chain Walk: for each node in bucket                     │
 * │     - Compare: StringViewEqual{}(stored_key, input_key)     │
 * │     - Zero-copy: No temporary string creation               │
 * │     - Match: Return reference to stored value               │
 * │                            ↓                                │
 * │  4. Insert (operator[]): Convert to std::string for storage │
 * │                            ↓                                │
 * │  Result: O(1) average case, heterogeneous lookup support    │
 * └─────────────────────────────────────────────────────────────┘
 * ```
 */

#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "nfx/config.h"
#include "functors/StringFunctors.h"

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
		//----------------------------------------------
		// Inherited Constructors & Core Methods
		//----------------------------------------------

		/**
		 * @brief Inherits all std::unordered_map constructors
		 *
		 * Available constructors:
		 * - Default:           StringMap<int> map;
		 * - Initializer list:  StringMap<int> map{{"key1", 1}, {"key2", 2}};
		 * - Range:             StringMap<int> map(other.begin(), other.end());
		 * - Copy:              StringMap<int> map2(map1);
		 * - Move:              StringMap<int> map2(std::move(map1));
		 * - Bucket count:      StringMap<int> map(100);  // Pre-allocate buckets
		 * - Custom allocator:  StringMap<int> map(alloc);
		 *
		 * @note All constructors support automatic conversion from string-like types to std::string keys
		 */
		using Base::Base;

		/**
		 * @brief Inherited operator[] for std::string keys
		 * @note Use heterogeneous overloads below for zero-copy lookups with const char*, char*, string_view
		 */
		using Base::operator[];

		/**
		 * @brief Inherited try_emplace for std::string keys
		 * @note Use heterogeneous overloads below for zero-copy operations with other string types
		 */
		using Base::try_emplace;

		//----------------------------------------------
		// Heterogeneous operator[] overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous operator[] for const char*
		 * @param key C-string key
		 * @return Reference to the mapped value
		 */
		NFX_CORE_INLINE T& operator[]( const char* key ) noexcept;

		/**
		 * @brief Heterogeneous operator[] for char*
		 * @param key Mutable C-string key
		 * @return Reference to the mapped value
		 */
		NFX_CORE_INLINE T& operator[]( char* key ) noexcept;

		/**
		 * @brief Heterogeneous operator[] for string_view
		 * @param key String view key
		 * @return Reference to the mapped value
		 */
		NFX_CORE_INLINE T& operator[]( std::string_view key ) noexcept;

		//----------------------------------------------
		// Heterogeneous at() overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous at() for const char*
		 * @param key C-string key
		 * @return Const reference to the mapped value
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE const T& at( const char* key ) const;

		/**
		 * @brief Heterogeneous at() for const char* (non-const version)
		 * @param key C-string key
		 * @return Reference to the mapped value (read/write access)
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE T& at( const char* key );

		/**
		 * @brief Heterogeneous at() for char*
		 * @param key Mutable C-string key
		 * @return Const reference to the mapped value
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE const T& at( char* key ) const;

		/**
		 * @brief Heterogeneous at() for char* (non-const version)
		 * @param key Mutable C-string key
		 * @return Reference to the mapped value (read/write access)
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE T& at( char* key );

		/**
		 * @brief Heterogeneous at() for string_view
		 * @param key String view key
		 * @return Const reference to the mapped value
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE const T& at( std::string_view key ) const;

		/**
		 * @brief Heterogeneous at() for string_view (non-const version)
		 * @param key String view key
		 * @return Reference to the mapped value (read/write access)
		 * @throws std::out_of_range if key not found
		 */
		NFX_CORE_INLINE T& at( std::string_view key );

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
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> try_emplace( const char* key, Args&&... args ) noexcept(
			std::is_nothrow_constructible_v<T, Args...> );

		/**
		 * @brief Heterogeneous try_emplace for char*
		 * @param key Mutable C-string key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> try_emplace( char* key, Args&&... args ) noexcept(
			std::is_nothrow_constructible_v<T, Args...> );

		/**
		 * @brief Heterogeneous try_emplace for string_view
		 * @param key String view key
		 * @param args Arguments to construct the value
		 * @return Pair of iterator and bool indicating insertion
		 */
		template <typename... Args>
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> try_emplace( std::string_view key, Args&&... args ) noexcept(
			std::is_nothrow_constructible_v<T, Args...> );

		//----------------------------------------------
		// Heterogeneous insert_or_assign overloads
		//----------------------------------------------

		/**
		 * @brief Heterogeneous insert_or_assign for const char*
		 * @param key C-string key
		 * @param obj Value to insert or assign
		 * @return Pair of iterator and bool indicating insertion (true) or assignment (false)
		 */
		template <typename M>
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert_or_assign( const char* key, M&& obj ) noexcept(
			std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> );

		/**
		 * @brief Heterogeneous insert_or_assign for char*
		 * @param key Mutable C-string key
		 * @param obj Value to insert or assign
		 * @return Pair of iterator and bool indicating insertion (true) or assignment (false)
		 */
		template <typename M>
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert_or_assign( char* key, M&& obj ) noexcept(
			std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> );

		/**
		 * @brief Heterogeneous insert_or_assign for string_view
		 * @param key String view key
		 * @param obj Value to insert or assign
		 * @return Pair of iterator and bool indicating insertion (true) or assignment (false)
		 */
		template <typename M>
		NFX_CORE_INLINE std::pair<typename Base::iterator, bool> insert_or_assign( std::string_view key, M&& obj ) noexcept(
			std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> );
	};
}

#include "nfx/detail/containers/StringMap.inl"
