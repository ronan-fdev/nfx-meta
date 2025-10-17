/**
 * @file Sample_JSONSchemaValidator.cpp
 * @brief Comprehensive sample demonstrating JSON Schema validation functionality
 * @details Real-world examples showcasing JSON Schema validation for ship data (ISO19848),
 *          type validation, constraint checking, required field validation, and error reporting
 */

#include <iostream>
#include <string>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/SchemaValidator.h>

using namespace nfx::serialization::json;

//=====================================================================
// Basic schema validation
//=====================================================================

void demonstrateBasicValidation()
{
	std::cout << "\n=== Basic Schema Validation ===" << std::endl;

	// Define a simple user schema
	std::string userSchemaJson = R"({
		"type": "object",
		"properties": {
			"name": {
				"type": "string",
				"minLength": 1,
				"maxLength": 100
			},
			"age": {
				"type": "integer",
				"minimum": 0,
				"maximum": 150
			},
			"email": {
				"type": "string"
			}
		},
		"required": ["name", "age"]
	})";

	// Load the schema
	SchemaValidator validator;
	bool schemaLoaded = validator.loadSchemaFromString( userSchemaJson );

	std::cout << "Schema loaded: " << ( schemaLoaded ? "SUCCESS" : "FAILED" ) << std::endl;

	// Test Case 1: Valid user data
	std::cout << "\n--- Test Case 1: Valid User Data ---" << std::endl;

	Document validUser;

	validUser.set<std::string>( "name", "Alice Johnson" );
	validUser.set<int64_t>( "age", 30 );
	validUser.set<std::string>( "email", "alice@example.com" );

	std::cout << "User data: " << validUser.toJsonString( 2 ) << std::endl;

	ValidationResult result1 = validator.validate( validUser );
	if ( result1.isValid() )
	{
		std::cout << "[PASS] Validation PASSED - User data is valid!" << std::endl;
	}
	else
	{
		std::cout << "[FAIL] Validation FAILED:" << std::endl;
		std::cout << result1.errorSummary() << std::endl;
	}

	// Test Case 2: Missing required field
	std::cout << "\n--- Test Case 2: Missing Required Field ---" << std::endl;

	Document invalidUser;

	invalidUser.set<std::string>( "email", "bob@example.com" );
	// Missing required "name" and "age"

	std::cout << "User data: " << invalidUser.toJsonString( 2 ) << std::endl;

	ValidationResult result2 = validator.validate( invalidUser );
	if ( result2.isValid() )
	{
		std::cout << "[PASS] Validation PASSED" << std::endl;
	}
	else
	{
		std::cout << "[FAIL] Validation FAILED (Expected):" << std::endl;
		std::cout << result2.errorSummary() << std::endl;
	}

	// Test Case 3: Type mismatch
	std::cout << "\n--- Test Case 3: Type Mismatch ---" << std::endl;

	Document typeError;

	typeError.set<std::string>( "name", "Charlie" );
	typeError.set<std::string>( "age", "thirty" ); // Should be integer
	typeError.set<std::string>( "email", "charlie@example.com" );

	std::cout << "User data: " << typeError.toJsonString( 2 ) << std::endl;

	ValidationResult result3 = validator.validate( typeError );
	if ( result3.isValid() )
	{
		std::cout << "[PASS] Validation PASSED" << std::endl;
	}
	else
	{
		std::cout << "[FAIL] Validation FAILED (Expected):" << std::endl;
		std::cout << result3.errorSummary() << std::endl;
	}
}

//=====================================================================
// Error handling and schema information
//=====================================================================

void demonstrateErrorHandling()
{
	std::cout << "\n=== Error Handling and Schema Information ===" << std::endl;

	// Test validation without schema
	std::cout << "\n--- Validation Without Schema ---" << std::endl;

	SchemaValidator emptyValidator;

	Document testDoc;

	testDoc.set<std::string>( "test", "data" );

	try
	{
		ValidationResult result = emptyValidator.validate( testDoc );
		std::cout << "Validation result without schema:" << std::endl;
		std::cout << "  Is valid: " << ( result.isValid() ? "Yes" : "No" ) << std::endl;
		std::cout << "  Error count: " << result.errorCount() << std::endl;
		if ( !result.isValid() )
		{
			std::cout << "  Error summary:" << std::endl;
			std::cout << result.errorSummary() << std::endl;
		}
		else
		{
			std::cout << "  Unexpected: validation succeeded without schema!" << std::endl;
		}
	}
	catch ( const std::exception& e )
	{
		std::cout << "[PASS] Correctly caught exception: " << e.what() << std::endl;
	}

	// Test invalid schema
	std::cout << "\n--- Invalid Schema Loading ---" << std::endl;

	std::string invalidSchema = "{ invalid json }";
	bool loaded = emptyValidator.loadSchemaFromString( invalidSchema );
	std::cout << "Invalid schema load result: " << ( loaded ? "Loaded (unexpected)" : "Failed (correct)" ) << std::endl;

	// Test schema information
	std::cout << "\n--- Schema Information ---" << std::endl;

	std::string infoSchemaJson = R"({
		"$schema": "https://json-schema.org/draft/2020-12/schema",
		"title": "Demo Schema",
		"description": "A demonstration schema for testing",
		"type": "object",
		"properties": {
			"name": { "type": "string" }
		}
	})";

	SchemaValidator infoValidator;
	infoValidator.loadSchemaFromString( infoSchemaJson );

	std::cout << "Schema version: '" << infoValidator.schemaVersion() << "'" << std::endl;
	std::cout << "Schema title: '" << infoValidator.schemaTitle() << "'" << std::endl;
	std::cout << "Schema description: '" << infoValidator.schemaDescription() << "'" << std::endl;
	std::cout << "Has valid schema: " << ( infoValidator.hasSchema() ? "Yes" : "No" ) << std::endl;
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
	std::cout << "NFX-Core JSON Schema Validation Samples" << std::endl;
	std::cout << "=======================================" << std::endl;

	try
	{
		demonstrateBasicValidation();
		demonstrateErrorHandling();

		std::cout << "\n[SUCCESS] All schema validation samples completed successfully!" << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "[ERROR] Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
