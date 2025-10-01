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
	// FNV-1a 32-bit hash algorithm constants
	//----------------------------------------------

	/** @brief FNV-1a 32-bit offset basis constant. */
	inline constexpr uint32_t DEFAULT_FNV_OFFSET_BASIS{ 0x811C9DC5 }; // Fowler-Noll-Vo algorithm

	/** @brief FNV-1a 32-bit prime constant. */
	inline constexpr uint32_t DEFAULT_FNV_PRIME{ 0x01000193 }; // Fowler-Noll-Vo algorithm

	//----------------------------------------------
	// 64-bit generic hash mixing constants
	//----------------------------------------------

	/** @brief Generic 64-bit hash constant for bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_HASH_MIX_64{ 0x2545F4914F6CDD1DUL };

	//----------------------------------------------
	// Integer hashing constants
	//----------------------------------------------

	/** @brief Integer hash constant for 32-bit values. */
	inline constexpr uint32_t DEFAULT_INTEGER_HASH_32{ 0x45d9f3b }; // Donald Knuth, TAOCP Vol 3

	/** @brief Integer hash constant #1 for 64-bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_INTEGER_HASH_64_C1{ 0xbf58476d1ce4e5b9ull }; // Thomas Wang (2007)

	/** @brief Integer hash constant #2 for 64-bit avalanche mixing. */
	inline constexpr uint64_t DEFAULT_INTEGER_HASH_64_C2{ 0x94d049bb133111ebull }; // Thomas Wang (2007)

	//----------------------------------------------
	// 64-bit hashing constants
	//----------------------------------------------

	/** @brief Golden ratio constant for hash combining (φ = 2^64 / golden_ratio). */
	inline constexpr uint64_t DEFAULT_GOLDEN_RATIO_64{ 0x9e3779b97f4a7c15ULL };

	/** @brief MurmurHash3 64-bit avalanche constant #1. */
	inline constexpr uint64_t DEFAULT_MURMUR3_C1{ 0xff51afd7ed558ccdULL };

	/** @brief MurmurHash3 64-bit avalanche constant #2. */
	inline constexpr uint64_t DEFAULT_MURMUR3_C2{ 0xc4ceb9fe1a85ec53ULL };

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
	// Hash combination
	//----------------------------------------------

	/**
	 * @brief Combines two hash values using FNV-1a mixing.
	 * @param[in] existing The current accumulated 32-bit hash value.
	 * @param[in] newHash The new 32-bit hash value to combine.
	 * @param[in] prime The FNV prime constant for mixing.
	 * @details Uses XOR followed by multiplication for optimal bit mixing.
	 * @return Combined hash value with good distribution properties.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr uint32_t combine( uint32_t existing, uint32_t newHash, uint32_t prime ) noexcept;

	/**
	 * @brief Combines two 64-bit hash values using Boost hash_combine with MurmurHash3 finalizer
	 * @param[in] existing The current accumulated 64-bit hash value.
	 * @param[in] newHash The new 64-bit hash value to combine.
	 * @details Hybrid algorithm combining Boost's hash_combine formula with MurmurHash3's 64-bit finalizer.
	 *
	 *          **Phase 1 - Initial Mixing (Boost-style):**
	 *          - Uses golden ratio constant (φ = 0x9E3779B97F4A7C15) for uniform distribution
	 *          - Incorporates bit-shift mixing to prevent linear correlation between inputs
	 *
	 *          **Phase 2 - Avalanche Finalization (MurmurHash3):**
	 *          - Triple avalanche rounds ensure complete bit interdependency
	 *          - Uses proven MurmurHash3 constants for optimal statistical properties
	 *          - Guarantees that single-bit input changes affect ~50% of output bits
	 *
	 *          **Performance:** O(1) with ~6 operations, excellent for combining multiple hash values
	 *          in composite keys, tuples, or hash table chaining scenarios.
	 * @return Combined hash value with strong collision resistance and uniform distribution.
	 * @see https://github.com/aappleby/smhasher/wiki/MurmurHash3
	 * @see https://www.boost.org/doc/libs/1_89_0/boost/hash2/legacy/murmur3.hpp
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr size_t combine( size_t existing, size_t newHash ) noexcept;

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
	template <uint32_t FnvOffsetBasis, uint32_t FnvPrime>
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
