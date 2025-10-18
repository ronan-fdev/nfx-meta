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
		Document obj;
		EXPECT_TRUE( obj.isValid() );
		EXPECT_EQ( obj.toJsonString(), "{}" );

		Document arr;
		arr.set<Document::Array>( "" );
		EXPECT_TRUE( arr.isValid() );
		EXPECT_EQ( arr.toJsonString(), "[]" );
	}

	TEST( DocumentTest, CopyAndMove )
	{
		Document original;
		original.set<std::string>( "test", "value" );

		// Copy constructor
		Document copied( original );
		EXPECT_EQ( copied.get<std::string>( "test" ), "value" );

		// Move constructor
		Document moved( std::move( copied ) );
		EXPECT_EQ( moved.get<std::string>( "test" ), "value" );

		// Copy assignment
		Document assigned;
		assigned = original;
		EXPECT_EQ( assigned.get<std::string>( "test" ), "value" );
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

		EXPECT_EQ( doc.get<std::string>( "name" ), "John" );
		EXPECT_EQ( doc.get<int64_t>( "age" ), 30 );
		EXPECT_EQ( doc.get<bool>( "active" ), true );
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
			EXPECT_EQ( validWithWhitespaceResult->get<std::string>( "test" ), "value" );
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

		EXPECT_EQ( doc.get<std::string>( "user.profile.name" ), "Alice" );
		EXPECT_EQ( doc.get<std::string>( "user.profile.settings.theme" ), "dark" );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	TEST( DocumentTest, BasicValueAccess )
	{
		Document doc;
		doc.set<std::string>( "name", "Bob" );
		doc.set<int64_t>( "age", 25 );
		doc.set<double>( "height", 1.75 );
		doc.set<bool>( "married", false );
		doc.setNull( "spouse" );

		EXPECT_EQ( doc.get<std::string>( "name" ), "Bob" );
		EXPECT_EQ( doc.get<int64_t>( "age" ), 25 );
		EXPECT_EQ( doc.get<double>( "height" ), 1.75 );
		EXPECT_EQ( doc.get<bool>( "married" ), false );
		EXPECT_TRUE( doc.hasValue( "spouse" ) );
	}

	TEST( DocumentTest, PathBasedAccess )
	{
		Document doc;
		doc.set<std::string>( "user.profile.firstName", "Charlie" );
		doc.set<std::string>( "user.profile.lastName", "Brown" );
		doc.set<int64_t>( "user.settings.notifications", 1 );

		EXPECT_EQ( doc.get<std::string>( "user.profile.firstName" ), "Charlie" );
		EXPECT_EQ( doc.get<std::string>( "user.profile.lastName" ), "Brown" );
		EXPECT_EQ( doc.get<int64_t>( "user.settings.notifications" ), 1 );
	}

	TEST( DocumentTest, NonExistentFields )
	{
		Document doc;

		// Test that non-existent fields return empty optionals
		EXPECT_FALSE( doc.hasValue( "nonexistent" ) );
		EXPECT_FALSE( doc.get<std::string>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<int64_t>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<double>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<bool>( "nonexistent" ).has_value() );
	}

	//----------------------------------------------
	// Array operations
	//----------------------------------------------

	TEST( DocumentTest, BasicArrayOperations )
	{
		Document doc;

		auto numbersOpt = doc.get<Document::Array>( "numbers" );
		if ( !numbersOpt.has_value() )
		{
			doc.set<Document::Array>( "numbers" );
			numbersOpt = doc.get<Document::Array>( "numbers" );
		}
		auto numbers = numbersOpt.value();
		numbers.add<int64_t>( 1 );
		numbers.add<int64_t>( 2 );
		numbers.add<int64_t>( 3 );

		EXPECT_TRUE( doc.is<Document::Array>( "numbers" ) );
		EXPECT_EQ( doc.get<Document::Array>( "numbers" ).value().size(), 3 );
	}

	TEST( DocumentTest, ArrayWithDifferentTypes )
	{
		Document doc;

		auto mixedOpt = doc.get<Document::Array>( "mixed" );
		if ( !mixedOpt.has_value() )
		{
			doc.set<Document::Array>( "mixed" );
			mixedOpt = doc.get<Document::Array>( "mixed" );
		}
		auto mixed = mixedOpt.value();
		mixed.add<std::string>( "hello" );
		mixed.add<int64_t>( 42 );
		mixed.add<double>( 3.14 );

		EXPECT_EQ( doc.get<Document::Array>( "mixed" ).value().size(), 3 );
	}

	TEST( DocumentTest, ArrayElementAccess )
	{
		std::string jsonStr = R"({"items": [{"name": "item1"}, {"name": "item2"}]})";
		auto maybeDoc = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		auto array = doc.get<Document::Array>( "items" );
		ASSERT_TRUE( array.has_value() );
		auto firstItem = array->get<Document>( 0 );
		auto secondItem = array->get<Document>( 1 );
		ASSERT_TRUE( firstItem.has_value() );
		ASSERT_TRUE( secondItem.has_value() );

		EXPECT_EQ( firstItem->get<std::string>( "name" ).value_or( "" ), "item1" );
		EXPECT_EQ( secondItem->get<std::string>( "name" ).value_or( "" ), "item2" );
	}

	TEST( DocumentTest, ClearArray )
	{
		Document doc;
		auto numbersOpt = doc.get<Document::Array>( "numbers" );
		if ( !numbersOpt.has_value() )
		{
			doc.set<Document::Array>( "numbers" );
			numbersOpt = doc.get<Document::Array>( "numbers" );
		}
		auto numbers = numbersOpt.value();
		numbers.add<int64_t>( 1 );
		numbers.add<int64_t>( 2 );
		numbers.add<int64_t>( 3 );

		EXPECT_EQ( doc.get<Document::Array>( "numbers" ).value().size(), 3 );

		// Clear the array
		doc.get<Document::Array>( "numbers" ).value().clear();
		EXPECT_EQ( doc.get<Document::Array>( "numbers" ).value().size(), 0 );
		EXPECT_TRUE( doc.is<Document::Array>( "numbers" ) ); // Should still be an array, just empty

		// Test clearing non-existent array (should not crash)
		auto nonexistentArray = doc.get<Document::Array>( "nonexistent" );
		if ( nonexistentArray.has_value() )
		{
			nonexistentArray.value().clear();
		}

		// Test clearing non-array field (should not crash)
		doc.set<std::string>( "notAnArray", "value" );
		auto notAnArray = doc.get<Document::Array>( "notAnArray" );
		if ( notAnArray.has_value() )
		{
			notAnArray.value().clear();
		}
		EXPECT_EQ( doc.get<std::string>( "notAnArray" ), "value" ); // Should remain unchanged
	}

	TEST( DocumentTest, ArrayElementPrimitiveAccess )
	{
		Document doc;

		// Create arrays with different primitive types
		auto stringsOpt = doc.get<Document::Array>( "strings" );
		if ( !stringsOpt.has_value() )
		{
			doc.set<Document::Array>( "strings" );
			stringsOpt = doc.get<Document::Array>( "strings" );
		}
		auto strings = stringsOpt.value();
		strings.add<std::string>( "hello" );
		strings.add<std::string>( "world" );
		strings.add<std::string>( "test" );

		auto numbersOpt = doc.get<Document::Array>( "numbers" );
		if ( !numbersOpt.has_value() )
		{
			doc.set<Document::Array>( "numbers" );
			numbersOpt = doc.get<Document::Array>( "numbers" );
		}
		auto numbers = numbersOpt.value();
		numbers.add<int64_t>( 10 );
		numbers.add<int64_t>( 20 );
		numbers.add<int64_t>( 30 );

		auto doublesOpt = doc.get<Document::Array>( "doubles" );
		if ( !doublesOpt.has_value() )
		{
			doc.set<Document::Array>( "doubles" );
			doublesOpt = doc.get<Document::Array>( "doubles" );
		}
		auto doubles = doublesOpt.value();
		doubles.add<double>( 1.5 );
		doubles.add<double>( 2.5 );
		doubles.add<double>( 3.5 );

		auto boolsOpt = doc.get<Document::Array>( "bools" );
		if ( !boolsOpt.has_value() )
		{
			doc.set<Document::Array>( "bools" );
			boolsOpt = doc.get<Document::Array>( "bools" );
		}
		auto bools = boolsOpt.value();
		bools.add<bool>( true );
		bools.add<bool>( false );
		bools.add<bool>( true );

		// Test string array element access
		auto stringArray = doc.get<Document::Array>( "strings" ).value();
		EXPECT_EQ( stringArray.get<std::string>( 0 ).value_or( "" ), "hello" );
		EXPECT_EQ( stringArray.get<std::string>( 1 ).value_or( "" ), "world" );
		EXPECT_EQ( stringArray.get<std::string>( 2 ).value_or( "" ), "test" );
		EXPECT_FALSE( stringArray.get<std::string>( 10 ).has_value() ); // Out of bounds

		// Test integer array element access
		auto numberArray = doc.get<Document::Array>( "numbers" ).value();
		EXPECT_EQ( numberArray.get<int64_t>( 0 ).value_or( 0 ), 10 );
		EXPECT_EQ( numberArray.get<int64_t>( 1 ).value_or( 0 ), 20 );
		EXPECT_EQ( numberArray.get<int64_t>( 2 ).value_or( 0 ), 30 );
		EXPECT_FALSE( numberArray.get<int64_t>( 10 ).has_value() ); // Out of bounds

		// Test double array element access
		auto doubleArray = doc.get<Document::Array>( "doubles" ).value();
		EXPECT_EQ( doubleArray.get<double>( 0 ).value_or( 0.0 ), 1.5 );
		EXPECT_EQ( doubleArray.get<double>( 1 ).value_or( 0.0 ), 2.5 );
		EXPECT_EQ( doubleArray.get<double>( 2 ).value_or( 0.0 ), 3.5 );
		EXPECT_FALSE( doubleArray.get<double>( 10 ).has_value() ); // Out of bounds

		// Test boolean array element access
		auto boolArray = doc.get<Document::Array>( "bools" ).value();
		EXPECT_EQ( boolArray.get<bool>( 0 ).value_or( false ), true );
		EXPECT_EQ( boolArray.get<bool>( 1 ).value_or( false ), false );
		EXPECT_EQ( boolArray.get<bool>( 2 ).value_or( false ), true );
		EXPECT_FALSE( boolArray.get<bool>( 10 ).has_value() ); // Out of bounds

		// Test type safety - accessing wrong types should return nullopt
		EXPECT_FALSE( numberArray.get<std::string>( 0 ).has_value() );							  // int accessed as string
		EXPECT_FALSE( stringArray.get<int64_t>( 0 ).has_value() );								  // string accessed as int
		EXPECT_FALSE( boolArray.get<double>( 0 ).has_value() );									  // bool accessed as double
		EXPECT_FALSE( doc.get<Document::Array>( "strings" ).value().get<bool>( 0 ).has_value() ); // string accessed as bool

		// Test non-existent arrays
		EXPECT_FALSE( doc.get<Document::Array>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Array>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Array>( "nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Array>( "nonexistent" ).has_value() );

		// Test accessing non-array fields
		doc.set<std::string>( "notArray", "value" );
		EXPECT_FALSE( doc.get<Document::Array>( "notArray" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Array>( "notArray" ).has_value() );
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
		EXPECT_EQ( doc.get<Document::Array>( "users" ).value().get<std::string>( 0 ).value_or( "" ), "alice" );
		EXPECT_EQ( doc.get<Document::Array>( "users" ).value().get<std::string>( 1 ).value_or( "" ), "bob" );
		EXPECT_EQ( doc.get<Document::Array>( "users" ).value().get<std::string>( 2 ).value_or( "" ), "charlie" );

		// Test integer array from parsed JSON
		EXPECT_EQ( doc.get<Document::Array>( "scores" ).value().get<int64_t>( 0 ).value_or( 0 ), 95 );
		EXPECT_EQ( doc.get<Document::Array>( "scores" ).value().get<int64_t>( 1 ).value_or( 0 ), 87 );
		EXPECT_EQ( doc.get<Document::Array>( "scores" ).value().get<int64_t>( 2 ).value_or( 0 ), 92 );
		EXPECT_EQ( doc.get<Document::Array>( "scores" ).value().get<int64_t>( 3 ).value_or( 0 ), 78 );

		// Test double array from parsed JSON
		EXPECT_EQ( doc.get<Document::Array>( "prices" ).value().get<double>( 0 ).value_or( 0.0 ), 19.99 );
		EXPECT_EQ( doc.get<Document::Array>( "prices" ).value().get<double>( 1 ).value_or( 0.0 ), 25.50 );
		EXPECT_EQ( doc.get<Document::Array>( "prices" ).value().get<double>( 2 ).value_or( 0.0 ), 12.75 );

		// Test boolean array from parsed JSON
		EXPECT_EQ( doc.get<Document::Array>( "flags" ).value().get<bool>( 0 ).value_or( false ), true );
		EXPECT_EQ( doc.get<Document::Array>( "flags" ).value().get<bool>( 1 ).value_or( false ), false );
		EXPECT_EQ( doc.get<Document::Array>( "flags" ).value().get<bool>( 2 ).value_or( false ), true );
		EXPECT_EQ( doc.get<Document::Array>( "flags" ).value().get<bool>( 3 ).value_or( false ), false );
	}

	//----------------------------------------------
	// Integer Types
	//----------------------------------------------

	TEST( DocumentTest, AllIntegerTypes )
	{
		Document doc;

		// Test values for each integer type
		int8_t val_i8 = -127;
		int16_t val_i16 = -32767;
		int32_t val_i32 = -2147483647;
		int64_t val_i64 = -9223372036854775807LL;
		uint8_t val_u8 = 255;
		uint16_t val_u16 = 65535;
		uint32_t val_u32 = 4294967295U;
		uint64_t val_u64 = 18446744073709551615ULL;

		// Test Document-level set/get for all integer types
		doc.set<int8_t>( "signed/i8", val_i8 );
		doc.set<int16_t>( "signed/i16", val_i16 );
		doc.set<int32_t>( "signed/i32", val_i32 );
		doc.set<int64_t>( "signed/i64", val_i64 );
		doc.set<uint8_t>( "unsigned/u8", val_u8 );
		doc.set<uint16_t>( "unsigned/u16", val_u16 );
		doc.set<uint32_t>( "unsigned/u32", val_u32 );
		doc.set<uint64_t>( "unsigned/u64", val_u64 );

		// Test Document-level get for all integer types
		EXPECT_EQ( doc.get<int8_t>( "signed/i8" ).value(), val_i8 );
		EXPECT_EQ( doc.get<int16_t>( "signed/i16" ).value(), val_i16 );
		EXPECT_EQ( doc.get<int32_t>( "signed/i32" ).value(), val_i32 );
		EXPECT_EQ( doc.get<int64_t>( "signed/i64" ).value(), val_i64 );
		EXPECT_EQ( doc.get<uint8_t>( "unsigned/u8" ).value(), val_u8 );
		EXPECT_EQ( doc.get<uint16_t>( "unsigned/u16" ).value(), val_u16 );
		EXPECT_EQ( doc.get<uint32_t>( "unsigned/u32" ).value(), val_u32 );
		EXPECT_EQ( doc.get<uint64_t>( "unsigned/u64" ).value(), val_u64 );

		// Test Object-level set/get for all integer types
		doc.set<std::string>( "/testObject/dummy", "temp" );
		auto obj = doc.get<Document::Object>( "/testObject" );
		ASSERT_TRUE( obj.has_value() );
		obj->set<int8_t>( "signed/i8", val_i8 );
		obj->set<int16_t>( "signed/i16", val_i16 );
		obj->set<int32_t>( "signed/i32", val_i32 );
		obj->set<int64_t>( "signed/i64", val_i64 );
		obj->set<uint8_t>( "unsigned/u8", val_u8 );
		obj->set<uint16_t>( "unsigned/u16", val_u16 );
		obj->set<uint32_t>( "unsigned/u32", val_u32 );
		obj->set<uint64_t>( "unsigned/u64", val_u64 );

		EXPECT_EQ( obj->get<int8_t>( "signed/i8" ).value(), val_i8 );
		EXPECT_EQ( obj->get<int16_t>( "signed/i16" ).value(), val_i16 );
		EXPECT_EQ( obj->get<int32_t>( "signed/i32" ).value(), val_i32 );
		EXPECT_EQ( obj->get<int64_t>( "signed/i64" ).value(), val_i64 );
		EXPECT_EQ( obj->get<uint8_t>( "unsigned/u8" ).value(), val_u8 );
		EXPECT_EQ( obj->get<uint16_t>( "unsigned/u16" ).value(), val_u16 );
		EXPECT_EQ( obj->get<uint32_t>( "unsigned/u32" ).value(), val_u32 );
		EXPECT_EQ( obj->get<uint64_t>( "unsigned/u64" ).value(), val_u64 );

		// Test Array-level add/get for all integer types
		doc.set<Document::Array>( "/testArray" );
		auto arr = doc.get<Document::Array>( "/testArray" );
		ASSERT_TRUE( arr.has_value() );
		arr->add<int8_t>( val_i8 );	   // index 0
		arr->add<int16_t>( val_i16 );  // index 1
		arr->add<int32_t>( val_i32 );  // index 2
		arr->add<int64_t>( val_i64 );  // index 3
		arr->add<uint8_t>( val_u8 );   // index 4
		arr->add<uint16_t>( val_u16 ); // index 5
		arr->add<uint32_t>( val_u32 ); // index 6
		arr->add<uint64_t>( val_u64 ); // index 7

		EXPECT_EQ( arr->get<int8_t>( 0 ).value(), val_i8 );
		EXPECT_EQ( arr->get<int16_t>( 1 ).value(), val_i16 );
		EXPECT_EQ( arr->get<int32_t>( 2 ).value(), val_i32 );
		EXPECT_EQ( arr->get<int64_t>( 3 ).value(), val_i64 );
		EXPECT_EQ( arr->get<uint8_t>( 4 ).value(), val_u8 );
		EXPECT_EQ( arr->get<uint16_t>( 5 ).value(), val_u16 );
		EXPECT_EQ( arr->get<uint32_t>( 6 ).value(), val_u32 );
		EXPECT_EQ( arr->get<uint64_t>( 7 ).value(), val_u64 );

		// Test Array-level set for all integer types
		arr->set<int8_t>( 0, static_cast<int8_t>( -100 ) );
		arr->set<int16_t>( 1, static_cast<int16_t>( -30000 ) );
		arr->set<int32_t>( 2, static_cast<int32_t>( -2000000000 ) );
		arr->set<int64_t>( 3, static_cast<int64_t>( -8000000000000000000LL ) );
		arr->set<uint8_t>( 4, static_cast<uint8_t>( 200 ) );
		arr->set<uint16_t>( 5, static_cast<uint16_t>( 60000 ) );
		arr->set<uint32_t>( 6, static_cast<uint32_t>( 3000000000U ) );
		arr->set<uint64_t>( 7, static_cast<uint64_t>( 15000000000000000000ULL ) );

		EXPECT_EQ( arr->get<int8_t>( 0 ).value(), -100 );
		EXPECT_EQ( arr->get<int16_t>( 1 ).value(), -30000 );
		EXPECT_EQ( arr->get<int32_t>( 2 ).value(), -2000000000 );
		EXPECT_EQ( arr->get<int64_t>( 3 ).value(), -8000000000000000000LL );
		EXPECT_EQ( arr->get<uint8_t>( 4 ).value(), 200 );
		EXPECT_EQ( arr->get<uint16_t>( 5 ).value(), 60000 );
		EXPECT_EQ( arr->get<uint32_t>( 6 ).value(), 3000000000U );
		EXPECT_EQ( arr->get<uint64_t>( 7 ).value(), 15000000000000000000ULL );

		// Test Array-level insert for all integer types
		doc.set<Document::Array>( "/insertArray" );
		auto insertArr = doc.get<Document::Array>( "/insertArray" );
		ASSERT_TRUE( insertArr.has_value() );
		insertArr->insert<int8_t>( 0, val_i8 );
		insertArr->insert<int16_t>( 1, val_i16 );
		insertArr->insert<int32_t>( 2, val_i32 );
		insertArr->insert<int64_t>( 3, val_i64 );
		insertArr->insert<uint8_t>( 4, val_u8 );
		insertArr->insert<uint16_t>( 5, val_u16 );
		insertArr->insert<uint32_t>( 6, val_u32 );
		insertArr->insert<uint64_t>( 7, val_u64 );

		EXPECT_EQ( insertArr->get<int8_t>( 0 ).value(), val_i8 );
		EXPECT_EQ( insertArr->get<int16_t>( 1 ).value(), val_i16 );
		EXPECT_EQ( insertArr->get<int32_t>( 2 ).value(), val_i32 );
		EXPECT_EQ( insertArr->get<int64_t>( 3 ).value(), val_i64 );
		EXPECT_EQ( insertArr->get<uint8_t>( 4 ).value(), val_u8 );
		EXPECT_EQ( insertArr->get<uint16_t>( 5 ).value(), val_u16 );
		EXPECT_EQ( insertArr->get<uint32_t>( 6 ).value(), val_u32 );
		EXPECT_EQ( insertArr->get<uint64_t>( 7 ).value(), val_u64 );

		// Test range validation for smaller integer types
		// Test that values outside the range return nullopt
		doc.set( "overflow_test", static_cast<int64_t>( 300 ) ); // Out of int8_t range
		EXPECT_FALSE( doc.get<int8_t>( "overflow_test" ).has_value() );

		doc.set( "overflow_test", static_cast<int64_t>( 70000 ) ); // Out of int16_t range
		EXPECT_FALSE( doc.get<int16_t>( "overflow_test" ).has_value() );

		doc.set( "overflow_test", static_cast<uint64_t>( 300 ) ); // Out of uint8_t range
		EXPECT_FALSE( doc.get<uint8_t>( "overflow_test" ).has_value() );

		doc.set( "overflow_test", static_cast<uint64_t>( 70000 ) ); // Out of uint16_t range
		EXPECT_FALSE( doc.get<uint16_t>( "overflow_test" ).has_value() );

		// Test edge values
		doc.set( "edge/i8_min", std::numeric_limits<int8_t>::min() );
		doc.set( "edge/i8_max", std::numeric_limits<int8_t>::max() );
		doc.set( "edge/u8_max", std::numeric_limits<uint8_t>::max() );

		EXPECT_EQ( doc.get<int8_t>( "edge/i8_min" ).value(), std::numeric_limits<int8_t>::min() );
		EXPECT_EQ( doc.get<int8_t>( "edge/i8_max" ).value(), std::numeric_limits<int8_t>::max() );
		EXPECT_EQ( doc.get<uint8_t>( "edge/u8_max" ).value(), std::numeric_limits<uint8_t>::max() );
	}

	//----------------------------------------------
	// Floating-Point Types
	//----------------------------------------------

	TEST( DocumentTest, AllFloatingPointTypes )
	{
		Document doc;

		// Test values
		float val_float = 3.14159f;
		double val_double = 2.718281828459045;

		// Set floating-point values
		doc.set<float>( "floats/float_val", val_float );
		doc.set<double>( "floats/double_val", val_double );

		// Test retrieval
		EXPECT_EQ( doc.get<float>( "floats/float_val" ).value(), val_float );
		EXPECT_EQ( doc.get<double>( "floats/double_val" ).value(), val_double );

		// Test type checking
		EXPECT_TRUE( doc.is<float>( "floats/float_val" ) );
		EXPECT_TRUE( doc.is<double>( "floats/double_val" ) );

		// Test cross-type compatibility (float to double, double to float)
		EXPECT_TRUE( doc.is<double>( "floats/float_val" ) ); // float should also be valid as double
		EXPECT_TRUE( doc.is<float>( "floats/double_val" ) ); // double should also be valid as float

		// Test array operations with floating-point types
		doc.set<Document::Array>( "float_array" );
		Document::Array floatArray = doc.get<Document::Array>( "float_array" ).value();
		floatArray.add<float>( val_float );
		floatArray.add<double>( val_double );

		EXPECT_EQ( floatArray.size(), 2 );
		EXPECT_EQ( floatArray.get<float>( 0 ).value(), val_float );
		EXPECT_EQ( floatArray.get<double>( 1 ).value(), val_double );

		// Test edge cases
		float float_zero = 0.0f;
		float float_negative = -123.456f;
		double double_large = 1.7976931348623157e+308; // Near max double
		double double_small = 2.2250738585072014e-308; // Near min positive double

		doc.set<float>( "edge/float_zero", float_zero );
		doc.set<float>( "edge/float_negative", float_negative );
		doc.set<double>( "edge/double_large", double_large );
		doc.set<double>( "edge/double_small", double_small );

		EXPECT_EQ( doc.get<float>( "edge/float_zero" ).value(), float_zero );
		EXPECT_EQ( doc.get<float>( "edge/float_negative" ).value(), float_negative );
		EXPECT_EQ( doc.get<double>( "edge/double_large" ).value(), double_large );
		EXPECT_EQ( doc.get<double>( "edge/double_small" ).value(), double_small );

		// Test JSON Pointer syntax
		doc.set<float>( "/json_pointer/float", val_float );
		doc.set<double>( "/json_pointer/double", val_double );

		EXPECT_EQ( doc.get<float>( "/json_pointer/float" ).value(), val_float );
		EXPECT_EQ( doc.get<double>( "/json_pointer/double" ).value(), val_double );

		// Test serialization round-trip
		std::string jsonStr = doc.toJsonString();
		auto docFromJson = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( docFromJson.has_value() );

		EXPECT_EQ( docFromJson->get<float>( "floats/float_val" ).value(), val_float );
		EXPECT_EQ( docFromJson->get<double>( "floats/double_val" ).value(), val_double );
	}

	//----------------------------------------------
	// Advanced Document operations
	//----------------------------------------------

	TEST( DocumentTest, DocumentArrayOperations )
	{
		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		Document item1;
		item1.set<std::string>( "name", "Alice" );
		item1.set<int64_t>( "age", 30 );

		Document item2;
		item2.set<std::string>( "name", "Bob" );
		item2.set<int64_t>( "age", 25 );

		// Get array wrapper and add documents
		auto array = arrayDoc.get<Document::Array>( "" );
		ASSERT_TRUE( array.has_value() );
		array->add<Document>( item1 );
		array->add<Document>( item2 );

		EXPECT_EQ( array->size(), 2 );
	}

	TEST( DocumentTest, SetArrayDocument )
	{
		Document mainDoc;

		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		auto arrayOpt = arrayDoc.get<Document::Array>( "" );
		if ( arrayOpt.has_value() )
		{
			auto array = arrayOpt.value();
			array.add<std::string>( "value1" );
			array.add<std::string>( "value2" );
		}

		mainDoc.set<Document>( "myArray", arrayDoc );

		EXPECT_TRUE( mainDoc.is<Document::Array>( "myArray" ) );
		EXPECT_EQ( mainDoc.get<Document::Array>( "myArray" ).value().size(), 2 );
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	TEST( DocumentTest, TypeCheckingMethods )
	{
		Document doc;

		// Set up different types
		doc.set<std::string>( "stringField", "hello" );
		doc.set<int64_t>( "intField", 42 );
		doc.set<double>( "doubleField", 3.14 );
		doc.set<bool>( "boolField", true );
		doc.setNull( "nullField" );

		// Create nested object
		doc.set<std::string>( "nested.object.field", "nested_value" );

		// Create array
		auto arrayOpt = doc.get<Document::Array>( "arrayField" );
		if ( !arrayOpt.has_value() )
		{
			doc.set<Document::Array>( "arrayField" );
			arrayOpt = doc.get<Document::Array>( "arrayField" );
		}
		arrayOpt.value().add<std::string>( "item1" );

		// Test string type checking
		EXPECT_TRUE( doc.is<std::string>( "stringField" ) );
		EXPECT_FALSE( doc.is<std::string>( "intField" ) );
		EXPECT_FALSE( doc.is<std::string>( "nonexistent" ) );

		// Test integer type checking
		EXPECT_TRUE( doc.is<int>( "intField" ) );
		EXPECT_FALSE( doc.is<int>( "stringField" ) );
		EXPECT_FALSE( doc.is<int>( "doubleField" ) );
		EXPECT_FALSE( doc.is<int>( "nonexistent" ) );

		// Test double type checking
		EXPECT_TRUE( doc.is<double>( "doubleField" ) );
		EXPECT_FALSE( doc.is<double>( "intField" ) ); // Note: integers are not floats
		EXPECT_FALSE( doc.is<double>( "stringField" ) );
		EXPECT_FALSE( doc.is<double>( "nonexistent" ) );

		// Test boolean type checking
		EXPECT_TRUE( doc.is<bool>( "boolField" ) );
		EXPECT_FALSE( doc.is<bool>( "stringField" ) );
		EXPECT_FALSE( doc.is<bool>( "intField" ) );
		EXPECT_FALSE( doc.is<bool>( "nonexistent" ) );

		// Test null type checking
		EXPECT_TRUE( doc.isNull( "nullField" ) );
		EXPECT_FALSE( doc.isNull( "stringField" ) );
		EXPECT_FALSE( doc.isNull( "nonexistent" ) );

		// Test object type checking (nested objects)
		EXPECT_TRUE( doc.is<Document::Object>( "nested" ) );
		EXPECT_TRUE( doc.is<Document::Object>( "nested.object" ) );
		EXPECT_FALSE( doc.is<Document::Object>( "nested.object.field" ) ); // This is a string, not object
		EXPECT_FALSE( doc.is<Document::Object>( "stringField" ) );
		EXPECT_FALSE( doc.is<Document::Object>( "nonexistent" ) );

		// Test array type checking
		EXPECT_TRUE( doc.is<Document::Array>( "arrayField" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "stringField" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "nonexistent" ) );
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
		EXPECT_TRUE( doc.is<Document::Object>( "user" ) );

		// String fields
		EXPECT_TRUE( doc.is<std::string>( "user.name" ) );
		EXPECT_TRUE( doc.is<std::string>( "user.preferences.theme" ) );

		// Numeric fields
		EXPECT_TRUE( doc.is<int>( "user.age" ) );
		EXPECT_TRUE( doc.is<double>( "user.height" ) );

		// Boolean field
		EXPECT_TRUE( doc.is<bool>( "user.active" ) );

		// Null field
		EXPECT_TRUE( doc.isNull( "user.spouse" ) );

		// Nested object
		EXPECT_TRUE( doc.is<Document::Object>( "user.preferences" ) );

		// Array field
		EXPECT_TRUE( doc.is<Document::Array>( "user.hobbies" ) );

		// Cross-validation (ensuring fields are not other types)
		EXPECT_FALSE( doc.is<int>( "user.name" ) );
		EXPECT_FALSE( doc.is<std::string>( "user.age" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "user.active" ) );
		EXPECT_FALSE( doc.is<Document::Object>( "user.hobbies" ) ); // Array, not object
	}

	//----------------------------------------------
	// Field operations
	//----------------------------------------------

	TEST( DocumentTest, RemoveField )
	{
		Document doc;
		doc.set<std::string>( "name", "Alice" );
		doc.set<int64_t>( "age", 30 );
		doc.set<std::string>( "user.profile.firstName", "Bob" );
		doc.set<std::string>( "user.profile.lastName", "Smith" );
		doc.set<int64_t>( "user.settings.theme", 1 );

		// Test removing root-level field using Object wrapper
		EXPECT_TRUE( doc.hasValue( "name" ) );
		auto rootObj = doc.get<Document::Object>( "" );
		ASSERT_TRUE( rootObj.has_value() );
		EXPECT_TRUE( rootObj->removeField( "name" ) );
		EXPECT_FALSE( doc.hasValue( "name" ) );

		// Test removing nested field using Object wrapper
		EXPECT_TRUE( doc.hasValue( "user.profile.firstName" ) );
		auto profileObj = doc.get<Document::Object>( "user.profile" );
		ASSERT_TRUE( profileObj.has_value() );
		EXPECT_TRUE( profileObj->removeField( "firstName" ) );
		EXPECT_FALSE( doc.hasValue( "user.profile.firstName" ) );
		EXPECT_TRUE( doc.hasValue( "user.profile.lastName" ) ); // Other fields should remain

		// Test removing non-existent field
		EXPECT_FALSE( rootObj->removeField( "nonexistent" ) );
		auto userObj = doc.get<Document::Object>( "user" );
		ASSERT_TRUE( userObj.has_value() );
		EXPECT_FALSE( userObj->removeField( "nonexistent" ) );

		// Test removing empty path (should fail for field name)
		EXPECT_FALSE( rootObj->removeField( "" ) );

		// Verify remaining structure is intact
		EXPECT_EQ( doc.get<int64_t>( "age" ), 30 );
		EXPECT_EQ( doc.get<std::string>( "user.profile.lastName" ), "Smith" );
		EXPECT_EQ( doc.get<int64_t>( "user.settings.theme" ), 1 );
	}

	//----------------------------------------------
	// Merge / update operations
	//----------------------------------------------

	TEST( DocumentTest, MergeDocuments )
	{
		// Create base document
		Document base;
		base.set<std::string>( "name", "Alice" );
		base.set<int64_t>( "age", 30 );
		base.set<std::string>( "user.profile.city", "New York" );
		auto hobbiesOpt = base.get<Document::Array>( "hobbies" );
		if ( !hobbiesOpt.has_value() )
		{
			base.set<Document::Array>( "hobbies" );
			hobbiesOpt = base.get<Document::Array>( "hobbies" );
		}
		auto hobbies = hobbiesOpt.value();
		hobbies.add<std::string>( "reading" );
		hobbies.add<std::string>( "gaming" );

		// Create merge document
		Document merge;
		merge.set<std::string>( "name", "Bob" );				 // Should overwrite
		merge.set<std::string>( "email", "bob@example.com" );	 // Should add new field
		merge.set<std::string>( "user.profile.country", "USA" ); // Should add to nested object
		merge.set<std::string>( "user.profile.city", "Boston" ); // Should overwrite nested field
		auto mergeHobbiesOpt = merge.get<Document::Array>( "hobbies" );
		if ( !mergeHobbiesOpt.has_value() )
		{
			merge.set<Document::Array>( "hobbies" );
			mergeHobbiesOpt = merge.get<Document::Array>( "hobbies" );
		}
		mergeHobbiesOpt.value().add<std::string>( "hiking" ); // Should merge with existing array

		auto skillsOpt = merge.get<Document::Array>( "skills" );
		if ( !skillsOpt.has_value() )
		{
			merge.set<Document::Array>( "skills" );
			skillsOpt = merge.get<Document::Array>( "skills" );
		}
		skillsOpt.value().add<std::string>( "C++" ); // Should create new array

		// Test merge with array merging (default behavior)
		Document result1 = base;
		result1.merge( merge, false ); // Don't overwrite arrays

		EXPECT_EQ( result1.get<std::string>( "name" ), "Bob" );					  // Overwritten
		EXPECT_EQ( result1.get<std::string>( "email" ), "bob@example.com" );	  // Added
		EXPECT_EQ( result1.get<int64_t>( "age" ), 30 );							  // Preserved
		EXPECT_EQ( result1.get<std::string>( "user.profile.city" ), "Boston" );	  // Overwritten
		EXPECT_EQ( result1.get<std::string>( "user.profile.country" ), "USA" );	  // Added
		EXPECT_EQ( result1.get<Document::Array>( "hobbies" ).value().size(), 3 ); // Merged array
		EXPECT_EQ( result1.get<Document::Array>( "skills" ).value().size(), 1 );  // New array

		// Test merge with array overwriting
		Document result2 = base;
		result2.merge( merge, true ); // Overwrite arrays

		EXPECT_EQ( result2.get<std::string>( "name" ), "Bob" );
		EXPECT_EQ( result2.get<Document::Array>( "hobbies" ).value().size(), 1 ); // Overwritten array
	}

	TEST( DocumentTest, UpdateField )
	{
		Document doc;
		doc.set<std::string>( "name", "Alice" );
		doc.set<int64_t>( "age", 30 );

		// Create update value
		Document updateValue;
		updateValue.set<std::string>( "firstName", "Bob" );
		updateValue.set<std::string>( "lastName", "Smith" );
		updateValue.set<int64_t>( "id", 123 );

		// Update existing field
		doc.update( "name", updateValue );
		EXPECT_FALSE( doc.is<std::string>( "name" ) );	   // Should no longer be a string
		EXPECT_TRUE( doc.is<Document::Object>( "name" ) ); // Should now be an object
		EXPECT_EQ( doc.get<std::string>( "name.firstName" ), "Bob" );
		EXPECT_EQ( doc.get<std::string>( "name.lastName" ), "Smith" );
		EXPECT_EQ( doc.get<int64_t>( "name.id" ), 123 );

		// Update nested path (should create intermediate objects)
		Document nestedUpdate;
		nestedUpdate.set<std::string>( "theme", "dark" );
		nestedUpdate.set<bool>( "notifications", true );

		doc.update( "user.settings", nestedUpdate );
		EXPECT_EQ( doc.get<std::string>( "user.settings.theme" ), "dark" );
		EXPECT_EQ( doc.get<bool>( "user.settings.notifications" ), true );

		// Verify other fields remain unchanged
		EXPECT_EQ( doc.get<int64_t>( "age" ), 30 );
	}

	//----------------------------------------------
	// Validation and error handling
	//----------------------------------------------

	TEST( DocumentTest, PublicValidationMethods )
	{
		Document doc;
		doc.set<std::string>( "validString", "test" );
		auto validArrayOpt = doc.get<Document::Array>( "validArray" );
		if ( !validArrayOpt.has_value() )
		{
			doc.set<Document::Array>( "validArray" );
			validArrayOpt = doc.get<Document::Array>( "validArray" );
		}
		validArrayOpt.value().add<std::string>( "item" );

		// Test public validation methods
		EXPECT_TRUE( doc.isValid() );
		EXPECT_TRUE( doc.hasValue( "validString" ) );
		EXPECT_FALSE( doc.hasValue( "missingField" ) );

		// Test array validation
		EXPECT_TRUE( doc.is<Document::Array>( "validArray" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "validString" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "nonexistent" ) );
	}

	//----------------------------------------------
	// JSON output
	//----------------------------------------------

	TEST( DocumentTest, JsonOutputFormatting )
	{
		Document doc;
		doc.set<std::string>( "name", "Test" );
		doc.set<int64_t>( "value", 123 );

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
		Document doc;
		doc.set<std::string>( "test", "value" );

		std::vector<uint8_t> bytes = doc.toJsonBytes();
		std::string jsonStr = doc.toJsonString( 0 );

		EXPECT_EQ( bytes.size(), jsonStr.length() );
	}

	TEST( DocumentTest, FromJsonBytesValidDocument )
	{
		std::string jsonStr = R"({"name": "Alice", "age": 30, "active": true})";
		std::vector<uint8_t> jsonBytes( jsonStr.begin(), jsonStr.end() );

		auto maybeDoc = Document::fromJsonBytes( jsonBytes );
		ASSERT_TRUE( maybeDoc.has_value() );
		Document doc = maybeDoc.value();

		EXPECT_EQ( doc.get<std::string>( "name" ).value_or( "" ), "Alice" );
		EXPECT_EQ( doc.get<int64_t>( "age" ).value_or( 0 ), 30 );
		EXPECT_EQ( doc.get<bool>( "active" ).value_or( false ), true );
	}

	TEST( DocumentTest, FromJsonBytesInvalidDocument )
	{
		std::string invalidJson = R"({"name": "Alice", "age":})";
		std::vector<uint8_t> jsonBytes( invalidJson.begin(), invalidJson.end() );

		auto maybeDoc = Document::fromJsonBytes( jsonBytes );
		EXPECT_FALSE( maybeDoc.has_value() );
	}

	TEST( DocumentTest, FromJsonBytesEmptyInput )
	{
		std::vector<uint8_t> emptyBytes;
		auto maybeDoc = Document::fromJsonBytes( emptyBytes );
		EXPECT_FALSE( maybeDoc.has_value() );
	}

	TEST( DocumentTest, DocumentRoundtripSerializationBytes )
	{
		// Create a complex document
		Document originalDoc;
		originalDoc.set<std::string>( "name", "Bob" );
		originalDoc.set<int64_t>( "age", 25 );
		originalDoc.set<double>( "height", 1.80 );
		originalDoc.set<bool>( "active", true );
		originalDoc.set<std::string>( "address.city", "Seattle" );
		originalDoc.set<std::string>( "address.country", "USA" );

		// Serialize to bytes
		std::vector<uint8_t> jsonBytes = originalDoc.toJsonBytes();
		EXPECT_FALSE( jsonBytes.empty() );

		// Deserialize back
		auto deserializedDoc = Document::fromJsonBytes( jsonBytes );
		ASSERT_TRUE( deserializedDoc.has_value() );

		// Verify all fields are preserved
		EXPECT_EQ( deserializedDoc.value().get<std::string>( "name" ).value_or( "" ), "Bob" );
		EXPECT_EQ( deserializedDoc.value().get<int64_t>( "age" ).value_or( 0 ), 25 );
		EXPECT_EQ( deserializedDoc.value().get<double>( "height" ).value_or( 0.0 ), 1.80 );
		EXPECT_EQ( deserializedDoc.value().get<bool>( "active" ).value_or( false ), true );
		EXPECT_EQ( deserializedDoc.value().get<std::string>( "address.city" ).value_or( "" ), "Seattle" );
		EXPECT_EQ( deserializedDoc.value().get<std::string>( "address.country" ).value_or( "" ), "USA" );
	}

	TEST( DocumentTest, DocumentRoundtripSerializationString )
	{
		// Test string roundtrip with arrays
		Document originalDoc;
		originalDoc.set<std::string>( "title", "Test Document" );

		// Add array
		auto numbersOpt = originalDoc.get<Document::Array>( "numbers" );
		if ( !numbersOpt.has_value() )
		{
			originalDoc.set<Document::Array>( "numbers" );
			numbersOpt = originalDoc.get<Document::Array>( "numbers" );
		}
		auto numbers = numbersOpt.value();
		numbers.add<int64_t>( 1 );
		numbers.add<int64_t>( 2 );
		numbers.add<int64_t>( 3 );

		// Serialize to string
		std::string jsonStr = originalDoc.toJsonString();
		EXPECT_FALSE( jsonStr.empty() );

		// Deserialize back
		auto deserializedDoc = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( deserializedDoc.has_value() );

		// Verify fields are preserved
		EXPECT_EQ( deserializedDoc.value().get<std::string>( "title" ).value_or( "" ), "Test Document" );

		auto deserializedNumbers = deserializedDoc.value().get<Document::Array>( "numbers" );
		ASSERT_TRUE( deserializedNumbers.has_value() );
		EXPECT_EQ( deserializedNumbers.value().size(), 3 );
		EXPECT_EQ( deserializedNumbers.value().get<int64_t>( 0 ).value_or( 0 ), 1 );
		EXPECT_EQ( deserializedNumbers.value().get<int64_t>( 1 ).value_or( 0 ), 2 );
		EXPECT_EQ( deserializedNumbers.value().get<int64_t>( 2 ).value_or( 0 ), 3 );
	}

	//----------------------------------------------
	// JSON Pointer (RFC 6901)
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerBasicAccess )
	{
		// Create a document structure
		Document doc;
		doc.set<std::string>( "/name", "Alice Johnson" );
		doc.set<int64_t>( "/age", 30 );
		doc.set<double>( "/height", 1.75 );
		doc.set<bool>( "/active", true );
		doc.setNull( "/spouse" );

		// Test basic JSON Pointer access
		EXPECT_EQ( doc.get<std::string>( "/name" ), "Alice Johnson" );
		EXPECT_EQ( doc.get<int64_t>( "/age" ), 30 );
		EXPECT_EQ( doc.get<double>( "/height" ), 1.75 );
		EXPECT_EQ( doc.get<bool>( "/active" ), true );

		// Test field existence
		EXPECT_TRUE( doc.hasValue( "/name" ) );
		EXPECT_TRUE( doc.hasValue( "/age" ) );
		EXPECT_TRUE( doc.hasValue( "/spouse" ) );
		EXPECT_FALSE( doc.hasValue( "/nonexistent" ) );

		// Test root document access
		EXPECT_TRUE( doc.hasValue( "" ) );
	}

	TEST( DocumentTest, JsonPointerNestedObjects )
	{
		Document doc;

		// Create nested structure using JSON Pointers
		doc.set<std::string>( "/user/profile/firstName", "John" );
		doc.set<std::string>( "/user/profile/lastName", "Doe" );
		doc.set<int64_t>( "/user/profile/age", 25 );
		doc.set<std::string>( "/user/settings/theme", "dark" );
		doc.set<bool>( "/user/settings/notifications", false );

		// Verify nested access
		EXPECT_EQ( doc.get<std::string>( "/user/profile/firstName" ), "John" );
		EXPECT_EQ( doc.get<std::string>( "/user/profile/lastName" ), "Doe" );
		EXPECT_EQ( doc.get<int64_t>( "/user/profile/age" ), 25 );
		EXPECT_EQ( doc.get<std::string>( "/user/settings/theme" ), "dark" );
		EXPECT_EQ( doc.get<bool>( "/user/settings/notifications" ), false );

		// Test intermediate path existence
		EXPECT_TRUE( doc.hasValue( "/user" ) );
		EXPECT_TRUE( doc.hasValue( "/user/profile" ) );
		EXPECT_TRUE( doc.hasValue( "/user/settings" ) );
	}

	TEST( DocumentTest, JsonPointerArrayAccess )
	{
		Document doc;

		// Create arrays using JSON Pointers
		doc.set<std::string>( "/users/0/name", "Alice" );
		doc.set<int64_t>( "/users/0/age", 28 );
		doc.set<std::string>( "/users/1/name", "Bob" );
		doc.set<int64_t>( "/users/1/age", 32 );

		doc.set<double>( "/scores/0", 95.5 );
		doc.set<double>( "/scores/1", 87.2 );
		doc.set<double>( "/scores/2", 91.8 );

		// Verify array element access
		EXPECT_EQ( doc.get<std::string>( "/users/0/name" ), "Alice" );
		EXPECT_EQ( doc.get<int64_t>( "/users/0/age" ), 28 );
		EXPECT_EQ( doc.get<std::string>( "/users/1/name" ), "Bob" );
		EXPECT_EQ( doc.get<int64_t>( "/users/1/age" ), 32 );

		EXPECT_EQ( doc.get<double>( "/scores/0" ), 95.5 );
		EXPECT_EQ( doc.get<double>( "/scores/1" ), 87.2 );
		EXPECT_EQ( doc.get<double>( "/scores/2" ), 91.8 );

		// Test field existence using Object API (object fields only)
		auto rootObj = doc.get<Document::Object>( "" );
		ASSERT_TRUE( rootObj.has_value() );
		EXPECT_TRUE( rootObj->hasField( "/users" ) );	 // Field in root object
		EXPECT_FALSE( rootObj->hasField( "/users/0" ) ); // Array element, not field
		EXPECT_FALSE( rootObj->hasField( "/users/1" ) ); // Array element, not field
		EXPECT_TRUE( rootObj->hasField( "/scores" ) );	 // Field in root object

		// Test field existence in nested objects
		auto userObj = doc.get<Document::Object>( "/users/0" );
		ASSERT_TRUE( userObj.has_value() );
		EXPECT_TRUE( userObj->hasField( "name" ) ); // Field in nested object
		EXPECT_TRUE( userObj->hasField( "age" ) );	// Field in nested object

		// Test value existence (any JSON value)
		EXPECT_TRUE( doc.hasValue( "/users" ) );		// Array value
		EXPECT_TRUE( doc.hasValue( "/users/0" ) );		// Array element (object)
		EXPECT_TRUE( doc.hasValue( "/users/1" ) );		// Array element (object)
		EXPECT_TRUE( doc.hasValue( "/scores" ) );		// Array value
		EXPECT_TRUE( doc.hasValue( "/scores/0" ) );		// Array element (primitive)
		EXPECT_TRUE( doc.hasValue( "/users/0/name" ) ); // Object field value
		EXPECT_FALSE( doc.hasValue( "/users/5" ) );		// Non-existent array element
		EXPECT_FALSE( doc.hasValue( "/nonexistent" ) ); // Non-existent field
	}

	TEST( DocumentTest, JsonPointerEscapedCharacters )
	{
		Document doc;

		// Test escaped characters according to RFC 6901
		// ~0 represents ~ and ~1 represents /
		doc.set<std::string>( "/field~1with~0tilde", "value1" );
		doc.set<std::string>( "/normal~1field", "value2" );
		doc.set<int64_t>( "/path~1to~1data", 42 );

		// Verify escaped character handling
		EXPECT_EQ( doc.get<std::string>( "/field~1with~0tilde" ), "value1" );
		EXPECT_EQ( doc.get<std::string>( "/normal~1field" ), "value2" );
		EXPECT_EQ( doc.get<int64_t>( "/path~1to~1data" ), 42 );

		// Verify field existence with escaped names
		EXPECT_TRUE( doc.hasValue( "/field~1with~0tilde" ) );
		EXPECT_TRUE( doc.hasValue( "/normal~1field" ) );
		EXPECT_TRUE( doc.hasValue( "/path~1to~1data" ) );
	}

	TEST( DocumentTest, JsonPointerErrorHandling )
	{
		Document doc;
		doc.set<std::string>( "/existing/field", "value" );

		// Test invalid pointers
		EXPECT_FALSE( doc.hasValue( "invalid" ) ); // Must start with /
		EXPECT_FALSE( doc.hasValue( "/nonexistent/field" ) );

		// Test type mismatches
		EXPECT_FALSE( doc.get<int64_t>( "/existing/field" ).has_value() ); // string accessed as int
		EXPECT_FALSE( doc.get<bool>( "/existing/field" ).has_value() );	   // string accessed as bool

		// Test array index errors
		doc.set<int64_t>( "/numbers/0", 10 );
		doc.set<int64_t>( "/numbers/1", 20 );

		EXPECT_FALSE( doc.hasValue( "/numbers/5" ) ); // Out of bounds
		EXPECT_FALSE( doc.get<int64_t>( "/numbers/5" ).has_value() );

		// Test invalid array indices
		EXPECT_FALSE( doc.hasValue( "/numbers/01" ) );	// Leading zero not allowed
		EXPECT_FALSE( doc.hasValue( "/numbers/abc" ) ); // Non-numeric
	}

	TEST( DocumentTest, HasFieldVsHasValueNewAPI )
	{
		// Create a document with mixed object and array structure
		auto docOpt = Document::fromJsonString( R"({
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
		ASSERT_TRUE( docOpt.has_value() );
		Document& doc = docOpt.value();

		// Object fields - use Object API
		auto rootObj = doc.get<Document::Object>( "" );
		ASSERT_TRUE( rootObj.has_value() );
		EXPECT_TRUE( rootObj->hasField( "/users" ) );	 // Field in root object
		EXPECT_TRUE( rootObj->hasField( "/count" ) );	 // Field in root object
		EXPECT_TRUE( rootObj->hasField( "/settings" ) ); // Field in root object

		// Nested object fields - use Object API
		auto settingsObj = doc.get<Document::Object>( "/settings" );
		ASSERT_TRUE( settingsObj.has_value() );
		EXPECT_TRUE( settingsObj->hasField( "theme" ) );		 // Field in nested object
		EXPECT_TRUE( settingsObj->hasField( "notifications" ) ); // Field in nested object

		auto userObj = doc.get<Document::Object>( "/users/0" );
		ASSERT_TRUE( userObj.has_value() );
		EXPECT_TRUE( userObj->hasField( "name" ) ); // Field in array element object
		EXPECT_TRUE( userObj->hasField( "age" ) );	// Field in array element object

		// Array elements - use Array API
		auto usersArray = doc.get<Document::Array>( "/users" );
		ASSERT_TRUE( usersArray.has_value() );
		EXPECT_TRUE( usersArray->hasElement( "0" ) );  // Array element exists
		EXPECT_TRUE( usersArray->hasElement( "1" ) );  // Array element exists
		EXPECT_FALSE( usersArray->hasElement( "5" ) ); // Out of bounds

		// Universal value existence - use Document API
		EXPECT_TRUE( doc.hasValue( "/users" ) );		  // Array value exists
		EXPECT_TRUE( doc.hasValue( "/users/0" ) );		  // Array element exists
		EXPECT_TRUE( doc.hasValue( "/users/1" ) );		  // Array element exists
		EXPECT_TRUE( doc.hasValue( "/settings/theme" ) ); // Nested field value
		EXPECT_TRUE( doc.hasValue( "/users/0/name" ) );	  // Field in array element

		// Non-existent paths should return false
		EXPECT_FALSE( rootObj->hasField( "/nonexistent" ) );
		EXPECT_FALSE( doc.hasValue( "/nonexistent" ) );
		EXPECT_FALSE( usersArray->hasElement( "5" ) );		// Out of bounds
		EXPECT_FALSE( doc.hasValue( "/users/5" ) );			// Out of bounds
		EXPECT_FALSE( userObj->hasField( "invalid" ) );		// Non-existent field
		EXPECT_FALSE( doc.hasValue( "/users/0/invalid" ) ); // Non-existent field

		// Test with array at root
		auto arrayDocOpt = Document::fromJsonString( R"([1, 2, {"key": "value"}])" );
		ASSERT_TRUE( arrayDocOpt.has_value() );
		Document& arrayDoc = arrayDocOpt.value();

		auto rootArray = arrayDoc.get<Document::Array>( "" );
		ASSERT_TRUE( rootArray.has_value() );
		EXPECT_TRUE( rootArray->hasElement( "0" ) );  // Array element exists
		EXPECT_TRUE( rootArray->hasElement( "1" ) );  // Array element exists
		EXPECT_TRUE( rootArray->hasElement( "2" ) );  // Array element exists
		EXPECT_TRUE( arrayDoc.hasValue( "/0" ) );	  // Array element exists
		EXPECT_TRUE( arrayDoc.hasValue( "/2/key" ) ); // Field within object in array

		auto objInArray = arrayDoc.get<Document::Object>( "/2" );
		ASSERT_TRUE( objInArray.has_value() );
		EXPECT_TRUE( objInArray->hasField( "key" ) ); // Field within object in array
	}

	TEST( DocumentTest, TypeSpecificMethods )
	{
		// Create a comprehensive test document with all JSON types
		auto docOpt = Document::fromJsonString( R"({
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
		ASSERT_TRUE( docOpt.has_value() );
		Document& doc = docOpt.value();

		// Test is<std::string>
		EXPECT_TRUE( doc.is<std::string>( "/stringField" ) );
		EXPECT_TRUE( doc.is<std::string>( "/objectField/nested" ) );
		EXPECT_TRUE( doc.is<std::string>( "/mixedArray/0" ) );
		EXPECT_FALSE( doc.is<std::string>( "/intField" ) );	   // Not a string
		EXPECT_FALSE( doc.is<std::string>( "/arrayField" ) );  // Not a string
		EXPECT_FALSE( doc.is<std::string>( "/nonexistent" ) ); // Doesn't exist

		// Test is<int>
		EXPECT_TRUE( doc.is<int>( "/intField" ) );
		EXPECT_TRUE( doc.is<int>( "/arrayField/0" ) );
		EXPECT_TRUE( doc.is<int>( "/mixedArray/1" ) );
		EXPECT_FALSE( doc.is<int>( "/doubleField" ) ); // Not an int
		EXPECT_FALSE( doc.is<int>( "/stringField" ) ); // Not an int
		EXPECT_FALSE( doc.is<int>( "/nonexistent" ) ); // Doesn't exist

		// Test is<double>
		EXPECT_TRUE( doc.is<double>( "/doubleField" ) );
		EXPECT_TRUE( doc.is<double>( "/mixedArray/2" ) );
		EXPECT_FALSE( doc.is<double>( "/intField" ) );	  // Not a double
		EXPECT_FALSE( doc.is<double>( "/stringField" ) ); // Not a double
		EXPECT_FALSE( doc.is<double>( "/nonexistent" ) ); // Doesn't exist

		// Test is<bool>
		EXPECT_TRUE( doc.is<bool>( "/boolField" ) );
		EXPECT_TRUE( doc.is<bool>( "/mixedArray/3" ) );
		EXPECT_FALSE( doc.is<bool>( "/stringField" ) ); // Not a bool
		EXPECT_FALSE( doc.is<bool>( "/intField" ) );	// Not a bool
		EXPECT_FALSE( doc.is<bool>( "/nonexistent" ) ); // Doesn't exist

		// Test isNull
		EXPECT_TRUE( doc.isNull( "/nullField" ) );
		EXPECT_TRUE( doc.isNull( "/mixedArray/4" ) );
		EXPECT_FALSE( doc.isNull( "/stringField" ) ); // Not null
		EXPECT_FALSE( doc.isNull( "/intField" ) );	  // Not null
		EXPECT_FALSE( doc.isNull( "/nonexistent" ) ); // Doesn't exist (different from null)

		// Test is<Document::Object>
		EXPECT_TRUE( doc.is<Document::Object>( "/objectField" ) );
		EXPECT_TRUE( doc.is<Document::Object>( "/mixedArray/5" ) );
		EXPECT_TRUE( doc.is<Document::Object>( "" ) );				// Root is object
		EXPECT_FALSE( doc.is<Document::Object>( "/arrayField" ) );	// Not an object
		EXPECT_FALSE( doc.is<Document::Object>( "/stringField" ) ); // Not an object
		EXPECT_FALSE( doc.is<Document::Object>( "/nonexistent" ) ); // Doesn't exist

		// Test is<Document::Array>
		EXPECT_TRUE( doc.is<Document::Array>( "/arrayField" ) );
		EXPECT_TRUE( doc.is<Document::Array>( "/mixedArray" ) );
		EXPECT_TRUE( doc.is<Document::Array>( "/mixedArray/6" ) );
		EXPECT_FALSE( doc.is<Document::Array>( "/objectField" ) ); // Not an array
		EXPECT_FALSE( doc.is<Document::Array>( "/stringField" ) ); // Not an array
		EXPECT_FALSE( doc.is<Document::Array>( "" ) );			   // Root is object, not array
		EXPECT_FALSE( doc.is<Document::Array>( "/nonexistent" ) ); // Doesn't exist

		// Test consistency with corresponding get methods
		// If type check returns true, get method should return a value
		if ( doc.is<std::string>( "/stringField" ) )
		{
			EXPECT_TRUE( doc.get<std::string>( "/stringField" ).has_value() );
		}
		if ( doc.is<int>( "/intField" ) )
		{
			EXPECT_TRUE( doc.get<int64_t>( "/intField" ).has_value() );
		}
		if ( doc.is<double>( "/doubleField" ) )
		{
			EXPECT_TRUE( doc.get<double>( "/doubleField" ).has_value() );
		}
		if ( doc.is<bool>( "/boolField" ) )
		{
			EXPECT_TRUE( doc.get<bool>( "/boolField" ).has_value() );
		}
		if ( doc.is<Document::Array>( "/arrayField" ) )
		{
			EXPECT_TRUE( doc.get<Document>( "/arrayField" ).has_value() );
		}
		if ( doc.is<Document::Object>( "/objectField" ) )
		{
			EXPECT_TRUE( doc.get<Document>( "/objectField" ).has_value() );
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

		EXPECT_TRUE( arrayDoc->is<Document::Array>( "" ) );	  // Root is array
		EXPECT_FALSE( arrayDoc->is<Document::Object>( "" ) ); // Root is not object
		EXPECT_TRUE( arrayDoc->is<std::string>( "/0" ) );
		EXPECT_TRUE( arrayDoc->is<int>( "/1" ) );
		EXPECT_TRUE( arrayDoc->is<double>( "/2" ) );
		EXPECT_TRUE( arrayDoc->is<bool>( "/3" ) );
		EXPECT_TRUE( arrayDoc->isNull( "/4" ) );
		EXPECT_TRUE( arrayDoc->is<Document::Object>( "/5" ) );
		EXPECT_TRUE( arrayDoc->is<Document::Array>( "/6" ) );
	}

	TEST( DocumentTest, JsonPointerCompatibilityWithDotNotation )
	{
		Document doc;

		// Set values using dot notation
		doc.set<std::string>( "user.name", "Alice" );
		doc.set<int64_t>( "user.age", 25 );
		doc.set<std::string>( "settings.theme", "dark" );

		// Access same values using JSON Pointer
		EXPECT_EQ( doc.get<std::string>( "/user/name" ), "Alice" );
		EXPECT_EQ( doc.get<int64_t>( "/user/age" ), 25 );
		EXPECT_EQ( doc.get<std::string>( "/settings/theme" ), "dark" );

		// Set values using JSON Pointer
		doc.set<std::string>( "/profile/email", "alice@example.com" );
		doc.set<bool>( "/profile/verified", true );

		// Access same values using dot notation
		EXPECT_EQ( doc.get<std::string>( "profile.email" ), "alice@example.com" );
		EXPECT_EQ( doc.get<bool>( "profile.verified" ), true );

		// Both notations should see the same structure
		EXPECT_TRUE( doc.hasValue( "user.name" ) );
		EXPECT_TRUE( doc.hasValue( "/user/name" ) );
		EXPECT_TRUE( doc.hasValue( "profile.email" ) );
		EXPECT_TRUE( doc.hasValue( "/profile/email" ) );
	}

	TEST( DocumentTest, JsonPointerComplexDocument )
	{
		// Test with a complex, realistic JSON structure
		Document doc;

		// API response structure
		doc.set<std::string>( "/status", "success" );
		doc.set<int64_t>( "/code", 200 );
		doc.set<std::string>( "/data/user/id", "12345" );
		doc.set<std::string>( "/data/user/profile/name", "Jane Smith" );
		doc.set<std::string>( "/data/user/profile/email", "jane@example.com" );
		doc.set<bool>( "/data/user/profile/verified", true );

		// Array of permissions
		doc.set<std::string>( "/data/permissions/0/resource", "users" );
		doc.set<std::string>( "/data/permissions/0/action", "read" );
		doc.set<std::string>( "/data/permissions/1/resource", "posts" );
		doc.set<std::string>( "/data/permissions/1/action", "write" );
		doc.set<std::string>( "/data/permissions/2/resource", "admin" );
		doc.set<std::string>( "/data/permissions/2/action", "manage" );

		// Metadata
		doc.set<std::string>( "/metadata/timestamp", "2025-10-03T14:30:00Z" );
		doc.set<double>( "/metadata/version", 2.1 );

		// Verify the entire structure
		EXPECT_EQ( doc.get<std::string>( "/status" ), "success" );
		EXPECT_EQ( doc.get<int64_t>( "/code" ), 200 );
		EXPECT_EQ( doc.get<std::string>( "/data/user/profile/name" ), "Jane Smith" );
		EXPECT_EQ( doc.get<bool>( "/data/user/profile/verified" ), true );

		EXPECT_EQ( doc.get<std::string>( "/data/permissions/0/resource" ), "users" );
		EXPECT_EQ( doc.get<std::string>( "/data/permissions/1/action" ), "write" );
		EXPECT_EQ( doc.get<std::string>( "/data/permissions/2/resource" ), "admin" );

		EXPECT_EQ( doc.get<double>( "/metadata/version" ), 2.1 );

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
		Document doc;

		// Create an array with mixed types
		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		auto arrayOpt = arrayDoc.get<Document::Array>( "" );
		if ( arrayOpt.has_value() )
		{
			auto array = arrayOpt.value();
			array.add<std::string>( "first" );
			array.add<std::string>( "second" );
			array.add<std::string>( "third" );
		}

		// Set the array using set<Document>
		doc.set<Document>( "/hobbies", arrayDoc );

		// Verify the array was set correctly
		EXPECT_TRUE( doc.hasValue( "/hobbies" ) );
		EXPECT_TRUE( doc.is<Document::Array>( "hobbies" ) );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().size(), 3 );

		// Get the array using get<Document>
		auto retrievedArray = doc.get<Document>( "/hobbies" );
		ASSERT_TRUE( retrievedArray.has_value() );
		// Get Array wrapper to access size
		auto arrayWrapper = retrievedArray->get<Document::Array>( "" );
		ASSERT_TRUE( arrayWrapper.has_value() );
		EXPECT_EQ( arrayWrapper->size(), 3 );

		// Verify individual elements through regular array access
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 0 ).value_or( "" ), "first" );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 1 ).value_or( "" ), "second" );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 2 ).value_or( "" ), "third" );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_NestedArrays )
	{
		Document doc;

		// Create a nested structure with arrays
		Document numbersArray;
		numbersArray.set<Document::Array>( "" );

		auto numbersOpt = numbersArray.get<Document::Array>( "" );
		if ( numbersOpt.has_value() )
		{
			auto numbers = numbersOpt.value();
			numbers.add<int64_t>( 10 );
			numbers.add<int64_t>( 20 );
			numbers.add<int64_t>( 30 );
		}

		Document stringsArray;
		stringsArray.set<Document::Array>( "" );

		auto stringsOpt = stringsArray.get<Document::Array>( "" );
		if ( stringsOpt.has_value() )
		{
			auto strings = stringsOpt.value();
			strings.add<std::string>( "alpha" );
			strings.add<std::string>( "beta" );
		}

		// Set arrays at nested paths
		doc.set<Document>( "/data/numbers", numbersArray );
		doc.set<Document>( "/data/strings", stringsArray );

		// Verify both arrays exist
		EXPECT_TRUE( doc.hasValue( "/data/numbers" ) );
		EXPECT_TRUE( doc.hasValue( "/data/strings" ) );

		// Get and verify the arrays
		auto retrievedNumbers = doc.get<Document>( "/data/numbers" );
		auto retrievedStrings = doc.get<Document>( "/data/strings" );

		ASSERT_TRUE( retrievedNumbers.has_value() );
		ASSERT_TRUE( retrievedStrings.has_value() );

		// Get Array wrappers to access size
		auto numbersArrayWrapper = retrievedNumbers->get<Document::Array>( "" );
		ASSERT_TRUE( numbersArrayWrapper.has_value() );
		EXPECT_EQ( numbersArrayWrapper->size(), 3 );
		auto stringsArrayWrapper = retrievedStrings->get<Document::Array>( "" );
		ASSERT_TRUE( stringsArrayWrapper.has_value() );
		EXPECT_EQ( stringsArrayWrapper->size(), 2 );

		// Verify content through direct JSON Pointer access
		EXPECT_EQ( doc.get<int64_t>( "/data/numbers/0" ), 10 );
		EXPECT_EQ( doc.get<std::string>( "/data/strings/1" ), "beta" );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_EmptyArray )
	{
		Document doc;

		// Create and set an empty array
		Document emptyArray;
		emptyArray.set<Document::Array>( "" );

		doc.set<Document>( "/empty", emptyArray );

		// Verify empty array handling
		EXPECT_TRUE( doc.hasValue( "/empty" ) );
		EXPECT_TRUE( doc.is<Document::Array>( "empty" ) );
		EXPECT_EQ( doc.get<Document::Array>( "empty" ).value().size(), 0 );

		auto retrievedEmpty = doc.get<Document>( "/empty" );
		ASSERT_TRUE( retrievedEmpty.has_value() );
		// Get Array wrapper to access size
		auto emptyArrayWrapper = retrievedEmpty->get<Document::Array>( "" );
		ASSERT_TRUE( emptyArrayWrapper.has_value() );
		EXPECT_EQ( emptyArrayWrapper->size(), 0 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ReplaceExistingArray )
	{
		Document doc;

		// Create initial array
		Document originalArray;
		originalArray.set<Document::Array>( "" );

		auto originalArrOpt = originalArray.get<Document::Array>( "" );
		if ( originalArrOpt.has_value() )
		{
			auto originalArr = originalArrOpt.value();
			originalArr.add<std::string>( "old1" );
			originalArr.add<std::string>( "old2" );
		}
		doc.set<Document>( "/items", originalArray );

		// Verify original array
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().size(), 2 );
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().get<std::string>( 0 ).value_or( "" ), "old1" );

		// Replace with new array
		Document newArray;
		newArray.set<Document::Array>( "" );

		auto newArrOpt = newArray.get<Document::Array>( "" );
		if ( newArrOpt.has_value() )
		{
			auto newArr = newArrOpt.value();
			newArr.add<std::string>( "new1" );
			newArr.add<std::string>( "new2" );
			newArr.add<std::string>( "new3" );
		}
		doc.set<Document>( "/items", newArray );

		// Verify replacement
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().size(), 3 );
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().get<std::string>( 0 ).value_or( "" ), "new1" );
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().get<std::string>( 2 ).value_or( "" ), "new3" );

		auto retrievedNew = doc.get<Document>( "/items" );
		ASSERT_TRUE( retrievedNew.has_value() );
		// Get Array wrapper to access size
		auto newArrayWrapper = retrievedNew->get<Document::Array>( "" );
		ASSERT_TRUE( newArrayWrapper.has_value() );
		EXPECT_EQ( newArrayWrapper->size(), 3 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ErrorHandling )
	{
		Document doc;
		doc.set<std::string>( "notArray", "this is a string" );

		// Try to get array from non-array field
		auto result = doc.get<Document::Array>( "/notArray" );
		EXPECT_FALSE( result.has_value() );

		// Try to get array from non-existent path
		auto nonExistent = doc.get<Document>( "/nonExistent" );
		EXPECT_FALSE( nonExistent.has_value() );

		// Try to get array with invalid pointer
		auto invalid = doc.get<Document>( "/invalid/deep/path" );
		EXPECT_FALSE( invalid.has_value() );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_ComplexArrayWithObjects )
	{
		Document doc;

		Document objectArray;
		objectArray.set<Document::Array>( "" );

		// Add first object to array
		Document obj1;

		obj1.set<std::string>( "name", "Alice" );
		obj1.set<int64_t>( "age", 30 );

		// Add second object to array
		Document obj2;

		obj2.set<std::string>( "name", "Bob" );
		obj2.set<int64_t>( "age", 25 );

		// Get array wrapper and add documents
		auto arrayWrapper = objectArray.get<Document::Array>( "" );
		ASSERT_TRUE( arrayWrapper.has_value() );
		arrayWrapper->add<Document>( obj1 );
		arrayWrapper->add<Document>( obj2 );

		// Set the complex array
		doc.set<Document>( "/users", objectArray );

		// Verify the complex array was set
		EXPECT_TRUE( doc.hasValue( "/users" ) );
		EXPECT_TRUE( doc.is<Document::Array>( "users" ) );
		EXPECT_EQ( doc.get<Document::Array>( "users" ).value().size(), 2 );

		// Get and verify the complex array
		auto retrievedUsers = doc.get<Document>( "/users" );
		ASSERT_TRUE( retrievedUsers.has_value() );
		// Get Array wrapper to access size
		auto usersArrayWrapper = retrievedUsers->get<Document::Array>( "" );
		ASSERT_TRUE( usersArrayWrapper.has_value() );
		EXPECT_EQ( usersArrayWrapper->size(), 2 );

		// Verify we can access nested object data through JSON Pointers
		EXPECT_EQ( doc.get<std::string>( "/users/0/name" ), "Alice" );
		EXPECT_EQ( doc.get<int64_t>( "/users/0/age" ), 30 );
		EXPECT_EQ( doc.get<std::string>( "/users/1/name" ), "Bob" );
		EXPECT_EQ( doc.get<int64_t>( "/users/1/age" ), 25 );
	}

	TEST( DocumentTest, JsonPointerArrayMethods_RoundtripSerialization )
	{
		Document original;

		// Create a mixed array
		Document mixedArray;
		mixedArray.set<Document::Array>( "" );

		auto mixedArrOpt = mixedArray.get<Document::Array>( "" );
		if ( mixedArrOpt.has_value() )
		{
			auto mixedArr = mixedArrOpt.value();
			mixedArr.add<std::string>( "string_value" );
			mixedArr.add<int64_t>( 42 );
			mixedArr.add<double>( 3.14 );
			mixedArr.add<bool>( true );
		}

		original.set<Document>( "/mixed", mixedArray );

		// Serialize and deserialize
		std::string jsonString = original.toJsonString();
		auto deserialized = Document::fromJsonString( jsonString );

		ASSERT_TRUE( deserialized.has_value() );

		// Verify the array survived serialization
		auto deserializedArray = deserialized->get<Document>( "/mixed" );
		ASSERT_TRUE( deserializedArray.has_value() );
		// Get Array wrapper to access size
		auto deserializedArrayWrapper = deserializedArray->get<Document::Array>( "" );
		ASSERT_TRUE( deserializedArrayWrapper.has_value() );
		EXPECT_EQ( deserializedArrayWrapper->size(), 4 );

		// Verify individual elements
		EXPECT_EQ( deserialized->get<Document::Array>( "mixed" ).value().get<std::string>( 0 ).value_or( "" ), "string_value" );
		EXPECT_EQ( deserialized->get<Document::Array>( "mixed" ).value().get<int64_t>( 1 ).value_or( 0 ), 42 );
		EXPECT_EQ( deserialized->get<Document::Array>( "mixed" ).value().get<double>( 2 ).value_or( 0.0 ), 3.14 );
		EXPECT_EQ( deserialized->get<Document::Array>( "mixed" ).value().get<bool>( 3 ).value_or( false ), true );
	}

	//----------------------------------------------
	// Generic Document Pointer methods tests
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerGenericMethods_BasicSetAndGet )
	{
		Document doc;

		// Test setting primitives with generic method
		Document stringDoc;

		stringDoc.set<std::string>( "", "test string" );
		doc.set<Document>( "/text", stringDoc );

		Document numberDoc;

		numberDoc.set<int64_t>( "", 42 );
		doc.set<Document>( "/number", numberDoc );

		Document boolDoc;
		boolDoc.set<bool>( "", true );
		doc.set<Document>( "/flag", boolDoc );

		// Test getting with generic method
		auto retrievedText = doc.get<Document>( "/text" );
		auto retrievedNumber = doc.get<Document>( "/number" );
		auto retrievedFlag = doc.get<Document>( "/flag" );

		ASSERT_TRUE( retrievedText.has_value() );
		ASSERT_TRUE( retrievedNumber.has_value() );
		ASSERT_TRUE( retrievedFlag.has_value() );

		EXPECT_EQ( retrievedText->get<std::string>( "" ), "test string" );
		EXPECT_EQ( retrievedNumber->get<int64_t>( "" ), 42 );
		EXPECT_EQ( retrievedFlag->get<bool>( "" ), true );
	}

	TEST( DocumentTest, JsonPointerGenericMethods_ArraysAndObjects )
	{
		Document doc;

		// Create an array
		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		auto arrayDocArrOpt = arrayDoc.get<Document::Array>( "" );
		if ( arrayDocArrOpt.has_value() )
		{
			auto arrayDocArr = arrayDocArrOpt.value();
			arrayDocArr.add<std::string>( "item1" );
			arrayDocArr.add<std::string>( "item2" );
			arrayDocArr.add<std::string>( "item3" );
		}

		// Create an object
		Document objectDoc;
		objectDoc.set<std::string>( "/name", "Test Object" );
		objectDoc.set<int64_t>( "/value", 100 );

		// Set using generic method
		doc.set<Document>( "/data/items", arrayDoc );
		doc.set<Document>( "/data/config", objectDoc );

		// Get using generic method
		auto retrievedArray = doc.get<Document>( "/data/items" );
		auto retrievedObject = doc.get<Document>( "/data/config" );

		ASSERT_TRUE( retrievedArray.has_value() );
		ASSERT_TRUE( retrievedObject.has_value() );

		// Verify array content
		auto arrayWrapper = retrievedArray->get<Document::Array>( "" );
		ASSERT_TRUE( arrayWrapper.has_value() );
		EXPECT_EQ( arrayWrapper->size(), 3 );
		EXPECT_TRUE( retrievedArray->is<Document::Array>( "" ) );

		// Verify object content
		EXPECT_EQ( retrievedObject->get<std::string>( "/name" ), "Test Object" );
		EXPECT_EQ( retrievedObject->get<int64_t>( "/value" ), 100 );
		EXPECT_TRUE( retrievedObject->is<Document::Object>( "" ) );

		// Also verify type-specific getters still work
		auto typedArray = doc.get<Document>( "/data/items" );
		auto typedObject = doc.get<Document>( "/data/config" );

		ASSERT_TRUE( typedArray.has_value() );
		ASSERT_TRUE( typedObject.has_value() );
	}

	TEST( DocumentTest, JsonPointerGenericMethods_TypeSafetyComparison )
	{
		Document doc;

		// Create array and object
		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		auto arrayDocOpt = arrayDoc.get<Document::Array>( "" );
		if ( arrayDocOpt.has_value() )
		{
			arrayDocOpt.value().add<std::string>( "test" );
		}

		Document objectDoc;
		objectDoc.set<std::string>( "/key", "value" );

		doc.set<Document>( "/myarray", arrayDoc );
		doc.set<Document>( "/myobject", objectDoc );

		// Generic getter returns both
		EXPECT_TRUE( doc.get<Document>( "/myarray" ).has_value() );
		EXPECT_TRUE( doc.get<Document>( "/myobject" ).has_value() );

		// Type-specific getters are selective
		EXPECT_TRUE( doc.get<Document::Array>( "/myarray" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Array>( "/myobject" ).has_value() ); // Object, not array

		EXPECT_TRUE( doc.get<Document::Object>( "/myobject" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Object>( "/myarray" ).has_value() ); // Array, not object
	}

	TEST( DocumentTest, JsonPointerGenericMethods_ErrorHandling )
	{
		Document doc;
		doc.set<std::string>( "/test", "value" );

		// Non-existent paths return nullopt
		EXPECT_FALSE( doc.get<Document>( "/nonexistent" ).has_value() );
		EXPECT_FALSE( doc.get<Document>( "/test/nested" ).has_value() );

		// Empty and root paths
		auto rootDoc = doc.get<Document>( "" );
		EXPECT_TRUE( rootDoc.has_value() );
		EXPECT_TRUE( rootDoc->is<Document::Object>( "" ) );
	}

	//----------------------------------------------
	// Object Pointer array methods tests
	//----------------------------------------------

	TEST( DocumentTest, JsonPointerObjectMethods_BasicSetAndGet )
	{
		Document doc;

		// Create a nested object
		Document profileObj;

		profileObj.set<std::string>( "/name", "John Doe" );
		profileObj.set<int64_t>( "/age", 30 );
		profileObj.set<bool>( "/active", true );

		// Set the object using JSON Pointer
		doc.set<Document>( "/profile", profileObj );

		// Verify the object was set
		EXPECT_TRUE( doc.hasValue( "/profile" ) );
		EXPECT_TRUE( doc.hasValue( "/profile/name" ) );
		EXPECT_TRUE( doc.hasValue( "/profile/age" ) );
		EXPECT_TRUE( doc.hasValue( "/profile/active" ) );

		// Get the object back
		auto retrievedProfile = doc.get<Document>( "/profile" );
		ASSERT_TRUE( retrievedProfile.has_value() );

		// Verify the content
		EXPECT_EQ( retrievedProfile->get<std::string>( "/name" ), "John Doe" );
		EXPECT_EQ( retrievedProfile->get<int64_t>( "/age" ), 30 );
		EXPECT_EQ( retrievedProfile->get<bool>( "/active" ), true );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_NestedObjects )
	{
		Document doc;

		// Create deeply nested structure
		Document addressObj;

		addressObj.set<std::string>( "/street", "123 Main St" );
		addressObj.set<std::string>( "/city", "Anytown" );
		addressObj.set<int64_t>( "/zipcode", 12345 );

		Document userObj;

		userObj.set<std::string>( "/name", "Jane Smith" );
		userObj.set<int64_t>( "/id", 456 );
		userObj.set<Document>( "/address", addressObj );

		// Set the nested user object
		doc.set<Document>( "/user", userObj );

		// Verify nested access
		EXPECT_TRUE( doc.hasValue( "/user/name" ) );
		EXPECT_TRUE( doc.hasValue( "/user/address/street" ) );
		EXPECT_TRUE( doc.hasValue( "/user/address/city" ) );

		// Retrieve and verify nested object
		auto retrievedUser = doc.get<Document>( "/user" );
		ASSERT_TRUE( retrievedUser.has_value() );

		auto retrievedAddress = retrievedUser->get<Document>( "/address" );
		ASSERT_TRUE( retrievedAddress.has_value() );

		EXPECT_EQ( retrievedAddress->get<std::string>( "/street" ), "123 Main St" );
		EXPECT_EQ( retrievedAddress->get<std::string>( "/city" ), "Anytown" );
		EXPECT_EQ( retrievedAddress->get<int64_t>( "/zipcode" ), 12345 );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ErrorHandling )
	{
		Document doc;
		doc.set<std::string>( "/name", "Test" );
		doc.set<int64_t>( "/value", 42 );

		// Try to get object from non-object fields
		EXPECT_FALSE( doc.get<Document::Object>( "/name" ).has_value() );
		EXPECT_FALSE( doc.get<Document::Object>( "/value" ).has_value() );

		// Try to get non-existent object
		EXPECT_FALSE( doc.get<Document::Object>( "/nonexistent" ).has_value() );

		// Try to get object from array
		Document arrayDoc;
		arrayDoc.set<Document::Array>( "" );

		auto arrayDocOpt2 = arrayDoc.get<Document::Array>( "" );
		if ( arrayDocOpt2.has_value() )
		{
			arrayDocOpt2.value().add<std::string>( "item1" );
		}
		doc.set<Document>( "/items", arrayDoc );

		EXPECT_FALSE( doc.get<Document::Object>( "/items" ).has_value() );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ReplaceExistingObject )
	{
		Document doc;

		// Set initial object
		Document obj1;

		obj1.set<std::string>( "/type", "original" );
		obj1.set<int64_t>( "/version", 1 );
		doc.set<Document>( "/config", obj1 );

		// Verify initial state
		auto retrieved1 = doc.get<Document>( "/config" );
		ASSERT_TRUE( retrieved1.has_value() );
		EXPECT_EQ( retrieved1->get<std::string>( "/type" ), "original" );
		EXPECT_EQ( retrieved1->get<int64_t>( "/version" ), 1 );

		// Replace with new object
		Document obj2;

		obj2.set<std::string>( "/type", "updated" );
		obj2.set<int64_t>( "/version", 2 );
		obj2.set<bool>( "/active", true );
		doc.set<Document>( "/config", obj2 );

		// Verify replacement
		auto retrieved2 = doc.get<Document>( "/config" );
		ASSERT_TRUE( retrieved2.has_value() );
		EXPECT_EQ( retrieved2->get<std::string>( "/type" ), "updated" );
		EXPECT_EQ( retrieved2->get<int64_t>( "/version" ), 2 );
		EXPECT_EQ( retrieved2->get<bool>( "/active" ), true );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_ComplexObjectWithArrays )
	{
		Document doc;

		// Create object with mixed content including arrays
		Document complexObj;

		complexObj.set<std::string>( "/title", "Complex Object" );

		// Add an array to the object
		Document tagsArray;
		tagsArray.set<Document::Array>( "" );

		auto tagsOpt = tagsArray.get<Document::Array>( "" );
		if ( tagsOpt.has_value() )
		{
			auto tags = tagsOpt.value();
			tags.add<std::string>( "tag1" );
			tags.add<std::string>( "tag2" );
			tags.add<std::string>( "tag3" );
		}
		complexObj.set<Document>( "/tags", tagsArray );

		// Add a nested object
		Document metaObj;

		metaObj.set<std::string>( "/author", "Test Author" );
		metaObj.set<int64_t>( "/created", 1234567890 );
		complexObj.set<Document>( "/metadata", metaObj );

		// Set the complex object
		doc.set<Document>( "/data", complexObj );

		// Retrieve and verify
		auto retrieved = doc.get<Document>( "/data" );
		ASSERT_TRUE( retrieved.has_value() );

		EXPECT_EQ( retrieved->get<std::string>( "/title" ), "Complex Object" );

		auto retrievedTags = retrieved->get<Document>( "/tags" );
		ASSERT_TRUE( retrievedTags.has_value() );
		// Get Array wrapper to access size
		auto tagsArrayWrapper = retrievedTags->get<Document::Array>( "" );
		ASSERT_TRUE( tagsArrayWrapper.has_value() );
		EXPECT_EQ( tagsArrayWrapper->size(), 3 );

		auto retrievedMeta = retrieved->get<Document>( "/metadata" );
		ASSERT_TRUE( retrievedMeta.has_value() );
		EXPECT_EQ( retrievedMeta->get<std::string>( "/author" ), "Test Author" );
		EXPECT_EQ( retrievedMeta->get<int64_t>( "/created" ), 1234567890 );
	}

	TEST( DocumentTest, JsonPointerObjectMethods_RoundtripSerialization )
	{
		Document original;

		// Create a complex nested structure
		Document userObj;

		userObj.set<std::string>( "/username", "testuser" );
		userObj.set<int64_t>( "/userId", 12345 );

		Document prefsObj;

		prefsObj.set<bool>( "/emailNotifications", true );
		prefsObj.set<std::string>( "/theme", "dark" );
		userObj.set<Document>( "/preferences", prefsObj );

		original.set<Document>( "/user", userObj );

		// Serialize to JSON string
		std::string jsonStr = original.toJsonString();

		// Deserialize back
		auto deserialized = Document::fromJsonString( jsonStr );
		ASSERT_TRUE( deserialized.has_value() );

		// Verify the object survived serialization
		auto deserializedUser = deserialized->get<Document>( "/user" );
		ASSERT_TRUE( deserializedUser.has_value() );

		EXPECT_EQ( deserializedUser->get<std::string>( "/username" ), "testuser" );
		EXPECT_EQ( deserializedUser->get<int64_t>( "/userId" ), 12345 );

		auto deserializedPrefs = deserializedUser->get<Document>( "/preferences" );
		ASSERT_TRUE( deserializedPrefs.has_value() );
		EXPECT_EQ( deserializedPrefs->get<bool>( "/emailNotifications" ), true );
		EXPECT_EQ( deserializedPrefs->get<std::string>( "/theme" ), "dark" );
	}

	//----------------------------------------------
	// Generic Document operations tests
	//----------------------------------------------

	TEST( DocumentTest, GenericGetDocument )
	{
		Document doc;

		// Test getting primitive values as documents
		doc.set<std::string>( "name", "Alice" );
		doc.set<int64_t>( "age", 30 );
		doc.set<bool>( "active", true );

		auto nameDoc = doc.get<Document>( "name" );
		ASSERT_TRUE( nameDoc.has_value() );
		EXPECT_EQ( nameDoc->get<std::string>( "" ), "Alice" );

		auto ageDoc = doc.get<Document>( "age" );
		ASSERT_TRUE( ageDoc.has_value() );
		EXPECT_EQ( ageDoc->get<int64_t>( "" ), 30 );

		auto activeDoc = doc.get<Document>( "active" );
		ASSERT_TRUE( activeDoc.has_value() );
		EXPECT_EQ( activeDoc->get<bool>( "" ), true );

		// Test getting nested objects
		doc.set<std::string>( "user.profile.firstName", "Bob" );
		doc.set<std::string>( "user.profile.lastName", "Smith" );

		auto userDoc = doc.get<Document>( "user" );
		ASSERT_TRUE( userDoc.has_value() );
		EXPECT_EQ( userDoc->get<std::string>( "profile.firstName" ), "Bob" );
		EXPECT_EQ( userDoc->get<std::string>( "profile.lastName" ), "Smith" );

		auto profileDoc = doc.get<Document>( "user.profile" );
		ASSERT_TRUE( profileDoc.has_value() );
		EXPECT_EQ( profileDoc->get<std::string>( "firstName" ), "Bob" );
		EXPECT_EQ( profileDoc->get<std::string>( "lastName" ), "Smith" );

		// Test getting arrays
		auto hobbiesArrOpt = doc.get<Document::Array>( "hobbies" );
		if ( !hobbiesArrOpt.has_value() )
		{
			doc.set<Document::Array>( "hobbies" );
			hobbiesArrOpt = doc.get<Document::Array>( "hobbies" );
		}
		auto hobbiesArr = hobbiesArrOpt.value();
		hobbiesArr.add<std::string>( "reading" );
		hobbiesArr.add<std::string>( "coding" );

		auto hobbiesDoc = doc.get<Document>( "hobbies" );
		ASSERT_TRUE( hobbiesDoc.has_value() );
		// Get Array wrapper to access size
		auto hobbiesArrayWrapper = hobbiesDoc->get<Document::Array>( "" );
		ASSERT_TRUE( hobbiesArrayWrapper.has_value() );
		EXPECT_EQ( hobbiesArrayWrapper->size(), 2 );
		EXPECT_EQ( hobbiesDoc->get<Document::Array>( "" ).value().get<std::string>( 0 ).value_or( "" ), "reading" );
		EXPECT_EQ( hobbiesDoc->get<Document::Array>( "" ).value().get<std::string>( 1 ).value_or( "" ), "coding" );

		// Test non-existent path
		auto nonExistent = doc.get<Document>( "doesnotexist" );
		EXPECT_FALSE( nonExistent.has_value() );
	}

	TEST( DocumentTest, GenericSetDocument )
	{
		Document doc;

		// Test setting primitive documents
		Document nameDoc;
		nameDoc.set<std::string>( "", "Alice" );

		doc.set<Document>( "name", nameDoc );
		EXPECT_EQ( doc.get<std::string>( "name" ), "Alice" );

		Document ageDoc;
		ageDoc.set<int64_t>( "", 25 );
		doc.set<Document>( "age", ageDoc );
		EXPECT_EQ( doc.get<int64_t>( "age" ), 25 );

		// Test setting complex objects
		Document profileDoc;
		profileDoc.set<std::string>( "firstName", "Bob" );
		profileDoc.set<std::string>( "lastName", "Smith" );
		profileDoc.set<int64_t>( "experience", 5 );

		doc.set<Document>( "user.profile", profileDoc );
		EXPECT_EQ( doc.get<std::string>( "user.profile.firstName" ), "Bob" );
		EXPECT_EQ( doc.get<std::string>( "user.profile.lastName" ), "Smith" );
		EXPECT_EQ( doc.get<int64_t>( "user.profile.experience" ), 5 );

		// Test setting arrays
		Document hobbiesDoc;
		hobbiesDoc.set<Document::Array>( "" );

		auto hobbiesDocArrOpt = hobbiesDoc.get<Document::Array>( "" );
		if ( hobbiesDocArrOpt.has_value() )
		{
			auto hobbiesDocArr = hobbiesDocArrOpt.value();
			hobbiesDocArr.add<std::string>( "reading" );
			hobbiesDocArr.add<std::string>( "gaming" );
			hobbiesDocArr.add<std::string>( "traveling" );
		}

		doc.set<Document>( "hobbies", hobbiesDoc );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().size(), 3 );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 0 ).value_or( "" ), "reading" );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 1 ).value_or( "" ), "gaming" );
		EXPECT_EQ( doc.get<Document::Array>( "hobbies" ).value().get<std::string>( 2 ).value_or( "" ), "traveling" );

		// Test overwriting existing values
		Document newNameDoc;
		newNameDoc.set<std::string>( "", "Charlie" );
		doc.set<Document>( "name", newNameDoc );
		EXPECT_EQ( doc.get<std::string>( "name" ), "Charlie" );

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
		Document doc;

		// Test adding primitive documents to array
		Document str1;

		str1.set<std::string>( "", "first" );
		auto stringsOpt = doc.get<Document::Array>( "strings" );
		if ( !stringsOpt.has_value() )
		{
			doc.set<Document::Array>( "strings" );
			stringsOpt = doc.get<Document::Array>( "strings" );
		}
		stringsOpt.value().add( str1 );

		Document str2;

		str2.set<std::string>( "", "second" );
		stringsOpt.value().add( str2 );

		EXPECT_EQ( doc.get<Document::Array>( "strings" ).value().size(), 2 );
		EXPECT_EQ( doc.get<Document::Array>( "strings" ).value().get<std::string>( 0 ).value_or( "" ), "first" );
		EXPECT_EQ( doc.get<Document::Array>( "strings" ).value().get<std::string>( 1 ).value_or( "" ), "second" );

		// Test adding complex objects to array
		Document user1;
		user1.set<std::string>( "name", "Alice" );
		user1.set<int64_t>( "age", 30 );
		user1.set<bool>( "active", true );

		Document user2;
		user2.set<std::string>( "name", "Bob" );
		user2.set<int64_t>( "age", 25 );
		user2.set<bool>( "active", false );

		auto usersOpt = doc.get<Document::Array>( "users" );
		if ( !usersOpt.has_value() )
		{
			doc.set<Document::Array>( "users" );
			usersOpt = doc.get<Document::Array>( "users" );
		}
		usersOpt.value().add( user1 );
		usersOpt.value().add( user2 );

		EXPECT_EQ( doc.get<Document::Array>( "users" ).value().size(), 2 );

		// Verify first user using direct access
		auto usersArray = doc.get<Document::Array>( "users" ).value();
		auto firstUserOpt = usersArray.get<Document>( 0 );
		ASSERT_TRUE( firstUserOpt.has_value() );
		auto firstUser = firstUserOpt.value();
		EXPECT_EQ( firstUser.get<std::string>( "name" ), "Alice" );
		EXPECT_EQ( firstUser.get<int64_t>( "age" ), 30 );
		EXPECT_EQ( firstUser.get<bool>( "active" ), true );

		// Verify second user
		auto secondUserOpt = usersArray.get<Document>( 1 );
		ASSERT_TRUE( secondUserOpt.has_value() );
		auto secondUser = secondUserOpt.value();
		EXPECT_EQ( secondUser.get<std::string>( "name" ), "Bob" );
		EXPECT_EQ( secondUser.get<int64_t>( "age" ), 25 );
		EXPECT_EQ( secondUser.get<bool>( "active" ), false );

		// Test adding nested arrays
		Document nestedArray;
		nestedArray.set<Document::Array>( "" );

		auto nestedArrOpt = nestedArray.get<Document::Array>( "" );
		if ( nestedArrOpt.has_value() )
		{
			auto nestedArr = nestedArrOpt.value();
			nestedArr.add<std::string>( "item1" );
			nestedArr.add<std::string>( "item2" );
		}

		auto nestedArraysOpt = doc.get<Document::Array>( "nested" );
		if ( !nestedArraysOpt.has_value() )
		{
			doc.set<Document::Array>( "nested" );
			nestedArraysOpt = doc.get<Document::Array>( "nested" );
		}
		nestedArraysOpt.value().add( nestedArray );
		EXPECT_EQ( doc.get<Document::Array>( "nested" ).value().size(), 1 );

		auto retrievedNestedArrayOpt = doc.get<Document::Array>( "nested" ).value().get<Document>( 0 );
		ASSERT_TRUE( retrievedNestedArrayOpt.has_value() );
		auto retrievedNestedArray = retrievedNestedArrayOpt.value();
		// Get Array wrapper to access size
		auto nestedArrayWrapper = retrievedNestedArray.get<Document::Array>( "" );
		ASSERT_TRUE( nestedArrayWrapper.has_value() );
		EXPECT_EQ( nestedArrayWrapper->size(), 2 );
		EXPECT_EQ( retrievedNestedArray.get<Document::Array>( "" ).value().get<std::string>( 0 ).value_or( "" ), "item1" );
		EXPECT_EQ( retrievedNestedArray.get<Document::Array>( "" ).value().get<std::string>( 1 ).value_or( "" ), "item2" );

		// Test adding to non-existent array (should create it)
		Document newItem;
		newItem.set<std::string>( "id", "test123" );
		auto itemsOpt = doc.get<Document::Array>( "items" );
		if ( !itemsOpt.has_value() )
		{
			doc.set<Document::Array>( "items" );
			itemsOpt = doc.get<Document::Array>( "items" );
		}
		itemsOpt.value().add( newItem );

		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().size(), 1 );
		auto itemOpt = doc.get<Document::Array>( "items" ).value().get<Document>( 0 );
		ASSERT_TRUE( itemOpt.has_value() );
		EXPECT_EQ( itemOpt.value().get<std::string>( "" ).value_or( "missing" ), "missing" ); // Not a string, it's an object

		auto retrievedItemOpt = doc.get<Document::Array>( "items" ).value().get<Document>( 0 );
		ASSERT_TRUE( retrievedItemOpt.has_value() );
		auto retrievedItem = retrievedItemOpt.value();
		EXPECT_EQ( retrievedItem.get<std::string>( "id" ), "test123" );
	}

	TEST( DocumentTest, GenericMethodsConsistency )
	{
		Document doc;

		// Test consistency between generic and specialized methods
		Document complexDoc;
		complexDoc.set<std::string>( "type", "user" );

		complexDoc.set<int64_t>( "id", 12345 );
		complexDoc.set<bool>( "verified", true );

		// Set using generic method
		doc.set<Document>( "profile", complexDoc );

		// Retrieve using specialized methods
		EXPECT_EQ( doc.get<std::string>( "profile.type" ), "user" );
		EXPECT_EQ( doc.get<int64_t>( "profile.id" ), 12345 );
		EXPECT_EQ( doc.get<bool>( "profile.verified" ), true );

		// Retrieve using generic method and verify
		auto retrievedProfile = doc.get<Document>( "profile" );
		ASSERT_TRUE( retrievedProfile.has_value() );
		EXPECT_EQ( retrievedProfile->get<std::string>( "type" ), "user" );
		EXPECT_EQ( retrievedProfile->get<int64_t>( "id" ), 12345 );
		EXPECT_EQ( retrievedProfile->get<bool>( "verified" ), true );

		// Test array consistency
		Document arrayItem;
		arrayItem.set<std::string>( "value", "test" );

		auto itemsConsistencyOpt = doc.get<Document::Array>( "items" );
		if ( !itemsConsistencyOpt.has_value() )
		{
			doc.set<Document::Array>( "items" );
			itemsConsistencyOpt = doc.get<Document::Array>( "items" );
		}
		itemsConsistencyOpt.value().add( arrayItem );
		// Verify via specialized array methods
		EXPECT_EQ( doc.get<Document::Array>( "items" ).value().size(), 1 );
		auto itemOpt = doc.get<Document::Array>( "items" ).value().get<Document>( 0 );
		ASSERT_TRUE( itemOpt.has_value() );
		auto item = itemOpt.value();
		EXPECT_EQ( item.get<std::string>( "value" ), "test" );

		// Verify via generic methods
		auto itemsArray = doc.get<Document>( "items" );
		ASSERT_TRUE( itemsArray.has_value() );
		// Get Array wrapper to access size
		auto itemsArrayWrapper = itemsArray->get<Document::Array>( "" );
		ASSERT_TRUE( itemsArrayWrapper.has_value() );
		EXPECT_EQ( itemsArrayWrapper->size(), 1 );
		auto itemFromGenericOpt = itemsArray->get<Document::Array>( "" ).value().get<Document>( 0 );
		ASSERT_TRUE( itemFromGenericOpt.has_value() );
		auto itemFromGeneric = itemFromGenericOpt.value();
		EXPECT_EQ( itemFromGeneric.get<std::string>( "value" ), "test" );
	}

	//----------------------------------------------
	// Character utility methods tests
	//----------------------------------------------

	TEST( DocumentTest, CharacterBasicOperations )
	{
		Document doc;

		// Test setChar and getChar
		doc.set<char>( "letter", 'A' );
		auto charResult = doc.get<char>( "letter" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_TRUE( doc.is<char>( "letter" ) );

		// Test with special characters
		doc.set<char>( "special", '\n' );
		charResult = doc.get<char>( "special" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), '\n' );
		EXPECT_TRUE( doc.is<char>( "special" ) );

		// Test with null character
		doc.set<char>( "null_char", '\0' );
		charResult = doc.get<char>( "null_char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), '\0' );
		EXPECT_TRUE( doc.is<char>( "null_char" ) );

		// Test nested path
		doc.set<char>( "nested.deep.char", 'Z' );
		charResult = doc.get<char>( "nested.deep.char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Z' );
		EXPECT_TRUE( doc.is<char>( "nested.deep.char" ) );
	}

	TEST( DocumentTest, CharacterJsonPointerMethods )
	{
		Document doc;

		// Test set/get with JSON Pointer notation (auto-detected)
		doc.set<char>( "/ptr_char", 'X' );
		auto charResult = doc.get<char>( "/ptr_char" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_TRUE( doc.is<char>( "/ptr_char" ) );

		// Test nested JSON Pointer paths
		doc.set<char>( "/user/initial", 'J' );
		charResult = doc.get<char>( "/user/initial" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'J' );
		EXPECT_TRUE( doc.is<char>( "/user/initial" ) );

		// Test array index JSON Pointer
		doc.set<char>( "/grades/0", 'A' );
		charResult = doc.get<char>( "/grades/0" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_TRUE( doc.is<char>( "/grades/0" ) );

		// Test is<char> with non-existent paths
		EXPECT_FALSE( doc.is<char>( "/nonexistent" ) );
		EXPECT_FALSE( doc.is<char>( "/user/nonexistent" ) );
	}

	TEST( DocumentTest, CharacterJsonPointerVsDotNotation )
	{
		Document doc;

		// Test that JSON Pointer and dot notation access different paths
		doc.set<char>( "user.name", 'D' );	// Dot notation - creates nested object
		doc.set<char>( "/user.name", 'P' ); // JSON Pointer - creates field named "user.name"

		// Verify they access different locations
		auto dotResult = doc.get<char>( "user.name" );
		ASSERT_TRUE( dotResult.has_value() );
		EXPECT_EQ( dotResult.value(), 'D' );

		auto pointerResult = doc.get<char>( "/user.name" );
		ASSERT_TRUE( pointerResult.has_value() );
		EXPECT_EQ( pointerResult.value(), 'P' );

		// Verify type checking works correctly
		EXPECT_TRUE( doc.is<char>( "user.name" ) );
		EXPECT_TRUE( doc.is<char>( "/user.name" ) );

		// Test nested structure with JSON Pointers
		doc.set<char>( "/profile/data/grade", 'A' );
		auto nestedResult = doc.get<char>( "/profile/data/grade" );
		ASSERT_TRUE( nestedResult.has_value() );
		EXPECT_EQ( nestedResult.value(), 'A' );
		EXPECT_TRUE( doc.is<char>( "/profile/data/grade" ) );
	}

	TEST( DocumentTest, CharacterArrayOperations )
	{
		Document doc;

		// Test addCharToArray
		auto charArrayOpt = doc.get<Document::Array>( "char_array" );
		if ( !charArrayOpt.has_value() )
		{
			doc.set<Document::Array>( "char_array" );
			charArrayOpt = doc.get<Document::Array>( "char_array" );
		}
		auto charArray = charArrayOpt.value();
		charArray.add<char>( 'A' );
		charArray.add<char>( 'B' );
		charArray.add<char>( 'C' );

		// Test getArrayElementChar
		auto charResult = doc.get<Document::Array>( "char_array" ).value().get<char>( 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );

		charResult = doc.get<Document::Array>( "char_array" ).value().get<char>( 1 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'B' );

		charResult = doc.get<Document::Array>( "char_array" ).value().get<char>( 2 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'C' );

		// Test array size
		EXPECT_EQ( doc.get<Document::Array>( "char_array" ).value().size(), 3 );

		// Test out of bounds access
		charResult = doc.get<Document::Array>( "char_array" ).value().get<char>( 10 );
		EXPECT_FALSE( charResult.has_value() );

		// Test adding to non-existent array (should create array)
		auto newArrayOpt = doc.get<Document::Array>( "new_array" );
		if ( !newArrayOpt.has_value() )
		{
			doc.set<Document::Array>( "new_array" );
			newArrayOpt = doc.get<Document::Array>( "new_array" );
		}
		newArrayOpt.value().add<char>( 'X' );
		charResult = doc.get<Document::Array>( "new_array" ).value().get<char>( 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_EQ( doc.get<Document::Array>( "new_array" ).value().size(), 1 );
	}

	TEST( DocumentTest, CharacterValidationMethods )
	{
		Document doc;

		// Test is<char> with valid single character
		doc.set<char>( "single", 'Q' );
		EXPECT_TRUE( doc.is<char>( "single" ) );

		// Test is<char> with multi-character string (should be false)
		doc.set<std::string>( "multi", "Hello" );
		EXPECT_FALSE( doc.is<char>( "multi" ) );

		// Test is<char> with empty string (should be false)
		doc.set<std::string>( "empty", "" );
		EXPECT_FALSE( doc.is<char>( "empty" ) );

		// Test is<char> with non-string types
		doc.set<int64_t>( "number", 42 );
		EXPECT_FALSE( doc.is<char>( "number" ) );

		doc.set<bool>( "boolean", true );
		EXPECT_FALSE( doc.is<char>( "boolean" ) );

		// Test is<char> with non-existent path
		EXPECT_FALSE( doc.is<char>( "nonexistent" ) );
	}

	TEST( DocumentTest, CharacterErrorHandling )
	{
		Document doc;

		// Test getChar on non-existent path
		auto charResult = doc.get<char>( "nonexistent" );
		EXPECT_FALSE( charResult.has_value() );

		// Test getChar on non-string value
		doc.set<int64_t>( "number", 123 );
		charResult = doc.get<char>( "number" );
		EXPECT_FALSE( charResult.has_value() );

		// Test getChar on empty string
		doc.set<std::string>( "empty", "" );
		charResult = doc.get<char>( "empty" );
		EXPECT_FALSE( charResult.has_value() );

		// Test array operations on non-existent array
		EXPECT_FALSE( doc.get<Document::Array>( "nonexistent_array" ).has_value() );

		// Test array operations on non-array value
		doc.set<std::string>( "not_array", "test" );
		EXPECT_FALSE( doc.get<Document::Array>( "not_array" ).has_value() );
	}

	TEST( DocumentTest, CharacterIntegrationWithOtherTypes )
	{
		Document doc;

		// Create a mixed document with characters and other types
		doc.set<char>( "initial", 'A' );
		doc.set<std::string>( "name", "John" );
		doc.set<int64_t>( "age", 30 );
		doc.set<bool>( "active", true );

		// Verify character operations don't interfere with other types
		auto charResult = doc.get<char>( "initial" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'A' );
		EXPECT_EQ( doc.get<std::string>( "name" ), "John" );
		EXPECT_EQ( doc.get<int64_t>( "age" ), 30 );
		EXPECT_EQ( doc.get<bool>( "active" ), true );

		// Test overwriting different types with characters
		doc.set<char>( "age", 'X' ); // Overwrite int with char
		charResult = doc.get<char>( "age" );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'X' );
		EXPECT_TRUE( doc.is<char>( "age" ) );

		// After overwriting with char, getInt should return nullopt since it's no longer an int
		auto intResult = doc.get<int64_t>( "age" );
		EXPECT_FALSE( intResult.has_value() );

		// Test character arrays mixed with regular arrays
		auto mixedArrayOpt = doc.get<Document::Array>( "mixed_array" );
		if ( !mixedArrayOpt.has_value() )
		{
			doc.set<Document::Array>( "mixed_array" );
			mixedArrayOpt = doc.get<Document::Array>( "mixed_array" );
		}
		auto mixedArray = mixedArrayOpt.value();
		mixedArray.add<char>( 'Z' );
		mixedArray.add<int64_t>( 42 ); // Explicit cast to avoid ambiguity
		mixedArray.add<char>( 'Y' );

		EXPECT_EQ( doc.get<Document::Array>( "mixed_array" ).value().size(), 3 );

		charResult = doc.get<Document::Array>( "mixed_array" ).value().get<char>( 0 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Z' );

		EXPECT_EQ( doc.get<Document::Array>( "mixed_array" ).value().get<int64_t>( 1 ).value_or( 0 ), 42 );

		charResult = doc.get<Document::Array>( "mixed_array" ).value().get<char>( 2 );
		ASSERT_TRUE( charResult.has_value() );
		EXPECT_EQ( charResult.value(), 'Y' );
	}
} // namespace nfx::serialization::json::test
