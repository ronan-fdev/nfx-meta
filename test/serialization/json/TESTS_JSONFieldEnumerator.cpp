/**
 * @file TESTS_JSONFieldEnumerator.cpp
 * @brief Comprehensive tests for JSON FieldEnumerator functionality
 * @details Tests covering object field navigation, key-value access, path validation,
 *          enumerator state management, and error handling scenarios.
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/FieldEnumerator.h>
#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// JSON FieldEnumerator tests
	//=====================================================================

	//----------------------------------------------
	// JSON FieldEnumerator test fixture
	//----------------------------------------------

	class FieldEnumeratorTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create complex test JSON document with nested objects
			std::string jsonStr = R"({
				"user": {
					"name": "Alice",
					"age": 30,
					"active": true,
					"height": 1.65,
					"spouse": null
				},
				"preferences": {
					"theme": "dark",
					"language": "en-US",
					"notifications": true,
					"fontSize": 12
				},
				"settings": {
					"nested": {
						"deep": "value"
					}
				},
				"empty": {},
				"mixed": {
					"string": "test",
					"number": 42,
					"boolean": false,
					"array": [1, 2, 3],
					"object": {"key": "value"}
				}
			})";

			auto maybeDoc = Document::fromJsonString( jsonStr );
			ASSERT_TRUE( maybeDoc.has_value() );
			document = std::move( maybeDoc.value() );
		}

		Document document;
	};

	//----------------------------------------------
	// Basic navigation
	//----------------------------------------------

	TEST_F( FieldEnumeratorTest, ConstructorAndBasicNavigation )
	{
		FieldEnumerator enumerator( document );

		// Test dot notation navigation
		EXPECT_TRUE( enumerator.setPath( "user" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 5 ); // name, age, active, height, spouse
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );

		// Test JSON Pointer navigation
		EXPECT_TRUE( enumerator.setPointer( "/preferences" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 4 ); // theme, language, notifications, fontSize
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );
	}

	TEST_F( FieldEnumeratorTest, InvalidPathHandling )
	{
		FieldEnumerator enumerator( document );

		// Test invalid paths
		EXPECT_FALSE( enumerator.setPath( "nonexistent" ) );
		EXPECT_FALSE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 0 );

		// Test array path (should fail for field enumerator)
		EXPECT_FALSE( enumerator.setPath( "mixed.array" ) );
		EXPECT_FALSE( enumerator.isValid() );

		// Test invalid JSON Pointer
		EXPECT_FALSE( enumerator.setPointer( "/invalid/path" ) );
		EXPECT_FALSE( enumerator.isValid() );

		// Test empty path (should be valid - points to root object)
		EXPECT_TRUE( enumerator.setPath( "" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_GT( enumerator.size(), 0 ); // Root object has fields
	}

	//----------------------------------------------
	// Field access
	//----------------------------------------------

	TEST_F( FieldEnumeratorTest, FieldKeyAndValueAccess )
	{
		FieldEnumerator enumerator( document );

		// Test user object field access
		ASSERT_TRUE( enumerator.setPath( "user" ) );

		// Fields should be in sorted order: active, age, height, name, spouse
		EXPECT_EQ( enumerator.currentKey(), "active" );
		auto activeValue = enumerator.currentBool();
		ASSERT_TRUE( activeValue.has_value() );
		EXPECT_TRUE( *activeValue );

		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.currentKey(), "age" );
		auto ageValue = enumerator.currentInt();
		ASSERT_TRUE( ageValue.has_value() );
		EXPECT_EQ( *ageValue, 30 );

		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.currentKey(), "height" );
		auto heightValue = enumerator.currentDouble();
		ASSERT_TRUE( heightValue.has_value() );
		EXPECT_DOUBLE_EQ( *heightValue, 1.65 );

		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.currentKey(), "name" );
		auto nameValue = enumerator.currentString();
		ASSERT_TRUE( nameValue.has_value() );
		EXPECT_EQ( *nameValue, "Alice" );

		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.currentKey(), "spouse" );
		// Null values return nullopt for typed accessors
		EXPECT_FALSE( enumerator.currentString().has_value() );
		EXPECT_FALSE( enumerator.currentInt().has_value() );
	}

	TEST_F( FieldEnumeratorTest, DocumentValueAccess )
	{
		FieldEnumerator enumerator( document );

		// Test accessing nested object as Document
		ASSERT_TRUE( enumerator.setPath( "mixed" ) );

		// Navigate to object field
		EXPECT_TRUE( enumerator.moveToKey( "object" ) );
		EXPECT_EQ( enumerator.currentKey(), "object" );

		// Get field value as Document
		Document objectDoc = enumerator.currentValue();
		auto keyValue = objectDoc.get<std::string>( "key" );
		ASSERT_TRUE( keyValue.has_value() );
		EXPECT_EQ( *keyValue, "value" );
	}

	TEST_F( FieldEnumeratorTest, WrongTypeAccess )
	{
		FieldEnumerator enumerator( document );

		// Test accessing string field as different types
		ASSERT_TRUE( enumerator.setPath( "user" ) );
		EXPECT_TRUE( enumerator.moveToKey( "name" ) );

		auto stringValue = enumerator.currentString();
		EXPECT_TRUE( stringValue.has_value() );
		EXPECT_EQ( *stringValue, "Alice" );

		// Should return nullopt for wrong types
		auto intValue = enumerator.currentInt();
		EXPECT_FALSE( intValue.has_value() );

		auto doubleValue = enumerator.currentDouble();
		EXPECT_FALSE( doubleValue.has_value() );

		auto boolValue = enumerator.currentBool();
		EXPECT_FALSE( boolValue.has_value() );
	}

	//----------------------------------------------
	// Enumerator movement
	//----------------------------------------------

	TEST_F( FieldEnumeratorTest, ForwardEnumeration )
	{
		FieldEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "preferences" ) );

		std::vector<std::string> expectedKeys = { "fontSize", "language", "notifications", "theme" }; // sorted order
		std::vector<std::string> actualKeys;

		// Enumerate through all fields
		while ( !enumerator.isEnd() )
		{
			actualKeys.push_back( enumerator.currentKey() );
			if ( !enumerator.next() )
			{
				break;
			}
		}

		EXPECT_EQ( actualKeys, expectedKeys );
		EXPECT_TRUE( enumerator.isEnd() );
		EXPECT_EQ( enumerator.index(), enumerator.size() );
	}

	TEST_F( FieldEnumeratorTest, RandomAccess )
	{
		FieldEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "preferences" ) );

		// Test moveTo specific indices (sorted: fontSize, language, notifications, theme)
		EXPECT_TRUE( enumerator.moveTo( 2 ) );
		EXPECT_EQ( enumerator.index(), 2 );
		EXPECT_EQ( enumerator.currentKey(), "notifications" );

		EXPECT_TRUE( enumerator.moveTo( 0 ) );
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_EQ( enumerator.currentKey(), "fontSize" );

		EXPECT_TRUE( enumerator.moveTo( 3 ) );
		EXPECT_EQ( enumerator.index(), 3 );
		EXPECT_EQ( enumerator.currentKey(), "theme" );

		// Test out of bounds
		EXPECT_FALSE( enumerator.moveTo( 10 ) );
		EXPECT_EQ( enumerator.index(), 3 ); // Should stay at last valid position
	}

	TEST_F( FieldEnumeratorTest, KeyBasedNavigation )
	{
		FieldEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "user" ) );

		// Test moveToKey
		EXPECT_TRUE( enumerator.moveToKey( "name" ) );
		EXPECT_EQ( enumerator.currentKey(), "name" );
		auto nameValue = enumerator.currentString();
		ASSERT_TRUE( nameValue.has_value() );
		EXPECT_EQ( *nameValue, "Alice" );

		EXPECT_TRUE( enumerator.moveToKey( "age" ) );
		EXPECT_EQ( enumerator.currentKey(), "age" );
		auto ageValue = enumerator.currentInt();
		ASSERT_TRUE( ageValue.has_value() );
		EXPECT_EQ( *ageValue, 30 );

		// Test non-existent key
		EXPECT_FALSE( enumerator.moveToKey( "nonexistent" ) );
		EXPECT_EQ( enumerator.currentKey(), "age" ); // Should stay at previous position
	}

	TEST_F( FieldEnumeratorTest, BackwardMovement )
	{
		FieldEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "preferences" ) );

		// Move to end
		EXPECT_TRUE( enumerator.moveTo( 3 ) ); // theme
		EXPECT_EQ( enumerator.currentKey(), "theme" );

		// Move backwards
		EXPECT_TRUE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 2 );
		EXPECT_EQ( enumerator.currentKey(), "notifications" );

		EXPECT_TRUE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 1 );
		EXPECT_EQ( enumerator.currentKey(), "language" );

		// Try to move before beginning
		enumerator.reset();
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 0 );
	}

	TEST_F( FieldEnumeratorTest, ResetFunctionality )
	{
		FieldEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "user" ) );

		// Move to middle
		EXPECT_TRUE( enumerator.moveTo( 2 ) );
		EXPECT_EQ( enumerator.index(), 2 );

		// Reset to beginning
		enumerator.reset();
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );
		EXPECT_EQ( enumerator.currentKey(), "active" ); // First field in sorted order
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST_F( FieldEnumeratorTest, EmptyObjectHandling )
	{
		FieldEnumerator enumerator( document );

		ASSERT_TRUE( enumerator.setPath( "empty" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 0 );
		EXPECT_TRUE( enumerator.isEnd() );
		EXPECT_EQ( enumerator.index(), 0 );

		// Try to access field in empty object
		EXPECT_THROW( enumerator.currentKey(), std::runtime_error );
		EXPECT_THROW( enumerator.currentValue(), std::runtime_error );
		EXPECT_FALSE( enumerator.currentString().has_value() );
		EXPECT_FALSE( enumerator.currentInt().has_value() );

		// Try to move in empty object
		EXPECT_FALSE( enumerator.next() );
		EXPECT_FALSE( enumerator.previous() );
		EXPECT_FALSE( enumerator.moveTo( 0 ) );
		EXPECT_FALSE( enumerator.moveToKey( "anything" ) );
	}

	TEST_F( FieldEnumeratorTest, MixedTypeFields )
	{
		FieldEnumerator enumerator( document );

		ASSERT_TRUE( enumerator.setPath( "mixed" ) );
		EXPECT_EQ( enumerator.size(), 5 ); // array, boolean, number, object, string

		// Check each field type
		EXPECT_TRUE( enumerator.moveToKey( "string" ) );
		auto stringVal = enumerator.currentString();
		EXPECT_TRUE( stringVal.has_value() );
		EXPECT_EQ( *stringVal, "test" );

		EXPECT_TRUE( enumerator.moveToKey( "number" ) );
		auto intVal = enumerator.currentInt();
		EXPECT_TRUE( intVal.has_value() );
		EXPECT_EQ( *intVal, 42 );

		EXPECT_TRUE( enumerator.moveToKey( "boolean" ) );
		auto boolVal = enumerator.currentBool();
		EXPECT_TRUE( boolVal.has_value() );
		EXPECT_FALSE( *boolVal );

		EXPECT_TRUE( enumerator.moveToKey( "array" ) );
		Document arrayDoc = enumerator.currentValue();
		EXPECT_TRUE( arrayDoc.is<Document::Array>( "" ) ); // Root is array

		EXPECT_TRUE( enumerator.moveToKey( "object" ) );
		Document objectDoc = enumerator.currentValue();
		EXPECT_TRUE( objectDoc.is<Document::Object>( "" ) ); // Root is object
	}

	//----------------------------------------------
	// JSON Pointer specific tests
	//----------------------------------------------

	TEST_F( FieldEnumeratorTest, NestedObjectAccess )
	{
		FieldEnumerator enumerator( document );

		// Test JSON Pointer navigation to nested object
		ASSERT_TRUE( enumerator.setPointer( "/settings/nested" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 1 );

		EXPECT_EQ( enumerator.currentKey(), "deep" );
		auto deepValue = enumerator.currentString();
		ASSERT_TRUE( deepValue.has_value() );
		EXPECT_EQ( *deepValue, "value" );
	}

	TEST_F( FieldEnumeratorTest, EnumeratorStateConsistency )
	{
		FieldEnumerator enumerator( document );

		// Set up navigation
		ASSERT_TRUE( enumerator.setPath( "user" ) );
		EXPECT_TRUE( enumerator.moveToKey( "name" ) );

		// Change navigation path
		ASSERT_TRUE( enumerator.setPath( "preferences" ) );
		EXPECT_EQ( enumerator.index(), 0 );				  // Should reset to beginning
		EXPECT_EQ( enumerator.currentKey(), "fontSize" ); // First field in sorted order

		// Verify state is completely refreshed
		EXPECT_EQ( enumerator.size(), 4 );
		EXPECT_FALSE( enumerator.isEnd() );
	}
} // namespace nfx::serialization::json::test
