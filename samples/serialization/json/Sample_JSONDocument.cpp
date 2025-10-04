/**
 * @file Sample_JSONDocument.cpp
 * @brief Comprehensive sample demonstrating JSON Document serialization capabilities
 * @details Real-world examples covering JSON parsing, path-based access, array operations,
 *          nested object navigation, configuration files, and API response handling
 */

#include <iostream>
#include <fstream>
#include <vector>

#include <nfx/serialization/json/Document.h>

using namespace nfx::serialization::json;

//=====================================================================
// Configuration file management
//=====================================================================

void demonstrateConfigurationManagement()
{
	std::cout << "\n=== Configuration File Management ===" << std::endl;

	// Create a complex configuration structure
	Document config = Document::createObject();

	// Application settings
	config.setString( "app.name", "MyApplication" );
	config.setString( "app.version", "1.2.3" );
	config.setBool( "app.debug", true );

	// Database configuration
	config.setString( "database.host", "localhost" );
	config.setInt( "database.port", 5432 );
	config.setString( "database.name", "myapp_db" );
	config.setString( "database.user", "admin" );

	// Server configuration with arrays
	config.addToArray( "server.allowedHosts", "localhost" );
	config.addToArray( "server.allowedHosts", "127.0.0.1" );
	config.addToArray( "server.allowedHosts", "::1" );

	config.setInt( "server.port", 8080 );
	config.setInt( "server.maxConnections", 1000 );

	// Feature flags
	config.setBool( "features.enableCaching", true );
	config.setBool( "features.enableLogging", false );
	config.setString( "features.logLevel", "INFO" );

	// Output the configuration
	std::cout << "Generated Configuration:" << std::endl;
	std::cout << config.toJsonString( 2 ) << std::endl;

	// Demonstrate path-based access
	std::cout << "\nReading configuration values:" << std::endl;
	std::cout << "App Name: " << config.getString( "app.name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "DB Port: " << config.getInt( "database.port" ).value_or( 0 ) << std::endl;
	std::cout << "Debug Mode: " << ( config.getBool( "app.debug" ).value_or( false ) ? "ON" : "OFF" ) << std::endl;
	std::cout << "Allowed Hosts: " << config.getArraySize( "server.allowedHosts" ) << " entries" << std::endl;
}

//=====================================================================
// API response processing
//=====================================================================

void demonstrateApiResponseHandling()
{
	std::cout << "\n=== API Response Processing ===" << std::endl;

	// Simulate a complex API response (e.g., user profile with nested data)
	std::string apiResponse = R"({
		"status": "success",
		"data": {
			"user": {
				"id": 12345,
				"profile": {
					"firstName": "Alice",
					"lastName": "Johnson",
					"email": "alice.johnson@example.com",
					"age": 28,
					"active": true
				},
				"preferences": {
					"theme": "dark",
					"notifications": true,
					"language": "en-US"
				},
				"roles": ["user", "moderator"],
				"lastLogin": "2025-10-02T10:30:00Z"
			},
			"permissions": [
				{
					"resource": "posts",
					"actions": ["read", "write", "delete"]
				},
				{
					"resource": "comments",
					"actions": ["read", "write"]
				}
			]
		}
	})";

	// Parse the JSON response
	auto maybeDoc = Document::fromJsonString( apiResponse );
	if ( !maybeDoc.has_value() )
	{
		std::cout << "Failed to parse API response!" << std::endl;
		return;
	}

	Document response = maybeDoc.value();

	// Extract user information using path-based access
	std::cout << "API Response Status: " << response.getString( "status" ).value_or( "Unknown" ) << std::endl;
	std::cout << "User ID: " << response.getInt( "data.user.id" ).value_or( 0 ) << std::endl;
	std::cout << "Full Name: " << response.getString( "data.user.profile.firstName" ).value_or( "Unknown" )
			  << " " << response.getString( "data.user.profile.lastName" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Email: " << response.getString( "data.user.profile.email" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Theme: " << response.getString( "data.user.preferences.theme" ).value_or( "default" ) << std::endl;

	// Process arrays
	std::cout << "User Roles:" << std::endl;
	size_t roleCount = response.getArraySize( "data.user.roles" );
	for ( size_t i = 0; i < roleCount; ++i )
	{
		Document roleDoc = response.getArrayElement( "data.user.roles", i );
		// For simple string arrays, we'd need to handle this differently
		// This is a limitation we could address in future versions
	}

	// Process permissions array (complex objects)
	std::cout << "Permissions:" << std::endl;
	size_t permCount = response.getArraySize( "data.permissions" );
	for ( size_t i = 0; i < permCount; ++i )
	{
		Document perm = response.getArrayElement( "data.permissions", i );
		std::cout << "  Resource: " << perm.getString( "resource" ).value_or( "unknown" ) << std::endl;
		std::cout << "  Actions: " << perm.getArraySize( "actions" ) << " permissions" << std::endl;
	}
}

//=====================================================================
// Building complex documents programmatically
//=====================================================================

void demonstrateProgrammaticConstruction()
{
	std::cout << "\n=== Programmatic Document Construction ===" << std::endl;

	// Build a complex document from scratch
	Document report = Document::createObject();

	// Report metadata
	report.setString( "reportId", "RPT-2025-001" );
	report.setString( "generatedAt", "2025-10-02T14:30:00Z" );
	report.setString( "title", "Monthly Sales Report" );

	// Create summary object
	report.setDouble( "summary.totalRevenue", 125478.50 );
	report.setInt( "summary.totalOrders", 1247 );
	report.setDouble( "summary.averageOrderValue", 100.62 );

	// Add sales data array
	Document salesArray = Document::createArray();

	// Add individual sales records
	for ( int i = 0; i < 5; ++i )
	{
		Document sale = Document::createObject();
		sale.setString( "date", "2025-09-" + std::to_string( 25 + i ) );
		sale.setDouble( "amount", 1250.00 + ( i * 50 ) );
		sale.setInt( "orderId", 1000 + i );

		salesArray.addDocument( sale );
	}

	report.setArray( "salesData", salesArray );

	// Add regional breakdown
	report.setDouble( "regions.north.revenue", 45000.00 );
	report.setInt( "regions.north.orders", 450 );
	report.setDouble( "regions.south.revenue", 38000.00 );
	report.setInt( "regions.south.orders", 380 );
	report.setDouble( "regions.east.revenue", 25000.00 );
	report.setInt( "regions.east.orders", 250 );
	report.setDouble( "regions.west.revenue", 17478.50 );
	report.setInt( "regions.west.orders", 167 );

	// Output the complete report
	std::cout << "Generated Sales Report:" << std::endl;
	std::cout << report.toJsonString( 2 ) << std::endl;

	// Demonstrate data extraction
	std::cout << "\nReport Summary:" << std::endl;
	std::cout << "Total Revenue: $" << report.getDouble( "summary.totalRevenue" ).value_or( 0.0 ) << std::endl;
	std::cout << "Total Orders: " << report.getInt( "summary.totalOrders" ).value_or( 0 ) << std::endl;
	std::cout << "Sales Records: " << report.getArraySize( "salesData" ) << " entries" << std::endl;
	std::cout << "North Region: $" << report.getDouble( "regions.north.revenue" ).value_or( 0.0 )
			  << " (" << report.getInt( "regions.north.orders" ).value_or( 0 ) << " orders)" << std::endl;
}

//=====================================================================
// Type checking and safe access
//=====================================================================

void demonstrateTypeChecking()
{
	std::cout << "\n=== Type Checking and Safe Access ===" << std::endl;

	// Create a document with all JSON types
	Document doc = Document::createObject();

	// Set up different data types
	doc.setString( "user.name", "Alice Johnson" );
	doc.setInt( "user.age", 28 );
	doc.setDouble( "user.height", 1.65 );
	doc.setBool( "user.active", true );
	doc.setNull( "user.spouse" );

	// Create nested object and array
	doc.setString( "user.address.city", "New York" );
	doc.setString( "user.address.country", "USA" );
	doc.addToArray( "user.hobbies", "reading" );
	doc.addToArray( "user.hobbies", "gaming" );
	doc.addToArray( "user.hobbies", "hiking" );

	std::cout << "Document structure:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	std::cout << "\nType checking results:" << std::endl;

	// String type checking
	std::cout << "user.name is string: " << ( doc.isString( "user.name" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.age is string: " << ( doc.isString( "user.age" ) ? "YES" : "NO" ) << std::endl;

	// Integer type checking
	std::cout << "user.age is integer: " << ( doc.isInt( "user.age" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.height is integer: " << ( doc.isInt( "user.height" ) ? "YES" : "NO" ) << std::endl;

	// Double type checking
	std::cout << "user.height is double: " << ( doc.isDouble( "user.height" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.age is double: " << ( doc.isDouble( "user.age" ) ? "YES" : "NO" ) << std::endl;

	// Boolean type checking
	std::cout << "user.active is boolean: " << ( doc.isBool( "user.active" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is boolean: " << ( doc.isBool( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Null type checking
	std::cout << "user.spouse is null: " << ( doc.isNull( "user.spouse" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is null: " << ( doc.isNull( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Object type checking
	std::cout << "user is object: " << ( doc.isObject( "user" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.address is object: " << ( doc.isObject( "user.address" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is object: " << ( doc.isObject( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Array type checking
	std::cout << "user.hobbies is array: " << ( doc.isArray( "user.hobbies" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.address is array: " << ( doc.isArray( "user.address" ) ? "YES" : "NO" ) << std::endl;

	std::cout << "\nSafe data access using type checking:" << std::endl;

	// Safe string access
	if ( auto name = doc.getString( "user.name" ) )
	{
		std::cout << "Name (safe): " << *name << std::endl;
	}

	// Safe numeric access
	if ( auto age = doc.getInt( "user.age" ) )
	{
		std::cout << "Age (safe): " << *age << " years" << std::endl;
	}

	if ( auto height = doc.getDouble( "user.height" ) )
	{
		std::cout << "Height (safe): " << *height << "m" << std::endl;
	}

	// Safe boolean access
	if ( auto active = doc.getBool( "user.active" ) )
	{
		std::cout << "Status (safe): " << ( *active ? "Active" : "Inactive" ) << std::endl;
	}

	// Safe null handling
	if ( doc.isNull( "user.spouse" ) )
	{
		std::cout << "Spouse (safe): Not specified" << std::endl;
	}

	// Safe array processing
	if ( doc.isArray( "user.hobbies" ) )
	{
		std::cout << "Hobbies (safe): " << doc.getArraySize( "user.hobbies" ) << " items" << std::endl;
	}

	std::cout << "\nType checking with non-existent fields:" << std::endl;
	std::cout << "nonexistent.field is string: " << ( doc.isString( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "nonexistent.field is int: " << ( doc.isInt( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "nonexistent.field is object: " << ( doc.isObject( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
}

//=====================================================================
// Error handling and validation
//=====================================================================

void demonstrateErrorHandling()
{
	std::cout << "\n=== Error Handling and Validation ===" << std::endl;

	// Test with invalid JSON
	std::string invalidJson = R"({
		"name": "Test",
		"value": 123,
		"missing": 
	})";

	auto maybeDoc = Document::fromJsonString( invalidJson );
	if ( !maybeDoc.has_value() )
	{
		std::cout << "Successfully detected invalid JSON" << std::endl;
	}

	// Test with valid document
	Document doc = Document::createObject();
	doc.setString( "config.database.host", "localhost" );
	doc.setInt( "config.database.port", 5432 );

	// Validation examples
	std::cout << "Validation Results:" << std::endl;
	std::cout << "  Has database config: " << ( doc.hasField( "config.database.host" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "  Has missing field: " << ( doc.hasField( "config.missing" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "  Document is valid: " << ( doc.isValid() ? "YES" : "NO" ) << std::endl;

	// Type safety demonstration
	std::cout << "Type Safety:" << std::endl;
	std::cout << "  String field as string: '" << doc.getString( "config.database.host" ).value_or( "unknown" ) << "'" << std::endl;
	std::cout << "  String field as int: " << doc.getInt( "config.database.host" ).value_or( 0 ) << " (returns default 0)" << std::endl;
	std::cout << "  Int field as int: " << doc.getInt( "config.database.port" ).value_or( 0 ) << std::endl;
	std::cout << "  Int field as string: '" << doc.getString( "config.database.port" ).value_or( "unknown" ) << "' (returns empty)" << std::endl;
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
	std::cout << "NFX-Core JSON Document Serialization Samples" << std::endl;
	std::cout << "=============================================" << std::endl;

	try
	{
		demonstrateConfigurationManagement();
		demonstrateApiResponseHandling();
		demonstrateProgrammaticConstruction();
		demonstrateTypeChecking();
		demonstrateErrorHandling();

		std::cout << "\nAll samples completed successfully!" << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
