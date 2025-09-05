/**
 * @file Utils.h
 * @brief High-performance string utilities for NFX C++ library
 * @details Provides fast, zero-allocation string operations including validation,
 *          parsing, and comparison functions using std::string_view for maximum performance.
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "nfx/config.h"

namespace nfx::string
{
	//=====================================================================
	// String utilities
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	/**
	 * @brief Fast check if string has exact length
	 * @param str String to check
	 * @param expectedLength Expected length
	 * @return True if string has exact expected length
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool hasExactLength( std::string_view str, std::size_t expectedLength ) noexcept;

	/**
	 * @brief Fast check if string is empty
	 * @param str String to check
	 * @return True if string is empty
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool isEmpty( std::string_view str ) noexcept;

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	/**
	 * @brief Fast boolean parsing with error handling
	 * @param str String to parse (case-insensitive)
	 * @param result Output boolean value
	 * @return True if parsing succeeded, false otherwise
	 * @details Supports: "true"/"false", "1"/"0", "yes"/"no", "on"/"off", "t"/"f", "y"/"n"
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] inline bool tryParseBool( std::string_view str, bool& result ) noexcept;

	/**
	 * @brief Fast integer parsing with error handling
	 * @param str String to parse
	 * @param result Output integer value
	 * @return True if parsing succeeded, false otherwise
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool tryParseInt( std::string_view str, int& result ) noexcept;

	/**
	 * @brief Fast unsigned integer parsing with error handling
	 * @param str String to parse
	 * @param result Output unsigned integer value
	 * @return True if parsing succeeded, false otherwise
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool tryParseUInt( std::string_view str, std::uint32_t& result ) noexcept;

	/**
	 * @brief Fast long integer parsing with error handling
	 * @param str String to parse
	 * @param result Output long integer value
	 * @return True if parsing succeeded, false otherwise
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool tryParseLong( std::string_view str, std::int64_t& result ) noexcept;

	/**
	 * @brief Fast double parsing with error handling
	 * @param str String to parse
	 * @param result Output double value
	 * @return True if parsing succeeded, false otherwise
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool tryParseDouble( std::string_view str, double& result ) noexcept;

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	/**
	 * @brief Fast check if string ends with suffix
	 * @param str String to check
	 * @param suffix Suffix to find
	 * @return True if str ends with suffix
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool endsWith( std::string_view str, std::string_view suffix ) noexcept;

	/**
	 * @brief Fast check if string starts with prefix
	 * @param str String to check
	 * @param prefix Prefix to find
	 * @return True if str starts with prefix
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool startsWith( std::string_view str, std::string_view prefix ) noexcept;

	/**
	 * @brief Fast check if string contains substring
	 * @param str String to check
	 * @param substr Substring to find
	 * @return True if str contains substr
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool contains( std::string_view str, std::string_view substr ) noexcept;

	/**
	 * @brief Fast case-sensitive string comparison
	 * @param lhs First string
	 * @param rhs Second string
	 * @return True if strings are exactly equal
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr bool equals( std::string_view lhs, std::string_view rhs ) noexcept;

	/**
	 * @brief Fast case-insensitive string comparison
	 * @param lhs First string
	 * @param rhs Second string
	 * @return True if strings are equal (case-insensitive)
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE bool iequals( std::string_view lhs, std::string_view rhs ) noexcept;

	//----------------------------------------------
	// Case Conversion
	//----------------------------------------------

	/**
	 * @brief Convert ASCII character to lowercase
	 * @param c Character to convert
	 * @return Lowercase character if ASCII letter, otherwise unchanged
	 * @details Only works with ASCII characters (A-Z, a-z). Non-ASCII characters are returned unchanged.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr char toLower( char c ) noexcept;

	/**
	 * @brief Convert ASCII character to uppercase
	 * @param c Character to convert
	 * @return Uppercase character if ASCII letter, otherwise unchanged
	 * @details Only works with ASCII characters (A-Z, a-z). Non-ASCII characters are returned unchanged.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] NFX_CORE_INLINE constexpr char toUpper( char c ) noexcept;

	/**
	 * @brief Convert string to lowercase (allocating)
	 * @param str String to convert
	 * @return New string with all ASCII characters converted to lowercase
	 * @details Only ASCII characters (A-Z) are converted. Non-ASCII characters are preserved unchanged.
	 *          This function allocates a new std::string.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] inline std::string toLower( std::string_view str );

	/**
	 * @brief Convert string to uppercase (allocating)
	 * @param str String to convert
	 * @return New string with all ASCII characters converted to uppercase
	 * @details Only ASCII characters (a-z) are converted. Non-ASCII characters are preserved unchanged.
	 *          This function allocates a new std::string.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] inline std::string toUpper( std::string_view str );
}

#include "Utils.inl"
