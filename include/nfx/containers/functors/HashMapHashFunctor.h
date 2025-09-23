/**
 * @file HashMapHashFunctor.h
 * @brief High-performance hash functor optimized for HashMap container
 * @details Provides hashing with SSE4.2/FNV-1a for strings and
 *          proper integer mixing, while maintaining STL compatibility
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include "nfx/config.h"

namespace nfx::containers
{
	//=====================================================================
	// HashMap-optimized hash functor
	//=====================================================================

	/**
	 * @brief High-performance hash functor for HashMap container
	 * @details Provides superior hash quality compared to std::hash while maintaining
	 *          transparent lookup support. Uses optimized algorithms for optimal
	 *          Robin Hood hashing performance.
	 *
	 * Features:
	 * - String hashing: SSE4.2 CRC32 + FNV-1a fallback for excellent distribution
	 * - Integer hashing: Multiplicative hashing with proper avalanche properties
	 * - Heterogeneous lookup: Supports string/string_view/const char*
	 * - Zero allocation: No temporary string creation during lookups
	 */
	struct HashMapHash final
	{
		/**
		 * @brief Enables heterogeneous lookup in HashMap
		 * @details Allows different but compatible key types without conversion
		 */
		using is_transparent = void;

		//----------------------------------------------
		// String type hashing
		//----------------------------------------------

		/**
		 * @brief Hash string_view using optimized algorithm
		 * @param sv String view to hash
		 * @return High-quality hash value with excellent distribution
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( std::string_view sv ) const noexcept;

		/**
		 * @brief Hash std::string using optimized algorithm
		 * @param s String to hash
		 * @return High-quality hash value with excellent distribution
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( const std::string& s ) const noexcept;

		/**
		 * @brief Hash C-string using optimized algorithm
		 * @param s C-string to hash
		 * @return High-quality hash value with excellent distribution
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( const char* s ) const noexcept;

		//----------------------------------------------
		// Integer type hashing (proper mixing)
		//----------------------------------------------

		/**
		 * @brief Hash integer with proper avalanche properties
		 * @tparam T Integer type
		 * @param value Integer value to hash
		 * @return Well-distributed hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] NFX_CORE_INLINE std::enable_if_t<std::is_integral_v<T>, size_t> operator()( T value ) const noexcept;

		//----------------------------------------------
		// Fallback for other types (delegate to std::hash)
		//----------------------------------------------

		/**
		 * @brief Fallback to std::hash for non-optimized types
		 * @tparam T Type to hash
		 * @param value Value to hash
		 * @return Standard library hash
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] NFX_CORE_INLINE std::enable_if_t<
			!std::is_integral_v<T> &&
				!std::is_same_v<T, std::string> &&
				!std::is_same_v<T, std::string_view> &&
				!std::is_same_v<T, const char*>,
			size_t>
		operator()( const T& value ) const noexcept;
	};
}

#include "nfx/detail/containers/functors/HashMapHashFunctor.inl"
