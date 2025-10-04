/**
 * @file SchemaValidator_impl.h
 * @brief Pimpl implementation for SchemaValidator, providing JSON Schema validation
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace nfx::serialization::json
{
	// Forward declarations
	class ValidationResult;
	class Document;
	class Document_impl;

	//=====================================================================
	// SchemaValidator_impl class
	//=====================================================================

	class SchemaValidator_impl final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor
		 */
		SchemaValidator_impl();

		/**
		 * @brief Constructor with schema
		 * @param schema The schema document to load
		 */
		explicit SchemaValidator_impl( const Document& schema );

		/**
		 * @brief Copy constructor
		 * @param other The enumerator to copy from
		 */
		SchemaValidator_impl( const SchemaValidator_impl& other );

		/**
		 * @brief Move constructor
		 * @param other The enumerator to move from
		 */
		SchemaValidator_impl( SchemaValidator_impl&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~SchemaValidator_impl() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The enumerator to copy from
		 * @return Reference to this enumerator
		 */
		SchemaValidator_impl& operator=( const SchemaValidator_impl& other );

		/**
		 * @brief Move assignment operator
		 * @param other The enumerator to move from
		 * @return Reference to this enumerator
		 */
		SchemaValidator_impl& operator=( SchemaValidator_impl&& other ) noexcept;

	public:
		//----------------------------------------------
		// Validation methods
		//----------------------------------------------
		/**
		 * @brief Validate a document node against schema node
		 * @param document Document to validate
		 * @param schema Schema to validate against
		 * @param path Current JSON path for error reporting
		 * @param result ValidationResult to accumulate errors
		 */
		void validateNode( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate JSON type constraint
		 * @param document Document to validate
		 * @param schema Schema containing type constraint
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateType( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate required fields for objects
		 * @param document Document to validate
		 * @param schema Schema containing required constraint
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateRequired( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate object properties
		 * @param document Document to validate
		 * @param schema Schema containing properties definition
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate array items
		 * @param document Document to validate
		 * @param schema Schema containing items definition
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateArray( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate numeric constraints (minimum, maximum)
		 * @param document Document to validate
		 * @param schema Schema containing numeric constraints
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateNumericConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Validate string constraints (minLength, maxLength, pattern, format)
		 * @param document Document to validate
		 * @param schema Schema containing string constraints
		 * @param path Current JSON path
		 * @param result ValidationResult to accumulate errors
		 */
		void validateStringConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Check if a schema reference exists
		 * @param reference Reference path (e.g., "#/definitions/Package")
		 * @return True if reference exists, false otherwise
		 */
		bool referenceExists( std::string_view reference ) const noexcept;

		/**
		 * @brief Resolve schema reference ($ref)
		 * @param reference Reference path (e.g., "#/definitions/Package")
		 * @return Resolved schema Document, or empty Document if not found
		 */
		Document resolveReference( std::string_view reference ) const;

		/**
		 * @brief Get actual type name of document value at path
		 * @param document Document to check
		 * @param path JSON path to check
		 * @return Type name string (e.g., "string", "number", "object", "array")
		 */
		std::string actualType( const Document& document, std::string_view path ) const noexcept;

		/**
		 * @brief Validate additionalProperties constraint (strict validation)
		 * @param document Document to validate
		 * @param schema Schema to validate against
		 * @param path Current JSON path for error reporting
		 * @param result ValidationResult to accumulate errors
		 */
		void validateAdditionalProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

		/**
		 * @brief Extract a sub-document from a document at specified path
		 * @param document Source document
		 * @param path Dot-notation path to extract
		 * @return Sub-document at path, or empty document if not found
		 */
		Document extractSubDocument( const Document& document, std::string_view path ) const;

		/**
		 * @brief Convert JSON Pointer path to dot notation path
		 * @param jsonPointerPath JSON Pointer path (e.g., "properties/Package")
		 * @return Dot notation path (e.g., "properties.Package")
		 */
		std::string convertJsonPointerToDotPath( std::string_view jsonPointerPath ) const noexcept;

		//----------------------------------------------
		// Main validation interface
		//----------------------------------------------
		/**
		 * @brief Validate document against loaded schema
		 * @param document Document to validate
		 * @return ValidationResult with errors if any
		 */
		ValidationResult validate( const Document& document ) const;

		/**
		 * @brief Validate document at specific path against schema at path
		 * @param document Document to validate
		 * @param documentPath Path within document to validate
		 * @param schemaPath Path within schema to validate against
		 * @return ValidationResult with errors if any
		 */
		ValidationResult validateAtPath( const Document& document, std::string_view documentPath, std::string_view schemaPath ) const;

		//----------------------------------------------
		// Accessor methods
		//----------------------------------------------

		/**
		 * @brief Check if schema is loaded
		 * @return True if schema is loaded, false otherwise
		 */
		bool isSchemaLoaded() const noexcept { return m_schemaLoaded; }

		/**
		 * @brief Get the loaded schema
		 * @return Reference to the schema document
		 */
		const Document& getSchema() const { return *m_schema; }

		/**
		 * @brief Get strict mode setting
		 * @return True if strict mode is enabled, false otherwise
		 */
		bool isStrictMode() const noexcept { return m_strictMode; }

		/**
		 * @brief Get maximum validation depth
		 * @return Maximum depth setting (0 = unlimited)
		 */
		size_t getMaxDepth() const noexcept { return m_maxDepth; }

		/**
		 * @brief Set schema loaded state
		 * @param loaded New loaded state
		 */
		void setSchemaLoaded( bool loaded ) noexcept { m_schemaLoaded = loaded; }

		/**
		 * @brief Set the schema document
		 * @param schema New schema document
		 */
		void setSchema( const Document& schema );

		/**
		 * @brief Set strict mode
		 * @param strict New strict mode setting
		 */
		void setStrictMode( bool strict ) noexcept { m_strictMode = strict; }

		/**
		 * @brief Set maximum validation depth
		 * @param maxDepth New maximum depth setting
		 */
		void setMaxDepth( size_t maxDepth ) noexcept { m_maxDepth = maxDepth; }

	private:
		//----------------------------------------------
		// Member variables
		//----------------------------------------------

		std::unique_ptr<Document> m_schema; ///< Owned schema document
		bool m_schemaLoaded = false;		///< Whether a schema has been loaded
		bool m_strictMode = false;			///< Whether strict validation mode is enabled
		size_t m_maxDepth = 0;				///< Maximum validation depth (0 = unlimited)
	};
} // namespace nfx::serialization::json
