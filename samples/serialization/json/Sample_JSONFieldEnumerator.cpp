/**
 * @file Sample_JSONFieldEnumerator.cpp
 * @brief Demonstration of FieldEnumerator functionality
 * @details Shows usage patterns for JSON object field enumeration using Document references.
 */

#include <iostream>
#include <string>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/FieldEnumerator.h>

using nfx::serialization::json::Document;
using nfx::serialization::json::FieldEnumerator;

int main()
{
	std::cout << "=== FieldEnumerator Sample ===" << std::endl;

	// Create test JSON document with nested objects
	std::string jsonStr = R"({
		"user": {
			"name": "Alice Johnson",
			"age": 28,
			"active": true,
			"height": 1.65,
			"department": "Engineering"
		},
		"preferences": {
			"theme": "dark",
			"language": "en-US",
			"notifications": true,
			"fontSize": 14
		},
		"config": {
			"server": {
				"host": "localhost",
				"port": 8080,
				"ssl": false
			}
		},
		"metadata": {
			"created": "2024-01-15T10:30:00Z",
			"version": "1.2.3",
			"tags": ["production", "web", "api"]
		}
	})";

	// Parse JSON
	auto doc = Document::fromJsonString( jsonStr );
	if ( !doc.has_value() )
	{
		std::cerr << "Failed to parse JSON!" << std::endl;
		return 1;
	}

	std::cout << "Successfully parsed JSON document." << std::endl;

	// Create FieldEnumerator
	FieldEnumerator enumerator( doc.value() );

	// Test 1: Enumerate through user object fields using dot notation
	std::cout << "\n--- Test 1: User Object Fields (Dot Notation) ---" << std::endl;

	if ( enumerator.setPath( "user" ) )
	{
		std::cout << "Object field count: " << enumerator.size() << std::endl;

		// Reset to beginning and enumerate
		enumerator.reset();
		size_t fieldIndex = 0;

		while ( !enumerator.isEnd() )
		{
			std::string key = enumerator.currentKey();
			std::cout << "Field " << fieldIndex << " - Key: '" << key << "'";

			// Try different typed accessors
			if ( auto stringVal = enumerator.currentString() )
			{
				std::cout << " (string): " << *stringVal << std::endl;
			}
			else if ( auto intVal = enumerator.currentInt() )
			{
				std::cout << " (int): " << *intVal << std::endl;
			}
			else if ( auto doubleVal = enumerator.currentDouble() )
			{
				std::cout << " (double): " << *doubleVal << std::endl;
			}
			else if ( auto boolVal = enumerator.currentBool() )
			{
				std::cout << " (bool): " << ( *boolVal ? "true" : "false" ) << std::endl;
			}
			else
			{
				std::cout << " (other type)" << std::endl;
			}

			// Move to next field
			if ( !enumerator.next() )
			{
				break;
			}
			fieldIndex++;
		}
	}
	else
	{
		std::cout << "Failed to navigate to user object!" << std::endl;
	}

	// Test 2: Enumerate through preferences using JSON Pointer
	std::cout << "\n--- Test 2: Preferences Object (JSON Pointer) ---" << std::endl;

	if ( enumerator.setPointer( "/preferences" ) )
	{
		std::cout << "Preferences field count: " << enumerator.size() << std::endl;

		enumerator.reset();
		while ( !enumerator.isEnd() )
		{
			std::string key = enumerator.currentKey();
			std::cout << "Preference '" << key << "': ";

			// Get as Document for flexible access
			try
			{
				Document fieldDoc = enumerator.currentValue();

				// Try to get as string first, then other types
				if ( auto stringVal = enumerator.currentString() )
				{
					std::cout << "\"" << *stringVal << "\"" << std::endl;
				}
				else if ( auto boolVal = enumerator.currentBool() )
				{
					std::cout << ( *boolVal ? "true" : "false" ) << std::endl;
				}
				else if ( auto intVal = enumerator.currentInt() )
				{
					std::cout << *intVal << std::endl;
				}
				else
				{
					std::cout << "(complex type)" << std::endl;
				}
			}
			catch ( const std::exception& e )
			{
				std::cout << "Error accessing field: " << e.what() << std::endl;
			}

			if ( !enumerator.next() )
			{
				break;
			}
		}
	}
	else
	{
		std::cout << "Failed to navigate to preferences object!" << std::endl;
	}

	// Test 3: Key-based navigation and random access
	std::cout << "\n--- Test 3: Key-Based Navigation ---" << std::endl;

	if ( enumerator.setPath( "user" ) )
	{
		// Direct key access
		if ( enumerator.moveToKey( "name" ) )
		{
			std::cout << "Direct access to 'name': " << *enumerator.currentString() << std::endl;
		}

		if ( enumerator.moveToKey( "age" ) )
		{
			std::cout << "Direct access to 'age': " << *enumerator.currentInt() << std::endl;
		}

		if ( enumerator.moveToKey( "active" ) )
		{
			std::cout << "Direct access to 'active': " << ( *enumerator.currentBool() ? "true" : "false" ) << std::endl;
		}

		// Index-based access
		std::cout << "Field at index 0: '" << enumerator.currentKey() << "'" << std::endl;

		if ( enumerator.moveTo( 2 ) )
		{
			std::cout << "Field at index 2: '" << enumerator.currentKey() << "'" << std::endl;
		}
	}

	// Test 4: Nested object enumeration
	std::cout << "\n--- Test 4: Nested Object Access ---" << std::endl;

	if ( enumerator.setPointer( "/config/server" ) )
	{
		std::cout << "Server configuration fields:" << std::endl;

		while ( !enumerator.isEnd() )
		{
			std::string key = enumerator.currentKey();

			if ( auto stringVal = enumerator.currentString() )
			{
				std::cout << "  " << key << ": \"" << *stringVal << "\"" << std::endl;
			}
			else if ( auto intVal = enumerator.currentInt() )
			{
				std::cout << "  " << key << ": " << *intVal << std::endl;
			}
			else if ( auto boolVal = enumerator.currentBool() )
			{
				std::cout << "  " << key << ": " << ( *boolVal ? "true" : "false" ) << std::endl;
			}

			if ( !enumerator.next() )
			{
				break;
			}
		}
	}

	// Test 5: Error handling and edge cases
	std::cout << "\n--- Test 5: Error Handling ---" << std::endl;

	// Try invalid path
	if ( !enumerator.setPath( "nonexistent" ) )
	{
		std::cout << "Correctly rejected invalid path 'nonexistent'" << std::endl;
	}

	// Try array path (should fail for field enumerator)
	if ( !enumerator.setPath( "metadata.tags" ) )
	{
		std::cout << "Correctly rejected array path 'metadata.tags'" << std::endl;
	}

	// Try invalid JSON Pointer
	if ( !enumerator.setPointer( "/invalid/nested/path" ) )
	{
		std::cout << "Correctly rejected invalid JSON Pointer '/invalid/nested/path'" << std::endl;
	}

	// Test root object enumeration
	std::cout << "\n--- Test 6: Root Object Enumeration ---" << std::endl;

	if ( enumerator.setPath( "" ) ) // Empty path = root object
	{
		std::cout << "Root object has " << enumerator.size() << " top-level fields:" << std::endl;

		while ( !enumerator.isEnd() )
		{
			std::string key = enumerator.currentKey();
			std::cout << "  - " << key << std::endl;

			if ( !enumerator.next() )
			{
				break;
			}
		}
	}

	std::cout << "\n=== FieldEnumerator Sample Complete ===" << std::endl;
	return 0;
}
