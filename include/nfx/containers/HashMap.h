/**
 * @file HashMap.h
 * @brief Map with Robin Hood hashing and string optimization
 * @note Extensible design supports future enhancements for custom hash policies and additional transparent lookup types
 */

#pragma once

#include <cstdint>
#include <vector>

#include "functors/StringFunctors.h"
#include "functors/HashMapHashFunctor.h"
#include "StringMap.h"

#include "nfx/config.h"

namespace nfx::containers
{
	//=====================================================================
	// HashMap class
	//=====================================================================

	/**
	 * @brief Hash table with Robin Hood hashing and string optimizations
	 * @details Generic hash map using Robin Hood algorithm for bounded probe distances.
	 *          Automatically enables zero-copy heterogeneous lookups when using string keys.
	 *
	 * @tparam TKey Key type (automatically optimized for std::string/string_view)
	 * @tparam TValue Value type
	 *
	 * Features:
	 * - Robin Hood hashing for consistent performance
	 * - Zero-copy string_view lookups for string keys
	 * - Bounded probe distances for predictable cache behavior
	 * - Template specialization for optimal string handling
	 */
	template <typename TKey, typename TValue>
	class HashMap final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor with initial capacity of 32 elements
		 * @details Initializes hash table with power-of-2 capacity for optimal
		 *          bitwise operations and cache-friendly memory layout
		 */
		NFX_CORE_INLINE HashMap();

		/**
		 * @brief Constructor with specified initial capacity
		 * @param initialCapacity Minimum initial capacity (rounded up to power of 2)
		 * @details Capacity will be rounded up to next power of 2 for optimal
		 *          hash distribution and bitwise mask operations
		 */
		NFX_CORE_INLINE explicit HashMap( size_t initialCapacity );

		//----------------------------------------------
		// Core operations
		//----------------------------------------------

		/**
		 * @brief Fast lookup with heterogeneous key types
		 * @param key The key to search for
		 * @param outValue Reference to pointer that will be set to the found value (or nullptr if not found)
		 * @return true if the key was found, false otherwise
		 */
		template <typename KeyType = TKey>
		NFX_CORE_INLINE bool tryGetValue( const KeyType& key, TValue*& outValue ) noexcept;

		//----------------------------------------------
		// Insertion
		//----------------------------------------------

		/**
		 * @brief Insert or update a key-value pair (move semantics)
		 * @param key The key to insert or update
		 * @param value The value to associate with the key (moved)
		 * @details Uses perfect forwarding and Robin Hood hashing for optimal
		 *          performance. If key exists, value is updated; otherwise new entry created.
		 */
		NFX_CORE_INLINE void insertOrAssign( const TKey& key, TValue&& value );

		/**
		 * @brief Insert or update a key-value pair (copy semantics)
		 * @param key The key to insert or update
		 * @param value The value to associate with the key (copied)
		 * @details Uses Robin Hood displacement algorithm to minimize maximum
		 *          probe distance and ensure O(log n) worst-case performance.
		 */
		NFX_CORE_INLINE void insertOrAssign( const TKey& key, const TValue& value );

		//----------------------------------------------
		// Capacity and memory management
		//----------------------------------------------

		/**
		 * @brief Reserve capacity for at least the specified number of elements
		 * @param minCapacity Minimum capacity to reserve
		 * @details Resizes hash table to accommodate at least minCapacity elements
		 *          without triggering automatic resize. Capacity rounded to power of 2.
		 *          Rehashes all existing elements to new table layout.
		 */
		NFX_CORE_INLINE void reserve( size_t minCapacity );

		/**
		 * @brief Remove a key-value pair from the map
		 * @param key The key to remove (supports heterogeneous lookup)
		 * @return true if the key was found and removed, false otherwise
		 * @details Uses Robin Hood algorithm for efficient lookup, then backward
		 *          shift deletion to maintain compact representation without tombstones.
		 */
		template <typename KeyType = TKey>
		NFX_CORE_INLINE bool erase( const KeyType& key ) noexcept;

		//----------------------------------------------
		// State insspection
		//----------------------------------------------

		/**
		 * @brief Get the number of elements in the map
		 * @return Current number of key-value pairs stored
		 * @details O(1) operation, maintained incrementally during modifications
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t size() const noexcept;

		/**
		 * @brief Get the current capacity of the hash table
		 * @return Maximum elements before resize (always power of 2)
		 * @details Actual capacity for efficient bitwise operations and optimal
		 *          memory allocation patterns
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t capacity() const noexcept;

		/**
		 * @brief Check if the map contains no elements
		 * @return true if size() == 0, false otherwise
		 * @details Equivalent to size() == 0 but may be more expressive in code
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE bool isEmpty() const noexcept;

	private:
		//----------------------------------------------
		// Robin Hood Hashing bucket structure
		//----------------------------------------------

		/**
		 * @brief Bucket structure for Robin Hood hashing algorithm
		 * @details Each bucket stores key-value pair with cached hash and displacement
		 *          distance for optimal cache performance and probe sequence bounds
		 */
		struct Bucket
		{
			TKey key{};				  ///< The stored key
			TValue value{};			  ///< The associated value
			std::uint32_t hash{};	  ///< Cached hash value for fast comparison
			std::uint16_t distance{}; ///< Robin Hood displacement distance
			bool occupied{};		  ///< Bucket occupancy flag
		};

		/**
		 * @brief Initial hash table capacity (power of 2 for bitwise operations)
		 * @details 32 elements provides good balance between memory usage and
		 *          resize frequency for typical use cases
		 */
		static constexpr size_t INITIAL_CAPACITY = 32;

		/**
		 * @brief Load factor threshold as percentage (75%)
		 * @details Resize triggered when (size * 100) >= (capacity * 75)
		 *          Balances space efficiency with probe sequence performance
		 */
		static constexpr size_t MAX_LOAD_FACTOR_PERCENT = 75;

		/**
		 * @brief Main bucket storage with contiguous memory layout
		 * @details Vector provides cache-friendly linear probing and automatic
		 *          memory management with strong exception safety guarantees
		 */
		std::vector<Bucket> m_buckets;

		size_t m_size{};					   ///< Current number of elements
		size_t m_capacity{ INITIAL_CAPACITY }; ///< Current hash table capacity
		size_t m_mask{ INITIAL_CAPACITY - 1 }; ///< Bitwise mask for hash modulo

		/**
		 * @brief Hash function object with zero-space optimization
		 * @details Uses high-performance HashMapHash functor providing string hashing
		 *          and proper integer mixing for optimal Robin Hood performance.
		 *          Supports heterogeneous lookup while maintaining excellent hash distribution.
		 */
		NFX_CORE_NO_UNIQUE_ADDRESS HashMapHash m_hasher;

		//----------------------------------------------
		// Internal implementation
		//----------------------------------------------

		/**
		 * @brief Internal insert or assign implementation with perfect forwarding
		 * @tparam ValueType Deduced value type supporting move/copy semantics
		 * @param key The key to insert or update
		 * @param value The value to forward (preserves value category)
		 * @details Core Robin Hood hashing implementation with displacement algorithm.
		 *          Handles both insertion of new elements and updates of existing keys.
		 */
		template <typename ValueType>
		inline void insertOrAssignInternal( const TKey& key, ValueType&& value );

		/**
		 * @brief Check if resize is needed based on load factor
		 * @return true if current load exceeds MAX_LOAD_FACTOR_PERCENT threshold
		 * @details Uses integer arithmetic to avoid floating-point operations:
		 *          resize when (size * 100) >= (capacity * 75)
		 */
		inline bool shouldResize() const noexcept;

		/**
		 * @brief Resize hash table to double capacity and rehash all elements
		 * @details Doubles capacity, allocates new bucket array, and rehashes all
		 *          existing elements. Provides strong exception safety - original
		 *          data preserved if rehashing fails.
		 */
		NFX_CORE_INLINE void resize();

		/**
		 * @brief Erase element at specific position using backward shift deletion
		 * @param pos Position in bucket array to erase
		 * @details Implements Robin Hood backward shift to maintain compact
		 *          representation without tombstones. Adjusts displacement distances
		 *          of shifted elements to preserve algorithm invariants.
		 */
		NFX_CORE_INLINE void eraseAtPosition( size_t pos ) noexcept;

		/**
		 * @brief Compare keys with heterogeneous lookup support for string types
		 * @tparam KeyType1 First key type
		 * @tparam KeyType2 Second key type
		 * @param k1 First key to compare
		 * @param k2 Second key to compare
		 * @return true if keys are equal, false otherwise
		 * @details Uses compile-time type detection to enable zero-copy comparisons
		 *          between std::string, std::string_view, and const char* types.
		 */
		template <typename KeyType1, typename KeyType2>
		NFX_CORE_INLINE bool keysEqual( const KeyType1& k1, const KeyType2& k2 ) const noexcept;
	};
}

#include "nfx/detail/containers/HashMap.inl"
