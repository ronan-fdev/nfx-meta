/**
 * @file StringFunctors.h
 * @brief Heterogeneous lookup functors for string-based containers
 * @details Provides transparent hash and equality functors enabling zero-copy
 *          string_view lookups in std::unordered_map and std::unordered_set
 */

#pragma once

#include <string>
#include <string_view>

#include "nfx/config.h"

namespace nfx::containers
{
	//=====================================================================
	// Heterogeneous lookup functors for zero-copy string operations
	//=====================================================================

	/**
	 * @brief Hash functor supporting both std::string and std::string_view
	 * @details Enables heterogeneous lookup in unordered containers,
	 *          allowing direct string_view lookups without string construction.
	 */

	//----------------------------------------------
	// StringViewHash struct
	//----------------------------------------------

	struct StringViewHash final
	{
		/**
		 * @brief Enables heterogeneous lookup in unordered containers
		 * @details This type alias allows the hash functor to work with different
		 *          but compatible key types (std::string, std::string_view, const char*)
		 *          without requiring conversion to the container's key type.
		 */
		using is_transparent = void;

		/**
		 * @brief Hash C-string via string_view conversion
		 * @param[in] s The C-string to hash
		 * @return Hash value for the string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( const char* s ) const noexcept;

		/**
		 * @brief Hash std::string via string_view conversion
		 * @param[in] s The std::string to hash
		 * @return Hash value for the string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( const std::string& s ) const noexcept;

		/**
		 * @brief Hash string_view directly
		 * @param[in] sv The string_view to hash
		 * @return Hash value for the string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE size_t operator()( std::string_view sv ) const noexcept;
	};

	//----------------------------------------------
	// StringViewEqual struct
	//----------------------------------------------

	/**
	 * @brief Equality functor supporting both std::string and std::string_view
	 * @details Enables heterogeneous lookup in unordered containers,
	 *          providing all comparison overloads for string types.
	 */
	struct StringViewEqual final
	{
		/**
		 * @brief Enables heterogeneous lookup in unordered containers
		 * @details This type alias allows the equality functor to work with different
		 *          but compatible key types (std::string, std::string_view, const char*)
		 *          without requiring conversion to the container's key type.
		 */
		using is_transparent = void;

		/**
		 * @brief Compare two C-strings
		 * @param[in] lhs Left-hand side C-string to compare
		 * @param[in] rhs Right-hand side C-string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE constexpr bool operator()( const char* lhs, const char* rhs ) const noexcept;

		/**
		 * @brief Compare two std::string objects
		 * @param[in] lhs Left-hand side std::string to compare
		 * @param[in] rhs Right-hand side std::string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline NFX_CORE_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, const std::string& rhs ) const noexcept;

		/**
		 * @brief Compare two string_view objects
		 * @param[in] lhs Left-hand side string_view to compare
		 * @param[in] rhs Right-hand side string_view to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] constexpr inline bool operator()( std::string_view lhs, std::string_view rhs ) const noexcept;

		/**
		 * @brief Compare C-string with std::string
		 * @param[in] lhs Left-hand side C-string to compare
		 * @param[in] rhs Right-hand side std::string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline NFX_CORE_CONDITIONAL_CONSTEXPR bool operator()( const char* lhs, const std::string& rhs ) const noexcept;

		/**
		 * @brief Compare C-string with string_view
		 * @param[in] lhs Left-hand side C-string to compare
		 * @param[in] rhs Right-hand side string_view to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE constexpr bool operator()( const char* lhs, std::string_view rhs ) const noexcept;

		/**
		 * @brief Compare std::string with C-string
		 * @param[in] lhs Left-hand side std::string to compare
		 * @param[in] rhs Right-hand side C-string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline NFX_CORE_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, const char* rhs ) const noexcept;

		/**
		 * @brief Compare std::string with string_view
		 * @param[in] lhs Left-hand side std::string to compare
		 * @param[in] rhs Right-hand side string_view to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline NFX_CORE_CONDITIONAL_CONSTEXPR bool operator()( const std::string& lhs, std::string_view rhs ) const noexcept;

		/**
		 * @brief Compare string_view with C-string
		 * @param[in] lhs Left-hand side string_view to compare
		 * @param[in] rhs Right-hand side C-string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE constexpr bool operator()( std::string_view lhs, const char* rhs ) const noexcept;

		/**
		 * @brief Compare string_view with std::string
		 * @param[in] lhs Left-hand side string_view to compare
		 * @param[in] rhs Right-hand side std::string to compare
		 * @return true if the strings are equal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline NFX_CORE_CONDITIONAL_CONSTEXPR bool operator()( std::string_view lhs, const std::string& rhs ) const noexcept;
	};
}

#include "StringFunctors.inl"
