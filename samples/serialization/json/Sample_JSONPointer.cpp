/**
 * @file Sample_JSONPointer.cpp
 * @brief Comprehensive sample demonstrating JSON Pointer (RFC 6901) functionality
 * @details Real-world examples showcasing JSON Pointer syntax for document navigation,
 *          array element access, nested object manipulation, and escaped character handling
 */

#include <iostream>
#include <string>
#include <vector>

#include <nfx/serialization/json/Document.h>

using namespace nfx::serialization::json;

//=====================================================================
// Basic JSON Pointer operations
//=====================================================================

void demonstrateBasicJsonPointer()
{
	std::cout << "\n=== Basic JSON Pointer Operations ===" << std::endl;

	// Create a document using JSON Pointer syntax (RFC 6901)
	Document doc = Document::createObject();

	// Set values using JSON Pointer notation
	doc.setStringByPointer( "/name", "Alice Johnson" );
	doc.setIntByPointer( "/age", 30 );
	doc.setDoubleByPointer( "/height", 1.75 );
	doc.setBoolByPointer( "/active", true );
	doc.setNullByPointer( "/spouse" );

	// Display the created document
	std::cout << "Created document using JSON Pointers:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Read values using JSON Pointer syntax
	std::cout << "\nReading values with JSON Pointers:" << std::endl;
	std::cout << "Name: " << doc.getStringByPointer( "/name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Age: " << doc.getIntByPointer( "/age" ).value_or( 0 ) << std::endl;
	std::cout << "Height: " << doc.getDoubleByPointer( "/height" ).value_or( 0.0 ) << "m" << std::endl;
	std::cout << "Active: " << ( doc.getBoolByPointer( "/active" ).value_or( false ) ? "Yes" : "No" ) << std::endl;

	// Check field existence
	std::cout << "\nField existence checks:" << std::endl;
	std::cout << "Has name: " << ( doc.hasFieldByPointer( "/name" ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Has spouse: " << ( doc.hasFieldByPointer( "/spouse" ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Has nonexistent: " << ( doc.hasFieldByPointer( "/nonexistent" ) ? "Yes" : "No" ) << std::endl;
}

//=====================================================================
// Nested object navigation
//=====================================================================

void demonstrateNestedObjectNavigation()
{
	std::cout << "\n=== Nested Object Navigation ===" << std::endl;

	Document doc = Document::createObject();

	// Create deeply nested structure using JSON Pointers
	doc.setStringByPointer( "/user/profile/firstName", "John" );
	doc.setStringByPointer( "/user/profile/lastName", "Doe" );
	doc.setStringByPointer( "/user/profile/email", "john.doe@example.com" );
	doc.setIntByPointer( "/user/profile/age", 28 );

	doc.setStringByPointer( "/user/address/street", "123 Main St" );
	doc.setStringByPointer( "/user/address/city", "New York" );
	doc.setStringByPointer( "/user/address/country", "USA" );
	doc.setStringByPointer( "/user/address/zipCode", "10001" );

	doc.setBoolByPointer( "/user/settings/theme/dark", true );
	doc.setBoolByPointer( "/user/settings/notifications/email", false );
	doc.setBoolByPointer( "/user/settings/notifications/push", true );

	std::cout << "Nested document structure:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Navigate through nested structures
	std::cout << "\nAccessing nested data:" << std::endl;
	std::cout << "Full name: "
			  << doc.getStringByPointer( "/user/profile/firstName" ).value_or( "" ) << " "
			  << doc.getStringByPointer( "/user/profile/lastName" ).value_or( "" ) << std::endl;
	std::cout << "Email: " << doc.getStringByPointer( "/user/profile/email" ).value_or( "N/A" ) << std::endl;
	std::cout << "Address: " << doc.getStringByPointer( "/user/address/street" ).value_or( "" )
			  << ", " << doc.getStringByPointer( "/user/address/city" ).value_or( "" )
			  << ", " << doc.getStringByPointer( "/user/address/country" ).value_or( "" ) << std::endl;
	std::cout << "Dark theme: " << ( doc.getBoolByPointer( "/user/settings/theme/dark" ).value_or( false ) ? "Yes" : "No" ) << std::endl;
}

//=====================================================================
// Array operations with JSON Pointers
//=====================================================================

void demonstrateArrayOperations()
{
	std::cout << "\n=== Array Operations with JSON Pointers ===" << std::endl;

	Document doc = Document::createObject();

	// Create arrays using JSON Pointer syntax
	doc.setStringByPointer( "/users/0/name", "Alice" );
	doc.setIntByPointer( "/users/0/age", 25 );
	doc.setStringByPointer( "/users/0/role", "Developer" );

	doc.setStringByPointer( "/users/1/name", "Bob" );
	doc.setIntByPointer( "/users/1/age", 30 );
	doc.setStringByPointer( "/users/1/role", "Manager" );

	doc.setStringByPointer( "/users/2/name", "Charlie" );
	doc.setIntByPointer( "/users/2/age", 35 );
	doc.setStringByPointer( "/users/2/role", "Architect" );

	// Create a numeric array
	doc.setDoubleByPointer( "/scores/0", 95.5 );
	doc.setDoubleByPointer( "/scores/1", 87.3 );
	doc.setDoubleByPointer( "/scores/2", 92.8 );
	doc.setDoubleByPointer( "/scores/3", 88.1 );

	std::cout << "Document with arrays:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Access array elements
	std::cout << "\nAccessing array elements:" << std::endl;
	for ( int i = 0; i < 3; ++i )
	{
		std::string namePath = "/users/" + std::to_string( i ) + "/name";
		std::string agePath = "/users/" + std::to_string( i ) + "/age";
		std::string rolePath = "/users/" + std::to_string( i ) + "/role";

		auto name = doc.getStringByPointer( namePath );
		auto age = doc.getIntByPointer( agePath );
		auto role = doc.getStringByPointer( rolePath );

		if ( name && age && role )
		{
			std::cout << "User " << i << ": " << *name << " (age " << *age << ", " << *role << ")" << std::endl;
		}
	}

	std::cout << "\nScores array:" << std::endl;
	for ( int i = 0; i < 4; ++i )
	{
		std::string scorePath = "/scores/" + std::to_string( i );
		auto score = doc.getDoubleByPointer( scorePath );
		if ( score )
		{
			std::cout << "Score " << i << ": " << *score << std::endl;
		}
	}
}

//=====================================================================
// Escaped characters in JSON Pointers
//=====================================================================

void demonstrateEscapedCharacters()
{
	std::cout << "\n=== Escaped Characters in JSON Pointers ===" << std::endl;

	Document doc = Document::createObject();

	// RFC 6901 defines escape sequences:
	// ~0 represents ~ (tilde)
	// ~1 represents / (forward slash)

	// Set values with escaped characters
	doc.setStringByPointer( "/field~1with~0tilde", "Contains / and ~ characters" );
	doc.setStringByPointer( "/path~1to~1data", "Deep path with slashes" );
	doc.setIntByPointer( "/config~1api~1port", 8080 );

	std::cout << "Document with escaped characters:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Access values with escaped characters
	std::cout << "\nReading escaped fields:" << std::endl;
	std::cout << "Field with / and ~: " << doc.getStringByPointer( "/field~1with~0tilde" ).value_or( "N/A" ) << std::endl;
	std::cout << "Path data: " << doc.getStringByPointer( "/path~1to~1data" ).value_or( "N/A" ) << std::endl;
	std::cout << "API Port: " << doc.getIntByPointer( "/config~1api~1port" ).value_or( 0 ) << std::endl;
}

//=====================================================================
// JSON Pointer vs Dot notation compatibility
//=====================================================================

void demonstrateCompatibility()
{
	std::cout << "\n=== JSON Pointer vs Dot Notation Compatibility ===" << std::endl;

	Document doc = Document::createObject();

	// Set values using dot notation
	doc.setString( "app.name", "MyApplication" );
	doc.setInt( "app.version.major", 2 );
	doc.setInt( "app.version.minor", 1 );

	// Access the same values using JSON Pointer
	std::cout << "Values set with dot notation, accessed with JSON Pointers:" << std::endl;
	std::cout << "App name: " << doc.getStringByPointer( "/app/name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Major version: " << doc.getIntByPointer( "/app/version/major" ).value_or( 0 ) << std::endl;
	std::cout << "Minor version: " << doc.getIntByPointer( "/app/version/minor" ).value_or( 0 ) << std::endl;

	// Set values using JSON Pointer
	doc.setStringByPointer( "/database/host", "localhost" );
	doc.setIntByPointer( "/database/port", 5432 );
	doc.setStringByPointer( "/database/name", "myapp_db" );

	// Access the same values using dot notation
	std::cout << "\nValues set with JSON Pointers, accessed with dot notation:" << std::endl;
	std::cout << "DB host: " << doc.getString( "database.host" ).value_or( "Unknown" ) << std::endl;
	std::cout << "DB port: " << doc.getInt( "database.port" ).value_or( 0 ) << std::endl;
	std::cout << "DB name: " << doc.getString( "database.name" ).value_or( "Unknown" ) << std::endl;

	std::cout << "\nComplete document:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;
}

//=====================================================================
// Error handling and validation
//=====================================================================

void demonstrateErrorHandling()
{
	std::cout << "\n=== Error Handling and Validation ===" << std::endl;

	Document doc = Document::createObject();
	doc.setStringByPointer( "/existing/field", "value" );
	doc.setIntByPointer( "/numbers/0", 10 );
	doc.setIntByPointer( "/numbers/1", 20 );

	std::cout << "Test document:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Test various error conditions
	std::cout << "\nError handling tests:" << std::endl;

	// Invalid pointer format
	std::cout << "Invalid pointer 'no-leading-slash': "
			  << ( doc.hasFieldByPointer( "no-leading-slash" ) ? "Found" : "Not found" ) << std::endl;

	// Non-existent fields
	std::cout << "Non-existent field '/missing/field': "
			  << ( doc.hasFieldByPointer( "/missing/field" ) ? "Found" : "Not found" ) << std::endl;

	// Array index out of bounds
	std::cout << "Out of bounds '/numbers/5': "
			  << ( doc.hasFieldByPointer( "/numbers/5" ) ? "Found" : "Not found" ) << std::endl;

	// Type mismatches
	auto wrongType = doc.getIntByPointer( "/existing/field" );
	std::cout << "String field accessed as int: "
			  << ( wrongType.has_value() ? std::to_string( *wrongType ) : "No value (correct)" ) << std::endl;

	// Invalid array indices
	std::cout << "Invalid array index '/numbers/01': "
			  << ( doc.hasFieldByPointer( "/numbers/01" ) ? "Found" : "Not found (correct)" ) << std::endl;
	std::cout << "Non-numeric array index '/numbers/abc': "
			  << ( doc.hasFieldByPointer( "/numbers/abc" ) ? "Found" : "Not found (correct)" ) << std::endl;
}

//=====================================================================
// Complex real-world scenario
//=====================================================================

void demonstrateRealWorldScenario()
{
	std::cout << "\n=== Real-World API Response Processing ===" << std::endl;

	// Simulate processing a complex API response
	Document response = Document::createObject();

	// Build a realistic API response structure
	response.setStringByPointer( "/status", "success" );
	response.setIntByPointer( "/code", 200 );
	response.setStringByPointer( "/timestamp", "2025-10-03T14:30:00Z" );

	// User data
	response.setStringByPointer( "/data/user/id", "usr_12345" );
	response.setStringByPointer( "/data/user/email", "john.doe@example.com" );
	response.setStringByPointer( "/data/user/profile/firstName", "John" );
	response.setStringByPointer( "/data/user/profile/lastName", "Doe" );
	response.setBoolByPointer( "/data/user/profile/verified", true );

	// Permissions array
	response.setStringByPointer( "/data/user/permissions/0/resource", "users" );
	response.setStringByPointer( "/data/user/permissions/0/actions/0", "read" );
	response.setStringByPointer( "/data/user/permissions/0/actions/1", "write" );

	response.setStringByPointer( "/data/user/permissions/1/resource", "posts" );
	response.setStringByPointer( "/data/user/permissions/1/actions/0", "read" );
	response.setStringByPointer( "/data/user/permissions/1/actions/1", "write" );
	response.setStringByPointer( "/data/user/permissions/1/actions/2", "delete" );

	// Metadata
	response.setDoubleByPointer( "/metadata/version", 2.1 );
	response.setStringByPointer( "/metadata/server", "api-server-01" );
	response.setIntByPointer( "/metadata/processingTime", 45 );

	std::cout << "API Response:" << std::endl;
	std::cout << response.toJsonString( 2 ) << std::endl;

	// Process the response using JSON Pointers
	std::cout << "\nProcessing API response:" << std::endl;

	// Check if request was successful
	auto status = response.getStringByPointer( "/status" );
	auto code = response.getIntByPointer( "/code" );

	if ( status && code && *status == "success" && *code == 200 )
	{
		std::cout << "Request successful!" << std::endl;

		// Extract user information
		auto userId = response.getStringByPointer( "/data/user/id" );
		auto firstName = response.getStringByPointer( "/data/user/profile/firstName" );
		auto lastName = response.getStringByPointer( "/data/user/profile/lastName" );
		auto verified = response.getBoolByPointer( "/data/user/profile/verified" );

		if ( userId && firstName && lastName )
		{
			std::cout << "User: " << *firstName << " " << *lastName << " (ID: " << *userId << ")" << std::endl;
			std::cout << "Verified: " << ( verified.value_or( false ) ? "Yes" : "No" ) << std::endl;
		}

		// Display permissions summary
		std::cout << "\nPermissions:" << std::endl;
		for ( int i = 0; i < 10; ++i ) // Check up to 10 permissions
		{
			std::string resourcePath = "/data/user/permissions/" + std::to_string( i ) + "/resource";
			auto resource = response.getStringByPointer( resourcePath );

			if ( !resource )
				break; // No more permissions

			std::cout << "- " << *resource << ": ";

			// Count actions
			int actionCount = 0;
			for ( int j = 0; j < 10; ++j ) // Check up to 10 actions
			{
				std::string actionPath = "/data/user/permissions/" + std::to_string( i ) + "/actions/" + std::to_string( j );
				if ( response.hasFieldByPointer( actionPath ) )
				{
					actionCount++;
				}
				else
				{
					break;
				}
			}
			std::cout << actionCount << " action(s)" << std::endl;
		}

		// Display metadata
		auto version = response.getDoubleByPointer( "/metadata/version" );
		auto server = response.getStringByPointer( "/metadata/server" );
		auto processingTime = response.getIntByPointer( "/metadata/processingTime" );

		std::cout << "\nMetadata:" << std::endl;
		if ( version )
			std::cout << "API Version: " << *version << std::endl;
		if ( server )
			std::cout << "Server: " << *server << std::endl;
		if ( processingTime )
			std::cout << "Processing Time: " << *processingTime << "ms" << std::endl;
	}
	else
	{
		std::cout << "Request failed!" << std::endl;
	}
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
	std::cout << "NFX-Core JSON Pointer (RFC 6901) Samples" << std::endl;
	std::cout << "=========================================" << std::endl;

	try
	{
		demonstrateBasicJsonPointer();
		demonstrateNestedObjectNavigation();
		demonstrateArrayOperations();
		demonstrateEscapedCharacters();
		demonstrateCompatibility();
		demonstrateErrorHandling();
		demonstrateRealWorldScenario();
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
