/**
 * @file Document.h
 * @brief Generic document abstraction for JSON serialization
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace nfx::serialization::json
{
	//=====================================================================
	// Document class
	//=====================================================================

	/**
	 * @brief Generic document abstraction
	 */
	class Document final
	{
		//----------------------------------------------
		// Friends
		//----------------------------------------------

		friend class ArrayEnumerator_impl;
		friend class FieldEnumerator_impl;
		friend class SchemaValidator_impl;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor - creates an empty document */
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

		/** @brief Destructor */
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
		// Factory
		//----------------------------------------------

		/**
		 * @brief Create document from JSON string
		 * @param jsonStr The JSON string to parse
		 * @return Optional document if parsing succeeds
		 */
		static std::optional<Document> fromJsonString( std::string_view jsonStr );

		/**
		 * @brief Create empty object document
		 * @return New document with empty object structure
		 */
		static Document createObject();

		/**
		 * @brief Create empty array document
		 * @return New document with empty array structure
		 */
		static Document createArray();

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
		 * @brief Convert document to JSON byte array
		 * @return Vector of bytes representing the JSON document
		 */
		std::vector<uint8_t> toJsonBytes() const;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Check if field exists at path
		 * @param path The path to check for field existence
		 * @return True if field exists, false otherwise
		 */
		bool hasField( std::string_view path ) const;

		/**
		 * @brief Get string value at path
		 * @param path The path to the string value
		 * @return Optional string value at the specified path, nullopt if not found or wrong type
		 */
		std::optional<std::string> getString( std::string_view path ) const;

		/**
		 * @brief Get integer value at path
		 * @param path The path to the integer value
		 * @return Optional integer value at the specified path, nullopt if not found or wrong type
		 */
		std::optional<int64_t> getInt( std::string_view path ) const;

		/**
		 * @brief Get double value at path
		 * @param path The path to the double value
		 * @return Optional double value at the specified path, nullopt if not found or wrong type
		 */
		std::optional<double> getDouble( std::string_view path ) const;

		/**
		 * @brief Get boolean value at path
		 * @param path The path to the boolean value
		 * @return Optional boolean value at the specified path, nullopt if not found or wrong type
		 */
		std::optional<bool> getBool( std::string_view path ) const;

		/**
		 * @brief Get document (any JSON type) at path
		 * @param path The path to the document
		 * @return Optional Document at the specified path, nullopt if not found
		 * @details Generic getter that returns any JSON structure as a Document.
		 *          Use this when you need maximum flexibility or don't know the type in advance.
		 *          For type-specific access, use the specialized getters (getString, getInt, etc.).
		 */
		std::optional<Document> getDocument( std::string_view path ) const;

		//----------------------------------------------
		// JSON Pointer access (RFC 6901)
		//----------------------------------------------

		/**
		 * @brief Check if field exists at JSON Pointer path (object fields only)
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/name", "/settings/enabled")
		 * @return True if pointer references a field in an object, false otherwise
		 * @details Only returns true for object fields. Array elements (e.g., "/items/0")
		 *          will return false since they are not object fields. Use hasValueByPointer()
		 *          for general existence checking that includes array elements.
		 *          Implements RFC 6901 JSON Pointer specification with object-field validation.
		 */
		bool hasFieldByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if any value exists at JSON Pointer path (fields, array elements, primitives)
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0", "/items/0", "/field", "/data/array/5")
		 * @return True if any value exists at the pointer path, false otherwise
		 * @details General-purpose existence checker that works with:
		 *          - Object fields ("/user/name")
		 *          - Array elements ("/items/0", "/data/5")
		 *          - Nested structures ("/users/0/hobbies/1")
		 *          - Root document ("")
		 *          Implements RFC 6901 JSON Pointer specification for all JSON value types.
		 */
		bool hasValueByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if array exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users", "/data/items")
		 * @return True if pointer references an array, false otherwise
		 * @details Type-specific existence checker that only returns true for arrays.
		 *          Use for type validation before calling getArrayByPointer().
		 */
		bool hasArrayByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if object exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/profile", "/settings")
		 * @return True if pointer references an object, false otherwise
		 * @details Type-specific existence checker that only returns true for objects.
		 *          Use for type validation before calling getObjectByPointer().
		 */
		bool hasObjectByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if string exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/name", "/data/0/title")
		 * @return True if pointer references a string, false otherwise
		 * @details Type-specific existence checker that only returns true for strings.
		 *          Use for type validation before calling getStringByPointer().
		 */
		bool hasStringByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if integer exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/age", "/items/0/count")
		 * @return True if pointer references an integer, false otherwise
		 * @details Type-specific existence checker that only returns true for integers.
		 *          Use for type validation before calling getIntByPointer().
		 */
		bool hasIntByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if double exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/metrics/0/value", "/coordinates/lat")
		 * @return True if pointer references a double, false otherwise
		 * @details Type-specific existence checker that only returns true for doubles.
		 *          Use for type validation before calling getDoubleByPointer().
		 */
		bool hasDoubleByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if boolean exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/settings/enabled", "/user/active")
		 * @return True if pointer references a boolean, false otherwise
		 * @details Type-specific existence checker that only returns true for booleans.
		 *          Use for type validation before calling getBoolByPointer().
		 */
		bool hasBoolByPointer( std::string_view pointer ) const;

		/**
		 * @brief Check if null exists at JSON Pointer path
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/optional/field", "/data/empty")
		 * @return True if pointer references null, false otherwise
		 * @details Type-specific existence checker that only returns true for null values.
		 *          Distinguishes between non-existent paths and explicit null values.
		 */
		bool hasNullByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get any document (array, object, or primitive) using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/profile")
		 * @return Optional Document at the specified pointer, nullopt if not found
		 * @details Returns the document at the pointer regardless of its type (array, object, or primitive).
		 */
		std::optional<Document> getDocumentByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get array document using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/hobbies")
		 * @return Optional Document representing the array at the specified pointer, nullopt if not found or wrong type
		 * @details Type-safe getter that only returns arrays. Use for compile-time type safety.
		 */
		std::optional<Document> getArrayByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get object document using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/profile")
		 * @return Optional Document representing the object at the specified pointer, nullopt if not found or wrong type
		 * @details Type-safe getter that only returns objects. Use for compile-time type safety.
		 */
		std::optional<Document> getObjectByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get string value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/profile/name")
		 * @return Optional string value at the specified pointer, nullopt if not found or wrong type
		 * @details Implements RFC 6901 JSON Pointer specification with support for:
		 *          - Object property access ("/user/name")
		 *          - Array element access ("/items/0")
		 *          - Escaped characters ("/field~1with~0slash")
		 *          - Root document access ("")
		 */
		std::optional<std::string> getStringByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get integer value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/age")
		 * @return Optional integer value at the specified pointer, nullopt if not found or wrong type
		 */
		std::optional<int64_t> getIntByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get double value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/metrics/0/value")
		 * @return Optional double value at the specified pointer, nullopt if not found or wrong type
		 */
		std::optional<double> getDoubleByPointer( std::string_view pointer ) const;

		/**
		 * @brief Get boolean value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/settings/enabled")
		 * @return Optional boolean value at the specified pointer, nullopt if not found or wrong type
		 */
		std::optional<bool> getBoolByPointer( std::string_view pointer ) const;

		//----------------------------------------------
		// Value setting
		//----------------------------------------------

		/**
		 * @brief Set string value at path
		 * @param path The path where to set the string value
		 * @param value The string value to set
		 */
		void setString( std::string_view path, std::string_view value );

		/**
		 * @brief Set integer value at path
		 * @param path The path where to set the integer value
		 * @param value The integer value to set
		 */
		void setInt( std::string_view path, int64_t value );

		/**
		 * @brief Set double value at path
		 * @param path The path where to set the double value
		 * @param value The double value to set
		 */
		void setDouble( std::string_view path, double value );

		/**
		 * @brief Set boolean value at path
		 * @param path The path where to set the boolean value
		 * @param value The boolean value to set
		 */
		void setBool( std::string_view path, bool value );

		/**
		 * @brief Set null value at path
		 * @param path The path where to set null value
		 */
		void setNull( std::string_view path );

		/**
		 * @brief Set document (any JSON type) at path
		 * @param path The path where to set the document
		 * @param document The document to set (can be object, array, or primitive)
		 * @details Generic setter that can handle any JSON structure.
		 *          Use this for complex nested objects, arrays, or when maximum flexibility is needed.
		 *          For simple primitives, the specialized setters may be more convenient.
		 */
		void setDocument( std::string_view path, const Document& document );

		//----------------------------------------------
		// JSON Pointer value setting (RFC 6901)
		//----------------------------------------------

		/**
		 * @brief Set any document (array, object, or primitive) using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/profile")
		 * @param document The document containing data to set
		 * @details This is the generic method that handles all document types.
		 *          Creates intermediate objects and arrays as needed based on pointer structure.
		 */
		void setDocumentByPointer( std::string_view pointer, const Document& document );

		/**
		 * @brief Set array document using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/hobbies")
		 * @param arrayDocument The document containing array data to set
		 * @details Convenience wrapper around setDocumentByPointer for semantic clarity.
		 */
		void setArrayByPointer( std::string_view pointer, const Document& arrayDocument );

		/**
		 * @brief Set object document using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/profile")
		 * @param objectDocument The document containing object data to set
		 * @details Convenience wrapper around setDocumentByPointer for semantic clarity.
		 */
		void setObjectByPointer( std::string_view pointer, const Document& objectDocument );

		/**
		 * @brief Set string value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/name")
		 * @param value The string value to set
		 * @details Creates intermediate objects and arrays as needed based on pointer structure.
		 *          Numeric tokens create arrays, non-numeric tokens create objects.
		 */
		void setStringByPointer( std::string_view pointer, std::string_view value );

		/**
		 * @brief Set integer value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/users/0/age")
		 * @param value The integer value to set
		 */
		void setIntByPointer( std::string_view pointer, int64_t value );

		/**
		 * @brief Set double value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/metrics/0/value")
		 * @param value The double value to set
		 */
		void setDoubleByPointer( std::string_view pointer, double value );

		/**
		 * @brief Set boolean value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/settings/enabled")
		 * @param value The boolean value to set
		 */
		void setBoolByPointer( std::string_view pointer, bool value );

		/**
		 * @brief Set null value using JSON Pointer
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/optional/field")
		 */
		void setNullByPointer( std::string_view pointer );

		//----------------------------------------------
		// Array operations
		//----------------------------------------------

		/**
		 * @brief Get array size at path
		 * @param path The path to the array
		 * @return Size of the array at the specified path
		 */
		size_t getArraySize( std::string_view path ) const;

		/**
		 * @brief Add string value to array at path
		 * @param path The path to the array
		 * @param value The string value to add
		 */
		void addToArray( std::string_view path, std::string_view value );

		/**
		 * @brief Add C-string value to array at path
		 * @param path The path to the array
		 * @param value The C-string value to add
		 */
		void addToArray( std::string_view path, const char* value );

		/**
		 * @brief Add integer value to array at path
		 * @param path The path to the array
		 * @param value The integer value to add
		 */
		void addToArray( std::string_view path, int64_t value );

		/**
		 * @brief Add double value to array at path
		 * @param path The path to the array
		 * @param value The double value to add
		 */
		void addToArray( std::string_view path, double value );

		/**
		 * @brief Add boolean value to array at path
		 * @param path The path to the array
		 * @param value The boolean value to add
		 */
		void addToArray( std::string_view path, bool value );

		/**
		 * @brief Add document (any JSON type) to array at path
		 * @param path The path to the array
		 * @param document The document to add (can be object, array, or primitive)
		 * @details Generic method that can add complex nested structures to arrays.
		 *          Use this for objects, arrays, or when you need maximum flexibility.
		 *          For simple primitives, the specialized overloads may be more convenient.
		 */
		void addToArray( std::string_view path, const Document& document );

		/**
		 * @brief Clear all elements from array at path
		 * @param path The path to the array to clear
		 */
		void clearArray( std::string_view path );

		/**
		 * @brief Get string value from array element at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Optional string value of the array element, nullopt if array not found, index out of bounds, or wrong type
		 */
		std::optional<std::string> getArrayElementString( std::string_view path, size_t index ) const;

		/**
		 * @brief Get integer value from array element at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Optional integer value of the array element, nullopt if array not found, index out of bounds, or wrong type
		 */
		std::optional<int64_t> getArrayElementInt( std::string_view path, size_t index ) const;

		/**
		 * @brief Get double value from array element at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Optional double value of the array element, nullopt if array not found, index out of bounds, or wrong type
		 */
		std::optional<double> getArrayElementDouble( std::string_view path, size_t index ) const;

		/**
		 * @brief Get boolean value from array element at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Optional boolean value of the array element, nullopt if array not found, index out of bounds, or wrong type
		 */
		std::optional<bool> getArrayElementBool( std::string_view path, size_t index ) const;

		//----------------------------------------------
		// Advanced array and document operations
		//----------------------------------------------

		/**
		 * @brief Check if path points to an array
		 * @param path The path to check
		 * @return True if path points to an array, false otherwise
		 */
		bool isArray( std::string_view path ) const;

		/**
		 * @brief Get array element document at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Document representing the array element
		 */
		Document getArrayElement( std::string_view path, size_t index ) const;

		/**
		 * @brief Set entire array at path
		 * @param path The path where to set the array
		 * @param arrayDocument The document containing array data
		 */
		void setArray( std::string_view path, const Document& arrayDocument );

		/**
		 * @brief Add document to array (for array documents only)
		 * @param document The document to add to this array
		 */
		void addDocument( const Document& document );

		/**
		 * @brief Get number of elements in array document (returns 0 for non-array documents)
		 * @return Number of elements in the array document
		 */
		size_t size() const;

		//----------------------------------------------
		// Field operations
		//----------------------------------------------

		/**
		 * @brief Remove field at specified path
		 * @param path The path to the field to remove
		 * @return True if field was found and removed, false otherwise
		 */
		bool removeField( std::string_view path );

		//----------------------------------------------
		// Merge / update operations
		//----------------------------------------------

		/**
		 * @brief Merge another document into this one
		 * @param other The document to merge from
		 * @param overwriteArrays If true, arrays will be replaced; if false, arrays will be merged
		 */
		void merge( const Document& other, bool overwriteArrays = true );

		/**
		 * @brief Update field at path with document value
		 * @param path The path where to update the value
		 * @param value The document value to set at the path
		 */
		void update( std::string_view path, const Document& value );

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if path points to a string value
		 * @param path The path to check
		 * @return True if path points to a string, false otherwise
		 */
		bool isString( std::string_view path ) const;

		/**
		 * @brief Check if path points to an integer value
		 * @param path The path to check
		 * @return True if path points to an integer, false otherwise
		 */
		bool isInt( std::string_view path ) const;

		/**
		 * @brief Check if path points to a double value
		 * @param path The path to check
		 * @return True if path points to a double, false otherwise
		 */
		bool isDouble( std::string_view path ) const;

		/**
		 * @brief Check if path points to a boolean value
		 * @param path The path to check
		 * @return True if path points to a boolean, false otherwise
		 */
		bool isBool( std::string_view path ) const;

		/**
		 * @brief Check if path points to a null value
		 * @param path The path to check
		 * @return True if path points to null, false otherwise
		 */
		bool isNull( std::string_view path ) const;

		/**
		 * @brief Check if path points to an object
		 * @param path The path to check
		 * @return True if path points to an object, false otherwise
		 */
		bool isObject( std::string_view path ) const;

		//----------------------------------------------
		// Validation and error handling
		//----------------------------------------------

		/**
		 * @brief Check if document is valid
		 * @return True if document is valid, false otherwise
		 */
		bool isValid() const;

		/**
		 * @brief Get last error message from document operations
		 * @return Error message string, empty if no errors occurred
		 */
		std::string lastError() const;

		//----------------------------------------------
		// Character utility methods
		//----------------------------------------------

		/**
		 * @brief Set character value at path (stored as single-character string)
		 * @param path The path where to set the character value
		 * @param c The character to set
		 * @details Convenience method that stores a character as a single-character string.
		 *          JSON doesn't have a native character type, so characters are stored as strings.
		 */
		void setChar( std::string_view path, char c );

		/**
		 * @brief Get character value at path (from single-character string)
		 * @param path The path to the character value
		 * @return Optional character value, nullopt if not found, wrong type, or string length != 1
		 * @details Convenience method that extracts a character from a single-character string.
		 *          Returns nullopt if the path doesn't exist, isn't a string, or the string length is not exactly 1.
		 */
		std::optional<char> getChar( std::string_view path ) const;

		/**
		 * @brief Set character value using JSON Pointer (stored as single-character string)
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/initial", "/data/0/grade")
		 * @param c The character to set
		 * @details Convenience method that stores a character as a single-character string using JSON Pointer notation.
		 */
		void setCharByPointer( std::string_view pointer, char c );

		/**
		 * @brief Get character value using JSON Pointer (from single-character string)
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/initial", "/data/0/grade")
		 * @return Optional character value, nullopt if not found, wrong type, or string length != 1
		 * @details Convenience method that extracts a character from a single-character string using JSON Pointer notation.
		 */
		std::optional<char> getCharByPointer( std::string_view pointer ) const;

		/**
		 * @brief Add character value to array at path
		 * @param path The path to the array
		 * @param c The character to add (stored as single-character string)
		 * @details Convenience method that adds a character to an array as a single-character string.
		 */
		void addCharToArray( std::string_view path, char c );

		/**
		 * @brief Get character value from array element at index
		 * @param path The path to the array
		 * @param index The index of the element to get
		 * @return Optional character value, nullopt if array not found, index out of bounds, wrong type, or string length != 1
		 * @details Convenience method that extracts a character from an array element stored as a single-character string.
		 */
		std::optional<char> getArrayElementChar( std::string_view path, size_t index ) const;

		/**
		 * @brief Check if path points to a character value (single-character string)
		 * @param path The path to check
		 * @return True if path points to a single-character string, false otherwise
		 * @details Convenience method that checks if a path contains a character (single-character string).
		 */
		bool isChar( std::string_view path ) const;

		/**
		 * @brief Check if character exists at JSON Pointer path (single-character string)
		 * @param pointer RFC 6901 JSON Pointer (e.g., "/user/initial", "/grades/0")
		 * @return True if pointer references a single-character string, false otherwise
		 * @details Convenience method that checks if a JSON Pointer path contains a character.
		 */
		bool hasCharByPointer( std::string_view pointer ) const;

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};
} // namespace nfx::serialization::json
