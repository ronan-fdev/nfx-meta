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
	Document config;

	// Application settings
	config.set<std::string>( "app.name", "MyApplication" );
	config.set<std::string>( "app.version", "1.2.3" );
	config.set<bool>( "app.debug", true );

	// Database configuration
	config.set<std::string>( "database.host", "localhost" );
	config.set<int64_t>( "database.port", 5432 );
	config.set<std::string>( "database.name", "myapp_db" );
	config.set<std::string>( "database.user", "admin" );

	// Server configuration with arrays
	auto allowedHosts = config.get<Document::Array>( "server.allowedHosts" ).value_or( Document::Array{} );
	if ( !config.get<Document::Array>( "server.allowedHosts" ).has_value() )
	{
		// Create array at path if it doesn't exist
		config.set<Document::Array>( "server.allowedHosts" );
		allowedHosts = config.get<Document::Array>( "server.allowedHosts" ).value();
	}
	allowedHosts.add<std::string>( "localhost" );
	allowedHosts.add<std::string>( "127.0.0.1" );
	allowedHosts.add<std::string>( "::1" );

	config.set<int64_t>( "server.port", 8080 );
	config.set<int64_t>( "server.maxConnections", 1000 );

	// Feature flags
	config.set<bool>( "features.enableCaching", true );
	config.set<bool>( "features.enableLogging", false );
	config.set<std::string>( "features.logLevel", "INFO" );

	// Output the configuration
	std::cout << "Generated Configuration:" << std::endl;
	std::cout << config.toJsonString( 2 ) << std::endl;

	// Demonstrate path-based access
	std::cout << "\nReading configuration values:" << std::endl;
	std::cout << "App Name: " << config.get<std::string>( "app.name" ).value_or( "Unknown" ) << std::endl;
	std::cout << "DB Port: " << config.get<int64_t>( "database.port" ).value_or( 0 ) << std::endl;
	std::cout << "Debug Mode: " << ( config.get<bool>( "app.debug" ).value_or( false ) ? "ON" : "OFF" ) << std::endl;
	std::cout << "Allowed Hosts: " << config.get<Document::Array>( "server.allowedHosts" ).value().size() << " entries" << std::endl;
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
	std::cout << "API Response Status: " << response.get<std::string>( "status" ).value_or( "Unknown" ) << std::endl;
	std::cout << "User ID: " << response.get<int64_t>( "data.user.id" ).value_or( 0 ) << std::endl;
	std::cout << "Full Name: " << response.get<std::string>( "data.user.profile.firstName" ).value_or( "Unknown" )
			  << " " << response.get<std::string>( "data.user.profile.lastName" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Email: " << response.get<std::string>( "data.user.profile.email" ).value_or( "Unknown" ) << std::endl;
	std::cout << "Theme: " << response.get<std::string>( "data.user.preferences.theme" ).value_or( "default" ) << std::endl;

	// Process arrays
	std::cout << "User Roles:" << std::endl;
	size_t roleCount = response.get<Document::Array>( "data.user.roles" ).value().size();

	for ( size_t i = 0; i < roleCount; ++i )
	{
		auto roleDocOpt = response.get<Document::Array>( "data.user.roles" ).value().get<Document>( i );
		if ( roleDocOpt.has_value() )
		{
			Document roleDoc = roleDocOpt.value();
			// For simple string arrays, we'd need to handle this differently
			// This is a limitation we could address in future versions
		}
	}

	// Process permissions array (complex objects)
	std::cout << "Permissions:" << std::endl;
	size_t permCount = response.get<Document::Array>( "data.permissions" ).value().size();

	for ( size_t i = 0; i < permCount; ++i )
	{
		auto permOpt = response.get<Document::Array>( "data.permissions" ).value().get<Document>( i );
		if ( permOpt.has_value() )
		{
			Document perm = permOpt.value();
			std::cout << "  Resource: " << perm.get<std::string>( "resource" ).value_or( "unknown" ) << std::endl;
			std::cout << "  Actions: " << perm.get<Document::Array>( "actions" ).value().size() << " permissions" << std::endl;
		}
	}
}

//=====================================================================
// Building complex documents programmatically
//=====================================================================

void demonstrateProgrammaticConstruction()
{
	std::cout << "\n=== Programmatic Document Construction ===" << std::endl;

	// Build a complex document from scratch
	Document report;

	// Report metadata
	report.set<std::string>( "reportId", "RPT-2025-001" );
	report.set<std::string>( "generatedAt", "2025-10-02T14:30:00Z" );
	report.set<std::string>( "title", "Monthly Sales Report" );

	// Create summary object
	report.set<double>( "summary.totalRevenue", 125478.50 );
	report.set<int64_t>( "summary.totalOrders", 1247 );
	report.set<double>( "summary.averageOrderValue", 100.62 );

	Document salesArray;
	salesArray.set<Document::Array>( "" );
	auto salesArrayWrapper = salesArray.get<Document::Array>( "" );

	// Add individual sales records
	for ( int i = 0; i < 5; ++i )
	{
		Document sale;

		sale.set<std::string>( "date", "2025-09-" + std::to_string( 25 + i ) );
		sale.set<double>( "amount", 1250.00 + ( i * 50 ) );
		sale.set<int64_t>( "orderId", 1000 + i );

		if ( salesArrayWrapper.has_value() )
		{
			salesArrayWrapper->add<Document>( sale );
		}
	}

	report.set<Document>( "salesData", salesArray );

	// Add regional breakdown
	report.set<double>( "regions.north.revenue", 45000.00 );
	report.set<int64_t>( "regions.north.orders", 450 );
	report.set<double>( "regions.south.revenue", 38000.00 );
	report.set<int64_t>( "regions.south.orders", 380 );
	report.set<double>( "regions.east.revenue", 25000.00 );
	report.set<int64_t>( "regions.east.orders", 250 );
	report.set<double>( "regions.west.revenue", 17478.50 );
	report.set<int64_t>( "regions.west.orders", 167 );

	// Output the complete report
	std::cout << "Generated Sales Report:" << std::endl;
	std::cout << report.toJsonString( 2 ) << std::endl;

	// Demonstrate data extraction
	std::cout << "\nReport Summary:" << std::endl;
	std::cout << "Total Revenue: $" << report.get<double>( "summary.totalRevenue" ).value_or( 0.0 ) << std::endl;
	std::cout << "Total Orders: " << report.get<int64_t>( "summary.totalOrders" ).value_or( 0 ) << std::endl;
	std::cout << "Sales Records: " << report.get<Document::Array>( "salesData" ).value().size() << " entries" << std::endl;
	std::cout << "North Region: $" << report.get<double>( "regions.north.revenue" ).value_or( 0.0 )
			  << " (" << report.get<int64_t>( "regions.north.orders" ).value_or( 0 ) << " orders)" << std::endl;
}

//=====================================================================
// Type checking and safe access
//=====================================================================

void demonstrateTypeChecking()
{
	std::cout << "\n=== Type Checking and Safe Access ===" << std::endl;

	// Create a document with all JSON types
	Document doc;

	// Set up different data types
	doc.set<std::string>( "user.name", "Alice Johnson" );
	doc.set<int64_t>( "user.age", 28 );
	doc.set<double>( "user.height", 1.65 );
	doc.set<bool>( "user.active", true );
	doc.setNull( "user.spouse" );

	// Create nested object and array
	doc.set<std::string>( "user.address.city", "New York" );
	doc.set<std::string>( "user.address.country", "USA" );

	// Use the semantic Array API
	auto hobbiesArray = doc.get<Document::Array>( "user.hobbies" );
	if ( !hobbiesArray.has_value() )
	{
		// Create the array first if it doesn't exist
		doc.set<Document::Array>( "user.hobbies" );
		hobbiesArray = doc.get<Document::Array>( "user.hobbies" );
	}

	if ( hobbiesArray.has_value() )
	{
		hobbiesArray->add<std::string>( "reading" );
		hobbiesArray->add<std::string>( "gaming" );
		hobbiesArray->add<std::string>( "hiking" );
	}

	std::cout << "Document structure:" << std::endl;
	std::cout << doc.toJsonString( 2 ) << std::endl;

	std::cout << "\nType checking results:" << std::endl;

	// String type checking
	std::cout << "user.name is string: " << ( doc.is<std::string>( "user.name" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.age is string: " << ( doc.is<std::string>( "user.age" ) ? "YES" : "NO" ) << std::endl;

	// Integer type checking
	std::cout << "user.age is integer: " << ( doc.is<int>( "user.age" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.height is integer: " << ( doc.is<int>( "user.height" ) ? "YES" : "NO" ) << std::endl;

	// Double type checking
	std::cout << "user.height is double: " << ( doc.is<double>( "user.height" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.age is double: " << ( doc.is<double>( "user.age" ) ? "YES" : "NO" ) << std::endl;

	// Boolean type checking
	std::cout << "user.active is boolean: " << ( doc.is<bool>( "user.active" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is boolean: " << ( doc.is<bool>( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Null type checking
	std::cout << "user.spouse is null: " << ( doc.isNull( "user.spouse" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is null: " << ( doc.isNull( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Object type checking
	std::cout << "user is object: " << ( doc.is<Document::Object>( "user" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.address is object: " << ( doc.is<Document::Object>( "user.address" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.name is object: " << ( doc.is<Document::Object>( "user.name" ) ? "YES" : "NO" ) << std::endl;

	// Array type checking
	std::cout << "user.hobbies is array: " << ( doc.is<Document::Array>( "user.hobbies" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "user.address is array: " << ( doc.is<Document::Array>( "user.address" ) ? "YES" : "NO" ) << std::endl;

	std::cout << "\nSafe data access using type checking:" << std::endl;

	// Safe string access
	if ( auto name = doc.get<std::string>( "user.name" ) )
	{
		std::cout << "Name (safe): " << *name << std::endl;
	}

	// Safe numeric access
	if ( auto age = doc.get<int64_t>( "user.age" ) )
	{
		std::cout << "Age (safe): " << *age << " years" << std::endl;
	}

	if ( auto height = doc.get<double>( "user.height" ) )
	{
		std::cout << "Height (safe): " << *height << "m" << std::endl;
	}

	// Safe boolean access
	if ( auto active = doc.get<bool>( "user.active" ) )
	{
		std::cout << "Status (safe): " << ( *active ? "Active" : "Inactive" ) << std::endl;
	}

	// Safe null handling
	if ( doc.isNull( "user.spouse" ) )
	{
		std::cout << "Spouse (safe): Not specified" << std::endl;
	}

	// Safe array processing
	if ( doc.is<Document::Array>( "user.hobbies" ) )
	{
		std::cout << "Hobbies (safe): " << doc.get<Document::Array>( "user.hobbies" ).value().size() << " items" << std::endl;
	}

	std::cout << "\nType checking with non-existent fields:" << std::endl;
	std::cout << "nonexistent.field is string: " << ( doc.is<std::string>( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "nonexistent.field is int: " << ( doc.is<int>( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "nonexistent.field is object: " << ( doc.is<Document::Object>( "nonexistent.field" ) ? "YES" : "NO" ) << std::endl;
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
	Document doc;

	doc.set<std::string>( "config.database.host", "localhost" );
	doc.set<int64_t>( "config.database.port", 5432 );

	// Validation examples
	std::cout << "Validation Results:" << std::endl;
	std::cout << "  Has database config: " << ( doc.hasValue( "config.database.host" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "  Has missing field: " << ( doc.hasValue( "config.missing" ) ? "YES" : "NO" ) << std::endl;
	std::cout << "  Document is valid: " << ( doc.isValid() ? "YES" : "NO" ) << std::endl;

	// Type safety demonstration
	std::cout << "Type Safety:" << std::endl;
	std::cout << "  String field as string: '" << doc.get<std::string>( "config.database.host" ).value_or( "unknown" ) << "'" << std::endl;
	std::cout << "  String field as int: " << doc.get<int64_t>( "config.database.host" ).value_or( 0 ) << " (returns default 0)" << std::endl;
	std::cout << "  Int field as int: " << doc.get<int64_t>( "config.database.port" ).value_or( 0 ) << std::endl;
	std::cout << "  Int field as string: '" << doc.get<std::string>( "config.database.port" ).value_or( "unknown" ) << "' (returns empty)" << std::endl;
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
	std::cout << "nfx-meta JSON Document Serialization Samples" << std::endl;
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
