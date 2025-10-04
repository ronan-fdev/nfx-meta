/**
 * @file FieldEnumerator.h
 * @brief Stateful enumerator for JSON object field enumeration
 * @details Provides efficient enumeration over JSON object fields using Document references.
 *          Supports both dot notation and JSON Pointer navigation with explicit
 *          positioning and type-safe field access for key-value iteration.
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace nfx::serialization::json
{
	// Forward declaration
	class Document;

	//=====================================================================
	// FieldEnumerator class
	//=====================================================================

	/**
	 * @brief Stateful enumerator for JSON object fields
	 * @details Lightweight enumerator that references a Document and provides
	 *          efficient traversal of JSON object key-value pairs. Supports both
	 *          dot notation and JSON Pointer paths with explicit positioning and
	 *          type-safe field access.
	 */
	class FieldEnumerator final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct enumerator from Document reference
		 * @param document The JSON document to enumerate over
		 * @warning The document must remain valid for the lifetime of this enumerator
		 */
		explicit FieldEnumerator( const Document& document );

		/**
		 * @brief Copy constructor
		 * @param other The enumerator to copy from
		 */
		FieldEnumerator( const FieldEnumerator& other );

		/**
		 * @brief Move constructor
		 * @param other The enumerator to move from
		 */
		FieldEnumerator( FieldEnumerator&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FieldEnumerator();

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The enumerator to copy from
		 * @return Reference to this enumerator
		 */
		FieldEnumerator& operator=( const FieldEnumerator& other );

		/**
		 * @brief Move assignment operator
		 * @param other The enumerator to move from
		 * @return Reference to this enumerator
		 */
		FieldEnumerator& operator=( FieldEnumerator&& other ) noexcept;

		//----------------------------------------------
		// Navigation configuration
		//----------------------------------------------

		/**
		 * @brief Set object path using dot notation
		 * @param path Dot-separated path to JSON object (e.g., "data.user")
		 * @return True if path points to valid object, false otherwise
		 */
		bool setPath( std::string_view path );

		/**
		 * @brief Set object path using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer to object (e.g., "/data/user")
		 * @return True if pointer points to valid object, false otherwise
		 */
		bool setPointer( std::string_view pointer );

		/**
		 * @brief Reset enumerator to beginning of current object
		 */
		void reset();

		//----------------------------------------------
		// Field information
		//----------------------------------------------

		/**
		 * @brief Check if enumerator is positioned at valid object
		 * @return True if current path points to valid object, false otherwise
		 */
		bool isValid() const;

		/**
		 * @brief Get number of fields in current object
		 * @return Field count, or 0 if not positioned at valid object
		 */
		size_t size() const;

		/**
		 * @brief Check if enumerator has reached end of object fields
		 * @return True if at end position, false otherwise
		 */
		bool isEnd() const;

		/**
		 * @brief Get current field index position in object
		 * @return Current zero-based index, or size() if at end
		 */
		size_t index() const;

		//----------------------------------------------
		// Field access interface
		//----------------------------------------------

		/**
		 * @brief Get current field key name
		 * @return Current field key, or empty string if invalid position
		 */
		std::string currentKey() const;

		/**
		 * @brief Get current field value as Document
		 * @return Document representing current field value
		 * @throws std::runtime_error if enumerator is invalid or at end
		 */
		Document currentValue() const;

		/**
		 * @brief Get string value from current field
		 * @return Optional string value, nullopt if not string or invalid position
		 */
		std::optional<std::string> currentString() const;

		/**
		 * @brief Get integer value from current field
		 * @return Optional integer value, nullopt if not integer or invalid position
		 */
		std::optional<int64_t> currentInt() const;

		/**
		 * @brief Get double value from current field
		 * @return Optional double value, nullopt if not double or invalid position
		 */
		std::optional<double> currentDouble() const;

		/**
		 * @brief Get boolean value from current field
		 * @return Optional boolean value, nullopt if not boolean or invalid position
		 */
		std::optional<bool> currentBool() const;

		//----------------------------------------------
		// Enumerator navigation interface
		//----------------------------------------------

		/**
		 * @brief Advance to next field
		 * @return True if advancement successful, false if at end
		 */
		bool next();

		/**
		 * @brief Move to previous field
		 * @return True if move successful, false if at beginning
		 */
		bool previous();

		/**
		 * @brief Move to specific field by index
		 * @param index Target field index position
		 * @return True if move successful, false if index out of bounds
		 */
		bool moveTo( size_t index );

		/**
		 * @brief Move to specific field by key name
		 * @param key Target field key name
		 * @return True if move successful, false if key not found
		 */
		bool moveToKey( std::string_view key );

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};
} // namespace nfx::serialization::json
