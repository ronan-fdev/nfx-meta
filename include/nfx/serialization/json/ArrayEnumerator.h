/**
 * @file ArrayEnumerator.h
 * @brief Stateful enumerator for JSON array elements
 * @details Provides efficient enumeration over JSON arrays using Document references.
 *          Supports both dot notation and JSON Pointer navigation with explicit
 *          positioning and type-safe element access.
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
	// ArrayEnumerator class
	//=====================================================================

	/**
	 * @brief Stateful enumerator for JSON array elements
	 * @details Lightweight enumerator that references a Document and provides
	 *          efficient traversal of JSON arrays. Supports both dot notation
	 *          and JSON Pointer paths with explicit positioning and type-safe
	 *          element access.
	 */
	class ArrayEnumerator final
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
		explicit ArrayEnumerator( const Document& document );

		/**
		 * @brief Copy constructor
		 * @param other The enumerator to copy from
		 */
		ArrayEnumerator( const ArrayEnumerator& other );

		/**
		 * @brief Move constructor
		 * @param other The enumerator to move from
		 */
		ArrayEnumerator( ArrayEnumerator&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ArrayEnumerator();

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The enumerator to copy from
		 * @return Reference to this enumerator
		 */
		ArrayEnumerator& operator=( const ArrayEnumerator& other );

		/**
		 * @brief Move assignment operator
		 * @param other The enumerator to move from
		 * @return Reference to this enumerator
		 */
		ArrayEnumerator& operator=( ArrayEnumerator&& other ) noexcept;

		//----------------------------------------------
		// Navigation configuration
		//----------------------------------------------

		/**
		 * @brief Set array path using dot notation
		 * @param path Dot-separated path to JSON array (e.g., "data.users")
		 * @return True if path points to valid array, false otherwise
		 */
		bool setPath( std::string_view path );

		/**
		 * @brief Set array path using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer to array (e.g., "/data/users")
		 * @return True if pointer points to valid array, false otherwise
		 */
		bool setPointer( std::string_view pointer );

		/**
		 * @brief Reset enumerator to beginning of current array
		 */
		void reset();

		//----------------------------------------------
		// Array information
		//----------------------------------------------

		/**
		 * @brief Check if enumerator is positioned at valid array
		 * @return True if current path points to valid array, false otherwise
		 */
		bool isValid() const;

		/**
		 * @brief Get size of current array
		 * @return Array size, or 0 if not positioned at valid array
		 */
		size_t size() const;

		/**
		 * @brief Check if enumerator has reached end of array
		 * @return True if at end position, false otherwise
		 */
		bool isEnd() const;

		/**
		 * @brief Get current index position in array
		 * @return Current zero-based index, or size() if at end
		 */
		size_t index() const;

		//----------------------------------------------
		// Element access interface
		//----------------------------------------------

		/**
		 * @brief Get current array element as Document
		 * @return Document representing current array element
		 * @throws std::runtime_error if enumerator is invalid or at end
		 */
		Document currentElement() const;

		/**
		 * @brief Get string value from current array element
		 * @return Optional string value, nullopt if not string or invalid position
		 */
		std::optional<std::string> currentString() const;

		/**
		 * @brief Get integer value from current array element
		 * @return Optional integer value, nullopt if not integer or invalid position
		 */
		std::optional<int64_t> currentInt() const;

		/**
		 * @brief Get double value from current array element
		 * @return Optional double value, nullopt if not double or invalid position
		 */
		std::optional<double> currentDouble() const;

		/**
		 * @brief Get boolean value from current array element
		 * @return Optional boolean value, nullopt if not boolean or invalid position
		 */
		std::optional<bool> currentBool() const;

		//----------------------------------------------
		// Enumerator navigation interface
		//----------------------------------------------

		/**
		 * @brief Advance to next array element
		 * @return True if advancement successful, false if at end
		 */
		bool next();

		/**
		 * @brief Move to previous array element
		 * @return True if move successful, false if at beginning
		 */
		bool previous();

		/**
		 * @brief Move to specific index in array
		 * @param index Target index position
		 * @return True if move successful, false if index out of bounds
		 */
		bool moveTo( size_t index );

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};
} // namespace nfx::serialization::json
