/**
 * @file Document_impl.h
 * @brief Pimpl implementation for Document, wrapping nlohmann::ordered_json
 */

#pragma once

#include <string>
#include <string_view>
#include <optional>

#include <nlohmann/json.hpp>

namespace nfx::serialization::json
{
	class Document;

	class Document_impl final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		Document_impl();

		explicit Document_impl( nlohmann::ordered_json jsonData );

		/**
		 * @brief Copy constructor
		 * @param other The Document_impl to copy from
		 */
		Document_impl( const Document_impl& other );

		/**
		 * @brief Move constructor
		 * @param other The Document_impl to move from
		 */
		Document_impl( Document_impl&& other ) noexcept;

		/**
		 * @brief Copy assignment operator
		 * @param other The Document_impl to copy from
		 * @return Reference to this Document_impl
		 */
		Document_impl& operator=( const Document_impl& other );

		/**
		 * @brief Move assignment operator
		 * @param other The Document_impl to move from
		 * @return Reference to this Document_impl
		 */
		Document_impl& operator=( Document_impl&& other ) noexcept;

	public:
		//----------------------------------------------
		// Navigation methods
		//----------------------------------------------

		/**
		 * @brief Navigate to a JSON node at the specified dot-separated path
		 * @param path Dot-separated path (e.g., "user.profile.name" or "data.items[0]")
		 * @param createPath If true, creates intermediate objects/arrays if they don't exist
		 * @return Pointer to the JSON node at the path, or nullptr if path doesn't exist
		 * @details Supports both object field access ("user.name") and array indexing ("items[0]").
		 *          When createPath is true, missing intermediate nodes are created as objects.
		 */
		nlohmann::ordered_json* navigateToPath( std::string_view path, bool createPath = false );

		/**
		 * @brief Navigate to a JSON node at the specified dot-separated path (const version)
		 * @param path Dot-separated path (e.g., "user.profile.name" or "data.items[0]")
		 * @return Const pointer to the JSON node at the path, or nullptr if path doesn't exist
		 * @details Read-only version that never creates new nodes. Supports both object field
		 *          access ("user.name") and array indexing ("items[0]").
		 */
		const nlohmann::ordered_json* navigateToPath( std::string_view path ) const;

		/**
		 * @brief Navigate to a JSON node using RFC 6901 JSON Pointer syntax
		 * @param pointer JSON Pointer string (e.g., "/users/0/name" or "/data/items/-")
		 * @param createPath If true, creates intermediate objects/arrays if they don't exist
		 * @return Pointer to the JSON node at the pointer path, or nullptr if path doesn't exist
		 * @details Implements RFC 6901 JSON Pointer specification with support for:
		 *          - Object property access ("/user/name")
		 *          - Array element access ("/items/0", "/items/1")
		 *          - Escaped characters ("/field~1with~0slash" for "field/with~slash")
		 *          - Root document access ("")
		 *          When createPath is true, missing intermediate nodes are created as objects
		 *          or arrays based on context (numeric tokens create arrays).
		 */
		nlohmann::ordered_json* navigateToJsonPointer( std::string_view pointer, bool createPath = false );

		/**
		 * @brief Navigate to a JSON node using RFC 6901 JSON Pointer syntax (const version)
		 * @param pointer JSON Pointer string (e.g., "/users/0/name" or "/data/items")
		 * @return Const pointer to the JSON node at the pointer path, or nullptr if path doesn't exist
		 * @details Read-only version implementing RFC 6901 JSON Pointer specification.
		 *          Supports object property access, array indexing, and escaped characters.
		 *          Never creates new nodes.
		 */
		const nlohmann::ordered_json* navigateToJsonPointer( std::string_view pointer ) const;

		//----------------------------------------------
		// Helper methods
		//----------------------------------------------

		/**
		 * @brief Unescape JSON Pointer token according to RFC 6901
		 * @param token The escaped token to unescape
		 * @return The unescaped token as a string
		 * @details Converts "~1" to "/" and "~0" to "~" according to RFC 6901 Section 3
		 */
		static std::string unescapeJsonPointerToken( std::string_view token ) noexcept;

		/**
		 * @brief Check if a token represents a valid array index
		 * @param token The token to check
		 * @return True if the token is a valid array index (non-negative integer or "-")
		 */
		static bool isValidArrayIndex( std::string_view token ) noexcept;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get reference to internal JSON data
		 * @return Reference to nlohmann::ordered_json data
		 * @note For friend class access only
		 */
		nlohmann::ordered_json& data() noexcept { return m_data; }

		/**
		 * @brief Get const reference to internal JSON data
		 * @return Const reference to nlohmann::ordered_json data
		 * @note For friend class access only
		 */
		const nlohmann::ordered_json& data() const noexcept { return m_data; }

		/**
		 * @brief Set internal JSON data
		 * @param data New JSON data to set
		 * @note For friend class access only
		 */
		void setData( const nlohmann::ordered_json& data ) noexcept { m_data = data; }

		/**
		 * @brief Set last error message
		 * @param error Error message to set
		 * @note For internal use only
		 */
		void setLastError( std::string_view error ) { m_lastError = std::string{ error }; }

		/**
		 * @brief Get last error message
		 * @return Last error message
		 * @note For internal use only
		 */
		const std::string& lastError() const noexcept { return m_lastError; }

		/**
		 * @brief Get typed array element implementation (complete pimpl template method)
		 * @tparam T The type to retrieve (all supported Array types)
		 * @param arrayPath Path to the array within this document
		 * @param index Index of the element to retrieve
		 * @param docPtr Pointer to the Document for complex type construction
		 * @return Optional value of type T, nullopt if not found or type mismatch
		 */
		template <typename T>
		std::optional<T> getArrayImpl( std::string_view arrayPath, size_t index, const class Document* docPtr ) const;

		/**
		 * @brief Set array element implementation template method (complete pimpl pattern)
		 * @tparam T The type to set (all supported Array types with perfect forwarding)
		 * @param arrayPath Path to the array within this document
		 * @param index Index where to set the element
		 * @param value The value to set (perfect forwarded)
		 */
		template <typename T>
		void setArrayImpl( std::string_view arrayPath, size_t index, T&& value );

		/**
		 * @brief Add array element implementation template method (complete pimpl pattern)
		 * @tparam T The type to add (all supported Array types with perfect forwarding)
		 * @param arrayPath Path to the array within this document
		 * @param value The value to add (perfect forwarded)
		 */
		template <typename T>
		void addArrayImpl( std::string_view arrayPath, T&& value );

		/**
		 * @brief Insert array element implementation template method (complete pimpl pattern)
		 * @tparam T The type to insert (all supported Array types with perfect forwarding)
		 * @param arrayPath Path to the array within this document
		 * @param index Index where to insert the element
		 * @param value The value to insert (perfect forwarded)
		 */
		template <typename T>
		void insertArrayImpl( std::string_view arrayPath, size_t index, T&& value );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		nlohmann::ordered_json m_data; ///< JSON document data
		std::string m_lastError;	   ///< Last error message from operations
	};
} // namespace nfx::serialization::json
