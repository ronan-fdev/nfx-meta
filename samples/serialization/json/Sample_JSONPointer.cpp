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
	Document doc;

	// Set values using JSON Pointer notation
	doc.set<std::string>( "/name", "Alice Johnson" );
	doc.set<int64_t>( "/age", 30 );
	doc.set<double>( "/height", 1.75 );
	doc.set<bool>( "/active", true );
	doc.setNull( "/spouse" );

	// Display the created document
	std::cout << "Created document using JSON Pointers:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Read values using JSON Pointer syntax
	std::cout << "\nReading values with JSON Pointers:" << std::endl;
	std::cout << "Name: " << doc.get<std::string>( "/name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Age: " << doc.get<int64_t>( "/age" ).value_or( 0 ) << std::endl;
	std::cout << "Height: " << doc.get<double>( "/height" ).value_or( 0.0 ) << "m" << std::endl;
	std::cout << "Active: " << ( doc.get<bool>( "/active" ).value_or( false ) ? "Yes" : "No" ) << std::endl;

	// Check field existence
	std::cout << "\nField existence checks:" << std::endl;
	std::cout << "Has name: " << ( doc.hasValue( "/name" ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Has spouse: " << ( doc.hasValue( "/spouse" ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Has nonexistent: " << ( doc.hasValue( "/nonexistent" ) ? "Yes" : "No" ) << std::endl;
}

//=====================================================================
// Nested object navigation
//=====================================================================

void demonstrateNestedObjectNavigation()
{
	std::cout << "\n=== Nested Object Navigation ===" << std::endl;

	Document doc;

	// Create deeply nested structure using JSON Pointers
	doc.set<std::string>( "/user/profile/firstName", "John" );
	doc.set<std::string>( "/user/profile/lastName", "Doe" );
	doc.set<std::string>( "/user/profile/email", "john.doe@example.com" );
	doc.set<int64_t>( "/user/profile/age", 28 );

	doc.set<std::string>( "/user/address/street", "123 Main St" );
	doc.set<std::string>( "/user/address/city", "New York" );
	doc.set<std::string>( "/user/address/country", "USA" );
	doc.set<std::string>( "/user/address/zipCode", "10001" );

	doc.set<bool>( "/user/settings/theme/dark", true );
	doc.set<bool>( "/user/settings/notifications/email", false );
	doc.set<bool>( "/user/settings/notifications/push", true );

	std::cout << "Nested document structure:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Navigate through nested structures
	std::cout << "\nAccessing nested data:" << std::endl;
	std::cout << "Full name: "
			  << doc.get<std::string>( "/user/profile/firstName" ).value_or( "" ) << " "
			  << doc.get<std::string>( "/user/profile/lastName" ).value_or( "" ) << std::endl;
	std::cout << "Email: " << doc.get<std::string>( "/user/profile/email" ).value_or( "N/A" ) << std::endl;
	std::cout << "Address: " << doc.get<std::string>( "/user/address/street" ).value_or( "" )
			  << ", " << doc.get<std::string>( "/user/address/city" ).value_or( "" )
			  << ", " << doc.get<std::string>( "/user/address/country" ).value_or( "" ) << std::endl;
	std::cout << "Dark theme: " << ( doc.get<bool>( "/user/settings/theme/dark" ).value_or( false ) ? "Yes" : "No" ) << std::endl;
}

//=====================================================================
// Array operations with JSON Pointers
//=====================================================================

void demonstrateArrayOperations()
{
	std::cout << "\n=== Array Operations with JSON Pointers ===" << std::endl;

	Document doc;

	// Create arrays using JSON Pointer syntax
	doc.set<std::string>( "/users/0/name", "Alice" );
	doc.set<int64_t>( "/users/0/age", 25 );
	doc.set<std::string>( "/users/0/role", "Developer" );

	doc.set<std::string>( "/users/1/name", "Bob" );
	doc.set<int64_t>( "/users/1/age", 30 );
	doc.set<std::string>( "/users/1/role", "Manager" );

	doc.set<std::string>( "/users/2/name", "Charlie" );
	doc.set<int64_t>( "/users/2/age", 35 );
	doc.set<std::string>( "/users/2/role", "Architect" );

	// Create a numeric array
	doc.set<double>( "/scores/0", 95.5 );
	doc.set<double>( "/scores/1", 87.3 );
	doc.set<double>( "/scores/2", 92.8 );
	doc.set<double>( "/scores/3", 88.1 );

	std::cout << "Document with arrays:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Access array elements
	std::cout << "\nAccessing array elements:" << std::endl;
	for ( int i = 0; i < 3; ++i )
	{
		std::string namePath = "/users/" + std::to_string( i ) + "/name";
		std::string agePath = "/users/" + std::to_string( i ) + "/age";
		std::string rolePath = "/users/" + std::to_string( i ) + "/role";

		auto name = doc.get<std::string>( namePath );
		auto age = doc.get<int64_t>( agePath );
		auto role = doc.get<std::string>( rolePath );

		if ( name && age && role )
		{
			std::cout << "User " << i << ": " << *name << " (age " << *age << ", " << *role << ")" << std::endl;
		}
	}

	std::cout << "\nScores array:" << std::endl;
	for ( int i = 0; i < 4; ++i )
	{
		std::string scorePath = "/scores/" + std::to_string( i );
		auto score = doc.get<double>( scorePath );
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

	Document doc;

	// RFC 6901 defines escape sequences:
	// ~0 represents ~ (tilde)
	// ~1 represents / (forward slash)

	// Set values with escaped characters
	doc.set<std::string>( "/field~1with~0tilde", "Contains / and ~ characters" );
	doc.set<std::string>( "/path~1to~1data", "Deep path with slashes" );
	doc.set<int64_t>( "/config~1api~1port", 8080 );

	std::cout << "Document with escaped characters:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Access values with escaped characters
	std::cout << "\nReading escaped fields:" << std::endl;
	std::cout << "Field with / and ~: " << doc.get<std::string>( "/field~1with~0tilde" ).value_or( "N/A" ) << std::endl;
	std::cout << "Path data: " << doc.get<std::string>( "/path~1to~1data" ).value_or( "N/A" ) << std::endl;
	std::cout << "API Port: " << doc.get<int64_t>( "/config~1api~1port" ).value_or( 0 ) << std::endl;
}

//=====================================================================
// JSON Pointer vs Dot notation compatibility
//=====================================================================

void demonstrateCompatibility()
{
	std::cout << "\n=== JSON Pointer vs Dot Notation Compatibility ===" << std::endl;

	Document doc;

	// Set values using dot notation
	doc.set<std::string>( "app.name", "MyApplication" );
	doc.set<int64_t>( "app.version.major", 2 );
	doc.set<int64_t>( "app.version.minor", 1 );

	// Access the same values using JSON Pointer
	std::cout << "Values set with dot notation, accessed with JSON Pointers:" << std::endl;
	std::cout << "App name: " << doc.get<std::string>( "/app/name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Major version: " << doc.get<int64_t>( "/app/version/major" ).value_or( 0 ) << std::endl;
	std::cout << "Minor version: " << doc.get<int64_t>( "/app/version/minor" ).value_or( 0 ) << std::endl;

	// Set values using JSON Pointer
	doc.set<std::string>( "/database/host", "localhost" );
	doc.set<int64_t>( "/database/port", 5432 );
	doc.set<std::string>( "/database/name", "myapp_db" );

	// Access the same values using dot notation
	std::cout << "\nValues set with JSON Pointers, accessed with dot notation:" << std::endl;
	std::cout << "DB host: " << doc.get<std::string>( "database.host" ).value_or( "Unknown" ) << std::endl;
	std::cout << "DB port: " << doc.get<int64_t>( "database.port" ).value_or( 0 ) << std::endl;
	std::cout << "DB name: " << doc.get<std::string>( "database.name" ).value_or( "Unknown" ) << std::endl;

	std::cout << "\nComplete document:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;
}

//=====================================================================
// Error handling and validation
//=====================================================================

void demonstrateErrorHandling()
{
	std::cout << "\n=== Error Handling and Validation ===" << std::endl;

	Document doc;

	doc.set<std::string>( "/existing/field", "value" );
	doc.set<int64_t>( "/numbers/0", 10 );
	doc.set<int64_t>( "/numbers/1", 20 );

	std::cout << "Test document:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	// Test various error conditions
	std::cout << "\nError handling tests:" << std::endl;

	// Invalid pointer format
	std::cout << "Invalid pointer 'no-leading-slash': "
			  << ( doc.hasValue( "no-leading-slash" ) ? "Found" : "Not found" ) << std::endl;

	// Non-existent fields
	std::cout << "Non-existent field '/missing/field': "
			  << ( doc.hasValue( "/missing/field" ) ? "Found" : "Not found" ) << std::endl;

	// Array index out of bounds
	std::cout << "Out of bounds '/numbers/5': "
			  << ( doc.hasValue( "/numbers/5" ) ? "Found" : "Not found" ) << std::endl;

	// Type mismatches
	auto wrongType = doc.get<int64_t>( "/existing/field" );
	std::cout << "String field accessed as int: "
			  << ( wrongType.has_value() ? std::to_string( *wrongType ) : "No value (correct)" ) << std::endl;

	// Invalid array indices
	std::cout << "Invalid array index '/numbers/01': "
			  << ( doc.hasValue( "/numbers/01" ) ? "Found" : "Not found (correct)" ) << std::endl;
	std::cout << "Non-numeric array index '/numbers/abc': "
			  << ( doc.hasValue( "/numbers/abc" ) ? "Found" : "Not found (correct)" ) << std::endl;
}

//=====================================================================
// Complex real-world scenario
//=====================================================================

void demonstrateRealWorldScenario()
{
	std::cout << "\n=== Real-World API Response Processing ===" << std::endl;

	// Simulate processing a complex API response
	Document response;

	// Build a realistic API response structure
	response.set<std::string>( "/status", "success" );
	response.set<int64_t>( "/code", 200 );
	response.set<std::string>( "/timestamp", "2025-10-03T14:30:00Z" );

	// User data
	response.set<std::string>( "/data/user/id", "usr_12345" );
	response.set<std::string>( "/data/user/email", "john.doe@example.com" );
	response.set<std::string>( "/data/user/profile/firstName", "John" );
	response.set<std::string>( "/data/user/profile/lastName", "Doe" );
	response.set<bool>( "/data/user/profile/verified", true );

	// Permissions array
	response.set<std::string>( "/data/user/permissions/0/resource", "users" );
	response.set<std::string>( "/data/user/permissions/0/actions/0", "read" );
	response.set<std::string>( "/data/user/permissions/0/actions/1", "write" );

	response.set<std::string>( "/data/user/permissions/1/resource", "posts" );
	response.set<std::string>( "/data/user/permissions/1/actions/0", "read" );
	response.set<std::string>( "/data/user/permissions/1/actions/1", "write" );
	response.set<std::string>( "/data/user/permissions/1/actions/2", "delete" );

	// Metadata
	response.set<double>( "/metadata/version", 2.1 );
	response.set<std::string>( "/metadata/server", "api-server-01" );
	response.set<int64_t>( "/metadata/processingTime", 45 );

	std::cout << "API Response:" << std::endl;
	std::cout << response.toJsonString( 2 ) << std::endl;

	// Process the response using JSON Pointers
	std::cout << "\nProcessing API response:" << std::endl;

	// Check if request was successful
	auto status = response.get<std::string>( "/status" );
	auto code = response.get<int64_t>( "/code" );

	if ( status && code && *status == "success" && *code == 200 )
	{
		std::cout << "Request successful!" << std::endl;

		// Extract user information
		auto userId = response.get<std::string>( "/data/user/id" );
		auto firstName = response.get<std::string>( "/data/user/profile/firstName" );
		auto lastName = response.get<std::string>( "/data/user/profile/lastName" );
		auto verified = response.get<bool>( "/data/user/profile/verified" );

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
			auto resource = response.get<std::string>( resourcePath );

			if ( !resource )
				break; // No more permissions

			std::cout << "- " << *resource << ": ";

			// Count actions
			int actionCount = 0;
			for ( int j = 0; j < 10; ++j ) // Check up to 10 actions
			{
				std::string actionPath = "/data/user/permissions/" + std::to_string( i ) + "/actions/" + std::to_string( j );
				if ( response.hasValue( actionPath ) )
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
		auto version = response.get<double>( "/metadata/version" );
		auto server = response.get<std::string>( "/metadata/server" );
		auto processingTime = response.get<int64_t>( "/metadata/processingTime" );

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
	std::cout << "nfx-meta JSON Pointer (RFC 6901) Samples" << std::endl;
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
