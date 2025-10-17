/**
 * @file Sample_JSONArrayEnumerator.cpp
 * @brief Demonstration of ArrayEnumerator functionality
 * @details Shows usage patterns for JSON array enumeration using Document references.
 */

#include <iostream>
#include <string>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/ArrayEnumerator.h>

using nfx::serialization::json::Document;
using nfx::serialization::json::ArrayEnumerator;

int main()
{
	std::cout << "=== ArrayEnumerator Sample ===" << std::endl;

	// Create test JSON document with arrays
	std::string jsonStr = R"({
		"users": [
			{"name": "Alice", "age": 30, "active": true},
			{"name": "Bob", "age": 25, "active": false},
			{"name": "Charlie", "age": 35, "active": true}
		],
		"scores": [100, 95, 87, 92, 78],
		"tags": ["important", "urgent", "review"]
	})";

	// Parse JSON
	auto doc = Document::fromJsonString( jsonStr );
	if ( !doc.has_value() )
	{
		std::cerr << "Failed to parse JSON!" << std::endl;
		return 1;
	}

	std::cout << "Successfully parsed JSON document." << std::endl;

	// Create ArrayEnumerator
	ArrayEnumerator enumerator( doc.value() );

	// Test 1: Enumerate through users array using dot notation
	std::cout << "\n--- Test 1: Users Array (Dot Notation) ---" << std::endl;

	if ( enumerator.setPath( "users" ) )
	{
		std::cout << "Array size: " << enumerator.size() << std::endl;

		// Reset to beginning and enumerate
		enumerator.reset();
		size_t userIndex = 0;

		while ( !enumerator.isEnd() )
		{
			std::cout << "User " << userIndex << ":" << std::endl;

			// Get current user as Document
			try
			{
				Document user = enumerator.currentElement();

				// Access user properties
				auto name = user.get<std::string>( "name" );
				auto age = user.get<int64_t>( "age" );
				auto active = user.get<bool>( "active" );

				std::cout << "  Name: " << ( name ? *name : "N/A" ) << std::endl;
				std::cout << "  Age: " << ( age ? std::to_string( *age ) : "N/A" ) << std::endl;
				std::cout << "  Active: " << ( active ? ( *active ? "Yes" : "No" ) : "N/A" ) << std::endl;
			}
			catch ( const std::exception& e )
			{
				std::cout << "  Error accessing user: " << e.what() << std::endl;
			}

			// Move to next user
			if ( !enumerator.next() )
			{
				break;
			}
			userIndex++;
		}
	}
	else
	{
		std::cout << "Failed to navigate to users array!" << std::endl;
	}

	// Test 2: Enumerate through scores array using JSON Pointer
	std::cout << "\n--- Test 2: Scores Array (JSON Pointer) ---" << std::endl;

	if ( enumerator.setPointer( "/scores" ) )
	{
		std::cout << "Scores array size: " << enumerator.size() << std::endl;

		enumerator.reset();
		size_t scoreIndex = 0;

		while ( !enumerator.isEnd() )
		{
			auto score = enumerator.currentInt();
			std::cout << "Score[" << scoreIndex << "]: " << ( score ? std::to_string( *score ) : "N/A" ) << std::endl;

			if ( !enumerator.next() )
			{
				break;
			}
			scoreIndex++;
		}
	}
	else
	{
		std::cout << "Failed to navigate to scores array!" << std::endl;
	}

	// Test 3: Direct index access and backwards navigation
	std::cout << "\n--- Test 3: Tags Array (Random Access) ---" << std::endl;

	if ( enumerator.setPath( "tags" ) )
	{
		std::cout << "Tags array size: " << enumerator.size() << std::endl;

		// Access specific indices
		if ( enumerator.moveTo( 1 ) )
		{
			auto tag = enumerator.currentString();
			std::cout << "Tag at index 1: " << ( tag ? *tag : "N/A" ) << std::endl;
		}

		if ( enumerator.moveTo( 2 ) )
		{
			auto tag = enumerator.currentString();
			std::cout << "Tag at index 2: " << ( tag ? *tag : "N/A" ) << std::endl;
		}

		// Move backwards
		if ( enumerator.previous() )
		{
			auto tag = enumerator.currentString();
			std::cout << "Previous tag: " << ( tag ? *tag : "N/A" ) << std::endl;
		}

		// Move to beginning
		if ( enumerator.moveTo( 0 ) )
		{
			auto tag = enumerator.currentString();
			std::cout << "First tag: " << ( tag ? *tag : "N/A" ) << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to navigate to tags array!" << std::endl;
	}

	// Test 4: Error handling
	std::cout << "\n--- Test 4: Error Handling ---" << std::endl;

	// Try invalid path
	if ( !enumerator.setPath( "nonexistent" ) )
	{
		std::cout << "Correctly rejected invalid path 'nonexistent'" << std::endl;
	}

	// Try non-array path
	if ( !enumerator.setPath( "users.0" ) )
	{
		std::cout << "Correctly rejected non-array path 'users.0'" << std::endl;
	}

	// Try invalid JSON Pointer
	if ( !enumerator.setPointer( "/invalid/path" ) )
	{
		std::cout << "Correctly rejected invalid JSON Pointer '/invalid/path'" << std::endl;
	}

	std::cout << "\n=== ArrayEnumerator Sample Complete ===" << std::endl;
	return 0;
}
