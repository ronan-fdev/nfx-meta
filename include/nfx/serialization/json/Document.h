/**
 * @file Document.h
 * @brief Generic document abstraction for JSON serialization
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace nfx::serialization::json
{
	class Document_impl;

	//=====================================================================
	// Document class
	//=====================================================================

	/**
	 * @brief Generic JSON document abstraction for serialization
	 * @details Provides a high-level interface for JSON document manipulation with support
	 *          for JSON Pointer paths, type-safe value access, and nested object/array operations.
	 */
	class Document final
	{
	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Array;
		class Object;

		//----------------------------------------------
		// Friends
		//----------------------------------------------

		friend class Document::Array;
		friend class Document::Object;
		friend class Document_impl;
		friend class ArrayEnumerator_impl;
		friend class FieldEnumerator_impl;
		friend class SchemaValidator_impl;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor - creates an empty document
		 */
		Document();

		/**
		 * @brief Copy constructor
		 * @param other The document to copy from
		 */
		Document( const Document& other );

		/**
		 * @brief Move constructor
		 * @param other The document to move from
		 */
		Document( Document&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor - cleans up document resources
		 */
		~Document();

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The document to copy from
		 * @return Reference to this document
		 */
		Document& operator=( const Document& other );

		/**
		 * @brief Move assignment operator
		 * @param other The document to move from
		 * @return Reference to this document
		 */
		Document& operator=( Document&& other ) noexcept;

		//----------------------------------------------
		// Comparison
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param other The document to compare with
		 * @return true if documents are equal, false otherwise
		 */
		bool operator==( const Document& other ) const;

		/**
		 * @brief Inequality comparison operator
		 * @param other The document to compare with
		 * @return true if documents are not equal, false otherwise
		 */
		bool operator!=( const Document& other ) const;

		//----------------------------------------------
		// Factory
		//----------------------------------------------

		/**
		 * @brief Create document from JSON string
		 * @param jsonStr JSON string to parse
		 * @return Optional document if parsing succeeds, empty optional otherwise
		 */
		static std::optional<Document> fromJsonString( std::string_view jsonStr );

		/**
		 * @brief Create document from JSON bytes
		 * @param bytes JSON bytes to parse
		 * @return Optional document if parsing succeeds, empty optional otherwise
		 */
		static std::optional<Document> fromJsonBytes( std::vector<uint8_t>& bytes );

		//----------------------------------------------
		// Output
		//----------------------------------------------

		/**
		 * @brief Convert document to JSON string
		 * @param indent Indentation level for pretty printing (0 for compact)
		 * @return JSON string representation
		 */
		std::string toJsonString( int indent = 0 ) const;

		/**
		 * @brief Convert document to JSON bytes
		 * @return JSON byte representation
		 */
		std::vector<uint8_t> toJsonBytes() const;

		//----------------------------------------------
		// Value inspection
		//----------------------------------------------

		/**
		 * @brief Check if a value exists at the given path
		 * @param path JSON Pointer path to check
		 * @return true if value exists, false otherwise
		 */
		bool hasValue( std::string_view path ) const;

		//----------------------------------------------
		// Merge / update operations
		//----------------------------------------------

		/**
		 * @brief Merge another document into this one
		 * @param other Document to merge
		 * @param overwriteArrays Whether to overwrite arrays or merge them
		 */
		void merge( const Document& other, bool overwriteArrays = true );

		/**
		 * @brief Update value at specific path
		 * @param path JSON Pointer path to update
		 * @param value New value to set
		 */
		void update( std::string_view path, const Document& value );

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get typed value at specified path
		 * @tparam T Type to retrieve (string, int, double, bool, Document, Object, Array)
		 * @param path JSON Pointer path to value
		 * @return Optional containing value if exists and correct type, empty otherwise
		 */
		template <typename T>
			requires(
				std::is_same_v<std::decay_t<T>, std::string_view> ||
				std::is_same_v<std::decay_t<T>, std::string> ||
				std::is_same_v<std::decay_t<T>, char> ||
				std::is_same_v<std::decay_t<T>, bool> ||
				std::is_same_v<std::decay_t<T>, int32_t> ||
				std::is_same_v<std::decay_t<T>, int64_t> ||
				std::is_same_v<std::decay_t<T>, double> ||
				std::is_same_v<std::decay_t<T>, Document> ||
				std::is_same_v<std::decay_t<T>, Document::Object> ||
				std::is_same_v<std::decay_t<T>, Document::Array> )
		std::optional<T> get( std::string_view path ) const;

		//----------------------------------------------
		// Value modification
		//----------------------------------------------

		/**
		 * @brief Set typed value at specified path (copy version)
		 * @tparam T Type to set (string, int, double, bool, Document, Object, Array)
		 * @param path JSON Pointer path where to set value
		 * @param value Value to set (copied)
		 */
		template <typename T>
			requires(
				std::is_same_v<std::decay_t<T>, std::string_view> ||
				std::is_same_v<std::decay_t<T>, std::string> ||
				std::is_same_v<std::decay_t<T>, char> ||
				std::is_same_v<std::decay_t<T>, bool> ||
				std::is_same_v<std::decay_t<T>, int32_t> ||
				std::is_same_v<std::decay_t<T>, int64_t> ||
				std::is_same_v<std::decay_t<T>, double> ||
				std::is_same_v<std::decay_t<T>, Document> ||
				std::is_same_v<std::decay_t<T>, Document::Object> ||
				std::is_same_v<std::decay_t<T>, Document::Array> )
		void set( std::string_view path, const T& value );

		/**
		 * @brief Set typed value at specified path (move version)
		 * @tparam T Type to set (string, int, double, bool, Document, Object, Array)
		 * @param path JSON Pointer path where to set value
		 * @param value Value to set (moved)
		 */
		template <typename T>
			requires(
				std::is_same_v<std::decay_t<T>, std::string_view> ||
				std::is_same_v<std::decay_t<T>, std::string> ||
				std::is_same_v<std::decay_t<T>, char> ||
				std::is_same_v<std::decay_t<T>, bool> ||
				std::is_same_v<std::decay_t<T>, int32_t> ||
				std::is_same_v<std::decay_t<T>, int64_t> ||
				std::is_same_v<std::decay_t<T>, double> ||
				std::is_same_v<std::decay_t<T>, Document> ||
				std::is_same_v<std::decay_t<T>, Document::Object> ||
				std::is_same_v<std::decay_t<T>, Document::Array> )
		void set( std::string_view path, T&& value );

		//-----------------------------
		// Type-only creation
		//-----------------------------

		/**
		 * @brief Create empty container at specified path
		 * @tparam T Container type (Document, Object, Array)
		 * @param path JSON Pointer path where to create container
		 */
		template <typename T>
			requires(
				std::is_same_v<std::decay_t<T>, Document> ||
				std::is_same_v<std::decay_t<T>, Document::Object> ||
				std::is_same_v<std::decay_t<T>, Document::Array> )
		void set( std::string_view path );

		//-----------------------------
		// Null operations
		//-----------------------------

		/**
		 * @brief Set null value at specified path
		 * @param path JSON Pointer path where to set null
		 */
		void setNull( std::string_view path );

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if value at path is of specified type
		 * @tparam T Type to check for
		 * @param path JSON Pointer path to check
		 * @return true if value exists and is of type T, false otherwise
		 */
		template <typename T>
			requires(
				std::is_same_v<std::decay_t<T>, std::string_view> ||
				std::is_same_v<std::decay_t<T>, std::string> ||
				std::is_same_v<std::decay_t<T>, char> ||
				std::is_same_v<std::decay_t<T>, bool> ||
				std::is_same_v<std::decay_t<T>, int32_t> ||
				std::is_same_v<std::decay_t<T>, int64_t> ||
				std::is_same_v<std::decay_t<T>, double> ||
				std::is_same_v<std::decay_t<T>, Document> ||
				std::is_same_v<std::decay_t<T>, Document::Object> ||
				std::is_same_v<std::decay_t<T>, Document::Array> )
		bool is( std::string_view path ) const;

		/**
		 * @brief Check if value at path is null
		 * @param path JSON Pointer path to check
		 * @return true if value is null, false otherwise
		 */
		bool isNull( std::string_view path ) const;

		//----------------------------------------------
		// Validation and error handling
		//----------------------------------------------

		/**
		 * @brief Check if document is in valid state
		 * @return true if document is valid, false otherwise
		 */
		bool isValid() const;

		/**
		 * @brief Get last error message
		 * @return Error message string
		 */
		std::string lastError() const;

		//----------------------------------------------
		// Document::Object class
		//----------------------------------------------

		/**
		 * @brief JSON object wrapper for Document
		 * @details Provides type-safe access to JSON object fields with support for
		 *          field access, modification, removal, and nested operations using JSON Pointer paths.
		 */
		class Object final
		{
			//----------------------------------------------
			// Friends
			//----------------------------------------------

			friend class Document;
			friend class Document::Array;
			friend class Document_impl;

			//-----------------------------
			// Construction
			//-----------------------------

		private:
			Object( Document* doc, std::string_view path );

		public:
			/**
			 * @brief Default constructor - creates invalid object
			 */
			Object();
			
			/**
			 * @brief Copy constructor
			 * @param other Object to copy from
			 */
			Object( const Document::Object& other );
			
			/**
			 * @brief Move constructor
			 * @param other Object to move from
			 */
			Object( Document::Object&& other ) noexcept;

			//-----------------------------
			// Destruction
			//-----------------------------

			/** @brief Destructor */
			~Object() = default;

			//-----------------------------
			// Assignment
			//-----------------------------

			/**
			 * @brief Copy assignment operator
			 * @param other Object to copy from
			 * @return Reference to this object
			 */
			Document::Object& operator=( const Document::Object& other );
			
			/**
			 * @brief Move assignment operator
			 * @param other Object to move from
			 * @return Reference to this object
			 */
			Document::Object& operator=( Document::Object&& other ) noexcept;

			//-----------------------------
			// Comparison
			//-----------------------------

			/**
			 * @brief Equality comparison operator
			 * @param other Object to compare with
			 * @return true if objects are equal
			 */
			bool operator==( const Document::Object& other ) const;

			/**
			 * @brief Inequality comparison operator
			 * @param other Object to compare with
			 * @return true if objects are not equal
			 */
			bool operator!=( const Document::Object& other ) const;

			//-----------------------------
			// Output
			//-----------------------------

			/**
			 * @brief Convert object to JSON string
			 * @param indent Indentation level
			 * @return JSON string representation
			 */
			std::string toJsonString( int indent = 0 ) const;

			/**
			 * @brief Convert object to JSON bytes
			 * @return JSON byte representation
			 */
			std::vector<uint8_t> toJsonBytes() const;

			//-----------------------------
			// Field inspection
			//-----------------------------

			/**
			 * @brief Check if field exists
			 * @param key Field name to check
			 * @return true if field exists
			 */
			bool hasField( std::string_view key ) const;

			//-----------------------------
			// Size
			//-----------------------------

			/**
			 * @brief Get number of fields in object
			 * @return Number of key-value pairs
			 */
			size_t size() const;

			//-----------------------------
			// Clearing
			//-----------------------------

			/**
			 * @brief Clear all fields from object
			 */
			void clear();

			//-----------------------------
			// Field removal
			//-----------------------------

			/**
			 * @brief Remove field from object
			 * @param key Field name to remove
			 * @return true if field was removed
			 */
			bool removeField( std::string_view key );

			//-----------------------------
			// Field access
			//-----------------------------

			/**
			 * @brief Get field value by key
			 * @tparam T Type to retrieve
			 * @param path Field key or nested path
			 * @return Optional containing field value if exists and correct type
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			std::optional<T> get( std::string_view path ) const;

			//-----------------------------
			// Field modification
			//-----------------------------

			/**
			 * @brief Set field value (copy version)
			 * @tparam T Type to set
			 * @param path Field key or nested path
			 * @param value Value to copy and set
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( std::string_view path, const T& value );

			/**
			 * @brief Set field value (move version)
			 * @tparam T Type to set
			 * @param path Field key or nested path
			 * @param value Value to move and set
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( std::string_view path, T&& value );

			//-----------------------------
			// Validation and error handling
			//-----------------------------

			/**
			 * @brief Check if object is valid
			 * @return true if object is valid and accessible
			 */
			bool isValid() const;

			/**
			 * @brief Get last error message
			 * @return String describing the last error
			 */
			std::string lastError() const;

		private:
			//----------------------------------------------
			// Private data members
			//----------------------------------------------

			Document* m_doc;	///< Pointer to the original document
			std::string m_path; ///< Path to the object within the document
		};

		//----------------------------------------------
		// Document::Array class
		//----------------------------------------------

		/**
		 * @brief JSON array wrapper for Document
		 * @details Provides type-safe access to JSON array elements with support for
		 *          indexed access, element addition/insertion/removal, and nested operations.
		 */
		class Array final
		{
			//-----------------------------
			// Friends
			//-----------------------------

			friend class Document;
			friend class Document::Object;
			friend class Document_impl;

			//-----------------------------
			// Construction
			//-----------------------------

		private:
			Array( Document* doc, std::string_view path );

		public:
			/**
			 * @brief Default constructor - creates empty array
			 */
			Array();

			/**
			 * @brief Copy constructor
			 * @param other Array to copy from
			 */
			Array( const Document::Array& other );

			/**
			 * @brief Move constructor
			 * @param other Array to move from
			 */
			Array( Document::Array&& other ) noexcept;

			//-----------------------------
			// Destruction
			//-----------------------------

			/** @brief Destructor */
			~Array() = default;

			//-----------------------------
			// Assignment
			//-----------------------------

			/**
			 * @brief Copy assignment operator
			 * @param other Array to copy from
			 * @return Reference to this array
			 */
			Document::Array& operator=( const Document::Array& other );

			/**
			 * @brief Move assignment operator
			 * @param other Array to move from
			 * @return Reference to this array
			 */
			Document::Array& operator=( Document::Array&& other ) noexcept;

			//-----------------------------
			// Comparison
			//-----------------------------

			/**
			 * @brief Equality comparison
			 * @param other Array to compare with
			 * @return true if arrays are equal
			 */
			bool operator==( const Document::Array& other ) const;

			/**
			 * @brief Inequality comparison
			 * @param other Array to compare with
			 * @return true if arrays are not equal
			 */
			bool operator!=( const Document::Array& other ) const;

			//-----------------------------
			// Output
			//-----------------------------

			/**
			 * @brief Convert array to JSON string
			 * @param indent Indentation level
			 * @return JSON string representation
			 */
			std::string toJsonString( int indent = 0 ) const;

			/**
			 * @brief Convert array to JSON bytes
			 * @return JSON byte representation
			 */
			std::vector<uint8_t> toJsonBytes() const;

			//-----------------------------
			// Element inspection
			//-----------------------------

			/**
			 * @brief Check if element exists at path
			 * @param path JSON pointer path
			 * @return true if element exists
			 */
			bool hasElement( std::string_view path ) const;

			//-----------------------------
			// Size
			//-----------------------------

			/**
			 * @brief Get number of elements in array
			 * @return Size of array
			 */
			size_t size() const;

			//-----------------------------
			// Clearing
			//-----------------------------

			/**
			 * @brief Clear all elements from array
			 */
			void clear();

			//-----------------------------
			// Element removal
			//-----------------------------

			/**
			 * @brief Remove element at index
			 * @param index Array index
			 * @return true if element was removed
			 */
			bool remove( size_t index );

			//-----------------------------
			// Element access
			//-----------------------------

			/**
			 * @brief Get element at index
			 * @tparam T Type to retrieve
			 * @param index Array index
			 * @return Optional containing element if exists and correct type
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			std::optional<T> get( size_t index ) const;

			//-----------------------------
			// Nested element access
			//-----------------------------

			/**
			 * @brief Get nested element at path
			 * @tparam T Type to retrieve
			 * @param path JSON pointer path
			 * @return Optional containing element if exists and correct type
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			std::optional<T> get( std::string_view path ) const;

			//-----------------------------
			// Element modification
			//-----------------------------

			/**
			 * @brief Set element at index (copy version)
			 * @tparam T Type to set
			 * @param index Array index
			 * @param value Value to copy
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( size_t index, const T& value );

			/**
			 * @brief Set element at index (move version)
			 * @tparam T Type to set
			 * @param index Array index
			 * @param value Value to move
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( size_t index, T&& value );

			//-----------------------------
			// Nested element modification
			//-----------------------------

			/**
			 * @brief Set nested element at path (copy version)
			 * @tparam T Type to set
			 * @param path JSON pointer path
			 * @param value Value to copy
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( std::string_view path, const T& value );

			/**
			 * @brief Set nested element at path (move version)
			 * @tparam T Type to set
			 * @param path JSON pointer path
			 * @param value Value to move
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void set( std::string_view path, T&& value );

			//-----------------------------
			// Element addition
			//-----------------------------

			/**
			 * @brief Add element to end of array (copy version)
			 * @tparam T Type to add
			 * @param value Value to copy and add
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void add( const T& value );

			/**
			 * @brief Add element to end of array (move version)
			 * @tparam T Type to add
			 * @param value Value to move and add
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void add( T&& value );

			/**
			 * @brief Add Document to end of array (reference version)
			 * @param value Document reference to add
			 */
			void add( Document& value );

			/**
			 * @brief Add Array to end of array (reference version)
			 * @param value Array reference to add
			 */
			void add( Document::Array& value );

			/**
			 * @brief Add Object to end of array (reference version)
			 * @param value Object reference to add
			 */
			void add( Document::Object& value );

			//-----------------------------
			// Element insertion
			//-----------------------------

			/**
			 * @brief Insert element at index (copy version)
			 * @tparam T Type to insert
			 * @param index Position to insert at
			 * @param value Value to copy and insert
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void insert( size_t index, const T& value );

			/**
			 * @brief Insert element at index (move version)
			 * @tparam T Type to insert
			 * @param index Position to insert at
			 * @param value Value to move and insert
			 */
			template <typename T>
				requires(
					std::is_same_v<std::decay_t<T>, std::string_view> ||
					std::is_same_v<std::decay_t<T>, std::string> ||
					std::is_same_v<std::decay_t<T>, char> ||
					std::is_same_v<std::decay_t<T>, bool> ||
					std::is_same_v<std::decay_t<T>, int32_t> ||
					std::is_same_v<std::decay_t<T>, int64_t> ||
					std::is_same_v<std::decay_t<T>, double> ||
					std::is_same_v<std::decay_t<T>, Document> ||
					std::is_same_v<std::decay_t<T>, Document::Object> ||
					std::is_same_v<std::decay_t<T>, Document::Array> )
			void insert( size_t index, T&& value );

			/**
			 * @brief Insert Document at index (reference version)
			 * @param index Position to insert at
			 * @param value Document reference to insert
			 */
			void insert( size_t index, Document& value );
			
			/**
			 * @brief Insert Array at index (reference version)
			 * @param index Position to insert at
			 * @param value Array reference to insert
			 */
			void insert( size_t index, Document::Array& value );
			
			/**
			 * @brief Insert Object at index (reference version)
			 * @param index Position to insert at
			 * @param value Object reference to insert
			 */
			void insert( size_t index, Document::Object& value );

			//-----------------------------
			// Validation and error handling
			//-----------------------------

			/**
			 * @brief Check if array is valid
			 * @return true if array is valid and accessible
			 */
			bool isValid() const;

			/**
			 * @brief Get last error message
			 * @return String describing the last error
			 */
			std::string lastError() const;

		private:
			//-----------------------------
			// Private data members
			//-----------------------------

			Document* m_doc;	///< Pointer to the original document
			std::string m_path; ///< Path to the array within the document
		};

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};
} // namespace nfx::serialization::json
