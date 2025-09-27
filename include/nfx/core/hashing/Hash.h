/**
 * @file Hash.h
 * @brief Common hash algorithm constants for containers
 * @details FNV hash constants and other hashing parameters
 *          used across multiple container implementations
 */

#pragma once

#include <cstdint>
#include <string_view>

#include "nfx/config.h"

namespace nfx::core::hashing
{
	//=====================================================================
	// Hash algorithm constants
	//=====================================================================

	//----------------------------------------------
	// FNV-1a Hash Algorithm Constants
	//----------------------------------------------

	/** @brief FNV-1a 32-bit offset basis constant. */
	inline constexpr uint32_t DEFAULT_FNV_OFFSET_BASIS{ 0x811C9DC5 }; // Fowler-Noll-Vo algorithm

	/** @brief FNV-1a 32-bit prime constant. */
	inline constexpr uint32_t DEFAULT_FNV_PRIME{ 0x01000193 }; // Fowler-Noll-Vo algorithm

	//----------------------------------------------
	// Integer Hashing Constants
	//----------------------------------------------

	/** @brief Integer hash constant for 32-bit values. */
	inline constexpr uint32_t DEFAULT_INTEGER_HASH_32{ 0x45d9f3b }; // Donald Knuth, TAOCP Vol 3

	/** @brief Integer hash constant #1 for 64-bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_INTEGER_HASH_64_C1{ 0xbf58476d1ce4e5b9ull }; // Thomas Wang (2007)

	/** @brief Integer hash constant #2 for 64-bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_INTEGER_HASH_64_C2{ 0x94d049bb133111ebull }; // Thomas Wang (2007)

	//----------------------------------------------
	// Generic Hash Mixing Constants
	//----------------------------------------------

	/** @brief Generic 64-bit hash constant for bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_HASH_MIX_64{ 0x2545F4914F6CDD1DUL }; // Universal hashing

	//=====================================================================
	// Hash infrastructure
	//=====================================================================

	//----------------------------------------------
	// CPU feature detection
	//----------------------------------------------

	/**
	 * @brief Gets the cached SSE4.2 support status.
	 * @details Checks CPU capabilities for SSE4.2 CRC32 instructions, which provide
	 *          3-5x faster hashing compared to software fallback. Result is cached
	 *          via static initialization for zero runtime overhead.
	 * @return `true` if SSE4.2 is supported, `false` otherwise.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool hasSSE42Support() noexcept;

	//----------------------------------------------
	// Low-level hash building blocks
	//----------------------------------------------

	/**
	 * @brief Larson multiplicative hash function: 37 * hash + ch
	 * @details Simple hash by Paul Larson, provided for benchmarking.
	 * @param hash Current hash value to update
	 * @param ch Character (byte) to incorporate into the hash
	 * @return Updated hash value after incorporating the character
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr uint32_t Larson( uint32_t hash, uint8_t ch ) noexcept;

	/**
	 * @brief Computes one step of the FNV-1a hash function.
	 * @param[in] hash The current hash value.
	 * @param[in] ch The character (byte) to incorporate into the hash.
	 * @return The updated hash value.
	 * @see https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <uint32_t FnvPrime>
	[[nodiscard]] NFX_CORE_INLINE constexpr uint32_t fnv1a( uint32_t hash, uint8_t ch ) noexcept;

	/**
	 * @brief Computes one step of the CRC32 hash function using SSE4.2 instructions.
	 * @param[in] hash The current hash value.
	 * @param[in] ch The character (byte) to incorporate into the hash.
	 * @return The updated hash value.
	 * @note Requires SSE4.2 support. Use hasSSE42Support() to check availability.
	 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE uint32_t crc32( uint32_t hash, uint8_t ch ) noexcept;

	/**
	 * @brief Computes the final table index using the seed mixing function.
	 * @param[in] seed The seed value associated with the hash bucket.
	 * @param[in] hash The 32-bit hash value of the key.
	 * @param[in] size The total size (capacity) of the dictionary's main table. Must be a power of 2.
	 * @return The final table index for the key (as size_t).
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr uint32_t seedMix( uint32_t seed, uint32_t hash, size_t size ) noexcept;

	//----------------------------------------------
	// High-level complete hash operations
	//----------------------------------------------

	//----------------------------
	// String hashing
	//----------------------------

	/**
	 * @brief String hashing using SSE4.2/FNV-1a implementation
	 * @param key String to hash
	 * @return 32-bit hash value with excellent distribution
	 * @details Uses hardware-accelerated CRC32 when available, falls back to FNV-1a
	 */
	template <uint32_t FnvOffsetBasis = DEFAULT_FNV_OFFSET_BASIS, uint32_t FnvPrime = DEFAULT_FNV_PRIME>
	[[nodiscard]] NFX_CORE_INLINE uint32_t hashStringView( std::string_view key ) noexcept;

	//----------------------------
	// Integer hashing
	//----------------------------

	/**
	 * @brief Fast integer hash with proper avalanche properties
	 * @param value Integer to hash
	 * @return Well-distributed hash value
	 * @details Uses multiplicative hashing with constants from Knuth's work
	 */
	template <typename T>
	[[nodiscard]] NFX_CORE_INLINE constexpr std::enable_if_t<std::is_integral_v<T>, size_t> hashInteger( T value ) noexcept;
}

#include "nfx/detail/core/hashing/Hash.inl"
