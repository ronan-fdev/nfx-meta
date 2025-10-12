/**
 * @file TESTS_JSONDocument.cpp
 * @brief Comprehensive tests for JSON Document serialization and manipulation
 * @details Tests covering JSON parsing, path-based access, array operations, validation,
 *          nested object navigation, and enterprise-grade JSON document handling
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// JSON Document tests
	//=====================================================================

	//----------------------------------------------
	// JSON Document construction
	//----------------------------------------------

	TEST( DocumentTest, DefaultConstruction )
	{
		Document doc;
		EXPECT_TRUE( doc.isValid() );
		EXPECT_EQ( doc.toJsonString(), "{}" );
	}

	TEST( DocumentTest, FactoryMethods )
	{
		// Test createObject
		Document obj = Document::createObject();
		EXPECT_TRUE( obj.isValid() );
		EXPECT_EQ( obj.toJsonString(), "{}" );

		// Test createArray
		Document arr = Document::createArray();
		EXPECT_TRUE( arr.isValid() );
		EXPECT_EQ( arr.toJsonString(), "[]" );
	}

	TEST( DocumentTest, CopyAndMove )
	{
		Document original = Document::createObject();
		original.setString( "test", "value" );

		// Copy constructor
		Document copied( original );
		EXPECT_EQ( copied.getString( "test" ), "value" );

		// Move constructor
		Document moved( std::move( copied ) );
		EXPECT_EQ( moved.getString( "test" ), "value" );

		// Copy assignment
		Document assigned;
		assigned = original;
		EXPECT_EQ( assigned.getString( "test" ), "value" );
	}

	//----------------------------------------------
	// JSON parsing
	//----------------------------------------------

	TEST( DocumentTest, ParseValidJson )
	{
		std::string jsonStr = R"({"name": "John", "age": 30, "active": true})";
		auto maybeDoc = Document::fromJsonString( jsonStr );

		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		EXPECT_EQ( doc.getString( "name" ), "John" );
		EXPECT_EQ( doc.getInt( "age" ), 30 );
		EXPECT_EQ( doc.getBool( "active" ), true );
	}

	TEST( DocumentTest, ParseInvalidJson )
	{
		std::string invalidJson = R"({"name": "John", "age":})";
		auto maybeDoc = Document::fromJsonString( invalidJson );

		EXPECT_FALSE( maybeDoc.has_value() );
	}

	TEST( DocumentTest, ParseEmptyAndWhitespaceStrings )
	{
		// Test empty string - not valid JSON
		auto emptyResult = Document::fromJsonString( "" );
		EXPECT_FALSE( emptyResult.has_value() );

		// Test whitespace-only strings - not valid JSON
		auto spacesResult = Document::fromJsonString( "   " );
		EXPECT_FALSE( spacesResult.has_value() );

		auto tabsResult = Document::fromJsonString( "\t\t" );
		EXPECT_FALSE( tabsResult.has_value() );

		auto newlinesResult = Document::fromJsonString( "\n\n" );
		EXPECT_FALSE( newlinesResult.has_value() );

		auto mixedWhitespaceResult = Document::fromJsonString( " \t\n\r " );
		EXPECT_FALSE( mixedWhitespaceResult.has_value() );

		// Test valid JSON with surrounding whitespace (should work)
		auto validWithWhitespaceResult = Document::fromJsonString( "  {\"test\": \"value\"}  " );
		EXPECT_TRUE( validWithWhitespaceResult.has_value() );
		if ( validWithWhitespaceResult.has_value() )
		{
			EXPECT_EQ( validWithWhitespaceResult->getString( "test" ), "value" );
		}
	}

	TEST( DocumentTest, ParseMinimalValidJson )
	{
		// Test minimal valid JSON values
		auto nullResult = Document::fromJsonString( "null" );
		EXPECT_TRUE( nullResult.has_value() );

		auto trueResult = Document::fromJsonString( "true" );
		EXPECT_TRUE( trueResult.has_value() );

		auto falseResult = Document::fromJsonString( "false" );
		EXPECT_TRUE( falseResult.has_value() );

		auto numberResult = Document::fromJsonString( "42" );
		EXPECT_TRUE( numberResult.has_value() );

		auto stringResult = Document::fromJsonString( "\"hello\"" );
		EXPECT_TRUE( stringResult.has_value() );

		auto emptyArrayResult = Document::fromJsonString( "[]" );
		EXPECT_TRUE( emptyArrayResult.has_value() );

		auto emptyObjectResult = Document::fromJsonString( "{}" );
		EXPECT_TRUE( emptyObjectResult.has_value() );
	}

	TEST( DocumentTest, ParseJsonStringEdgeCases )
	{
		// Test various invalid JSON edge cases
		auto singleCharResult = Document::fromJsonString( "{" );
		EXPECT_FALSE( singleCharResult.has_value() );

		auto unclosedStringResult = Document::fromJsonString( "\"unclosed" );
		EXPECT_FALSE( unclosedStringResult.has_value() );

		auto invalidEscapeResult = Document::fromJsonString( "\"invalid\\xescape\"" );
		EXPECT_FALSE( invalidEscapeResult.has_value() );

		auto trailingCommaResult = Document::fromJsonString( "{\"key\": \"value\",}" );
		EXPECT_FALSE( trailingCommaResult.has_value() );

		// Test that single quote strings are invalid (JSON requires double quotes)
		auto singleQuoteResult = Document::fromJsonString( "{'key': 'value'}" );
		EXPECT_FALSE( singleQuoteResult.has_value() );
	}

	TEST( DocumentTest, ParseNestedJson )
	{
		std::string jsonStr = R"({
			"user": {
				"profile": {
					"name": "Alice",
					"settings": {
						"theme": "dark"
					}
				}
			}
		})";

		auto maybeDoc = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		EXPECT_EQ( doc.getString( "user.profile.name" ), "Alice" );
		EXPECT_EQ( doc.getString( "user.profile.settings.theme" ), "dark" );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	TEST( DocumentTest, BasicValueAccess )
	{
		Document doc = Document::createObject();
		doc.setString( "name", "Bob" );
		doc.setInt( "age", 25 );
		doc.setDouble( "height", 1.75 );
		doc.setBool( "married", false );
		doc.setNull( "spouse" );

		EXPECT_EQ( doc.getString( "name" ), "Bob" );
		EXPECT_EQ( doc.getInt( "age" ), 25 );
		EXPECT_EQ( doc.getDouble( "height" ), 1.75 );
		EXPECT_EQ( doc.getBool( "married" ), false );
		EXPECT_TRUE( doc.hasField( "spouse" ) );
	}

	TEST( DocumentTest, PathBasedAccess )
	{
		Document doc = Document::createObject();
		doc.setString( "user.profile.firstName", "Charlie" );
		doc.setString( "user.profile.lastName", "Brown" );
		doc.setInt( "user.settings.notifications", 1 );

		EXPECT_EQ( doc.getString( "user.profile.firstName" ), "Charlie" );
		EXPECT_EQ( doc.getString( "user.profile.lastName" ), "Brown" );
		EXPECT_EQ( doc.getInt( "user.settings.notifications" ), 1 );
	}

	TEST( DocumentTest, NonExistentFields )
	{
		Document doc = Document::createObject();

		// Test that non-existent fields return empty optionals
		EXPECT_FALSE( doc.hasField( "nonexistent" ) );
		EXPECT_FALSE( doc.getString( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.getInt( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.getDouble( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.getBool( "nonexistent" ).has_value() );
	}

	//----------------------------------------------
	// Array operations
	//----------------------------------------------

	TEST( DocumentTest, BasicArrayOperations )
	{
		Document doc = Document::createObject();
		doc.addToArray( "numbers", static_cast<int64_t>( 1 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 2 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 3 ) );

		EXPECT_TRUE( doc.isArray( "numbers" ) );
		EXPECT_EQ( doc.getArraySize( "numbers" ), 3 );
	}

	TEST( DocumentTest, ArrayWithDifferentTypes )
	{
		Document doc = Document::createObject();
		doc.addToArray( "mixed", "hello" );
		doc.addToArray( "mixed", static_cast<int64_t>( 42 ) );
		doc.addToArray( "mixed", 3.14 );

		EXPECT_EQ( doc.getArraySize( "mixed" ), 3 );
	}

	TEST( DocumentTest, ArrayElementAccess )
	{
		std::string jsonStr = R"({"items": [{"name": "item1"}, {"name": "item2"}]})";
		auto maybeDoc = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		Document firstItem = doc.getArrayElement( "items", 0 );
		Document secondItem = doc.getArrayElement( "items", 1 );

		EXPECT_EQ( firstItem.getString( "name" ), "item1" );
		EXPECT_EQ( secondItem.getString( "name" ), "item2" );
	}

	TEST( DocumentTest, ClearArray )
	{
		Document doc = Document::createObject();
		doc.addToArray( "numbers", static_cast<int64_t>( 1 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 2 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 3 ) );

		EXPECT_EQ( doc.getArraySize( "numbers" ), 3 );

		// Clear the array
		doc.clearArray( "numbers" );
		EXPECT_EQ( doc.getArraySize( "numbers" ), 0 );
		EXPECT_TRUE( doc.isArray( "numbers" ) ); // Should still be an array, just empty

		// Test clearing non-existent array (should not crash)
		doc.clearArray( "nonexistent" );

		// Test clearing non-array field (should not crash)
		doc.setString( "notAnArray", "value" );
		doc.clearArray( "notAnArray" );
		EXPECT_EQ( doc.getString( "notAnArray" ), "value" ); // Should remain unchanged
	}

	TEST( DocumentTest, ArrayElementPrimitiveAccess )
	{
		Document doc = Document::createObject();

		// Create arrays with different primitive types
		doc.addToArray( "strings", "hello" );
		doc.addToArray( "strings", "world" );
		doc.addToArray( "strings", "test" );

		doc.addToArray( "numbers", static_cast<int64_t>( 10 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 20 ) );
		doc.addToArray( "numbers", static_cast<int64_t>( 30 ) );

		doc.addToArray( "doubles", 1.5 );
		doc.addToArray( "doubles", 2.5 );
		doc.addToArray( "doubles", 3.5 );

		doc.addToArray( "bools", true );
		doc.addToArray( "bools", false );
		doc.addToArray( "bools", true );

		// Test string array element access
		EXPECT_EQ( doc.getArrayElementString( "strings", 0 ), "hello" );
		EXPECT_EQ( doc.getArrayElementString( "strings", 1 ), "world" );
		EXPECT_EQ( doc.getArrayElementString( "strings", 2 ), "test" );
		EXPECT_FALSE( doc.getArrayElementString( "strings", 10 ).has_value() ); // Out of bounds

		// Test integer array element access
		EXPECT_EQ( doc.getArrayElementInt( "numbers", 0 ), 10 );
		EXPECT_EQ( doc.getArrayElementInt( "numbers", 1 ), 20 );
		EXPECT_EQ( doc.getArrayElementInt( "numbers", 2 ), 30 );
		EXPECT_FALSE( doc.getArrayElementInt( "numbers", 10 ).has_value() ); // Out of bounds

		// Test double array element access
		EXPECT_EQ( doc.getArrayElementDouble( "doubles", 0 ), 1.5 );
		EXPECT_EQ( doc.getArrayElementDouble( "doubles", 1 ), 2.5 );
		EXPECT_EQ( doc.getArrayElementDouble( "doubles", 2 ), 3.5 );
		EXPECT_FALSE( doc.getArrayElementDouble( "doubles", 10 ).has_value() ); // Out of bounds

		// Test boolean array element access
		EXPECT_EQ( doc.getArrayElementBool( "bools", 0 ), true );
		EXPECT_EQ( doc.getArrayElementBool( "bools", 1 ), false );
		EXPECT_EQ( doc.getArrayElementBool( "bools", 2 ), true );
		EXPECT_FALSE( doc.getArrayElementBool( "bools", 10 ).has_value() ); // Out of bounds

		// Test type safety - accessing wrong types should return nullopt
		EXPECT_FALSE( doc.getArrayElementString( "numbers", 0 ).has_value() ); // int accessed as string
		EXPECT_FALSE( doc.getArrayElementInt( "strings", 0 ).has_value() );	   // string accessed as int
		EXPECT_FALSE( doc.getArrayElementDouble( "bools", 0 ).has_value() );   // bool accessed as double
		EXPECT_FALSE( doc.getArrayElementBool( "strings", 0 ).has_value() );   // string accessed as bool

		// Test non-existent arrays
		EXPECT_FALSE( doc.getArrayElementString( "nonexistent", 0 ).has_value() );
		EXPECT_FALSE( doc.getArrayElementInt( "nonexistent", 0 ).has_value() );
		EXPECT_FALSE( doc.getArrayElementDouble( "nonexistent", 0 ).has_value() );
		EXPECT_FALSE( doc.getArrayElementBool( "nonexistent", 0 ).has_value() );

		// Test accessing non-array fields
		doc.setString( "notArray", "value" );
		EXPECT_FALSE( doc.getArrayElementString( "notArray", 0 ).has_value() );
		EXPECT_FALSE( doc.getArrayElementInt( "notArray", 0 ).has_value() );
	}

	TEST( DocumentTest, ArrayElementAccessWithComplexJson )
	{
		std::string jsonStr = R"({
			"users": [
				"alice", "bob", "charlie"
			],
			"scores": [95, 87, 92, 78],
			"prices": [19.99, 25.50, 12.75],
			"flags": [true, false, true, false]
		})";

		auto maybeDoc = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		// Test string array from parsed JSON
		EXPECT_EQ( doc.getArrayElementString( "users", 0 ), "alice" );
		EXPECT_EQ( doc.getArrayElementString( "users", 1 ), "bob" );
		EXPECT_EQ( doc.getArrayElementString( "users", 2 ), "charlie" );

		// Test integer array from parsed JSON
		EXPECT_EQ( doc.getArrayElementInt( "scores", 0 ), 95 );
		EXPECT_EQ( doc.getArrayElementInt( "scores", 1 ), 87 );
		EXPECT_EQ( doc.getArrayElementInt( "scores", 2 ), 92 );
		EXPECT_EQ( doc.getArrayElementInt( "scores", 3 ), 78 );

		// Test double array from parsed JSON
		EXPECT_EQ( doc.getArrayElementDouble( "prices", 0 ), 19.99 );
		EXPECT_EQ( doc.getArrayElementDouble( "prices", 1 ), 25.50 );
		EXPECT_EQ( doc.getArrayElementDouble( "prices", 2 ), 12.75 );

		// Test boolean array from parsed JSON
		EXPECT_EQ( doc.getArrayElementBool( "flags", 0 ), true );
		EXPECT_EQ( doc.getArrayElementBool( "flags", 1 ), false );
		EXPECT_EQ( doc.getArrayElementBool( "flags", 2 ), true );
		EXPECT_EQ( doc.getArrayElementBool( "flags", 3 ), false );
	}

	//----------------------------------------------
	// Advanced Document operations
	//----------------------------------------------

	TEST( DocumentTest, DocumentArrayOperations )
	{
		Document arrayDoc = Document::createArray();

		Document item1 = Document::createObject();
		item1.setString( "name", "Alice" );
		item1.setInt( "age", 30 );

		Document item2 = Document::createObject();
		item2.setString( "name", "Bob" );
		item2.setInt( "age", 25 );

		arrayDoc.addDocument( item1 );
		arrayDoc.addDocument( item2 );

		EXPECT_EQ( arrayDoc.size(), 2 );
	}

	TEST( DocumentTest, SetArrayDocument )
	{
		Document mainDoc = Document::createObject();
		Document arrayDoc = Document::createArray();

		arrayDoc.addToArray( "", "value1" );
		arrayDoc.addToArray( "", "value2" );

		mainDoc.setArray( "myArray", arrayDoc );

		EXPECT_TRUE( mainDoc.isArray( "myArray" ) );
		EXPECT_EQ( mainDoc.getArraySize( "myArray" ), 2 );
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	TEST( DocumentTest, TypeCheckingMethods )
	{
		Document doc = Document::createObject();

		// Set up different types
		doc.setString( "stringField", "hello" );
		doc.setInt( "intField", 42 );
		doc.setDouble( "doubleField", 3.14 );
		doc.setBool( "boolField", true );
		doc.setNull( "nullField" );

		// Create nested object
		doc.setString( "nested.object.field", "nested_value" );

		// Create array
		doc.addToArray( "arrayField", "item1" );

		// Test string type checking
		EXPECT_TRUE( doc.isString( "stringField" ) );
		EXPECT_FALSE( doc.isString( "intField" ) );
		EXPECT_FALSE( doc.isString( "nonexistent" ) );

		// Test integer type checking
		EXPECT_TRUE( doc.isInt( "intField" ) );
		EXPECT_FALSE( doc.isInt( "stringField" ) );
		EXPECT_FALSE( doc.isInt( "doubleField" ) );
		EXPECT_FALSE( doc.isInt( "nonexistent" ) );

		// Test double type checking
		EXPECT_TRUE( doc.isDouble( "doubleField" ) );
		EXPECT_FALSE( doc.isDouble( "intField" ) ); // Note: integers are not floats
		EXPECT_FALSE( doc.isDouble( "stringField" ) );
		EXPECT_FALSE( doc.isDouble( "nonexistent" ) );

		// Test boolean type checking
		EXPECT_TRUE( doc.isBool( "boolField" ) );
		EXPECT_FALSE( doc.isBool( "stringField" ) );
		EXPECT_FALSE( doc.isBool( "intField" ) );
		EXPECT_FALSE( doc.isBool( "nonexistent" ) );

		// Test null type checking
		EXPECT_TRUE( doc.isNull( "nullField" ) );
		EXPECT_FALSE( doc.isNull( "stringField" ) );
		EXPECT_FALSE( doc.isNull( "nonexistent" ) );

		// Test object type checking (nested objects)
		EXPECT_TRUE( doc.isObject( "nested" ) );
		EXPECT_TRUE( doc.isObject( "nested.object" ) );
		EXPECT_FALSE( doc.isObject( "nested.object.field" ) ); // This is a string, not object
		EXPECT_FALSE( doc.isObject( "stringField" ) );
		EXPECT_FALSE( doc.isObject( "nonexistent" ) );

		// Test array type checking
		EXPECT_TRUE( doc.isArray( "arrayField" ) );
		EXPECT_FALSE( doc.isArray( "stringField" ) );
		EXPECT_FALSE( doc.isArray( "nonexistent" ) );
	}

	TEST( DocumentTest, TypeCheckingWithComplexJson )
	{
		std::string complexJson = R"({
			"user": {
				"name": "Alice",
				"age": 30,
				"height": 1.65,
				"active": true,
				"spouse": null,
				"preferences": {
					"theme": "dark"
				},
				"hobbies": ["reading", "gaming"]
			}
		})";

		auto maybeDoc = Document::fromJsonString( complexJson );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		// Root level
		EXPECT_TRUE( doc.isObject( "user" ) );

		// String fields
		EXPECT_TRUE( doc.isString( "user.name" ) );
		EXPECT_TRUE( doc.isString( "user.preferences.theme" ) );

		// Numeric fields
		EXPECT_TRUE( doc.isInt( "user.age" ) );
		EXPECT_TRUE( doc.isDouble( "user.height" ) );

		// Boolean field
		EXPECT_TRUE( doc.isBool( "user.active" ) );

		// Null field
		EXPECT_TRUE( doc.isNull( "user.spouse" ) );

		// Nested object
		EXPECT_TRUE( doc.isObject( "user.preferences" ) );

		// Array field
		EXPECT_TRUE( doc.isArray( "user.hobbies" ) );

		// Cross-validation (ensuring fields are not other types)
		EXPECT_FALSE( doc.isInt( "user.name" ) );
		EXPECT_FALSE( doc.isString( "user.age" ) );
		EXPECT_FALSE( doc.isArray( "user.active" ) );
		EXPECT_FALSE( doc.isObject( "user.hobbies" ) ); // Array, not object
	}

	//----------------------------------------------
	// Field operations
	//----------------------------------------------

	TEST( DocumentTest, RemoveField )
	{
		Document doc = Document::createObject();
		doc.setString( "name", "Alice" );
		doc.setInt( "age", 30 );
		doc.setString( "user.profile.firstName", "Bob" );
		doc.setString( "user.profile.lastName", "Smith" );
		doc.setInt( "user.settings.theme", 1 );

		// Test removing root-level field
		EXPECT_TRUE( doc.hasField( "name" ) );
		EXPECT_TRUE( doc.removeField( "name" ) );
		EXPECT_FALSE( doc.hasField( "name" ) );

		// Test removing nested field
		EXPECT_TRUE( doc.hasField( "user.profile.firstName" ) );
		EXPECT_TRUE( doc.removeField( "user.profile.firstName" ) );
		EXPECT_FALSE( doc.hasField( "user.profile.firstName" ) );
		EXPECT_TRUE( doc.hasField( "user.profile.lastName" ) ); // Other fields should remain

		// Test removing non-existent field
		EXPECT_FALSE( doc.removeField( "nonexistent" ) );
		EXPECT_FALSE( doc.removeField( "user.nonexistent" ) );

		// Test removing empty path (should fail)
		EXPECT_FALSE( doc.removeField( "" ) );

		// Verify remaining structure is intact
		EXPECT_EQ( doc.getInt( "age" ), 30 );
		EXPECT_EQ( doc.getString( "user.profile.lastName" ), "Smith" );
		EXPECT_EQ( doc.getInt( "user.settings.theme" ), 1 );
	}

	//----------------------------------------------
	// Merge / update operations
	//----------------------------------------------

	TEST( DocumentTest, MergeDocuments )
	{
		// Create base document
		Document base = Document::createObject();
		base.setString( "name", "Alice" );
		base.setInt( "age", 30 );
		base.setString( "user.profile.city", "New York" );
		base.addToArray( "hobbies", "reading" );
		base.addToArray( "hobbies", "gaming" );

		// Create merge document
		Document merge = Document::createObject();
		merge.setString( "name", "Bob" );				  // Should overwrite
		merge.setString( "email", "bob@example.com" );	  // Should add new field
		merge.setString( "user.profile.country", "USA" ); // Should add to nested object
		merge.setString( "user.profile.city", "Boston" ); // Should overwrite nested field
		merge.addToArray( "hobbies", "hiking" );		  // Should merge with existing array
		merge.addToArray( "skills", "C++" );			  // Should create new array

		// Test merge with array merging (default behavior)
		Document result1 = base;
		result1.merge( merge, false ); // Don't overwrite arrays

		EXPECT_EQ( result1.getString( "name" ), "Bob" );				 // Overwritten
		EXPECT_EQ( result1.getString( "email" ), "bob@example.com" );	 // Added
		EXPECT_EQ( result1.getInt( "age" ), 30 );						 // Preserved
		EXPECT_EQ( result1.getString( "user.profile.city" ), "Boston" ); // Overwritten
		EXPECT_EQ( result1.getString( "user.profile.country" ), "USA" ); // Added
		EXPECT_EQ( result1.getArraySize( "hobbies" ), 3 );				 // Merged array
		EXPECT_EQ( result1.getArraySize( "skills" ), 1 );				 // New array

		// Test merge with array overwriting
		Document result2 = base;
		result2.merge( merge, true ); // Overwrite arrays

		EXPECT_EQ( result2.getString( "name" ), "Bob" );
		EXPECT_EQ( result2.getArraySize( "hobbies" ), 1 ); // Overwritten array
	}

	TEST( DocumentTest, UpdateField )
	{
		Document doc = Document::createObject();
		doc.setString( "name", "Alice" );
		doc.setInt( "age", 30 );

		// Create update value
		Document updateValue = Document::createObject();
		updateValue.setString( "firstName", "Bob" );
		updateValue.setString( "lastName", "Smith" );
		updateValue.setInt( "id", 123 );

		// Update existing field
		doc.update( "name", updateValue );
		EXPECT_FALSE( doc.isString( "name" ) ); // Should no longer be a string
		EXPECT_TRUE( doc.isObject( "name" ) );	// Should now be an object
		EXPECT_EQ( doc.getString( "name.firstName" ), "Bob" );
		EXPECT_EQ( doc.getString( "name.lastName" ), "Smith" );
		EXPECT_EQ( doc.getInt( "name.id" ), 123 );

		// Update nested path (should create intermediate objects)
		Document nestedUpdate = Document::createObject();
		nestedUpdate.setString( "theme", "dark" );
		nestedUpdate.setBool( "notifications", true );

		doc.update( "user.settings", nestedUpdate );
		EXPECT_EQ( doc.getString( "user.settings.theme" ), "dark" );
		EXPECT_EQ( doc.getBool( "user.settings.notifications" ), true );

		// Verify other fields remain unchanged
		EXPECT_EQ( doc.getInt( "age" ), 30 );
	}

	//----------------------------------------------
	// Validation and error handling
	//----------------------------------------------

	TEST( DocumentTest, PublicValidationMethods )
	{
		Document doc = Document::createObject();
		doc.setString( "validString", "test" );
		doc.addToArray( "validArray", "item" );

		// Test public validation methods
		EXPECT_TRUE( doc.isValid() );
		EXPECT_TRUE( doc.hasField( "validString" ) );
		EXPECT_FALSE( doc.hasField( "missingField" ) );

		// Test array validation
		EXPECT_TRUE( doc.isArray( "validArray" ) );
		EXPECT_FALSE( doc.isArray( "validString" ) );
		EXPECT_FALSE( doc.isArray( "nonexistent" ) );
	}

	//----------------------------------------------
	// JSON output
	//----------------------------------------------

	TEST( DocumentTest, JsonOutputFormatting )
	{
		Document doc = Document::createObject();
		doc.setString( "name", "Test" );
		doc.setInt( "value", 123 );

		// Compact output
		std::string compact = doc.toJsonString( 0 );
		EXPECT_TRUE( compact.find( "\"name\"" ) != std::string::npos );
		EXPECT_TRUE( compact.find( "\"Test\"" ) != std::string::npos );
		EXPECT_TRUE( compact.find( "\"value\"" ) != std::string::npos );
		EXPECT_TRUE( compact.find( "123" ) != std::string::npos );

		// Pretty-printed output
		std::string pretty = doc.toJsonString( 2 );
		EXPECT_GT( pretty.length(), compact.length() );
	}

	TEST( DocumentTest, JsonBytesOutput )
	{
		Document doc = Document::createObject();
		doc.setString( "test", "value" );

		std::vector<uint8_t> bytes = doc.toJsonBytes();
		std::string jsonStr = doc.toJsonString( 0 );

		EXPECT_EQ( bytes.size(), jsonStr.length() );
	}

	//----------------------------------------------
	// JSON Pointer (RFC 6901)
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerBasicAccess )
	{
		// Create a document structure
		Document doc = Document::createObject();
		doc.setStringByPointer( "/name", "Alice Johnson" );
		doc.setIntByPointer( "/age", 30 );
		doc.setDoubleByPointer( "/height", 1.75 );
		doc.setBoolByPointer( "/active", true );
		doc.setNullByPointer( "/spouse" );

		// Test basic JSON Pointer access
		EXPECT_EQ( doc.getStringByPointer( "/name" ), "Alice Johnson" );
		EXPECT_EQ( doc.getIntByPointer( "/age" ), 30 );
		EXPECT_EQ( doc.getDoubleByPointer( "/height" ), 1.75 );
		EXPECT_EQ( doc.getBoolByPointer( "/active" ), true );

		// Test field existence
		EXPECT_TRUE( doc.hasFieldByPointer( "/name" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/age" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/spouse" ) );
		EXPECT_FALSE( doc.hasFieldByPointer( "/nonexistent" ) );

		// Test root document access
		EXPECT_TRUE( doc.hasFieldByPointer( "" ) );
	}

	TEST( DocumentTest, JsonPointerNestedObjects )
	{
		Document doc = Document::createObject();

		// Create nested structure using JSON Pointers
		doc.setStringByPointer( "/user/profile/firstName", "John" );
		doc.setStringByPointer( "/user/profile/lastName", "Doe" );
		doc.setIntByPointer( "/user/profile/age", 25 );
		doc.setStringByPointer( "/user/settings/theme", "dark" );
		doc.setBoolByPointer( "/user/settings/notifications", false );

		// Verify nested access
		EXPECT_EQ( doc.getStringByPointer( "/user/profile/firstName" ), "John" );
		EXPECT_EQ( doc.getStringByPointer( "/user/profile/lastName" ), "Doe" );
		EXPECT_EQ( doc.getIntByPointer( "/user/profile/age" ), 25 );
		EXPECT_EQ( doc.getStringByPointer( "/user/settings/theme" ), "dark" );
		EXPECT_EQ( doc.getBoolByPointer( "/user/settings/notifications" ), false );

		// Test intermediate path existence
		EXPECT_TRUE( doc.hasFieldByPointer( "/user" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/profile" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/settings" ) );
	}

	TEST( DocumentTest, JsonPointerArrayAccess )
	{
		Document doc = Document::createObject();

		// Create arrays using JSON Pointers
		doc.setStringByPointer( "/users/0/name", "Alice" );
		doc.setIntByPointer( "/users/0/age", 28 );
		doc.setStringByPointer( "/users/1/name", "Bob" );
		doc.setIntByPointer( "/users/1/age", 32 );

		doc.setDoubleByPointer( "/scores/0", 95.5 );
		doc.setDoubleByPointer( "/scores/1", 87.2 );
		doc.setDoubleByPointer( "/scores/2", 91.8 );

		// Verify array element access
		EXPECT_EQ( doc.getStringByPointer( "/users/0/name" ), "Alice" );
		EXPECT_EQ( doc.getIntByPointer( "/users/0/age" ), 28 );
		EXPECT_EQ( doc.getStringByPointer( "/users/1/name" ), "Bob" );
		EXPECT_EQ( doc.getIntByPointer( "/users/1/age" ), 32 );

		EXPECT_EQ( doc.getDoubleByPointer( "/scores/0" ), 95.5 );
		EXPECT_EQ( doc.getDoubleByPointer( "/scores/1" ), 87.2 );
		EXPECT_EQ( doc.getDoubleByPointer( "/scores/2" ), 91.8 );

		// Test field existence (object fields only)
		EXPECT_TRUE( doc.hasFieldByPointer( "/users" ) );		 // Field in root object
		EXPECT_FALSE( doc.hasFieldByPointer( "/users/0" ) );	 // Array element, not field
		EXPECT_FALSE( doc.hasFieldByPointer( "/users/1" ) );	 // Array element, not field
		EXPECT_TRUE( doc.hasFieldByPointer( "/scores" ) );		 // Field in root object
		EXPECT_TRUE( doc.hasFieldByPointer( "/users/0/name" ) ); // Field in object within array
		EXPECT_TRUE( doc.hasFieldByPointer( "/users/0/age" ) );	 // Field in object within array

		// Test value existence (any JSON value)
		EXPECT_TRUE( doc.hasValueByPointer( "/users" ) );		 // Array value
		EXPECT_TRUE( doc.hasValueByPointer( "/users/0" ) );		 // Array element (object)
		EXPECT_TRUE( doc.hasValueByPointer( "/users/1" ) );		 // Array element (object)
		EXPECT_TRUE( doc.hasValueByPointer( "/scores" ) );		 // Array value
		EXPECT_TRUE( doc.hasValueByPointer( "/scores/0" ) );	 // Array element (primitive)
		EXPECT_TRUE( doc.hasValueByPointer( "/users/0/name" ) ); // Object field value
		EXPECT_FALSE( doc.hasValueByPointer( "/users/5" ) );	 // Non-existent array element
		EXPECT_FALSE( doc.hasValueByPointer( "/nonexistent" ) ); // Non-existent field
	}

	TEST( DocumentTest, JsonPointerEscapedCharacters )
	{
		Document doc = Document::createObject();

		// Test escaped characters according to RFC 6901
		// ~0 represents ~ and ~1 represents /
		doc.setStringByPointer( "/field~1with~0tilde", "value1" );
		doc.setStringByPointer( "/normal~1field", "value2" );
		doc.setIntByPointer( "/path~1to~1data", 42 );

		// Verify escaped character handling
		EXPECT_EQ( doc.getStringByPointer( "/field~1with~0tilde" ), "value1" );
		EXPECT_EQ( doc.getStringByPointer( "/normal~1field" ), "value2" );
		EXPECT_EQ( doc.getIntByPointer( "/path~1to~1data" ), 42 );

		// Verify field existence with escaped names
		EXPECT_TRUE( doc.hasFieldByPointer( "/field~1with~0tilde" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/normal~1field" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/path~1to~1data" ) );
	}

	TEST( DocumentTest, JsonPointerErrorHandling )
	{
		Document doc = Document::createObject();
		doc.setStringByPointer( "/existing/field", "value" );

		// Test invalid pointers
		EXPECT_FALSE( doc.hasFieldByPointer( "invalid" ) ); // Must start with /
		EXPECT_FALSE( doc.hasFieldByPointer( "/nonexistent/field" ) );

		// Test type mismatches
		EXPECT_FALSE( doc.getIntByPointer( "/existing/field" ).has_value() );  // string accessed as int
		EXPECT_FALSE( doc.getBoolByPointer( "/existing/field" ).has_value() ); // string accessed as bool

		// Test array index errors
		doc.setIntByPointer( "/numbers/0", 10 );
		doc.setIntByPointer( "/numbers/1", 20 );

		EXPECT_FALSE( doc.hasFieldByPointer( "/numbers/5" ) ); // Out of bounds
		EXPECT_FALSE( doc.getIntByPointer( "/numbers/5" ).has_value() );

		// Test invalid array indices
		EXPECT_FALSE( doc.hasFieldByPointer( "/numbers/01" ) );	 // Leading zero not allowed
		EXPECT_FALSE( doc.hasFieldByPointer( "/numbers/abc" ) ); // Non-numeric
	}

	TEST( DocumentTest, HasFieldVsHasValuePointerDifference )
	{
		// Create a document with mixed object and array structure
		auto doc = Document::fromJsonString( R"({
			"users": [
				{"name": "Alice", "age": 30},
				{"name": "Bob", "age": 25}
			],
			"count": 2,
			"settings": {
				"theme": "dark",
				"notifications": true
			}
		})" );
		ASSERT_TRUE( doc.has_value() );

		// Object fields should work with both methods
		EXPECT_TRUE( doc->hasFieldByPointer( "/users" ) );	  // Field in root object
		EXPECT_TRUE( doc->hasValueByPointer( "/users" ) );	  // Same as above
		EXPECT_TRUE( doc->hasFieldByPointer( "/count" ) );	  // Field in root object
		EXPECT_TRUE( doc->hasValueByPointer( "/count" ) );	  // Same as above
		EXPECT_TRUE( doc->hasFieldByPointer( "/settings" ) ); // Field in root object
		EXPECT_TRUE( doc->hasValueByPointer( "/settings" ) ); // Same as above

		// Nested object fields should work with both methods
		EXPECT_TRUE( doc->hasFieldByPointer( "/settings/theme" ) );			// Field in nested object
		EXPECT_TRUE( doc->hasValueByPointer( "/settings/theme" ) );			// Same as above
		EXPECT_TRUE( doc->hasFieldByPointer( "/settings/notifications" ) ); // Field in nested object
		EXPECT_TRUE( doc->hasValueByPointer( "/settings/notifications" ) ); // Same as above
		EXPECT_TRUE( doc->hasFieldByPointer( "/users/0/name" ) );			// Field in array element object
		EXPECT_TRUE( doc->hasValueByPointer( "/users/0/name" ) );			// Same as above

		// Array elements should ONLY work with hasValueByPointer
		EXPECT_FALSE( doc->hasFieldByPointer( "/users/0" ) ); // Array element, not a field
		EXPECT_TRUE( doc->hasValueByPointer( "/users/0" ) );  // Array element exists
		EXPECT_FALSE( doc->hasFieldByPointer( "/users/1" ) ); // Array element, not a field
		EXPECT_TRUE( doc->hasValueByPointer( "/users/1" ) );  // Array element exists

		// Non-existent paths should return false for both
		EXPECT_FALSE( doc->hasFieldByPointer( "/nonexistent" ) );
		EXPECT_FALSE( doc->hasValueByPointer( "/nonexistent" ) );
		EXPECT_FALSE( doc->hasFieldByPointer( "/users/5" ) );		  // Out of bounds
		EXPECT_FALSE( doc->hasValueByPointer( "/users/5" ) );		  // Out of bounds
		EXPECT_FALSE( doc->hasFieldByPointer( "/users/0/invalid" ) ); // Non-existent field
		EXPECT_FALSE( doc->hasValueByPointer( "/users/0/invalid" ) ); // Non-existent field

		// Root document tests
		EXPECT_TRUE( doc->hasFieldByPointer( "" ) ); // Root is an object (has fields)
		EXPECT_TRUE( doc->hasValueByPointer( "" ) ); // Root exists

		// Test with array at root
		auto arrayDoc = Document::fromJsonString( R"([1, 2, {"key": "value"}])" );
		ASSERT_TRUE( arrayDoc.has_value() );

		EXPECT_FALSE( arrayDoc->hasFieldByPointer( "" ) );		// Root is array, not object (no fields)
		EXPECT_TRUE( arrayDoc->hasValueByPointer( "" ) );		// Root exists
		EXPECT_FALSE( arrayDoc->hasFieldByPointer( "/0" ) );	// Array element, not field
		EXPECT_TRUE( arrayDoc->hasValueByPointer( "/0" ) );		// Array element exists
		EXPECT_TRUE( arrayDoc->hasFieldByPointer( "/2/key" ) ); // Field within object in array
		EXPECT_TRUE( arrayDoc->hasValueByPointer( "/2/key" ) ); // Same as above
	}

	TEST( DocumentTest, TypeSpecificHasPointerMethods )
	{
		// Create a comprehensive test document with all JSON types
		auto doc = Document::fromJsonString( R"({
			"stringField": "hello world",
			"intField": 42,
			"doubleField": 3.14159,
			"boolField": true,
			"nullField": null,
			"objectField": {
				"nested": "value"
			},
			"arrayField": [1, 2, 3],
			"mixedArray": [
				"string",
				123,
				4.56,
				false,
				null,
				{"key": "value"},
				[7, 8, 9]
			]
		})" );
		ASSERT_TRUE( doc.has_value() );

		// Test hasStringByPointer
		EXPECT_TRUE( doc->hasStringByPointer( "/stringField" ) );
		EXPECT_TRUE( doc->hasStringByPointer( "/objectField/nested" ) );
		EXPECT_TRUE( doc->hasStringByPointer( "/mixedArray/0" ) );
		EXPECT_FALSE( doc->hasStringByPointer( "/intField" ) );	   // Not a string
		EXPECT_FALSE( doc->hasStringByPointer( "/arrayField" ) );  // Not a string
		EXPECT_FALSE( doc->hasStringByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test hasIntByPointer
		EXPECT_TRUE( doc->hasIntByPointer( "/intField" ) );
		EXPECT_TRUE( doc->hasIntByPointer( "/arrayField/0" ) );
		EXPECT_TRUE( doc->hasIntByPointer( "/mixedArray/1" ) );
		EXPECT_FALSE( doc->hasIntByPointer( "/doubleField" ) ); // Not an int
		EXPECT_FALSE( doc->hasIntByPointer( "/stringField" ) ); // Not an int
		EXPECT_FALSE( doc->hasIntByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test hasDoubleByPointer
		EXPECT_TRUE( doc->hasDoubleByPointer( "/doubleField" ) );
		EXPECT_TRUE( doc->hasDoubleByPointer( "/mixedArray/2" ) );
		EXPECT_FALSE( doc->hasDoubleByPointer( "/intField" ) );	   // Not a double
		EXPECT_FALSE( doc->hasDoubleByPointer( "/stringField" ) ); // Not a double
		EXPECT_FALSE( doc->hasDoubleByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test hasBoolByPointer
		EXPECT_TRUE( doc->hasBoolByPointer( "/boolField" ) );
		EXPECT_TRUE( doc->hasBoolByPointer( "/mixedArray/3" ) );
		EXPECT_FALSE( doc->hasBoolByPointer( "/stringField" ) ); // Not a bool
		EXPECT_FALSE( doc->hasBoolByPointer( "/intField" ) );	 // Not a bool
		EXPECT_FALSE( doc->hasBoolByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test hasNullByPointer
		EXPECT_TRUE( doc->hasNullByPointer( "/nullField" ) );
		EXPECT_TRUE( doc->hasNullByPointer( "/mixedArray/4" ) );
		EXPECT_FALSE( doc->hasNullByPointer( "/stringField" ) ); // Not null
		EXPECT_FALSE( doc->hasNullByPointer( "/intField" ) );	 // Not null
		EXPECT_FALSE( doc->hasNullByPointer( "/nonexistent" ) ); // Doesn't exist (different from null)

		// Test hasObjectByPointer
		EXPECT_TRUE( doc->hasObjectByPointer( "/objectField" ) );
		EXPECT_TRUE( doc->hasObjectByPointer( "/mixedArray/5" ) );
		EXPECT_TRUE( doc->hasObjectByPointer( "" ) );			   // Root is object
		EXPECT_FALSE( doc->hasObjectByPointer( "/arrayField" ) );  // Not an object
		EXPECT_FALSE( doc->hasObjectByPointer( "/stringField" ) ); // Not an object
		EXPECT_FALSE( doc->hasObjectByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test hasArrayByPointer
		EXPECT_TRUE( doc->hasArrayByPointer( "/arrayField" ) );
		EXPECT_TRUE( doc->hasArrayByPointer( "/mixedArray" ) );
		EXPECT_TRUE( doc->hasArrayByPointer( "/mixedArray/6" ) );
		EXPECT_FALSE( doc->hasArrayByPointer( "/objectField" ) ); // Not an array
		EXPECT_FALSE( doc->hasArrayByPointer( "/stringField" ) ); // Not an array
		EXPECT_FALSE( doc->hasArrayByPointer( "" ) );			  // Root is object, not array
		EXPECT_FALSE( doc->hasArrayByPointer( "/nonexistent" ) ); // Doesn't exist

		// Test consistency with corresponding get methods
		// If hasXByPointer returns true, getXByPointer should return a value
		if ( doc->hasStringByPointer( "/stringField" ) )
		{
			EXPECT_TRUE( doc->getStringByPointer( "/stringField" ).has_value() );
		}
		if ( doc->hasIntByPointer( "/intField" ) )
		{
			EXPECT_TRUE( doc->getIntByPointer( "/intField" ).has_value() );
		}
		if ( doc->hasDoubleByPointer( "/doubleField" ) )
		{
			EXPECT_TRUE( doc->getDoubleByPointer( "/doubleField" ).has_value() );
		}
		if ( doc->hasBoolByPointer( "/boolField" ) )
		{
			EXPECT_TRUE( doc->getBoolByPointer( "/boolField" ).has_value() );
		}
		if ( doc->hasArrayByPointer( "/arrayField" ) )
		{
			EXPECT_TRUE( doc->getArrayByPointer( "/arrayField" ).has_value() );
		}
		if ( doc->hasObjectByPointer( "/objectField" ) )
		{
			EXPECT_TRUE( doc->getObjectByPointer( "/objectField" ).has_value() );
		}

		// Test array document at root
		auto arrayDoc = Document::fromJsonString( R"([
			"string",
			42,
			3.14,
			true,
			null,
			{"key": "value"},
			[1, 2, 3]
		])" );
		ASSERT_TRUE( arrayDoc.has_value() );

		EXPECT_TRUE( arrayDoc->hasArrayByPointer( "" ) );	// Root is array
		EXPECT_FALSE( arrayDoc->hasObjectByPointer( "" ) ); // Root is not object
		EXPECT_TRUE( arrayDoc->hasStringByPointer( "/0" ) );
		EXPECT_TRUE( arrayDoc->hasIntByPointer( "/1" ) );
		EXPECT_TRUE( arrayDoc->hasDoubleByPointer( "/2" ) );
		EXPECT_TRUE( arrayDoc->hasBoolByPointer( "/3" ) );
		EXPECT_TRUE( arrayDoc->hasNullByPointer( "/4" ) );
		EXPECT_TRUE( arrayDoc->hasObjectByPointer( "/5" ) );
		EXPECT_TRUE( arrayDoc->hasArrayByPointer( "/6" ) );
	}

	TEST( DocumentTest, JsonPointerCompatibilityWithDotNotation )
	{
		Document doc = Document::createObject();

		// Set values using dot notation
		doc.setString( "user.name", "Alice" );
		doc.setInt( "user.age", 25 );
		doc.setString( "settings.theme", "dark" );

		// Access same values using JSON Pointer
		EXPECT_EQ( doc.getStringByPointer( "/user/name" ), "Alice" );
		EXPECT_EQ( doc.getIntByPointer( "/user/age" ), 25 );
		EXPECT_EQ( doc.getStringByPointer( "/settings/theme" ), "dark" );

		// Set values using JSON Pointer
		doc.setStringByPointer( "/profile/email", "alice@example.com" );
		doc.setBoolByPointer( "/profile/verified", true );

		// Access same values using dot notation
		EXPECT_EQ( doc.getString( "profile.email" ), "alice@example.com" );
		EXPECT_EQ( doc.getBool( "profile.verified" ), true );

		// Both notations should see the same structure
		EXPECT_TRUE( doc.hasField( "user.name" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/name" ) );
		EXPECT_TRUE( doc.hasField( "profile.email" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/profile/email" ) );
	}

	TEST( DocumentTest, JsonPointerComplexDocument )
	{
		// Test with a complex, realistic JSON structure
		Document doc = Document::createObject();

		// API response structure
		doc.setStringByPointer( "/status", "success" );
		doc.setIntByPointer( "/code", 200 );
		doc.setStringByPointer( "/data/user/id", "12345" );
		doc.setStringByPointer( "/data/user/profile/name", "Jane Smith" );
		doc.setStringByPointer( "/data/user/profile/email", "jane@example.com" );
		doc.setBoolByPointer( "/data/user/profile/verified", true );

		// Array of permissions
		doc.setStringByPointer( "/data/permissions/0/resource", "users" );
		doc.setStringByPointer( "/data/permissions/0/action", "read" );
		doc.setStringByPointer( "/data/permissions/1/resource", "posts" );
		doc.setStringByPointer( "/data/permissions/1/action", "write" );
		doc.setStringByPointer( "/data/permissions/2/resource", "admin" );
		doc.setStringByPointer( "/data/permissions/2/action", "manage" );

		// Metadata
		doc.setStringByPointer( "/metadata/timestamp", "2025-10-03T14:30:00Z" );
		doc.setDoubleByPointer( "/metadata/version", 2.1 );

		// Verify the entire structure
		EXPECT_EQ( doc.getStringByPointer( "/status" ), "success" );
		EXPECT_EQ( doc.getIntByPointer( "/code" ), 200 );
		EXPECT_EQ( doc.getStringByPointer( "/data/user/profile/name" ), "Jane Smith" );
		EXPECT_EQ( doc.getBoolByPointer( "/data/user/profile/verified" ), true );

		EXPECT_EQ( doc.getStringByPointer( "/data/permissions/0/resource" ), "users" );
		EXPECT_EQ( doc.getStringByPointer( "/data/permissions/1/action" ), "write" );
		EXPECT_EQ( doc.getStringByPointer( "/data/permissions/2/resource" ), "admin" );

		EXPECT_EQ( doc.getDoubleByPointer( "/metadata/version" ), 2.1 );

		// Test the generated JSON structure
		std::string jsonOutput = doc.toJsonString( 2 );
		EXPECT_TRUE( jsonOutput.find( "\"status\"" ) != std::string::npos );
		EXPECT_TRUE( jsonOutput.find( "\"Jane Smith\"" ) != std::string::npos );
		EXPECT_TRUE( jsonOutput.find( "\"permissions\"" ) != std::string::npos );
	}

	//----------------------------------------------
	// JSON Pointer array methods tests
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerArrayMethods_BasicSetAndGet )
	{
		Document doc = Document::createObject();

		// Create an array with mixed types
		Document arrayDoc = Document::createArray();
		arrayDoc.addToArray( "", "first" );
		arrayDoc.addToArray( "", "second" );
		arrayDoc.addToArray( "", "third" );

		// Set the array using setArrayByPointer
		doc.setArrayByPointer( "/hobbies", arrayDoc );

		// Verify the array was set correctly
		EXPECT_TRUE( doc.hasFieldByPointer( "/hobbies" ) );
		EXPECT_TRUE( doc.isArray( "hobbies" ) );
		EXPECT_EQ( doc.getArraySize( "hobbies" ), 3 );

		// Get the array using getArrayByPointer
		auto retrievedArray = doc.getArrayByPointer( "/hobbies" );
		ASSERT_TRUE( retrievedArray.has_value() );
		EXPECT_EQ( retrievedArray->size(), 3 );

		// Verify individual elements through regular array access
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 0 ), "first" );
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 1 ), "second" );
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 2 ), "third" );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_NestedArrays )
	{
		Document doc = Document::createObject();

		// Create a nested structure with arrays
		Document numbersArray = Document::createArray();
		numbersArray.addToArray( "", int64_t{ 10 } );
		numbersArray.addToArray( "", int64_t{ 20 } );
		numbersArray.addToArray( "", int64_t{ 30 } );

		Document stringsArray = Document::createArray();
		stringsArray.addToArray( "", "alpha" );
		stringsArray.addToArray( "", "beta" );

		// Set arrays at nested paths
		doc.setArrayByPointer( "/data/numbers", numbersArray );
		doc.setArrayByPointer( "/data/strings", stringsArray );

		// Verify both arrays exist
		EXPECT_TRUE( doc.hasFieldByPointer( "/data/numbers" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/data/strings" ) );

		// Get and verify the arrays
		auto retrievedNumbers = doc.getArrayByPointer( "/data/numbers" );
		auto retrievedStrings = doc.getArrayByPointer( "/data/strings" );

		ASSERT_TRUE( retrievedNumbers.has_value() );
		ASSERT_TRUE( retrievedStrings.has_value() );

		EXPECT_EQ( retrievedNumbers->size(), 3 );
		EXPECT_EQ( retrievedStrings->size(), 2 );

		// Verify content through direct JSON Pointer access
		EXPECT_EQ( doc.getIntByPointer( "/data/numbers/0" ), 10 );
		EXPECT_EQ( doc.getStringByPointer( "/data/strings/1" ), "beta" );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_EmptyArray )
	{
		Document doc = Document::createObject();

		// Create and set an empty array
		Document emptyArray = Document::createArray();
		doc.setArrayByPointer( "/empty", emptyArray );

		// Verify empty array handling
		EXPECT_TRUE( doc.hasFieldByPointer( "/empty" ) );
		EXPECT_TRUE( doc.isArray( "empty" ) );
		EXPECT_EQ( doc.getArraySize( "empty" ), 0 );

		auto retrievedEmpty = doc.getArrayByPointer( "/empty" );
		ASSERT_TRUE( retrievedEmpty.has_value() );
		EXPECT_EQ( retrievedEmpty->size(), 0 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ReplaceExistingArray )
	{
		Document doc = Document::createObject();

		// Create initial array
		Document originalArray = Document::createArray();
		originalArray.addToArray( "", "old1" );
		originalArray.addToArray( "", "old2" );
		doc.setArrayByPointer( "/items", originalArray );

		// Verify original array
		EXPECT_EQ( doc.getArraySize( "items" ), 2 );
		EXPECT_EQ( doc.getArrayElementString( "items", 0 ), "old1" );

		// Replace with new array
		Document newArray = Document::createArray();
		newArray.addToArray( "", "new1" );
		newArray.addToArray( "", "new2" );
		newArray.addToArray( "", "new3" );
		doc.setArrayByPointer( "/items", newArray );

		// Verify replacement
		EXPECT_EQ( doc.getArraySize( "items" ), 3 );
		EXPECT_EQ( doc.getArrayElementString( "items", 0 ), "new1" );
		EXPECT_EQ( doc.getArrayElementString( "items", 2 ), "new3" );

		auto retrievedNew = doc.getArrayByPointer( "/items" );
		ASSERT_TRUE( retrievedNew.has_value() );
		EXPECT_EQ( retrievedNew->size(), 3 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ErrorHandling )
	{
		Document doc = Document::createObject();
		doc.setString( "notArray", "this is a string" );

		// Try to get array from non-array field
		auto result = doc.getArrayByPointer( "/notArray" );
		EXPECT_FALSE( result.has_value() );

		// Try to get array from non-existent path
		auto nonExistent = doc.getArrayByPointer( "/nonExistent" );
		EXPECT_FALSE( nonExistent.has_value() );

		// Try to get array with invalid pointer
		auto invalid = doc.getArrayByPointer( "/invalid/deep/path" );
		EXPECT_FALSE( invalid.has_value() );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ComplexArrayWithObjects )
	{
		Document doc = Document::createObject();

		// Create an array containing objects (this tests the full document capability)
		Document objectArray = Document::createArray();

		// Add first object to array
		Document obj1 = Document::createObject();
		obj1.setString( "name", "Alice" );
		obj1.setInt( "age", 30 );
		objectArray.addDocument( obj1 );

		// Add second object to array
		Document obj2 = Document::createObject();
		obj2.setString( "name", "Bob" );
		obj2.setInt( "age", 25 );
		objectArray.addDocument( obj2 );

		// Set the complex array
		doc.setArrayByPointer( "/users", objectArray );

		// Verify the complex array was set
		EXPECT_TRUE( doc.hasFieldByPointer( "/users" ) );
		EXPECT_TRUE( doc.isArray( "users" ) );
		EXPECT_EQ( doc.getArraySize( "users" ), 2 );

		// Get and verify the complex array
		auto retrievedUsers = doc.getArrayByPointer( "/users" );
		ASSERT_TRUE( retrievedUsers.has_value() );
		EXPECT_EQ( retrievedUsers->size(), 2 );

		// Verify we can access nested object data through JSON Pointers
		EXPECT_EQ( doc.getStringByPointer( "/users/0/name" ), "Alice" );
		EXPECT_EQ( doc.getIntByPointer( "/users/0/age" ), 30 );
		EXPECT_EQ( doc.getStringByPointer( "/users/1/name" ), "Bob" );
		EXPECT_EQ( doc.getIntByPointer( "/users/1/age" ), 25 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_RoundtripSerialization )
	{
		Document original = Document::createObject();

		// Create a mixed array
		Document mixedArray = Document::createArray();
		mixedArray.addToArray( "", "string_value" );
		mixedArray.addToArray( "", static_cast<int64_t>( 42 ) );
		mixedArray.addToArray( "", 3.14 );
		mixedArray.addToArray( "", true );

		original.setArrayByPointer( "/mixed", mixedArray );

		// Serialize and deserialize
		std::string jsonString = original.toJsonString();
		auto deserialized = Document::fromJsonString( jsonString );

		ASSERT_TRUE( deserialized.has_value() );

		// Verify the array survived serialization
		auto deserializedArray = deserialized->getArrayByPointer( "/mixed" );
		ASSERT_TRUE( deserializedArray.has_value() );
		EXPECT_EQ( deserializedArray->size(), 4 );

		// Verify individual elements
		EXPECT_EQ( deserialized->getArrayElementString( "mixed", 0 ), "string_value" );
		EXPECT_EQ( deserialized->getArrayElementInt( "mixed", 1 ), 42 );
		EXPECT_EQ( deserialized->getArrayElementDouble( "mixed", 2 ), 3.14 );
		EXPECT_EQ( deserialized->getArrayElementBool( "mixed", 3 ), true );
	}

	//----------------------------------------------
	// Generic Document Pointer methods tests
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerGenericMethods_BasicSetAndGet )
	{
		Document doc = Document::createObject();

		// Test setting primitives with generic method
		Document stringDoc = Document::createObject();
		stringDoc.setStringByPointer( "", "test string" );
		doc.setDocumentByPointer( "/text", stringDoc );

		Document numberDoc = Document::createObject();
		numberDoc.setIntByPointer( "", 42 );
		doc.setDocumentByPointer( "/number", numberDoc );

		Document boolDoc = Document::createObject();
		boolDoc.setBoolByPointer( "", true );
		doc.setDocumentByPointer( "/flag", boolDoc );

		// Test getting with generic method
		auto retrievedText = doc.getDocumentByPointer( "/text" );
		auto retrievedNumber = doc.getDocumentByPointer( "/number" );
		auto retrievedFlag = doc.getDocumentByPointer( "/flag" );

		ASSERT_TRUE( retrievedText.has_value() );
		ASSERT_TRUE( retrievedNumber.has_value() );
		ASSERT_TRUE( retrievedFlag.has_value() );

		EXPECT_EQ( retrievedText->getStringByPointer( "" ), "test string" );
		EXPECT_EQ( retrievedNumber->getIntByPointer( "" ), 42 );
		EXPECT_EQ( retrievedFlag->getBoolByPointer( "" ), true );
	}

	TEST( DocumentTest, JsonPointerGenericMethods_ArraysAndObjects )
	{
		Document doc = Document::createObject();

		// Create an array
		Document arrayDoc = Document::createArray();
		arrayDoc.addToArray( "", "item1" );
		arrayDoc.addToArray( "", "item2" );
		arrayDoc.addToArray( "", "item3" );

		// Create an object
		Document objectDoc = Document::createObject();
		objectDoc.setStringByPointer( "/name", "Test Object" );
		objectDoc.setIntByPointer( "/value", 100 );

		// Set using generic method
		doc.setDocumentByPointer( "/data/items", arrayDoc );
		doc.setDocumentByPointer( "/data/config", objectDoc );

		// Get using generic method
		auto retrievedArray = doc.getDocumentByPointer( "/data/items" );
		auto retrievedObject = doc.getDocumentByPointer( "/data/config" );

		ASSERT_TRUE( retrievedArray.has_value() );
		ASSERT_TRUE( retrievedObject.has_value() );

		// Verify array content
		EXPECT_EQ( retrievedArray->size(), 3 );
		EXPECT_TRUE( retrievedArray->isArray( "" ) );

		// Verify object content
		EXPECT_EQ( retrievedObject->getStringByPointer( "/name" ), "Test Object" );
		EXPECT_EQ( retrievedObject->getIntByPointer( "/value" ), 100 );
		EXPECT_TRUE( retrievedObject->isObject( "" ) );

		// Also verify type-specific getters still work
		auto typedArray = doc.getArrayByPointer( "/data/items" );
		auto typedObject = doc.getObjectByPointer( "/data/config" );

		ASSERT_TRUE( typedArray.has_value() );
		ASSERT_TRUE( typedObject.has_value() );
	}

	TEST( DocumentTest, JsonPointerGenericMethods_TypeSafetyComparison )
	{
		Document doc = Document::createObject();

		// Create array and object
		Document arrayDoc = Document::createArray();
		arrayDoc.addToArray( "", "test" );

		Document objectDoc = Document::createObject();
		objectDoc.setStringByPointer( "/key", "value" );

		doc.setDocumentByPointer( "/myarray", arrayDoc );
		doc.setDocumentByPointer( "/myobject", objectDoc );

		// Generic getter returns both
		EXPECT_TRUE( doc.getDocumentByPointer( "/myarray" ).has_value() );
		EXPECT_TRUE( doc.getDocumentByPointer( "/myobject" ).has_value() );

		// Type-specific getters are selective
		EXPECT_TRUE( doc.getArrayByPointer( "/myarray" ).has_value() );
		EXPECT_FALSE( doc.getArrayByPointer( "/myobject" ).has_value() ); // Object, not array

		EXPECT_TRUE( doc.getObjectByPointer( "/myobject" ).has_value() );
		EXPECT_FALSE( doc.getObjectByPointer( "/myarray" ).has_value() ); // Array, not object
	}

	TEST( DocumentTest, JsonPointerGenericMethods_ErrorHandling )
	{
		Document doc = Document::createObject();
		doc.setStringByPointer( "/test", "value" );

		// Non-existent paths return nullopt
		EXPECT_FALSE( doc.getDocumentByPointer( "/nonexistent" ).has_value() );
		EXPECT_FALSE( doc.getDocumentByPointer( "/test/nested" ).has_value() );

		// Empty and root paths
		auto rootDoc = doc.getDocumentByPointer( "" );
		EXPECT_TRUE( rootDoc.has_value() );
		EXPECT_TRUE( rootDoc->isObject( "" ) );
	}

	//----------------------------------------------
	// Object Pointer array methods tests
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerObjectMethods_BasicSetAndGet )
	{
		Document doc = Document::createObject();

		// Create a nested object
		Document profileObj = Document::createObject();
		profileObj.setStringByPointer( "/name", "John Doe" );
		profileObj.setIntByPointer( "/age", 30 );
		profileObj.setBoolByPointer( "/active", true );

		// Set the object using JSON Pointer
		doc.setObjectByPointer( "/profile", profileObj );

		// Verify the object was set
		EXPECT_TRUE( doc.hasFieldByPointer( "/profile" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/profile/name" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/profile/age" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/profile/active" ) );

		// Get the object back
		auto retrievedProfile = doc.getObjectByPointer( "/profile" );
		ASSERT_TRUE( retrievedProfile.has_value() );

		// Verify the content
		EXPECT_EQ( retrievedProfile->getStringByPointer( "/name" ), "John Doe" );
		EXPECT_EQ( retrievedProfile->getIntByPointer( "/age" ), 30 );
		EXPECT_EQ( retrievedProfile->getBoolByPointer( "/active" ), true );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_NestedObjects )
	{
		Document doc = Document::createObject();

		// Create deeply nested structure
		Document addressObj = Document::createObject();
		addressObj.setStringByPointer( "/street", "123 Main St" );
		addressObj.setStringByPointer( "/city", "Anytown" );
		addressObj.setIntByPointer( "/zipcode", 12345 );

		Document userObj = Document::createObject();
		userObj.setStringByPointer( "/name", "Jane Smith" );
		userObj.setIntByPointer( "/id", 456 );
		userObj.setObjectByPointer( "/address", addressObj );

		// Set the nested user object
		doc.setObjectByPointer( "/user", userObj );

		// Verify nested access
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/name" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/address/street" ) );
		EXPECT_TRUE( doc.hasFieldByPointer( "/user/address/city" ) );

		// Retrieve and verify nested object
		auto retrievedUser = doc.getObjectByPointer( "/user" );
		ASSERT_TRUE( retrievedUser.has_value() );

		auto retrievedAddress = retrievedUser->getObjectByPointer( "/address" );
		ASSERT_TRUE( retrievedAddress.has_value() );

		EXPECT_EQ( retrievedAddress->getStringByPointer( "/street" ), "123 Main St" );
		EXPECT_EQ( retrievedAddress->getStringByPointer( "/city" ), "Anytown" );
		EXPECT_EQ( retrievedAddress->getIntByPointer( "/zipcode" ), 12345 );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ErrorHandling )
	{
		Document doc = Document::createObject();
		doc.setStringByPointer( "/name", "Test" );
		doc.setIntByPointer( "/value", 42 );

		// Try to get object from non-object fields
		EXPECT_FALSE( doc.getObjectByPointer( "/name" ).has_value() );
		EXPECT_FALSE( doc.getObjectByPointer( "/value" ).has_value() );

		// Try to get non-existent object
		EXPECT_FALSE( doc.getObjectByPointer( "/nonexistent" ).has_value() );

		// Try to get object from array
		Document arrayDoc = Document::createArray();
		arrayDoc.addToArray( "", "item1" );
		doc.setArrayByPointer( "/items", arrayDoc );

		EXPECT_FALSE( doc.getObjectByPointer( "/items" ).has_value() );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ReplaceExistingObject )
	{
		Document doc = Document::createObject();

		// Set initial object
		Document obj1 = Document::createObject();
		obj1.setStringByPointer( "/type", "original" );
		obj1.setIntByPointer( "/version", 1 );
		doc.setObjectByPointer( "/config", obj1 );

		// Verify initial state
		auto retrieved1 = doc.getObjectByPointer( "/config" );
		ASSERT_TRUE( retrieved1.has_value() );
		EXPECT_EQ( retrieved1->getStringByPointer( "/type" ), "original" );
		EXPECT_EQ( retrieved1->getIntByPointer( "/version" ), 1 );

		// Replace with new object
		Document obj2 = Document::createObject();
		obj2.setStringByPointer( "/type", "updated" );
		obj2.setIntByPointer( "/version", 2 );
		obj2.setBoolByPointer( "/active", true );
		doc.setObjectByPointer( "/config", obj2 );

		// Verify replacement
		auto retrieved2 = doc.getObjectByPointer( "/config" );
		ASSERT_TRUE( retrieved2.has_value() );
		EXPECT_EQ( retrieved2->getStringByPointer( "/type" ), "updated" );
		EXPECT_EQ( retrieved2->getIntByPointer( "/version" ), 2 );
		EXPECT_EQ( retrieved2->getBoolByPointer( "/active" ), true );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ComplexObjectWithArrays )
	{
		Document doc = Document::createObject();

		// Create object with mixed content including arrays
		Document complexObj = Document::createObject();
		complexObj.setStringByPointer( "/title", "Complex Object" );

		// Add an array to the object
		Document tagsArray = Document::createArray();
		tagsArray.addToArray( "", "tag1" );
		tagsArray.addToArray( "", "tag2" );
		tagsArray.addToArray( "", "tag3" );
		complexObj.setArrayByPointer( "/tags", tagsArray );

		// Add a nested object
		Document metaObj = Document::createObject();
		metaObj.setStringByPointer( "/author", "Test Author" );
		metaObj.setIntByPointer( "/created", 1234567890 );
		complexObj.setObjectByPointer( "/metadata", metaObj );

		// Set the complex object
		doc.setObjectByPointer( "/data", complexObj );

		// Retrieve and verify
		auto retrieved = doc.getObjectByPointer( "/data" );
		ASSERT_TRUE( retrieved.has_value() );

		EXPECT_EQ( retrieved->getStringByPointer( "/title" ), "Complex Object" );

		auto retrievedTags = retrieved->getArrayByPointer( "/tags" );
		ASSERT_TRUE( retrievedTags.has_value() );
		EXPECT_EQ( retrievedTags->size(), 3 );

		auto retrievedMeta = retrieved->getObjectByPointer( "/metadata" );
		ASSERT_TRUE( retrievedMeta.has_value() );
		EXPECT_EQ( retrievedMeta->getStringByPointer( "/author" ), "Test Author" );
		EXPECT_EQ( retrievedMeta->getIntByPointer( "/created" ), 1234567890 );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_RoundtripSerialization )
	{
		Document original = Document::createObject();

		// Create a complex nested structure
		Document userObj = Document::createObject();
		userObj.setStringByPointer( "/username", "testuser" );
		userObj.setIntByPointer( "/userId", 12345 );

		Document prefsObj = Document::createObject();
		prefsObj.setBoolByPointer( "/emailNotifications", true );
		prefsObj.setStringByPointer( "/theme", "dark" );
		userObj.setObjectByPointer( "/preferences", prefsObj );

		original.setObjectByPointer( "/user", userObj );

		// Serialize to JSON string
		std::string jsonStr = original.toJsonString();

		// Deserialize back
		auto deserialized = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( deserialized.has_value() );

		// Verify the object survived serialization
		auto deserializedUser = deserialized->getObjectByPointer( "/user" );
		ASSERT_TRUE( deserializedUser.has_value() );

		EXPECT_EQ( deserializedUser->getStringByPointer( "/username" ), "testuser" );
		EXPECT_EQ( deserializedUser->getIntByPointer( "/userId" ), 12345 );

		auto deserializedPrefs = deserializedUser->getObjectByPointer( "/preferences" );
		ASSERT_TRUE( deserializedPrefs.has_value() );
		EXPECT_EQ( deserializedPrefs->getBoolByPointer( "/emailNotifications" ), true );
		EXPECT_EQ( deserializedPrefs->getStringByPointer( "/theme" ), "dark" );
	}

	//----------------------------------------------
	// Generic Document operations tests
	//----------------------------------------------

	TEST( DocumentTest, GenericGetDocument )
	{
		Document doc = Document::createObject();

		// Test getting primitive values as documents
		doc.setString( "name", "Alice" );
		doc.setInt( "age", 30 );
		doc.setBool( "active", true );

		auto nameDoc = doc.getDocument( "name" );
		ASSERT_TRUE( nameDoc.has_value() );
		EXPECT_EQ( nameDoc->getString( "" ), "Alice" );

		auto ageDoc = doc.getDocument( "age" );
		ASSERT_TRUE( ageDoc.has_value() );
		EXPECT_EQ( ageDoc->getInt( "" ), 30 );

		auto activeDoc = doc.getDocument( "active" );
		ASSERT_TRUE( activeDoc.has_value() );
		EXPECT_EQ( activeDoc->getBool( "" ), true );

		// Test getting nested objects
		doc.setString( "user.profile.firstName", "Bob" );
		doc.setString( "user.profile.lastName", "Smith" );

		auto userDoc = doc.getDocument( "user" );
		ASSERT_TRUE( userDoc.has_value() );
		EXPECT_EQ( userDoc->getString( "profile.firstName" ), "Bob" );
		EXPECT_EQ( userDoc->getString( "profile.lastName" ), "Smith" );

		auto profileDoc = doc.getDocument( "user.profile" );
		ASSERT_TRUE( profileDoc.has_value() );
		EXPECT_EQ( profileDoc->getString( "firstName" ), "Bob" );
		EXPECT_EQ( profileDoc->getString( "lastName" ), "Smith" );

		// Test getting arrays
		doc.addToArray( "hobbies", "reading" );
		doc.addToArray( "hobbies", "coding" );

		auto hobbiesDoc = doc.getDocument( "hobbies" );
		ASSERT_TRUE( hobbiesDoc.has_value() );
		EXPECT_EQ( hobbiesDoc->size(), 2 );
		EXPECT_EQ( hobbiesDoc->getArrayElementString( "", 0 ), "reading" );
		EXPECT_EQ( hobbiesDoc->getArrayElementString( "", 1 ), "coding" );

		// Test non-existent path
		auto nonExistent = doc.getDocument( "doesnotexist" );
		EXPECT_FALSE( nonExistent.has_value() );
	}

	TEST( DocumentTest, GenericSetDocument )
	{
		Document doc = Document::createObject();

		// Test setting primitive documents
		Document nameDoc = Document::createObject();
		nameDoc.setString( "", "Alice" );
		doc.setDocument( "name", nameDoc );
		EXPECT_EQ( doc.getString( "name" ), "Alice" );

		Document ageDoc = Document::createObject();
		ageDoc.setInt( "", 25 );
		doc.setDocument( "age", ageDoc );
		EXPECT_EQ( doc.getInt( "age" ), 25 );

		// Test setting complex objects
		Document profileDoc = Document::createObject();
		profileDoc.setString( "firstName", "Bob" );
		profileDoc.setString( "lastName", "Smith" );
		profileDoc.setInt( "experience", 5 );

		doc.setDocument( "user.profile", profileDoc );
		EXPECT_EQ( doc.getString( "user.profile.firstName" ), "Bob" );
		EXPECT_EQ( doc.getString( "user.profile.lastName" ), "Smith" );
		EXPECT_EQ( doc.getInt( "user.profile.experience" ), 5 );

		// Test setting arrays
		Document hobbiesDoc = Document::createArray();
		hobbiesDoc.addToArray( "", "reading" );
		hobbiesDoc.addToArray( "", "gaming" );
		hobbiesDoc.addToArray( "", "traveling" );

		doc.setDocument( "hobbies", hobbiesDoc );
		EXPECT_EQ( doc.getArraySize( "hobbies" ), 3 );
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 0 ), "reading" );
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 1 ), "gaming" );
		EXPECT_EQ( doc.getArrayElementString( "hobbies", 2 ), "traveling" );

		// Test overwriting existing values
		Document newNameDoc = Document::createObject();
		newNameDoc.setString( "", "Charlie" );
		doc.setDocument( "name", newNameDoc );
		EXPECT_EQ( doc.getString( "name" ), "Charlie" );

		// Verify JSON structure contains all expected elements (accounting for pretty-printing)
		std::string jsonStr = doc.toJsonString( 0 );
		EXPECT_TRUE( jsonStr.find( "\"age\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "25" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"name\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"Charlie\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"hobbies\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"reading\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"gaming\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"traveling\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"firstName\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"Bob\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"lastName\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"Smith\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "\"experience\"" ) != std::string::npos );
		EXPECT_TRUE( jsonStr.find( "5" ) != std::string::npos );
	}

	TEST( DocumentTest, GenericAddToArrayWithDocument )
	{
		Document doc = Document::createObject();

		// Test adding primitive documents to array
		Document str1 = Document::createObject();
		str1.setString( "", "first" );
		doc.addToArray( "strings", str1 );

		Document str2 = Document::createObject();
		str2.setString( "", "second" );
		doc.addToArray( "strings", str2 );

		EXPECT_EQ( doc.getArraySize( "strings" ), 2 );
		EXPECT_EQ( doc.getArrayElementString( "strings", 0 ), "first" );
		EXPECT_EQ( doc.getArrayElementString( "strings", 1 ), "second" );

		// Test adding complex objects to array
		Document user1 = Document::createObject();
		user1.setString( "name", "Alice" );
		user1.setInt( "age", 30 );
		user1.setBool( "active", true );

		Document user2 = Document::createObject();
		user2.setString( "name", "Bob" );
		user2.setInt( "age", 25 );
		user2.setBool( "active", false );

		doc.addToArray( "users", user1 );
		doc.addToArray( "users", user2 );

		EXPECT_EQ( doc.getArraySize( "users" ), 2 );

		// Verify first user
		auto firstUser = doc.getArrayElement( "users", 0 );
		EXPECT_EQ( firstUser.getString( "name" ), "Alice" );
		EXPECT_EQ( firstUser.getInt( "age" ), 30 );
		EXPECT_EQ( firstUser.getBool( "active" ), true );

		// Verify second user
		auto secondUser = doc.getArrayElement( "users", 1 );
		EXPECT_EQ( secondUser.getString( "name" ), "Bob" );
		EXPECT_EQ( secondUser.getInt( "age" ), 25 );
		EXPECT_EQ( secondUser.getBool( "active" ), false );

		// Test adding nested arrays
		Document nestedArray = Document::createArray();
		nestedArray.addToArray( "", "item1" );
		nestedArray.addToArray( "", "item2" );

		doc.addToArray( "nested", nestedArray );
		EXPECT_EQ( doc.getArraySize( "nested" ), 1 );

		auto retrievedNestedArray = doc.getArrayElement( "nested", 0 );
		EXPECT_EQ( retrievedNestedArray.size(), 2 );
		EXPECT_EQ( retrievedNestedArray.getArrayElementString( "", 0 ), "item1" );
		EXPECT_EQ( retrievedNestedArray.getArrayElementString( "", 1 ), "item2" );

		// Test adding to non-existent array (should create it)
		Document newItem = Document::createObject();
		newItem.setString( "id", "test123" );
		doc.addToArray( "items", newItem );

		EXPECT_EQ( doc.getArraySize( "items" ), 1 );
		EXPECT_EQ( doc.getArrayElementString( "items", 0 ).value_or( "missing" ), "missing" ); // Not a string, it's an object

		auto retrievedItem = doc.getArrayElement( "items", 0 );
		EXPECT_EQ( retrievedItem.getString( "id" ), "test123" );
	}

	TEST( DocumentTest, GenericMethodsConsistency )
	{
		Document doc = Document::createObject();

		// Test consistency between generic and specialized methods
		Document complexDoc = Document::createObject();
		complexDoc.setString( "type", "user" );
		complexDoc.setInt( "id", 12345 );
		complexDoc.setBool( "verified", true );

		// Set using generic method
		doc.setDocument( "profile", complexDoc );

		// Retrieve using specialized methods
		EXPECT_EQ( doc.getString( "profile.type" ), "user" );
		EXPECT_EQ( doc.getInt( "profile.id" ), 12345 );
		EXPECT_EQ( doc.getBool( "profile.verified" ), true );

		// Retrieve using generic method and verify
		auto retrievedProfile = doc.getDocument( "profile" );
		ASSERT_TRUE( retrievedProfile.has_value() );
		EXPECT_EQ( retrievedProfile->getString( "type" ), "user" );
		EXPECT_EQ( retrievedProfile->getInt( "id" ), 12345 );
		EXPECT_EQ( retrievedProfile->getBool( "verified" ), true );

		// Test array consistency
		Document arrayItem = Document::createObject();
		arrayItem.setString( "value", "test" );

		doc.addToArray( "items", arrayItem );

		// Verify via specialized array methods
		EXPECT_EQ( doc.getArraySize( "items" ), 1 );
		auto item = doc.getArrayElement( "items", 0 );
		EXPECT_EQ( item.getString( "value" ), "test" );

		// Verify via generic methods
		auto itemsArray = doc.getDocument( "items" );
		ASSERT_TRUE( itemsArray.has_value() );
		EXPECT_EQ( itemsArray->size(), 1 );
		auto itemFromGeneric = itemsArray->getArrayElement( "", 0 );
		EXPECT_EQ( itemFromGeneric.getString( "value" ), "test" );
	}

	//----------------------------------------------
	// Character utility methods tests
	//----------------------------------------------

	TEST( DocumentTest, CharacterBasicOperations )
	{
		Document doc = Document::createObject();

		// Test setChar and getChar
		doc.setChar( "letter", 'A' );
		auto charResult = doc.getChar( "letter" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_TRUE( doc.isChar( "letter" ) );

		// Test with special characters
		doc.setChar( "special", '\n' );
		charResult = doc.getChar( "special" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), '\n' );
		EXPECT_TRUE( doc.isChar( "special" ) );

		// Test with null character
		doc.setChar( "null_char", '\0' );
		charResult = doc.getChar( "null_char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), '\0' );
		EXPECT_TRUE( doc.isChar( "null_char" ) );

		// Test nested path
		doc.setChar( "nested.deep.char", 'Z' );
		charResult = doc.getChar( "nested.deep.char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Z' );
		EXPECT_TRUE( doc.isChar( "nested.deep.char" ) );
	}

	TEST( DocumentTest, CharacterJsonPointerMethods )
	{
		Document doc = Document::createObject();

		// Test setCharByPointer with JSON Pointer notation
		doc.setCharByPointer( "/ptr_char", 'X' );
		auto charResult = doc.getCharByPointer( "/ptr_char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_TRUE( doc.hasCharByPointer( "/ptr_char" ) );

		// Test nested JSON Pointer paths
		doc.setCharByPointer( "/user/initial", 'J' );
		charResult = doc.getCharByPointer( "/user/initial" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'J' );
		EXPECT_TRUE( doc.hasCharByPointer( "/user/initial" ) );

		// Test array index JSON Pointer
		doc.setCharByPointer( "/grades/0", 'A' );
		charResult = doc.getCharByPointer( "/grades/0" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_TRUE( doc.hasCharByPointer( "/grades/0" ) );

		// Test hasCharByPointer with non-existent paths
		EXPECT_FALSE( doc.hasCharByPointer( "/nonexistent" ) );
		EXPECT_FALSE( doc.hasCharByPointer( "/user/nonexistent" ) );
	}

	TEST( DocumentTest, CharacterJsonPointerVsDotNotation )
	{
		Document doc = Document::createObject();

		// Test that JSON Pointer and dot notation access different paths
		doc.setChar( "user.name", 'D' );		   // Dot notation - creates nested object
		doc.setCharByPointer( "/user.name", 'P' ); // JSON Pointer - creates field named "user.name"

		// Verify they access different locations
		auto dotResult = doc.getChar( "user.name" );
		ASSERT_TRUE( dotResult.has_value() );
		EXPECT_EQ( dotResult.value(), 'D' );

		auto pointerResult = doc.getCharByPointer( "/user.name" );
		ASSERT_TRUE( pointerResult.has_value() );
		EXPECT_EQ( pointerResult.value(), 'P' );

		// Verify type checking works correctly
		EXPECT_TRUE( doc.isChar( "user.name" ) );
		EXPECT_TRUE( doc.hasCharByPointer( "/user.name" ) );

		// Test nested structure with JSON Pointers
		doc.setCharByPointer( "/profile/data/grade", 'A' );
		auto nestedResult = doc.getCharByPointer( "/profile/data/grade" );
		ASSERT_TRUE( nestedResult.has_value() );
		EXPECT_EQ( nestedResult.value(), 'A' );
		EXPECT_TRUE( doc.hasCharByPointer( "/profile/data/grade" ) );
	}

	TEST( DocumentTest, CharacterArrayOperations )
	{
		Document doc = Document::createObject();

		// Test addCharToArray
		doc.addToArray( "char_array", 'A' );
		doc.addToArray( "char_array", 'B' );
		doc.addToArray( "char_array", 'C' );

		// Test getArrayElementChar
		auto charResult = doc.getArrayElementChar( "char_array", 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );

		charResult = doc.getArrayElementChar( "char_array", 1 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'B' );

		charResult = doc.getArrayElementChar( "char_array", 2 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'C' );

		// Test array size
		EXPECT_EQ( doc.getArraySize( "char_array" ), 3 );

		// Test out of bounds access
		charResult = doc.getArrayElementChar( "char_array", 10 );
		EXPECT_FALSE( charResult.has_value() );

		// Test adding to non-existent array (should create array)
		doc.addToArray( "new_array", 'X' );
		charResult = doc.getArrayElementChar( "new_array", 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_EQ( doc.getArraySize( "new_array" ), 1 );
	}

	TEST( DocumentTest, CharacterValidationMethods )
	{
		Document doc = Document::createObject();

		// Test isChar with valid single character
		doc.setChar( "single", 'Q' );
		EXPECT_TRUE( doc.isChar( "single" ) );

		// Test isChar with multi-character string (should be false)
		doc.setString( "multi", "Hello" );
		EXPECT_FALSE( doc.isChar( "multi" ) );

		// Test isChar with empty string (should be false)
		doc.setString( "empty", "" );
		EXPECT_FALSE( doc.isChar( "empty" ) );

		// Test isChar with non-string types
		doc.setInt( "number", 42 );
		EXPECT_FALSE( doc.isChar( "number" ) );

		doc.setBool( "boolean", true );
		EXPECT_FALSE( doc.isChar( "boolean" ) );

		// Test isChar with non-existent path
		EXPECT_FALSE( doc.isChar( "nonexistent" ) );
	}

	TEST( DocumentTest, CharacterErrorHandling )
	{
		Document doc = Document::createObject();

		// Test getChar on non-existent path
		auto charResult = doc.getChar( "nonexistent" );
		EXPECT_FALSE( charResult.has_value() );

		// Test getChar on non-string value
		doc.setInt( "number", 123 );
		charResult = doc.getChar( "number" );
		EXPECT_FALSE( charResult.has_value() );

		// Test getChar on empty string
		doc.setString( "empty", "" );
		charResult = doc.getChar( "empty" );
		EXPECT_FALSE( charResult.has_value() );

		// Test array operations on non-existent array
		charResult = doc.getArrayElementChar( "nonexistent_array", 0 );
		EXPECT_FALSE( charResult.has_value() );

		// Test array operations on non-array value
		doc.setString( "not_array", "test" );
		charResult = doc.getArrayElementChar( "not_array", 0 );
		EXPECT_FALSE( charResult.has_value() );
	}

	TEST( DocumentTest, CharacterIntegrationWithOtherTypes )
	{
		Document doc = Document::createObject();

		// Create a mixed document with characters and other types
		doc.setChar( "initial", 'A' );
		doc.setString( "name", "John" );
		doc.setInt( "age", 30 );
		doc.setBool( "active", true );

		// Verify character operations don't interfere with other types
		auto charResult = doc.getChar( "initial" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_EQ( doc.getString( "name" ), "John" );
		EXPECT_EQ( doc.getInt( "age" ), 30 );
		EXPECT_EQ( doc.getBool( "active" ), true );

		// Test overwriting different types with characters
		doc.setChar( "age", 'X' ); // Overwrite int with char
		charResult = doc.getChar( "age" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_TRUE( doc.isChar( "age" ) );

		// After overwriting with char, getInt should return nullopt since it's no longer an int
		auto intResult = doc.getInt( "age" );
		EXPECT_FALSE( intResult.has_value() );

		// Test character arrays mixed with regular arrays
		doc.addToArray( "mixed_array", 'Z' );
		doc.addToArray( "mixed_array", static_cast<int64_t>( 42 ) ); // Explicit cast to avoid ambiguity
		doc.addToArray( "mixed_array", 'Y' );

		EXPECT_EQ( doc.getArraySize( "mixed_array" ), 3 );

		charResult = doc.getArrayElementChar( "mixed_array", 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Z' );

		EXPECT_EQ( doc.getArrayElementInt( "mixed_array", 1 ), 42 );

		charResult = doc.getArrayElementChar( "mixed_array", 2 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Y' );
	}
} // namespace nfx::serialization::json::test
