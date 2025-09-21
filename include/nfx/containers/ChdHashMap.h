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
 * **Cross-Platform Hash Compatibility:**
 * Maintains identical hash values with C# version by matching UTF-16
 * byte processing pattern for ASCII strings.
 *
 * **License Compliance:**
 * This C++ adaptation is derivative work based on Vista.SDK's CHD implementation.
 * Original MIT License terms apply to the algorithmic foundation and design patterns.
 * C++ implementation additions and optimizations are part of nfx-core.
 *
 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
 * @see Vista.SDK.Internal.ChdDictionary (original C# implementation)
 * @see https://github.com/dnv-opensource/vista-sdk/blob/main/LICENSE (MIT License)
 */

/**
 * @note **Configuration:**
 * Algorithm parameters are currently configured via `nfx::containers::constants::chd`:
 * - FNV_OFFSET_BASIS and FNV_PRIME: FNV-1a hash constants
 * - MAX_SEED_SEARCH_MULTIPLIER: CHD collision resolution threshold
 *
 * @todo Future enhancement: Make configurable via ChdHashMapOption struct:
 * - Per-instance hash algorithm selection (SSE4.2/FNV-1a/custom)
 * - Custom FNV constants for specialized use cases
 * - Unicode normalization options (UTF-8/UTF-16 compatibility modes)
 * - Advanced CHD tuning parameters (load factor, seed search strategy)
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "nfx/config.h"

namespace nfx::containers
{
	namespace
	{
		//=====================================================================
		// CHD HashMap configuration
		//=====================================================================

		/** @brief Maximum multiplier for seed search iterations in CHD construction. */
		inline constexpr uint32_t MAX_SEED_SEARCH_MULTIPLIER = 100;

		//=====================================================================
		// Internal helper components
		//=====================================================================

		//----------------------------------------------
		// ThrowHelper class
		//----------------------------------------------

		class ThrowHelper final
		{
		public:
			//----------------------------
			// Construction
			//----------------------------

			/** @brief Default constructor. */
			ThrowHelper() = delete;

			/** @brief Copy constructor */
			ThrowHelper( const ThrowHelper& ) = delete;

			/** @brief Move constructor */
			ThrowHelper( ThrowHelper&& ) noexcept = delete;

			//----------------------------
			// Destruction
			//----------------------------

			/** @brief Destructor */
			~ThrowHelper() = delete;

			//----------------------------
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			ThrowHelper& operator=( const ThrowHelper& ) = delete;

			/** @brief Move assignment operator */
			ThrowHelper& operator=( ThrowHelper&& ) noexcept = delete;

			//----------------------------
			// Public static methods
			//----------------------------

			/**
			 * @brief Throws a key not found exception with the specified key.
			 * @param[in] key The key that was not found.
			 * @throws KeyNotFoundException Always.
			 */
			[[noreturn]] inline static void throwKeyNotFoundException( std::string_view key );

			/**
			 * @brief Throws an invalid operation exception.
			 * @details This exception is thrown when an operation is not valid due to
			 *          the current state of the object, such as accessing Current on
			 *          an enumerator that hasn't been positioned properly.
			 * @throws InvalidOperationException Always.
			 */
			[[noreturn]] inline static void throwInvalidOperationException();
		};
	}

	//======================================================================
	// Exception classes
	//======================================================================

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
	 * optional SSE4.2 hashing and thread-local caching.
	 *
	 * @tparam TValue The type of values stored in the dictionary.
	 *
	 * @warning **C# Cross-Platform Compatibility:**
	 * This C++ implementation is designed to be fully compatible with the C# version,
	 * producing identical hash values for ASCII strings. The compatibility is achieved by
	 * matching the C# byte processing pattern:
	 *
	 *   - **C# Behavior:** Processes UTF-16 strings by reading only the low byte of each
	 *     character (using `curr = ref Unsafe.Add(ref curr, 2)` to skip high bytes)
	 *   - **C++ Behavior:** Processes ASCII strings byte-by-byte, which produces identical
	 *     results since ASCII characters have zero high bytes in UTF-16 encoding
	 *
	 * This ensures **perfect hash compatibility** and allows dictionaries created by either
	 * implementation to be used interchangeably across platforms.
	 *
	 * @see https://en.wikipedia.org/wiki/Perfect_hash_function#CHD_algorithm
	 */
	template <typename TValue>
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
		 * @throws std::invalid_argument if duplicate keys are found.
		 * @throws std::runtime_error if perfect hash construction fails.
		 */
		inline explicit ChdHashMap( std::vector<std::pair<std::string, TValue>>&& items );

		/** @brief Default constructor */
		ChdHashMap() = default;

		/** @brief Copy constructor */
		ChdHashMap( const ChdHashMap& other ) = default;

		/** @brief Move constructor */
		ChdHashMap( ChdHashMap&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ChdHashMap() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ChdHashMap& operator=( const ChdHashMap& other ) = default;

		/** @brief Move assignment operator */
		ChdHashMap& operator=( ChdHashMap&& other ) noexcept = default;

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
		[[nodiscard]] NFX_CORE_INLINE TValue& operator[]( std::string_view key );

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
		[[nodiscard]] NFX_CORE_INLINE bool tryGetValue( std::string_view key, TValue*& outValue ) noexcept;

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
		 * **Cross-Platform Compatibility:**
		 * This implementation produces identical hash values to the C# version by matching
		 * its byte processing pattern. C# processes UTF-16 strings by reading only the
		 * low byte of each character (skipping high bytes). For ASCII strings, this is
		 * equivalent to processing each character directly since high bytes are zero.
		 *
		 * @param[in] key ASCII string key to hash
		 * @return 32-bit hash value compatible with C# implementation
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static NFX_CORE_INLINE uint32_t hash( std::string_view key ) noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief The primary storage table containing the key-value pairs. Order determined during construction. */
		std::vector<std::pair<std::string, TValue>> m_table;

		/** @brief The seed values used by the CHD perfect hash function to resolve hash collisions. Size matches `m_table`. */
		std::vector<int> m_seeds;

	public:
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

			using iterator_category = std::forward_iterator_tag;
			using value_type = std::pair<std::string, TValue>;
			using difference_type = std::ptrdiff_t;
			using pointer = const value_type*;
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
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Iterator& operator=( const Iterator& ) = default;

			/** @brief Move assignment operator */
			Iterator& operator=( Iterator&& ) noexcept = default;

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
			// Assignment operators
			//----------------------------

			/** @brief Copy assignment operator */
			Enumerator& operator=( const Enumerator& ) = default;

			/** @brief Move assignment operator */
			Enumerator& operator=( Enumerator&& ) noexcept = default;

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
	};
}

#include "ChdHashMap.inl"
