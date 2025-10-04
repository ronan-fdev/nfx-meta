/**
 * @file TESTS_JSONSchemaValidator.cpp
 * @brief Comprehensive tests for JSON Schema validation functionality
 * @details Tests covering ValidationError, ValidationResult, and SchemaValidator classes
 *          with complete API coverage, error handling, and validation scenarios.
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/SchemaValidator.h>
#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// ValidationError tests
	//=====================================================================

	//----------------------------------------------
	// ValidationError test fixture
	//----------------------------------------------

	class ValidationErrorTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create test error data
			errorEntry = {
				"user.email",
				"Invalid email format",
				"format",
				"valid email",
				"invalid@" };
		}

		ValidationError::ErrorEntry errorEntry;
	};

	TEST_F( ValidationErrorTest, ConstructorFromErrorEntry )
	{
		ValidationError error( errorEntry );

		EXPECT_EQ( error.path(), "user.email" );
		EXPECT_EQ( error.message(), "Invalid email format" );
		EXPECT_EQ( error.constraint(), "format" );
		EXPECT_EQ( error.expectedValue(), "valid email" );
		EXPECT_EQ( error.actualValue(), "invalid@" );
	}

	TEST_F( ValidationErrorTest, ConstructorWithParameters )
	{
		ValidationError error( "user.age", "Value too low", "minimum", "18", "16" );

		EXPECT_EQ( error.path(), "user.age" );
		EXPECT_EQ( error.message(), "Value too low" );
		EXPECT_EQ( error.constraint(), "minimum" );
		EXPECT_EQ( error.expectedValue(), "18" );
		EXPECT_EQ( error.actualValue(), "16" );
	}

	TEST_F( ValidationErrorTest, ConstructorWithOptionalParameters )
	{
		ValidationError error( "user.name", "Required field missing", "required" );

		EXPECT_EQ( error.path(), "user.name" );
		EXPECT_EQ( error.message(), "Required field missing" );
		EXPECT_EQ( error.constraint(), "required" );
		EXPECT_EQ( error.expectedValue(), "" );
		EXPECT_EQ( error.actualValue(), "" );
	}

	TEST_F( ValidationErrorTest, ToStringFormatting )
	{
		ValidationError error( "user.email", "Invalid format", "format", "email", "not-email" );
		std::string errorStr = error.toString();

		EXPECT_NE( errorStr.find( "user.email" ), std::string::npos );
		EXPECT_NE( errorStr.find( "Invalid format" ), std::string::npos );
		EXPECT_NE( errorStr.find( "format" ), std::string::npos );
		EXPECT_NE( errorStr.find( "email" ), std::string::npos );
		EXPECT_NE( errorStr.find( "not-email" ), std::string::npos );
	}

	TEST_F( ValidationErrorTest, ToStringWithoutOptionalValues )
	{
		ValidationError error( "user.name", "Field missing", "required" );
		std::string errorStr = error.toString();

		EXPECT_NE( errorStr.find( "user.name" ), std::string::npos );
		EXPECT_NE( errorStr.find( "Field missing" ), std::string::npos );
		EXPECT_NE( errorStr.find( "required" ), std::string::npos );
	}

	//=====================================================================
	// ValidationResult tests
	//=====================================================================

	//----------------------------------------------
	// ValidationResult test fixture
	//----------------------------------------------

	class ValidationResultTest : public ::testing::Test
	{
	protected:
		ValidationError error1{ "user.name", "Required field missing", "required", "name", "undefined" };
		ValidationError error2{ "user.age", "Type mismatch", "type", "integer", "string" };
		ValidationError error3{ "user.email", "Invalid format", "format", "email", "invalid" };
	};

	TEST_F( ValidationResultTest, DefaultConstructorCreatesValidResult )
	{
		ValidationResult result;

		EXPECT_TRUE( result.isValid() );
		EXPECT_FALSE( result.hasErrors() );
		EXPECT_EQ( result.errorCount(), 0 );
		EXPECT_TRUE( result.errors().empty() );
	}

	TEST_F( ValidationResultTest, ConstructorWithErrors )
	{
		std::vector<ValidationError> errors = { error1, error2 };
		ValidationResult result( errors );

		EXPECT_FALSE( result.isValid() );
		EXPECT_TRUE( result.hasErrors() );
		EXPECT_EQ( result.errorCount(), 2 );
		EXPECT_EQ( result.errors().size(), 2 );
	}

	TEST_F( ValidationResultTest, AddErrorFromErrorEntry )
	{
		ValidationResult result;
		ValidationError::ErrorEntry entry = { "test.path", "Test message", "test", "expected", "actual" };

		result.addError( entry );

		EXPECT_FALSE( result.isValid() );
		EXPECT_EQ( result.errorCount(), 1 );
		EXPECT_EQ( result.error( 0 ).path(), "test.path" );
	}

	TEST_F( ValidationResultTest, AddErrorFromValidationError )
	{
		ValidationResult result;

		result.addError( error1 );

		EXPECT_FALSE( result.isValid() );
		EXPECT_EQ( result.errorCount(), 1 );
		EXPECT_EQ( result.error( 0 ).path(), "user.name" );
	}

	TEST_F( ValidationResultTest, AddErrorWithParameters )
	{
		ValidationResult result;

		result.addError( "user.score", "Value out of range", "maximum", "100", "150" );

		EXPECT_FALSE( result.isValid() );
		EXPECT_EQ( result.errorCount(), 1 );
		EXPECT_EQ( result.error( 0 ).path(), "user.score" );
		EXPECT_EQ( result.error( 0 ).message(), "Value out of range" );
	}

	TEST_F( ValidationResultTest, MultipleErrors )
	{
		ValidationResult result;

		result.addError( error1 );
		result.addError( error2 );
		result.addError( error3 );

		EXPECT_FALSE( result.isValid() );
		EXPECT_TRUE( result.hasErrors() );
		EXPECT_EQ( result.errorCount(), 3 );

		EXPECT_EQ( result.error( 0 ).path(), "user.name" );
		EXPECT_EQ( result.error( 1 ).path(), "user.age" );
		EXPECT_EQ( result.error( 2 ).path(), "user.email" );
	}

	TEST_F( ValidationResultTest, ErrorAccessOutOfRange )
	{
		ValidationResult result;
		result.addError( error1 );

		EXPECT_THROW( result.error( 1 ), std::out_of_range );
		EXPECT_THROW( result.error( 10 ), std::out_of_range );
	}

	TEST_F( ValidationResultTest, ErrorSummaryFormatting )
	{
		ValidationResult result;
		result.addError( error1 );
		result.addError( error2 );

		std::string summary = result.errorSummary();

		EXPECT_NE( summary.find( "2 error" ), std::string::npos );
		EXPECT_NE( summary.find( "user.name" ), std::string::npos );
		EXPECT_NE( summary.find( "user.age" ), std::string::npos );
		EXPECT_NE( summary.find( "1." ), std::string::npos );
		EXPECT_NE( summary.find( "2." ), std::string::npos );
	}

	TEST_F( ValidationResultTest, ErrorSummaryForValidResult )
	{
		ValidationResult result;
		std::string summary = result.errorSummary();

		EXPECT_NE( summary.find( "No validation errors" ), std::string::npos );
	}

	//=====================================================================
	// SchemaValidator tests
	//=====================================================================

	//----------------------------------------------
	// SchemaValidator test fixture
	//----------------------------------------------

	class SchemaValidatorTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create basic test schema
			basicSchemaJson = R"({
				"$schema": "https://json-schema.org/draft/2019-09/schema",
				"title": "User Schema",
				"description": "A schema for user data validation",
				"type": "object",
				"properties": {
					"name": {
						"type": "string",
						"minLength": 2,
						"maxLength": 50
					},
					"age": {
						"type": "integer",
						"minimum": 0,
						"maximum": 150
					},
					"email": {
						"type": "string",
						"format": "date-time"
					},
					"active": {
						"type": "boolean"
					}
				},
				"required": ["name", "age"],
				"additionalProperties": false
			})";

			auto maybeSchema = Document::fromJsonString( basicSchemaJson );
			ASSERT_TRUE( maybeSchema.has_value() );
			basicSchema = std::move( maybeSchema.value() );

			// Create schema with references
			schemaWithRefsJson = R"({
				"$schema": "https://json-schema.org/draft/2019-09/schema",
				"title": "Organization Schema",
				"type": "object",
				"properties": {
					"company": {
						"$ref": "#/definitions/Company"
					},
					"employees": {
						"type": "array",
						"items": {
							"$ref": "#/definitions/Employee"
						}
					}
				},
				"definitions": {
					"Company": {
						"type": "object",
						"properties": {
							"name": { "type": "string" },
							"founded": { "type": "integer" }
						},
						"required": ["name"]
					},
					"Employee": {
						"type": "object",
						"properties": {
							"id": { "type": "integer" },
							"name": { "type": "string" },
							"department": { "type": "string" }
						},
						"required": ["id", "name"]
					}
				}
			})";

			auto maybeRefSchema = Document::fromJsonString( schemaWithRefsJson );
			ASSERT_TRUE( maybeRefSchema.has_value() );
			schemaWithRefs = std::move( maybeRefSchema.value() );

			// Create valid test document
			validDocumentJson = R"({
				"name": "John Doe",
				"age": 30,
				"email": "2023-10-03T14:30:00Z",
				"active": true
			})";

			auto maybeValidDoc = Document::fromJsonString( validDocumentJson );
			ASSERT_TRUE( maybeValidDoc.has_value() );
			validDocument = std::move( maybeValidDoc.value() );

			// Create invalid test document
			invalidDocumentJson = R"({
				"name": "A",
				"age": "not-a-number",
				"email": "invalid-date",
				"active": "not-boolean",
				"extra": "property"
			})";

			auto maybeInvalidDoc = Document::fromJsonString( invalidDocumentJson );
			ASSERT_TRUE( maybeInvalidDoc.has_value() );
			invalidDocument = std::move( maybeInvalidDoc.value() );
		}

		std::string basicSchemaJson;
		std::string schemaWithRefsJson;
		std::string validDocumentJson;
		std::string invalidDocumentJson;

		Document basicSchema;
		Document schemaWithRefs;
		Document validDocument;
		Document invalidDocument;
	};

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, DefaultConstructor )
	{
		SchemaValidator validator;

		EXPECT_FALSE( validator.hasSchema() );
		EXPECT_EQ( validator.schemaVersion(), "" );
		EXPECT_EQ( validator.schemaTitle(), "" );
		EXPECT_EQ( validator.schemaDescription(), "" );
		EXPECT_FALSE( validator.isStrictMode() );
		EXPECT_EQ( validator.maxDepth(), 0 );
	}

	TEST_F( SchemaValidatorTest, ConstructorWithSchema )
	{
		SchemaValidator validator( basicSchema );

		EXPECT_TRUE( validator.hasSchema() );
		EXPECT_EQ( validator.schemaTitle(), "User Schema" );
		EXPECT_EQ( validator.schemaDescription(), "A schema for user data validation" );
	}

	TEST_F( SchemaValidatorTest, CopyConstructor )
	{
		SchemaValidator original( basicSchema );
		original.setStrictMode( true );
		original.setMaxDepth( 10 );

		SchemaValidator copy( original );

		EXPECT_TRUE( copy.hasSchema() );
		EXPECT_EQ( copy.schemaTitle(), original.schemaTitle() );
		EXPECT_EQ( copy.isStrictMode(), original.isStrictMode() );
		EXPECT_EQ( copy.maxDepth(), original.maxDepth() );
	}

	TEST_F( SchemaValidatorTest, MoveConstructor )
	{
		SchemaValidator original( basicSchema );
		std::string expectedTitle = original.schemaTitle();

		SchemaValidator moved( std::move( original ) );

		EXPECT_TRUE( moved.hasSchema() );
		EXPECT_EQ( moved.schemaTitle(), expectedTitle );
		EXPECT_FALSE( original.hasSchema() ); // Original should be moved from
	}

	TEST_F( SchemaValidatorTest, CopyAssignment )
	{
		SchemaValidator validator1( basicSchema );
		SchemaValidator validator2;

		validator2 = validator1;

		EXPECT_TRUE( validator2.hasSchema() );
		EXPECT_EQ( validator2.schemaTitle(), validator1.schemaTitle() );
	}

	TEST_F( SchemaValidatorTest, MoveAssignment )
	{
		SchemaValidator validator1( basicSchema );
		std::string expectedTitle = validator1.schemaTitle();
		SchemaValidator validator2;

		validator2 = std::move( validator1 );

		EXPECT_TRUE( validator2.hasSchema() );
		EXPECT_EQ( validator2.schemaTitle(), expectedTitle );
		EXPECT_FALSE( validator1.hasSchema() ); // Original should be moved from
	}

	//----------------------------------------------
	// Schema management
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, LoadSchemaFromDocument )
	{
		SchemaValidator validator;

		EXPECT_TRUE( validator.loadSchema( basicSchema ) );
		EXPECT_TRUE( validator.hasSchema() );
		EXPECT_EQ( validator.schemaTitle(), "User Schema" );
	}

	TEST_F( SchemaValidatorTest, LoadSchemaFromString )
	{
		SchemaValidator validator;

		EXPECT_TRUE( validator.loadSchemaFromString( basicSchemaJson ) );
		EXPECT_TRUE( validator.hasSchema() );
		EXPECT_EQ( validator.schemaTitle(), "User Schema" );
	}

	TEST_F( SchemaValidatorTest, LoadSchemaFromInvalidString )
	{
		SchemaValidator validator;
		std::string invalidJson = "{ invalid json }";

		EXPECT_FALSE( validator.loadSchemaFromString( invalidJson ) );
		EXPECT_FALSE( validator.hasSchema() );
	}

	TEST_F( SchemaValidatorTest, LoadInvalidSchema )
	{
		SchemaValidator validator;

		// Test with a non-object schema (JSON Schema must be an object)
		auto arraySchemaOpt = Document::fromJsonString( "[]" );
		ASSERT_TRUE( arraySchemaOpt.has_value() );
		Document arraySchema = arraySchemaOpt.value();

		EXPECT_FALSE( validator.loadSchema( arraySchema ) );
		EXPECT_FALSE( validator.hasSchema() );

		// Test with a string schema (JSON Schema must be an object)
		auto stringSchemaOpt = Document::fromJsonString( "\"not a schema\"" );
		ASSERT_TRUE( stringSchemaOpt.has_value() );
		Document stringSchema = stringSchemaOpt.value();

		EXPECT_FALSE( validator.loadSchema( stringSchema ) );
		EXPECT_FALSE( validator.hasSchema() );

		// Test with number schema (JSON Schema must be an object)
		auto numberSchemaOpt = Document::fromJsonString( "42" );
		ASSERT_TRUE( numberSchemaOpt.has_value() );
		Document numberSchema = numberSchemaOpt.value();

		EXPECT_FALSE( validator.loadSchema( numberSchema ) );
		EXPECT_FALSE( validator.hasSchema() );
	}

	TEST_F( SchemaValidatorTest, LoadValidEmptySchema )
	{
		SchemaValidator validator;

		// Empty object is a valid JSON Schema (matches anything)
		Document emptySchema; // Default-constructed = {}

		EXPECT_TRUE( validator.loadSchema( emptySchema ) );
		EXPECT_TRUE( validator.hasSchema() );

		// Also test with explicit empty object
		auto explicitEmptyOpt = Document::fromJsonString( "{}" );
		ASSERT_TRUE( explicitEmptyOpt.has_value() );
		Document explicitEmpty = explicitEmptyOpt.value();

		validator.clearSchema();
		EXPECT_TRUE( validator.loadSchema( explicitEmpty ) );
		EXPECT_TRUE( validator.hasSchema() );
	}

	TEST_F( SchemaValidatorTest, LoadSchemaFromInvalidJsonString )
	{
		SchemaValidator validator;

		// Test with empty string (not valid JSON)
		EXPECT_FALSE( validator.loadSchemaFromString( "" ) );
		EXPECT_FALSE( validator.hasSchema() );

		// Test with whitespace-only string (not valid JSON)
		EXPECT_FALSE( validator.loadSchemaFromString( "   \n\t  " ) );
		EXPECT_FALSE( validator.hasSchema() );

		// Test with malformed JSON
		EXPECT_FALSE( validator.loadSchemaFromString( "{invalid json}" ) );
		EXPECT_FALSE( validator.hasSchema() );

		// Test with valid JSON but non-object (should fail schema loading)
		EXPECT_FALSE( validator.loadSchemaFromString( "\"string\"" ) );
		EXPECT_FALSE( validator.hasSchema() );

		EXPECT_FALSE( validator.loadSchemaFromString( "123" ) );
		EXPECT_FALSE( validator.hasSchema() );

		EXPECT_FALSE( validator.loadSchemaFromString( "[]" ) );
		EXPECT_FALSE( validator.hasSchema() );
	}

	TEST_F( SchemaValidatorTest, ClearSchema )
	{
		SchemaValidator validator( basicSchema );
		EXPECT_TRUE( validator.hasSchema() );

		validator.clearSchema();

		EXPECT_FALSE( validator.hasSchema() );
		EXPECT_EQ( validator.schemaTitle(), "" );
		EXPECT_EQ( validator.schemaDescription(), "" );
	}

	TEST_F( SchemaValidatorTest, GetSchema )
	{
		SchemaValidator validator( basicSchema );

		Document retrievedSchema = validator.schema();
		auto title = retrievedSchema.getString( "title" );
		EXPECT_TRUE( title.has_value() );
		EXPECT_EQ( title.value(), "User Schema" );
	}

	//----------------------------------------------
	// Schema information
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, SchemaVersion )
	{
		SchemaValidator validator( basicSchema );
		EXPECT_EQ( validator.schemaVersion(), "https://json-schema.org/draft/2019-09/schema" );
	}

	TEST_F( SchemaValidatorTest, SchemaTitle )
	{
		SchemaValidator validator( basicSchema );
		EXPECT_EQ( validator.schemaTitle(), "User Schema" );
	}

	TEST_F( SchemaValidatorTest, SchemaDescription )
	{
		SchemaValidator validator( basicSchema );
		EXPECT_EQ( validator.schemaDescription(), "A schema for user data validation" );
	}

	TEST_F( SchemaValidatorTest, SchemaInfoWithoutSchema )
	{
		SchemaValidator validator;

		EXPECT_EQ( validator.schemaVersion(), "" );
		EXPECT_EQ( validator.schemaTitle(), "" );
		EXPECT_EQ( validator.schemaDescription(), "" );
	}

	//----------------------------------------------
	// Validation configuration
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, StrictModeConfiguration )
	{
		SchemaValidator validator;

		EXPECT_FALSE( validator.isStrictMode() );

		validator.setStrictMode( true );
		EXPECT_TRUE( validator.isStrictMode() );

		validator.setStrictMode( false );
		EXPECT_FALSE( validator.isStrictMode() );
	}

	TEST_F( SchemaValidatorTest, MaxDepthConfiguration )
	{
		SchemaValidator validator;

		EXPECT_EQ( validator.maxDepth(), 0 );

		validator.setMaxDepth( 10 );
		EXPECT_EQ( validator.maxDepth(), 10 );

		validator.setMaxDepth( 0 );
		EXPECT_EQ( validator.maxDepth(), 0 );
	}

	//----------------------------------------------
	// Basic validation
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, ValidateValidDocument )
	{
		SchemaValidator validator( basicSchema );

		ValidationResult result = validator.validate( validDocument );

		EXPECT_TRUE( result.isValid() );
		EXPECT_EQ( result.errorCount(), 0 );
	}

	TEST_F( SchemaValidatorTest, ValidateInvalidDocument )
	{
		SchemaValidator validator( basicSchema );

		ValidationResult result = validator.validate( invalidDocument );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GT( result.errorCount(), 0 );
	}

	TEST_F( SchemaValidatorTest, ValidateWithoutSchema )
	{
		SchemaValidator validator;

		EXPECT_THROW( validator.validate( validDocument ), std::runtime_error );
	}

	TEST_F( SchemaValidatorTest, ValidateInvalidJsonDocument )
	{
		SchemaValidator validator( basicSchema );
		Document emptyDoc; // Default-constructed document = {} (valid JSON but invalid schema)

		ValidationResult result = validator.validate( emptyDoc );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GT( result.errorCount(), 0 );
		EXPECT_TRUE( result.error( 0 ).message().find( "required" ) != std::string::npos ||
					 result.error( 0 ).constraint() == "required" );
	}

	TEST_F( SchemaValidatorTest, IsValidMethod )
	{
		SchemaValidator validator( basicSchema );

		EXPECT_TRUE( validator.isValid( validDocument ) );
		EXPECT_FALSE( validator.isValid( invalidDocument ) );
	}

	TEST_F( SchemaValidatorTest, IsValidWithoutSchema )
	{
		SchemaValidator validator;

		EXPECT_FALSE( validator.isValid( validDocument ) );
	}

	//----------------------------------------------
	// Path-specific validation
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, ValidateAtPathBasic )
	{
		SchemaValidator validator( schemaWithRefs );

		std::string docJson = R"({
			"company": {
				"name": "Acme Corp",
				"founded": 1990
			},
			"employees": [
				{
					"id": 1,
					"name": "John Doe",
					"department": "Engineering"
				}
			]
		})";

		auto maybeDoc = Document::fromJsonString( docJson );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document document = maybeDoc.value();

		// Validate entire document
		ValidationResult fullResult = validator.validateAtPath( document );
		EXPECT_TRUE( fullResult.isValid() );

		// Validate just company
		ValidationResult companyResult = validator.validateAtPath( document, "company", "#/definitions/Company" );
		EXPECT_TRUE( companyResult.isValid() );

		// Validate specific employee
		ValidationResult employeeResult = validator.validateAtPath( document, "employees[0]", "#/definitions/Employee" );
		EXPECT_TRUE( employeeResult.isValid() );
	}

	TEST_F( SchemaValidatorTest, ValidateAtPathInvalidDocumentPath )
	{
		SchemaValidator validator( basicSchema );

		ValidationResult result = validator.validateAtPath( validDocument, "nonexistent.path" );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GT( result.errorCount(), 0 );
		EXPECT_NE( result.error( 0 ).message().find( "Document path not found" ), std::string::npos );
	}

	TEST_F( SchemaValidatorTest, ValidateAtPathInvalidSchemaPath )
	{
		SchemaValidator validator( basicSchema );

		ValidationResult result = validator.validateAtPath( validDocument, "name", "#/definitions/NonExistent" );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GT( result.errorCount(), 0 );
		EXPECT_NE( result.error( 0 ).message().find( "Schema path not found:" ), std::string::npos );
	}

	TEST_F( SchemaValidatorTest, ValidateAtPathWithoutSchema )
	{
		SchemaValidator validator;

		EXPECT_THROW( validator.validateAtPath( validDocument ), std::runtime_error );
	}

	//----------------------------------------------
	// Validation constraint
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, TypeValidationErrors )
	{
		SchemaValidator validator( basicSchema );

		std::string wrongTypeJson = R"({
			"name": 123,
			"age": "not-a-number",
			"active": "not-boolean"
		})";

		auto maybeDoc = Document::fromJsonString( wrongTypeJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 3 );

		// Check for type mismatch errors
		bool foundNameError = false, foundAgeError = false, foundActiveError = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "name" && error.constraint() == "type" )
			{
				foundNameError = true;
			}
			if ( error.path() == "age" && error.constraint() == "type" )
			{
				foundAgeError = true;
			}
			if ( error.path() == "active" && error.constraint() == "type" )
			{
				foundActiveError = true;
			}
		}

		EXPECT_TRUE( foundNameError );
		EXPECT_TRUE( foundAgeError );
		EXPECT_TRUE( foundActiveError );
	}

	TEST_F( SchemaValidatorTest, RequiredFieldValidation )
	{
		SchemaValidator validator( basicSchema );

		std::string missingFieldsJson = R"({
			"email": "2023-10-03T14:30:00Z",
			"active": true
		})";

		auto maybeDoc = Document::fromJsonString( missingFieldsJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 2 );

		// Check for required field errors
		bool foundNameRequired = false, foundAgeRequired = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "name" && error.constraint() == "required" )
			{
				foundNameRequired = true;
			}
			if ( error.path() == "age" && error.constraint() == "required" )
			{
				foundAgeRequired = true;
			}
		}

		EXPECT_TRUE( foundNameRequired );
		EXPECT_TRUE( foundAgeRequired );
	}

	TEST_F( SchemaValidatorTest, StringConstraintValidation )
	{
		SchemaValidator validator( basicSchema );

		std::string constraintViolationJson = R"({
			"name": "A",
			"age": 25
		})";

		auto maybeDoc = Document::fromJsonString( constraintViolationJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 1 );

		// Check for minLength error
		bool foundMinLengthError = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "name" && error.constraint() == "minLength" )
			{
				foundMinLengthError = true;
			}
		}

		EXPECT_TRUE( foundMinLengthError );
	}

	TEST_F( SchemaValidatorTest, NumericConstraintValidation )
	{
		SchemaValidator validator( basicSchema );

		std::string constraintViolationJson = R"({
			"name": "John Doe",
			"age": -5
		})";

		auto maybeDoc = Document::fromJsonString( constraintViolationJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 1 );

		// Check for minimum error
		bool foundMinimumError = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "age" && error.constraint() == "minimum" )
			{
				foundMinimumError = true;
			}
		}

		EXPECT_TRUE( foundMinimumError );
	}

	TEST_F( SchemaValidatorTest, AdditionalPropertiesValidation )
	{
		SchemaValidator validator( basicSchema );

		std::string extraPropertyJson = R"({
			"name": "John Doe",
			"age": 30,
			"extraProperty": "not allowed"
		})";

		auto maybeDoc = Document::fromJsonString( extraPropertyJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 1 );

		// Check for additional property error
		bool foundAdditionalPropError = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "extraProperty" && error.constraint() == "additionalProperties" )
			{
				foundAdditionalPropError = true;
			}
		}

		EXPECT_TRUE( foundAdditionalPropError );
	}

	//----------------------------------------------
	// Array validation
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, ArrayValidationWithReferences )
	{
		SchemaValidator validator( schemaWithRefs );

		std::string arrayDocJson = R"({
			"company": {
				"name": "Test Company",
				"founded": 2000
			},
			"employees": [
				{
					"id": 1,
					"name": "Alice Smith",
					"department": "HR"
				},
				{
					"id": 2,
					"name": "Bob Jones",
					"department": "IT"
				}
			]
		})";

		auto maybeDoc = Document::fromJsonString( arrayDocJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_TRUE( result.isValid() );
	}

	TEST_F( SchemaValidatorTest, ArrayValidationWithInvalidItems )
	{
		SchemaValidator validator( schemaWithRefs );

		std::string invalidArrayJson = R"({
			"company": {
				"name": "Test Company"
			},
			"employees": [
				{
					"id": "not-a-number",
					"name": "Alice Smith"
				},
				{
					"id": 2
				}
			]
		})";

		auto maybeDoc = Document::fromJsonString( invalidArrayJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 2 );

		// Check for array item validation errors
		bool foundIdTypeError = false, foundNameRequiredError = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path().find( "employees[0].id" ) != std::string::npos && error.constraint() == "type" )
				foundIdTypeError = true;
			if ( error.path().find( "employees[1].name" ) != std::string::npos && error.constraint() == "required" )
			{
				foundNameRequiredError = true;
			}
		}

		EXPECT_TRUE( foundIdTypeError );
		EXPECT_TRUE( foundNameRequiredError );
	}

	//----------------------------------------------
	// Reference resolution
	//----------------------------------------------

	TEST_F( SchemaValidatorTest, ValidateWithSchemaReferences )
	{
		SchemaValidator validator( schemaWithRefs );

		std::string validRefDocJson = R"({
			"company": {
				"name": "Acme Corp",
				"founded": 1990
			},
			"employees": []
		})";

		auto maybeDoc = Document::fromJsonString( validRefDocJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_TRUE( result.isValid() );
	}

	TEST_F( SchemaValidatorTest, ValidateWithInvalidReferences )
	{
		SchemaValidator validator( schemaWithRefs );

		std::string invalidRefDocJson = R"({
			"company": {
				"founded": 1990
			},
			"employees": []
		})";

		auto maybeDoc = Document::fromJsonString( invalidRefDocJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 1 );

		// Check for missing required field in referenced object
		bool foundCompanyNameRequired = false;
		for ( const auto& error : result.errors() )
		{
			if ( error.path() == "company.name" && error.constraint() == "required" )
			{
				foundCompanyNameRequired = true;
			}
		}

		EXPECT_TRUE( foundCompanyNameRequired );
	}

	//=====================================================================
	// Edge cases and error scenarios
	//=====================================================================

	TEST_F( SchemaValidatorTest, EmptyDocumentValidation )
	{
		SchemaValidator validator( basicSchema );
		Document emptyDoc = Document::createObject();

		ValidationResult result = validator.validate( emptyDoc );

		EXPECT_FALSE( result.isValid() );
		EXPECT_GE( result.errorCount(), 2 ); // Missing required fields
	}

	TEST_F( SchemaValidatorTest, NullValueHandling )
	{
		std::string nullSchemaJson = R"({
			"type": "object",
			"properties": {
				"nullableField": {
					"type": ["string", "null"]
				},
				"nonNullableField": {
					"type": "string"
				}
			}
		})";

		auto maybeSchema = Document::fromJsonString( nullSchemaJson );
		ASSERT_TRUE( maybeSchema.has_value() );

		SchemaValidator validator( maybeSchema.value() );

		std::string nullDocJson = R"({
			"nullableField": null,
			"nonNullableField": null
		})";

		auto maybeDoc = Document::fromJsonString( nullDocJson );
		ASSERT_TRUE( maybeDoc.has_value() );

		ValidationResult result = validator.validate( maybeDoc.value() );

		// Should fail only for nonNullableField
		EXPECT_FALSE( result.isValid() );
	}
} // namespace nfx::serialization::json::test
