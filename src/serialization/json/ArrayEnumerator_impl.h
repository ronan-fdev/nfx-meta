/**
 * @file ArrayEnumerator_impl.h
 * @brief Pimpl implementation for ArrayEnumerator, providing JSON array navigation
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

namespace nfx::serialization::json
{
	// Forward declarations
	class Document;
	class Document_impl;

	//=====================================================================
	// ArrayEnumerator_impl class
	//=====================================================================

	class ArrayEnumerator_impl final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct from Document reference
		 * @param document The source document containing JSON data
		 */
		explicit ArrayEnumerator_impl( const Document& document );

		/**
		 * @brief Copy constructor
		 * @param other The enumerator to copy from
		 */
		ArrayEnumerator_impl( const ArrayEnumerator_impl& other );

		/**
		 * @brief Move constructor
		 * @param other The enumerator to move from
		 */
		ArrayEnumerator_impl( ArrayEnumerator_impl&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ArrayEnumerator_impl() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The enumerator to copy from
		 * @return Reference to this enumerator
		 */
		ArrayEnumerator_impl& operator=( const ArrayEnumerator_impl& other );

		/**
		 * @brief Move assignment operator
		 * @param other The enumerator to move from
		 * @return Reference to this enumerator
		 */
		ArrayEnumerator_impl& operator=( ArrayEnumerator_impl&& other ) noexcept;

	public:
		//----------------------------------------------
		// Navigation methods
		//----------------------------------------------

		/**
		 * @brief Navigate to array using dot notation path
		 * @param path Dot-separated path to array
		 * @return True if navigation successful, false otherwise
		 */
		bool navigateToPath( std::string_view path );

		/**
		 * @brief Navigate to array using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer to array
		 * @return True if navigation successful, false otherwise
		 */
		bool navigateToPointer( std::string_view pointer );

		/**
		 * @brief Reset enumerator position to beginning
		 */
		void reset() noexcept;

		//----------------------------------------------
		// Array access methods
		//----------------------------------------------

		/**
		 * @brief Check if currently positioned at valid array
		 * @return True if valid array, false otherwise
		 */
		bool isValidArray() const noexcept;

		/**
		 * @brief Get array size
		 * @return Size of current array, 0 if invalid
		 */
		size_t getArraySize() const noexcept;

		/**
		 * @brief Check if at end position
		 * @return True if at end, false otherwise
		 */
		bool isAtEnd() const noexcept;

		/**
		 * @brief Get current index
		 * @return Current zero-based index position
		 */
		size_t currentIndex() const noexcept;

		/**
		 * @brief Advance to next element
		 * @return True if advancement successful, false if at end
		 */
		bool advance() noexcept;

		/**
		 * @brief Get current array element as JSON
		 * @return Reference to current JSON element
		 * @throws std::runtime_error if invalid position
		 */
		const nlohmann::json& currentElement() const;

		/**
		 * @brief Create Document wrapper for current element
		 * @return Document representing current array element
		 * @throws std::runtime_error if invalid position
		 */
		std::unique_ptr<Document> currentElementAsDocument() const;

		//----------------------------------------------
		// Comparison methods
		//----------------------------------------------

		/**
		 * @brief Move to previous element
		 * @return True if movement successful, false if at beginning
		 */
		bool movePrevious() noexcept;

		/**
		 * @brief Move to specific index
		 * @param index Target index position
		 * @return True if movement successful, false if out of bounds
		 */
		bool moveToIndex( size_t index ) noexcept;

	private:
		//----------------------------------------------
		// Member variables
		//----------------------------------------------

		const Document& m_document;							   ///< Reference to source document
		std::string m_currentPath;							   ///< Current path to array
		const nlohmann::json* m_currentArray;				   ///< Pointer to current JSON array
		size_t m_currentIndex;								   ///< Current position in array
		mutable std::unique_ptr<Document> m_currentElementDoc; ///< Cache for current element Document
	};
} // namespace nfx::serialization::json
