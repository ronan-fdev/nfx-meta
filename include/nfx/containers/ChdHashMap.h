/**
 * @file ChdHashMap.h
 * @brief Perfect hashing dictionary using the CHD algorithm for nfx library
 *
 * @details This implementation is derived from Vista.SDK's C# ChdDictionary,
 *          adapted to C++ with cross-platform compatibility maintained.
 *
 * **Original Source:**
 * - Vista.SDK C# implementation: Vista.SDK.Internal.ChdDictionary<T>
 * - Author: Vista SDK Team
 * - Repository: https://github.com/dnv-opensource/vista-sdk
 * - License: MIT License
 * - Copyright (c) 2024 DNV
 *
 * **Key Adaptations for C++:**
 * - STL container integration (std::vector, std::pair)
 * - Iterator and range-based for loop support
 * - Exception safety and RAII principles
 * - Template-based generic implementation
 * - Cross-platform SSE4.2 detection
 *
 * **License Compliance:**
 * This C++ adaptation is derivative work based on Vista.SDK's CHD implementation.
 * Original MIT License terms apply to the algorithmic foundation and design patterns.
 * C++ implementation additions and optimizations are part of nfx-meta.
 *
 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
 * @see Vista.SDK.Internal.ChdDictionary (original C# implementation)
 * @see https://github.com/dnv-opensource/vista-sdk/blob/main/LICENSE (MIT License)
 *
 * ## Memory Layout & CHD Algorithm Structure:
 *
 * ```
 * ChdHashMap Internal Structure:
 * ┌─────────────────────────────────────────────────────────────┐
 * │                      ChdHashMap<TValue>                     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ ┌─────────────────────────────────────────────────────────┐ │
 * │ │                       m_table                           │ │ ← Primary storage
 * │ │                std::vector<std::pair>                   │ │
 * │ │ ┌─────────────────────────────────────────────────────┐ │ │
 * │ │ │           [0] │ "key1"     │ value1     │           │ │ │ ← Key-value pairs
 * │ │ │           [1] │ "key2"     │ value2     │           │ │ │
 * │ │ │           [2] │ "key3"     │ value3     │           │ │ │
 * │ │ │           ... │ ...        │ ...        │           │ │ │
 * │ │ │           [n] │ "keyN"     │ valueN     │           │ │ │
 * │ │ └─────────────────────────────────────────────────────┘ │ │
 * │ └─────────────────────────────────────────────────────────┘ │
 * │ ┌─────────────────────────────────────────────────────────┐ │
 * │ │                      m_seeds                            │ │ ← CHD seeds
 * │ │                  std::vector<int>                       │ │
 * │ │ ┌─────────────────────────────────────────────────────┐ │ │
 * │ │ │           [0] │ seed_0  │ ← Maps to table[0]        │ │ │
 * │ │ │           [1] │ seed_1  │ ← Maps to table[1]        │ │ │
 * │ │ │           [2] │ seed_2  │ ← Maps to table[2]        │ │ │
 * │ │ │           ... │ ...     │                           │ │ │
 * │ │ │           [n] │ seed_n  │ ← Maps to table[n]        │ │ │
 * │ │ └─────────────────────────────────────────────────────┘ │ │
 * │ └─────────────────────────────────────────────────────────┘ │
 * └─────────────────────────────────────────────────────────────┘
 *                              ↓
 *                   Perfect Hash Lookup Process
 *                              ↓
 * ┌─────────────────────────────────────────────────────────────┐
 * │                  CHD Hash Resolution                        │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Input: "search_key"                                        │
 * │                            ↓                                │
 * │  1. Primary Hash: hash = <CRC32 || FNV-1a>(key, FnvOffset)  │
 * │                            ↓                                │
 * │  2. Index Mapping: idx = hash & (size - 1)                  │
 * │                            ↓                                │
 * │  3. Seed Mixing: final = seedMix(seeds[idx], hash, size)    │
 * │                            ↓                                │
 * │  4. Direct Access: return table[final]                      │
 * │                            ↓                                │
 * │  Result: O(1) guaranteed lookup with zero collisions        │
 * └─────────────────────────────────────────────────────────────┘
 * ```
 *
 * ## CHD Construction Algorithm:
 *
 * ```
 * Construction Phase (One-time):
 * ┌─────────────────────────────────────────────────────────────┐
 * │                CHD Perfect Hash Construction                │
 * ├─────────────────────────────────────────────────────────────┤
 * │  Input: vector<pair<string, TValue>>                        │
 * │                            ↓                                │
 * │  1. Hash all keys with CRC32 || FNV-1a                      │
 * │  2. Group keys by hash collision buckets                    │
 * │  3. For each bucket with collisions:                        │
 * │     - Search for seed value (up to MAX_SEED_SEARCH × size)  │
 * │     - Seed creates collision-free sub-mapping               │
 * │  4. Store seeds aligned with final table positions          │
 * │  5. Verify: All keys map to unique table positions          │
 * │                            ↓                                │
 * │  Result: Perfect hash function with zero collisions         │
 * └─────────────────────────────────────────────────────────────┘
 * ```
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "nfx/config.h"
#include "nfx/core/Hashing.h"

namespace nfx::containers
{
	//=====================================================================
	// ChdHashMap class
	//=====================================================================

	/**
	 * @class ChdHashMap
	 * @brief A read-only dictionary using the Compress, Hash, and Displace (CHD)
	 * perfect hashing algorithm for guaranteed O(1) worst-case lookups after construction.
	 *
	 * @details Provides O(1) expected lookup time with minimal memory overhead for essentially read-only
	 * dictionaries. It uses a two-level perfect hashing scheme based on the CHD algorithm
	 * by Botelho, Pagh, and Ziviani, ensuring no collisions for the stored keys.
	 * This implementation is suitable for scenarios where a fixed set of key-value pairs
	 * needs to be queried frequently and efficiently. It includes optimizations like
	 * optional SSE4.2 hashing and configurable FNV-1a hash constants for consistent
	 * hashing across different components and external projects.
	 *
	 * @tparam TValue The type of values stored in the dictionary.
	 * @tparam FnvOffsetBasis FNV-1a offset basis constant for hash calculation (default: 0x811C9DC5)
	 * @tparam FnvPrime FNV-1a prime constant for hash calculation (default: 0x01000193)
	 *
	 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
	 */
	template <typename TValue,
		uint32_t FnvOffsetBasis = core::hashing::constants::DEFAULT_FNV_OFFSET_BASIS,
		uint32_t FnvPrime = core::hashing::constants::DEFAULT_FNV_PRIME>
	class ChdHashMap final
	{
	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Iterator;
		class Enumerator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs the dictionary from a vector of key-value pairs.
		 * @param[in] items A vector of key-value pairs. The keys must be unique.
		 * @param[in] maxSeedSearchMultiplier Maximum multiplier for seed search iterations in CHD construction (default: 100).
		 * @throws std::invalid_argument if duplicate keys are found.
		 * @throws std::runtime_error if perfect hash construction fails.
		 *
		 * @todo Consider implementing auto-adaptive seed search multiplier:
		 *       - Progressive approach: try multipliers [50, 100, 200, 500, 1000] until success
		 *       - Key pattern analysis: analyze collision rate and key similarity to predict optimal multiplier
		 *       - Would improve UX by eliminating need for manual tuning in edge cases
		 *       - Could add overload: ChdHashMap(items) for auto-adaptive, ChdHashMap(items, multiplier) for explicit control
		 */
		inline explicit ChdHashMap( std::vector<std::pair<std::string, TValue>>&& items, uint32_t maxSeedSearchMultiplier = 100 );

		/**
		 * @brief Default constructor - creates an empty ChdHashMap
		 * @details Creates an empty dictionary with no elements. While ChdHashMap is immutable after
		 *          construction with data, this default constructor is required for JSON deserialization
		 *          (creates empty object then deserializes into it).
		 */
		ChdHashMap() = default;

		/**
		 * @brief Copy constructor
		 * @param[in] other The other ChdHashMap to copy from
		 */
		ChdHashMap( const ChdHashMap& other ) = default;

		/**
		 * @brief Move constructor
		 * @param[in] other The other ChdHashMap to move from
		 */
		ChdHashMap( ChdHashMap&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ChdHashMap() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param[in] other The other ChdHashMap to copy from
		 * @return Reference to this ChdHashMap after assignment
		 */
		ChdHashMap& operator=( const ChdHashMap& other ) = default;

		/**
		 * @brief Move assignment operator
		 * @param[in] other The other ChdHashMap to move from
		 * @return Reference to this ChdHashMap after assignment
		 */
		ChdHashMap& operator=( ChdHashMap&& other ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param[in] other The other ChdHashMap to compare against
		 * @return `true` if both ChdHashMaps contain the same key-value pairs, `false` otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const ChdHashMap& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param[in] other The other ChdHashMap to compare against
		 * @return `true` if the ChdHashMaps contain different key-value pairs, `false` otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const ChdHashMap& other ) const noexcept;

		//----------------------------------------------
		// Lookup operators
		//----------------------------------------------

		/**
		 * @brief Accesses the value associated with the specified key (non-const version).
		 * @details Provides read-write access to the value. Performs a lookup using the perfect hash function.
		 *          Allows modification of the retrieved value if TValue is mutable.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @return A reference to the value associated with `key`.
		 * @throws KeyNotFoundException if the `key` is not found in the dictionary or if the dictionary is empty.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_META_INLINE TValue& operator[]( std::string_view key );

		//----------------------------------------------
		// Lookup methods
		//----------------------------------------------

		/**
		 * @brief Accesses the value associated with the specified key with bounds checking.
		 * @details Provides read-only access to the value. Performs a lookup using the perfect hash function.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @return A constant reference to the value associated with `key`.
		 * @throws KeyNotFoundException if the `key` is not found in the dictionary or if the dictionary is empty.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const TValue& at( std::string_view key );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Returns the number of elements in the dictionary.
		 * @return The number of key-value pairs stored in the dictionary.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline size_t size() const noexcept;

		/**
		 * @brief Returns the maximum seed search multiplier used during CHD construction.
		 * @return The multiplier value that determines the seed search threshold (size × multiplier).
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline uint32_t maxSeedSearchMultiplier() const noexcept;

		//----------------------------------------------
		// State inspection methods
		//----------------------------------------------

		/**
		 * @brief Checks if the dictionary is empty.
		 * @return `true` if the dictionary contains no elements, `false` otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isEmpty() const noexcept;

		//----------------------------------------------
		// Static query methods
		//----------------------------------------------

		/**
		 * @brief Attempts to retrieve the value associated with the specified key without throwing exceptions.
		 * @details Performs a lookup using the perfect hash function. If the key is found, the output
		 *          parameter `outValue` is updated to point to the associated value within the dictionary's
		 *          internal storage, allowing in-place modification of the retrieved value.
		 * @param[in] key The key whose associated value is to be retrieved.
		 * @param[out] outValue A reference to a mutable pointer to TValue. On success, this pointer will be
		 *                      set to the address of the found value, enabling direct modification. On failure, it will be set to `nullptr`.
		 * @return `true` if the `key` was found and `outValue` was updated, `false` otherwise.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_META_INLINE bool tryGetValue( std::string_view key, TValue*& outValue ) noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Gets an iterator pointing to the first element of the dictionary.
		 * @return An `Iterator` positioned at the beginning of the dictionary's data.
		 *         If the dictionary is empty, this will be equal to `end()`.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator begin() const noexcept;

		/**
		 * @brief Gets an iterator pointing past the last element of the dictionary.
		 * @return An `Iterator` representing the position after the last element.
		 *         This iterator should not be dereferenced.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Iterator end() const noexcept;

		//----------------------------------------------
		// Enumeration
		//----------------------------------------------

		/**
		 * @brief Gets an enumerator for the dictionary key-value pairs.
		 * @return An enumerator for iterating through the key-value pairs.
		 * @details The enumerator starts positioned before the first element.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Enumerator enumerator() const noexcept;

		//---------------------------
		// Hashing
		//---------------------------

		/**
		 * @brief Calculates hash value using hardware-accelerated CRC32 or software FNV-1a fallback.
		 * @details Two-path implementation optimized for performance:
		 *   - **SSE4.2 Path:** Uses hardware CRC32 instruction (_mm_crc32_u8) for maximum speed
		 *   - **Fallback Path:** Uses FNV-1a algorithm for universal compatibility
		 *
		 * @param[in] key ASCII string key to hash
		 * @return 32-bit hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static NFX_META_INLINE uint32_t hash( std::string_view key ) noexcept;

		//----------------------------------------------
		// Exception classes
		//----------------------------------------------

		//----------------------------
		// ChdHashMap::KeyNotFoundException
		//----------------------------

		/**
		 * @brief Exception thrown when a requested key is not found in the dictionary.
		 * @details This exception is thrown by operator[] and at() methods when
		 *          the specified key does not exist in the dictionary.
		 */
		class KeyNotFoundException : public std::runtime_error
		{
		public:
			/**
			 * @brief Constructs a key not found exception.
			 * @param[in] key The key that was not found.
			 */
			inline explicit KeyNotFoundException( std::string_view key );
		};

		//----------------------------
		// ChdHashMap::InvalidOperationException
		//----------------------------

		/**
		 * @brief Exception thrown when an operation is not valid due to the current state.
		 * @details This exception is thrown by enumerator operations when the enumerator
		 *          is not positioned on a valid element.
		 */
		class InvalidOperationException : public std::runtime_error
		{
		public:
			/**
			 * @brief Constructs an invalid operation exception with default message.
			 */
			inline InvalidOperationException();

			/**
			 * @brief Constructs an invalid operation exception with custom message.
			 * @param[in] message The exception message.
			 */
			inline explicit InvalidOperationException( std::string_view message );
		};

		//----------------------------------------------
		// ChdHashMap::Iterator class
		//----------------------------------------------

		/**
		 * @class Iterator
		 * @brief Enables iteration over dictionary key-value pairs.
		 *
		 * @details Provides sequential access to all stored elements with support for:
		 *          - Range-based for loops
		 *          - STL algorithms (std::find_if, std::for_each, etc.)
		 *          - Manual iteration with ++, *, and -> operators
		 *
		 * @code{.cpp}
		 * ChdHashMap<int> dict(items);
		 *
		 * // Range-based for loop (recommended)
		 * for (const auto& [key, value] : dict) {
		 *     std::cout << key << " = " << value << std::endl;
		 * }
		 *
		 * // STL algorithm usage
		 * auto found = std::find_if(dict.begin(), dict.end(),
		 *     [](const auto& pair) { return pair.second > 100; });
		 *
		 * // Manual iteration
		 * for (auto it = dict.begin(); it != dict.end(); ++it) {
		 *     std::cout << it->first << " = " << it->second << std::endl;
		 * }
		 * @endcode
		 */
		class Iterator final
		{
		public:
			//----------------------------
			// STL iterator traits
			//----------------------------

			/** @brief STL iterator category - indicates this is a forward iterator */
			using iterator_category = std::forward_iterator_tag;

			/** @brief STL value type - the type of object pointed to by the iterator */
			using value_type = std::pair<std::string, TValue>;

			/** @brief STL difference type - the type for representing iterator distances */
			using difference_type = std::ptrdiff_t;

			/** @brief STL pointer type - the type of pointer to the value */
			using pointer = const value_type*;

			/** @brief STL reference type - the type of reference to the value */
			using reference = const value_type&;

			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an iterator pointing to a specific element in the dictionary's table.
			 * @param[in] table Pointer to the dictionary's internal storage vector. Must not be null.
			 * @param[in] index The index within the table this iterator should point to.
			 * @note If index >= table->size(), the iterator represents an end iterator.
			 */
			inline explicit Iterator( const std::vector<std::pair<std::string, TValue>>* table, size_t index ) noexcept;

			/** @brief Default constructor */
			Iterator() = default;

			/** @brief Copy constructor */
			Iterator( const Iterator& ) = default;

			/** @brief Move constructor */
			Iterator( Iterator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Iterator() = default;

			//----------------------------
			// Assignment
			//----------------------------

			/**
			 * @brief Copy assignment operator
			 * @param[in] other The other Iterator to copy from
			 * @return Reference to this Iterator after assignment
			 */
			Iterator& operator=( const Iterator& other ) = default;

			/**
			 * @brief Move assignment operator
			 * @param[in] other The other Iterator to move from
			 * @return Reference to this Iterator after assignment
			 */
			Iterator& operator=( Iterator&& other ) noexcept = default;

			//---------------------------
			// Operations
			//---------------------------

			/**
			 * @brief Dereferences the iterator to access the current key-value pair.
			 * @return A constant reference to the `std::pair<std::string, TValue>` at the current position.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>& operator*() const;

			/**
			 * @brief Provides member access to the current key-value pair.
			 * @return A constant pointer to the `std::pair<std::string, TValue>` at the current position.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>* operator->() const;

			/**
			 * @brief Advances the iterator to the next element (pre-increment).
			 * @return A reference to this iterator after advancing.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			inline Iterator& operator++() noexcept;

			/**
			 * @brief Advances the iterator to the next element (post-increment).
			 * @return A copy of the iterator *before* it was advanced.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline Iterator operator++( int ) noexcept;

			//---------------------------
			// Comparison
			//---------------------------

			/**
			 * @brief Checks if this iterator is equal to another iterator.
			 * @param[in] other The iterator to compare against.
			 * @return `true` if both iterators point to the same element or are both end iterators for the same container, `false` otherwise.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool operator==( const Iterator& other ) const noexcept;

			/**
			 * @brief Checks if this iterator is not equal to another iterator.
			 * @param[in] other The iterator to compare against.
			 * @return `true` if the iterators point to different elements, `false` otherwise.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool operator!=( const Iterator& other ) const noexcept;

		private:
			//---------------------------
			// Private member variables
			//---------------------------

			/** @brief Pointer to the dictionary's internal data table. Null for default-constructed iterators. */
			const std::vector<std::pair<std::string, TValue>>* m_table = nullptr;

			/** @brief Current index within the `m_table`. */
			size_t m_index = 0;
		};

		//----------------------------------------------
		// ChdHashMap::Enumerator class
		//----------------------------------------------

		/**
		 * @class Enumerator
		 * @brief Provides explicit enumeration over dictionary key-value pairs.
		 *
		 * @details Maintains internal position state with these methods:
		 *          - `next()` - Advances to next element, returns true if successful
		 *          - `current()` - Gets the current element
		 *          - `reset()` - Returns to initial position
		 *
		 *          Must call `next()` before first `current()` access.
		 *
		 * @code{.cpp}
		 * auto enumerator = dict.enumerator();
		 *
		 * // Enumerate all elements
		 * while (enumerator.next()) {
		 *     const auto& [key, value] = enumerator.current();
		 *     std::cout << key << " = " << value << std::endl;
		 * }
		 *
		 * // Reset and enumerate with condition
		 * enumerator.reset();
		 * while (enumerator.next()) {
		 *     if (enumerator.current().second > 100) {
		 *         processHighValue(enumerator.current());
		 *     }
		 * }
		 * @endcode
		 */
		class Enumerator final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/**
			 * @brief Constructs an enumerator for the given dictionary table.
			 * @param table Pointer to the dictionary's internal storage vector.
			 */
			explicit Enumerator( const std::vector<std::pair<std::string, TValue>>* table ) noexcept;

			/** @brief Default constructor */
			Enumerator() = delete;

			/** @brief Copy constructor */
			Enumerator( const Enumerator& ) = default;

			/** @brief Move constructor */
			Enumerator( Enumerator&& ) noexcept = default;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~Enumerator() = default;

			//----------------------------
			// Assignment
			//----------------------------

			/**
			 * @brief Copy assignment operator
			 * @param[in] other The other Enumerator to copy from
			 * @return Reference to this Enumerator after assignment
			 */
			Enumerator& operator=( const Enumerator& other ) = default;

			/**
			 * @brief Move assignment operator
			 * @param[in] other The other Enumerator to move from
			 * @return Reference to this Enumerator after assignment
			 */
			Enumerator& operator=( Enumerator&& other ) noexcept = default;

			//----------------------------
			// Enumeration
			//----------------------------

			/**
			 * @brief Advances the enumerator to the next element.
			 * @return True if the enumerator successfully moved to the next element;
			 *         false if the enumerator has passed the end of the collection.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool next() noexcept;

			/**
			 * @brief Gets the current key-value pair from the enumerator.
			 * @details Returns the element that the enumerator is currently positioned on.
			 *          The enumerator must be positioned on a valid element by calling `next()`
			 *          and ensuring it returned `true`.
			 * @return A constant reference to the current `std::pair<std::string, TValue>`.
			 * @throws InvalidOperationException if the enumerator is not positioned on a valid element.
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::pair<std::string, TValue>& current() const;

			/** @brief Resets the enumerator to its initial position. */
			inline void reset() noexcept;

		private:
			//----------------------------
			// Private member variables
			//----------------------------

			/** @brief Pointer to the dictionary's internal data table. */
			const std::vector<std::pair<std::string, TValue>>* m_table = nullptr;

			/** @brief Current index within the table.*/
			size_t m_index;
		};

	private:
		//----------------------------------------------
		// ChdHashMap::ThrowHelper
		//----------------------------------------------

		/**
		 * @brief Internal exception throwing utility for ChdHashMap.
		 * @details Provides centralized exception handling with consistent error messages.
		 *          This class is implementation-specific and not intended for external use.
		 */
		class ThrowHelper final
		{
		public:
			//----------------------------
			// Construction (deleted)
			//----------------------------

			ThrowHelper() = delete;
			ThrowHelper( const ThrowHelper& ) = delete;
			ThrowHelper( ThrowHelper&& ) noexcept = delete;
			ThrowHelper& operator=( const ThrowHelper& ) = delete;
			ThrowHelper& operator=( ThrowHelper&& ) noexcept = delete;
			~ThrowHelper() = delete;

			//----------------------------
			// Static exception methods
			//----------------------------

			/**
			 * @brief Throws a key not found exception with the specified key.
			 * @param[in] key The key that was not found.
			 * @throws KeyNotFoundException Always.
			 */
			[[noreturn]] inline static void throwKeyNotFoundException( std::string_view key );

			/**
			 * @brief Throws an invalid operation exception.
			 * @throws InvalidOperationException Always.
			 */
			[[noreturn]] inline static void throwInvalidOperationException();
		};

		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Maximum multiplier for seed search iterations in CHD construction. */
		uint32_t m_maxSeedSearchMultiplier;

		/** @brief The primary storage table containing the key-value pairs. Order determined during construction. */
		std::vector<std::pair<std::string, TValue>> m_table;

		/** @brief The seed values used by the CHD perfect hash function to resolve hash collisions. Size matches `m_table`. */
		std::vector<int> m_seeds;
	};
} // namespace nfx::containers

#include "nfx/detail/containers/ChdHashMap.inl"
