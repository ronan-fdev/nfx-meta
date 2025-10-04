/**
 * @file SchemaValidator.h
 * @brief JSON Schema validation for Document instances
 * @details Provides JSON Schema Draft 7 validation capabilities for verifying Document
 *          structure, types, constraints, and business rules against schema definitions.
 *          Supports comprehensive validation with detailed error reporting and path tracking.
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "nfx/config.h"
#include "Document.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// ValidationError class
	//=====================================================================

	/**
	 * @brief Represents a single JSON schema validation error
	 * @details Contains detailed information about validation failures including
	 *          the failed constraint, document path, expected vs actual values,
	 *          and human-readable error messages.
	 */
	class ValidationError final
	{
	public:
		/**
		 * @brief Simple error entry structure for convenient ValidationError construction
		 * @details Provides aggregate initialization syntax for creating validation errors
		 *          with all error details in a single struct. Useful for functional-style
		 *          error creation and collection patterns.
		 */
		struct ErrorEntry
		{
			std::string path;				///< JSON path where error occurred
			std::string message;			///< Human-readable error message
			std::string constraint;			///< Schema constraint that failed
			std::string expectedValue = {}; ///< Expected value or constraint
			std::string actualValue = {};	///< Actual value found in document
		};

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------
		/**
		 * @brief Construct validation error from ErrorEntry struct
		 * @param entry Error entry with all validation details
		 */
		explicit ValidationError( const ErrorEntry& entry );

		/**
		 * @brief Construct validation error
		 * @param path JSON path where validation failed
		 * @param message Human-readable error description
		 * @param constraint The schema constraint that failed (e.g., "type", "minLength")
		 * @param expectedValue Expected value or constraint (optional)
		 * @param actualValue Actual value found in document (optional)
		 */
		ValidationError( std::string path,
			std::string message,
			std::string constraint,
			std::string expectedValue = {},
			std::string actualValue = {} );

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor
		 */
		~ValidationError() = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the JSON path where validation failed
		 * @return Path string (e.g., "/users/0/email")
		 */
		NFX_CORE_INLINE const std::string& path() const { return m_error.path; }

		/**
		 * @brief Get human-readable error message
		 * @return Error description
		 */
		NFX_CORE_INLINE const std::string& message() const { return m_error.message; }

		/**
		 * @brief Get the constraint type that failed
		 * @return Constraint name (e.g., "type", "minLength", "required")
		 */
		NFX_CORE_INLINE const std::string& constraint() const { return m_error.constraint; }

		/**
		 * @brief Get expected value or constraint
		 * @return Expected value string, or empty if not applicable
		 */
		NFX_CORE_INLINE const std::string& expectedValue() const { return m_error.expectedValue; }

		/**
		 * @brief Get actual value found in document
		 * @return Actual value string, or empty if not applicable
		 */
		NFX_CORE_INLINE const std::string& actualValue() const { return m_error.actualValue; }

		/**
		 * @brief Get formatted error string
		 * @return Complete formatted error message with path and details
		 */
		std::string toString() const;

	private:
		ErrorEntry m_error;
	};

	//=====================================================================
	// ValidationResult class
	//=====================================================================

	/**
	 * @brief Result of JSON schema validation operation
	 * @details Contains validation success status and comprehensive error reporting
	 *          with all validation failures found during document validation.
	 */
	class ValidationResult final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct successful validation result
		 */
		ValidationResult() = default;

		/**
		 * @brief Construct validation result with errors
		 * @param errors List of validation errors found
		 */
		explicit ValidationResult( std::vector<ValidationError> errors );

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor
		 */
		~ValidationResult() = default;

		//----------------------------------------------
		// Status checking
		//----------------------------------------------

		/**
		 * @brief Check if validation was successful
		 * @return True if no validation errors, false otherwise
		 */
		NFX_CORE_INLINE bool isValid() const { return m_errors.empty(); }

		/**
		 * @brief Check if validation failed
		 * @return True if validation errors exist, false otherwise
		 */
		NFX_CORE_INLINE bool hasErrors() const { return !m_errors.empty(); }

		/**
		 * @brief Get number of validation errors
		 * @return Count of validation errors found
		 */
		NFX_CORE_INLINE size_t errorCount() const { return m_errors.size(); }

		//----------------------------------------------
		// Error access
		//----------------------------------------------

		/**
		 * @brief Get all validation errors
		 * @return Vector of validation errors
		 */
		NFX_CORE_INLINE const std::vector<ValidationError>& errors() const { return m_errors; }

		/**
		 * @brief Get validation error by index
		 * @param index Error index (0-based)
		 * @return Validation error at specified index
		 * @throws std::out_of_range if index is invalid
		 */
		const ValidationError& error( size_t index ) const;

		/**
		 * @brief Get formatted error summary
		 * @return Multi-line string with all validation errors
		 */
		std::string errorSummary() const;

		//----------------------------------------------
		// Error manipulation
		//----------------------------------------------

		/**
		 * @brief Add validation error from ErrorEntry
		 * @param entry The error entry to add
		 */
		void addError( const ValidationError::ErrorEntry& entry );

		/**
		 * @brief Add validation error to result
		 * @param error The validation error to add
		 */
		void addError( const ValidationError& error );

		/**
		 * @brief Add validation error with details
		 * @param path JSON path where validation failed
		 * @param message Human-readable error description
		 * @param constraint The schema constraint that failed
		 * @param expectedValue Expected value or constraint (optional)
		 * @param actualValue Actual value found in document (optional)
		 */
		void addError( std::string_view path,
			std::string_view message,
			std::string_view constraint,
			std::string_view expectedValue = {},
			std::string_view actualValue = {} );

	private:
		std::vector<ValidationError> m_errors;
	};

	//=====================================================================
	// SchemaValidator class
	//=====================================================================

	/**
	 * @brief JSON Schema validator for Document instances
	 * @details Provides comprehensive JSON Schema Draft 7 validation capabilities
	 *          including type checking, constraint validation, object/array validation,
	 *          and custom validation rules. Supports schema loading from Documents
	 *          and detailed error reporting with path tracking.
	 */
	class SchemaValidator final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor - creates empty validator
		 * @details Creates validator without schema. Must load schema before validation.
		 */
		SchemaValidator();

		/**
		 * @brief Construct validator with schema
		 * @param schema JSON Schema document to use for validation
		 */
		explicit SchemaValidator( const Document& schema );

		/**
		 * @brief Copy constructor
		 * @param other The validator to copy from
		 */
		SchemaValidator( const SchemaValidator& other );

		/**
		 * @brief Move constructor
		 * @param other The validator to move from
		 */
		SchemaValidator( SchemaValidator&& other ) noexcept;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/**
		 * @brief Destructor
		 */
		~SchemaValidator();

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The validator to copy from
		 * @return Reference to this validator
		 */
		SchemaValidator& operator=( const SchemaValidator& other );

		/**
		 * @brief Move assignment operator
		 * @param other The validator to move from
		 * @return Reference to this validator
		 */
		SchemaValidator& operator=( SchemaValidator&& other ) noexcept;

		//----------------------------------------------
		// Schema management
		//----------------------------------------------

		/**
		 * @brief Load JSON schema from Document
		 * @param schema JSON Schema document
		 * @return True if schema loaded successfully, false on parse error
		 */
		bool loadSchema( const Document& schema );

		/**
		 * @brief Load JSON schema from string
		 * @param schemaJson JSON Schema as string
		 * @return True if schema loaded successfully, false on parse error
		 */
		bool loadSchemaFromString( std::string_view schemaJson );

		/**
		 * @brief Check if validator has valid schema loaded
		 * @return True if schema is loaded and valid, false otherwise
		 */
		bool hasSchema() const;

		/**
		 * @brief Clear loaded schema
		 */
		void clearSchema();

		/**
		 * @brief Get current schema as Document
		 * @return Current schema Document, or empty Document if no schema loaded
		 */
		Document schema() const;

		//----------------------------------------------
		// Validation operations
		//----------------------------------------------

		/**
		 * @brief Validate Document against loaded schema
		 * @param document The JSON document to validate
		 * @return ValidationResult with success status and error details
		 * @throws std::runtime_error if no schema is loaded
		 */
		ValidationResult validate( const Document& document ) const;

		/**
		 * @brief Validate Document at specific path against schema
		 * @param document The JSON document to validate
		 * @param documentPath Path within document to validate (empty for root)
		 * @param schemaPath Path within schema to validate against (empty for root schema)
		 * @return ValidationResult with success status and error details
		 */
		ValidationResult validateAtPath( const Document& document,
			std::string_view documentPath = "",
			std::string_view schemaPath = "" ) const;

		/**
		 * @brief Quick validation check without detailed errors
		 * @param document The JSON document to validate
		 * @return True if document is valid, false otherwise
		 */
		bool isValid( const Document& document ) const;

		//----------------------------------------------
		// Schema information
		//----------------------------------------------

		/**
		 * @brief Get schema version/draft
		 * @return Schema version string (e.g., "draft-07"), or empty if not specified
		 */
		std::string schemaVersion() const;

		/**
		 * @brief Get schema title
		 * @return Schema title string, or empty if not specified
		 */
		std::string schemaTitle() const;

		/**
		 * @brief Get schema description
		 * @return Schema description string, or empty if not specified
		 */
		std::string schemaDescription() const;

		//----------------------------------------------
		// Validation configuration
		//----------------------------------------------

		/**
		 * @brief Set strict validation mode
		 * @param strict If true, unknown properties cause validation failure
		 */
		void setStrictMode( bool strict );

		/**
		 * @brief Check if strict validation mode is enabled
		 * @return True if strict mode is enabled, false otherwise
		 */
		bool isStrictMode() const;

		/**
		 * @brief Set maximum validation depth for nested objects/arrays
		 * @param maxDepth Maximum recursion depth (0 = unlimited)
		 */
		void setMaxDepth( size_t maxDepth );

		/**
		 * @brief Get maximum validation depth
		 * @return Current maximum recursion depth (0 = unlimited)
		 */
		size_t maxDepth() const;

	private:
		//----------------------------------------------
		// Pimpl
		//----------------------------------------------

		void* m_impl;
	};
} // namespace nfx::serialization::json
