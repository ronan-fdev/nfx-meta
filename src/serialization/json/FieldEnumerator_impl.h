/**
 * @file FieldEnumerator_impl.h
 * @brief Pimpl implementation for FieldEnumerator, providing JSON object field navigation
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace nfx::serialization::json
{
	// Forward declarations
	class Document;
	class Document_impl;

	//=====================================================================
	// FieldEnumerator_impl class
	//=====================================================================

	class FieldEnumerator_impl final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct from Document reference
		 * @param document The source document containing JSON data
		 */
		explicit FieldEnumerator_impl( const Document& document );

		/**
		 * @brief Copy constructor
		 * @param other The enumerator to copy from
		 */
		FieldEnumerator_impl( const FieldEnumerator_impl& other );

		/**
		 * @brief Move constructor
		 * @param other The enumerator to move from
		 */
		FieldEnumerator_impl( FieldEnumerator_impl&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FieldEnumerator_impl() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The enumerator to copy from
		 * @return Reference to this enumerator
		 */
		FieldEnumerator_impl& operator=( const FieldEnumerator_impl& other );

		/**
		 * @brief Move assignment operator
		 * @param other The enumerator to move from
		 * @return Reference to this enumerator
		 */
		FieldEnumerator_impl& operator=( FieldEnumerator_impl&& other ) noexcept;

	public:
		//----------------------------------------------
		// Navigation methods
		//----------------------------------------------

		/**
		 * @brief Navigate to object using dot notation path
		 * @param path Dot-separated path to object
		 * @return True if navigation successful, false otherwise
		 */
		bool navigateToPath( std::string_view path );

		/**
		 * @brief Navigate to object using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer to object
		 * @return True if navigation successful, false otherwise
		 */
		bool navigateToPointer( std::string_view pointer );

		/**
		 * @brief Reset enumerator position to beginning
		 */
		void reset() noexcept;

		//----------------------------------------------
		// Object access methods
		//----------------------------------------------

		/**
		 * @brief Check if currently positioned at valid object
		 * @return True if valid object, false otherwise
		 */
		bool isValidObject() const noexcept;

		/**
		 * @brief Get object field count
		 * @return Size of current object, 0 if invalid
		 */
		size_t objectSize() const noexcept;

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
		 * @brief Advance to next field
		 * @return True if advancement successful, false if at end
		 */
		bool advance() noexcept;

		/**
		 * @brief Get current field key
		 * @return Current field key as string
		 * @throws std::runtime_error if invalid position
		 */
		std::string currentKey() const;

		/**
		 * @brief Get current field value as JSON
		 * @return Reference to current JSON field value
		 * @throws std::runtime_error if invalid position
		 */
		const nlohmann::ordered_json& currentValue() const;

		/**
		 * @brief Create Document wrapper for current field value
		 * @return Document representing current field value
		 * @throws std::runtime_error if invalid position
		 */
		std::unique_ptr<Document> currentValueAsDocument() const;

		//----------------------------------------------
		// Movement methods
		//----------------------------------------------

		/**
		 * @brief Move to previous field
		 * @return True if movement successful, false if at beginning
		 */
		bool movePrevious() noexcept;

		/**
		 * @brief Move to specific index
		 * @param index Target index position
		 * @return True if movement successful, false if out of bounds
		 */
		bool moveToIndex( size_t index ) noexcept;

		/**
		 * @brief Move to specific field by key name
		 * @param key Target field key
		 * @return True if movement successful, false if key not found
		 */
		bool moveToFieldKey( std::string_view key ) noexcept;

	private:
		/**
		 * @brief Build field key list for indexed access
		 */
		void buildFieldList();

		//----------------------------------------------
		// Member variables
		//----------------------------------------------

		const Document& m_document;							 ///< Reference to source document
		std::string m_currentPath;							 ///< Current path to object
		const nlohmann::ordered_json* m_currentObject;		 ///< Pointer to current JSON object
		std::vector<std::string> m_fieldKeys;				 ///< Cached field keys for indexed access
		size_t m_currentIndex;								 ///< Current position in field list
		mutable std::unique_ptr<Document> m_currentValueDoc; ///< Cache for current field value Document
	};
} // namespace nfx::serialization::json
