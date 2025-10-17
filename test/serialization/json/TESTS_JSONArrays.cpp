/**
 * @file TESTS_JSONArrays.cpp
 * @brief Comprehensive tests for JSON Array class functionality
 * @details Tests covering array construction, element access, modification, insertion,
 *          type-safe operations, nested arrays/objects, and auto-detection features
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// Array construction and basic operations
	//=====================================================================

	class JSONArrayTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create test document with various array structures
			std::string jsonStr = R"({
				"numbers": [1, 2, 3, 42, 100],
				"strings": ["hello", "world", "test"],
				"booleans": [true, false, true],
				"doubles": [3.14, 2.71, 1.41],
				"mixed": [1, "hello", true, 3.14],
				"nested_arrays": [[1, 2], [3, 4], ["a", "b"]],
				"nested_objects": [
					{"name": "Alice", "age": 30},
					{"name": "Bob", "age": 25}
				],
				"empty_array": [],
				"single_char": ["a", "b", "X"]
			})";

			auto doc = Document::fromJsonString( jsonStr );
			ASSERT_TRUE( doc.has_value() );
			testDoc = std::move( doc.value() );
		}

		Document testDoc;
	};

	//----------------------------------------------
	// Array construction
	//----------------------------------------------

	TEST_F( JSONArrayTest, DefaultConstructor )
	{
		Document::Array emptyArray;
		EXPECT_EQ( emptyArray.size(), 0 );
	}

	TEST_F( JSONArrayTest, GetArrayFromDocument )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" );
		ASSERT_TRUE( numbersArray.has_value() );
		EXPECT_EQ( numbersArray.value().size(), 5 );

		auto emptyArray = testDoc.get<Document::Array>( "empty_array" );
		ASSERT_TRUE( emptyArray.has_value() );
		EXPECT_EQ( emptyArray.value().size(), 0 );
	}

	//----------------------------------------------
	// Element access (get<T>)
	//----------------------------------------------

	TEST_F( JSONArrayTest, GetStringElements )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		EXPECT_EQ( stringsArray.get<std::string>( 0 ).value_or( "" ), "hello" );
		EXPECT_EQ( stringsArray.get<std::string>( 1 ).value_or( "" ), "world" );
		EXPECT_EQ( stringsArray.get<std::string>( 2 ).value_or( "" ), "test" );

		// Out of bounds
		EXPECT_FALSE( stringsArray.get<std::string>( 10 ).has_value() );
	}

	TEST_F( JSONArrayTest, GetIntegerElements )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();

		EXPECT_EQ( numbersArray.get<int64_t>( 0 ).value_or( 0 ), 1 );
		EXPECT_EQ( numbersArray.get<int64_t>( 1 ).value_or( 0 ), 2 );
		EXPECT_EQ( numbersArray.get<int64_t>( 3 ).value_or( 0 ), 42 );

		// Test int32_t support
		EXPECT_EQ( numbersArray.get<int32_t>( 0 ).value_or( 0 ), 1 );
		EXPECT_EQ( numbersArray.get<int32_t>( 3 ).value_or( 0 ), 42 );
	}

	TEST_F( JSONArrayTest, GetDoubleElements )
	{
		auto doublesArray = testDoc.get<Document::Array>( "doubles" ).value();

		EXPECT_DOUBLE_EQ( doublesArray.get<double>( 0 ).value_or( 0.0 ), 3.14 );
		EXPECT_DOUBLE_EQ( doublesArray.get<double>( 1 ).value_or( 0.0 ), 2.71 );
		EXPECT_DOUBLE_EQ( doublesArray.get<double>( 2 ).value_or( 0.0 ), 1.41 );
	}

	TEST_F( JSONArrayTest, GetBooleanElements )
	{
		auto boolsArray = testDoc.get<Document::Array>( "booleans" ).value();

		EXPECT_EQ( boolsArray.get<bool>( 0 ).value_or( false ), true );
		EXPECT_EQ( boolsArray.get<bool>( 1 ).value_or( true ), false );
		EXPECT_EQ( boolsArray.get<bool>( 2 ).value_or( false ), true );
	}

	TEST_F( JSONArrayTest, GetCharacterElements )
	{
		auto charsArray = testDoc.get<Document::Array>( "single_char" ).value();

		EXPECT_EQ( charsArray.get<char>( 0 ).value_or( '\0' ), 'a' );
		EXPECT_EQ( charsArray.get<char>( 1 ).value_or( '\0' ), 'b' );
		EXPECT_EQ( charsArray.get<char>( 2 ).value_or( '\0' ), 'X' );
	}

	TEST_F( JSONArrayTest, GetDocumentElements )
	{
		auto mixedArray = testDoc.get<Document::Array>( "mixed" ).value();

		// Any element can be retrieved as Document
		auto doc0 = mixedArray.get<Document>( 0 );
		auto doc1 = mixedArray.get<Document>( 1 );
		auto doc2 = mixedArray.get<Document>( 2 );
		auto doc3 = mixedArray.get<Document>( 3 );

		ASSERT_TRUE( doc0.has_value() );
		ASSERT_TRUE( doc1.has_value() );
		ASSERT_TRUE( doc2.has_value() );
		ASSERT_TRUE( doc3.has_value() );
	}

	TEST_F( JSONArrayTest, GetNestedArrayElements )
	{
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();

		// Get nested array
		auto subArray0 = nestedArrays.get<Document::Array>( 0 );
		ASSERT_TRUE( subArray0.has_value() );
		EXPECT_EQ( subArray0.value().size(), 2 );
		EXPECT_EQ( subArray0.value().get<int64_t>( 0 ).value_or( 0 ), 1 );
		EXPECT_EQ( subArray0.value().get<int64_t>( 1 ).value_or( 0 ), 2 );

		auto subArray2 = nestedArrays.get<Document::Array>( 2 );
		ASSERT_TRUE( subArray2.has_value() );
		EXPECT_EQ( subArray2.value().get<std::string>( 0 ).value_or( "" ), "a" );
		EXPECT_EQ( subArray2.value().get<std::string>( 1 ).value_or( "" ), "b" );
	}

	TEST_F( JSONArrayTest, GetNestedObjectElements )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Get nested object
		auto obj0 = nestedObjects.get<Document::Object>( 0 );
		ASSERT_TRUE( obj0.has_value() );
		EXPECT_EQ( obj0.value().get<std::string>( "name" ).value_or( "" ), "Alice" );
		EXPECT_EQ( obj0.value().get<int64_t>( "age" ).value_or( 0 ), 30 );

		auto obj1 = nestedObjects.get<Document::Object>( 1 );
		ASSERT_TRUE( obj1.has_value() );
		EXPECT_EQ( obj1.value().get<std::string>( "name" ).value_or( "" ), "Bob" );
		EXPECT_EQ( obj1.value().get<int64_t>( "age" ).value_or( 0 ), 25 );
	}

	//----------------------------------------------
	// Type mismatch
	//----------------------------------------------

	TEST_F( JSONArrayTest, TypeMismatchReturnsNullopt )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		// Try to get string as number
		EXPECT_FALSE( stringsArray.get<int64_t>( 0 ).has_value() );
		EXPECT_FALSE( stringsArray.get<double>( 0 ).has_value() );
		EXPECT_FALSE( stringsArray.get<bool>( 0 ).has_value() );

		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();

		// Try to get number as string
		EXPECT_FALSE( numbersArray.get<std::string>( 0 ).has_value() );
	}

	//----------------------------------------------
	// Array element modification (set<T>)
	//----------------------------------------------

	TEST_F( JSONArrayTest, SetStringElements )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		stringsArray.set<std::string>( 1, "modified" );
		EXPECT_EQ( stringsArray.get<std::string>( 1 ).value_or( "" ), "modified" );

		// Test string_view
		std::string_view sv = "view_test";
		stringsArray.set<std::string_view>( 2, std::move( sv ) );
		EXPECT_EQ( stringsArray.get<std::string>( 2 ).value_or( "" ), "view_test" );
	}

	TEST_F( JSONArrayTest, SetIntegerElements )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();

		numbersArray.set<int64_t>( 0, 999 );
		EXPECT_EQ( numbersArray.get<int64_t>( 0 ).value_or( 0 ), 999 );

		// Test int32_t
		numbersArray.set<int32_t>( 1, 888 );
		EXPECT_EQ( numbersArray.get<int32_t>( 1 ).value_or( 0 ), 888 );
	}

	TEST_F( JSONArrayTest, SetDoubleElements )
	{
		auto doublesArray = testDoc.get<Document::Array>( "doubles" ).value();

		doublesArray.set<double>( 0, 9.99 );
		EXPECT_DOUBLE_EQ( doublesArray.get<double>( 0 ).value_or( 0.0 ), 9.99 );
	}

	TEST_F( JSONArrayTest, SetBooleanElements )
	{
		auto boolsArray = testDoc.get<Document::Array>( "booleans" ).value();

		boolsArray.set<bool>( 0, false );
		EXPECT_EQ( boolsArray.get<bool>( 0 ).value_or( true ), false );
	}

	TEST_F( JSONArrayTest, SetCharacterElements )
	{
		auto charsArray = testDoc.get<Document::Array>( "single_char" ).value();

		charsArray.set<char>( 0, 'Z' );
		EXPECT_EQ( charsArray.get<char>( 0 ).value_or( '\0' ), 'Z' );
	}

	TEST_F( JSONArrayTest, SetDocumentElements )
	{
		auto mixedArray = testDoc.get<Document::Array>( "mixed" ).value();

		Document newDoc;

		newDoc.set<std::string>( "test", "value" );

		mixedArray.set<Document>( 0, std::move( newDoc ) );
		auto retrievedDoc = mixedArray.get<Document>( 0 );
		ASSERT_TRUE( retrievedDoc.has_value() );
		EXPECT_EQ( retrievedDoc.value().get<std::string>( "test" ).value_or( "" ), "value" );
	}

	TEST_F( JSONArrayTest, SetObjectElements )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Create new object and set it
		Document objDoc;

		objDoc.set<std::string>( "name", "Charlie" );
		objDoc.set<int64_t>( "age", 35 );
		auto newObj = objDoc.get<Document::Object>( "" ).value();

		nestedObjects.set<Document::Object>( 0, std::move( newObj ) );
		auto retrievedObj = nestedObjects.get<Document::Object>( 0 );
		ASSERT_TRUE( retrievedObj.has_value() );
		EXPECT_EQ( retrievedObj.value().get<std::string>( "name" ).value_or( "" ), "Charlie" );
	}

	TEST_F( JSONArrayTest, SetArrayElements )
	{
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();

		// Create new array and set it
		Document arrDoc;
		arrDoc.set<Document::Array>( "" );

		arrDoc.set<int64_t>( "/0", 100 );
		arrDoc.set<int64_t>( "/1", 200 );
		auto newArr = arrDoc.get<Document::Array>( "" ).value();

		nestedArrays.set<Document::Array>( 0, std::move( newArr ) );
		auto retrievedArr = nestedArrays.get<Document::Array>( 0 );
		ASSERT_TRUE( retrievedArr.has_value() );
		EXPECT_EQ( retrievedArr.value().get<int64_t>( 0 ).value_or( 0 ), 100 );
		EXPECT_EQ( retrievedArr.value().get<int64_t>( 1 ).value_or( 0 ), 200 );
	}

	//----------------------------------------------
	// Array element addition (add<T>)
	//----------------------------------------------

	TEST_F( JSONArrayTest, AddStringElements )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();
		size_t originalSize = stringsArray.size();

		stringsArray.add<std::string>( "new_string" );
		EXPECT_EQ( stringsArray.size(), originalSize + 1 );
		EXPECT_EQ( stringsArray.get<std::string>( originalSize ).value_or( "" ), "new_string" );

		// Test string_view
		std::string_view sv = "view_added";
		stringsArray.add<std::string_view>( std::move( sv ) );
		EXPECT_EQ( stringsArray.get<std::string>( originalSize + 1 ).value_or( "" ), "view_added" );
	}

	TEST_F( JSONArrayTest, AddIntegerElements )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();
		size_t originalSize = numbersArray.size();

		numbersArray.add<int64_t>( 777 );
		EXPECT_EQ( numbersArray.size(), originalSize + 1 );
		EXPECT_EQ( numbersArray.get<int64_t>( originalSize ).value_or( 0 ), 777 );

		// Test int32_t
		numbersArray.add<int32_t>( 555 );
		EXPECT_EQ( numbersArray.get<int32_t>( originalSize + 1 ).value_or( 0 ), 555 );
	}

	TEST_F( JSONArrayTest, AddDoubleElements )
	{
		auto doublesArray = testDoc.get<Document::Array>( "doubles" ).value();
		size_t originalSize = doublesArray.size();

		doublesArray.add<double>( 7.77 );
		EXPECT_EQ( doublesArray.size(), originalSize + 1 );
		EXPECT_DOUBLE_EQ( doublesArray.get<double>( originalSize ).value_or( 0.0 ), 7.77 );
	}

	TEST_F( JSONArrayTest, AddBooleanElements )
	{
		auto boolsArray = testDoc.get<Document::Array>( "booleans" ).value();
		size_t originalSize = boolsArray.size();

		boolsArray.add<bool>( false );
		EXPECT_EQ( boolsArray.size(), originalSize + 1 );
		EXPECT_EQ( boolsArray.get<bool>( originalSize ).value_or( true ), false );
	}

	TEST_F( JSONArrayTest, AddCharacterElements )
	{
		auto charsArray = testDoc.get<Document::Array>( "single_char" ).value();
		size_t originalSize = charsArray.size();

		charsArray.add<char>( 'Y' );
		EXPECT_EQ( charsArray.size(), originalSize + 1 );
		EXPECT_EQ( charsArray.get<char>( originalSize ).value_or( '\0' ), 'Y' );
	}

	TEST_F( JSONArrayTest, AddDocumentElements )
	{
		auto mixedArray = testDoc.get<Document::Array>( "mixed" ).value();
		size_t originalSize = mixedArray.size();

		Document newDoc;
		newDoc.set<std::string>( "added", "document" );

		mixedArray.add<Document>( std::move( newDoc ) );
		EXPECT_EQ( mixedArray.size(), originalSize + 1 );

		auto retrievedDoc = mixedArray.get<Document>( originalSize );
		ASSERT_TRUE( retrievedDoc.has_value() );
		EXPECT_EQ( retrievedDoc.value().get<std::string>( "added" ).value_or( "" ), "document" );
	}

	TEST_F( JSONArrayTest, AddObjectElements )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();
		size_t originalSize = nestedObjects.size();

		Document objDoc;

		objDoc.set<std::string>( "name", "David" );
		objDoc.set<int64_t>( "age", 40 );
		auto newObj = objDoc.get<Document::Object>( "" ).value();

		nestedObjects.add<Document::Object>( std::move( newObj ) );
		EXPECT_EQ( nestedObjects.size(), originalSize + 1 );

		auto retrievedObj = nestedObjects.get<Document::Object>( originalSize );
		ASSERT_TRUE( retrievedObj.has_value() );
		EXPECT_EQ( retrievedObj.value().get<std::string>( "name" ).value_or( "" ), "David" );
	}

	TEST_F( JSONArrayTest, AddArrayElements )
	{
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();
		size_t originalSize = nestedArrays.size();

		Document arrDoc;
		arrDoc.set<Document::Array>( "" );

		arrDoc.set<std::string>( "/0", "added" );
		arrDoc.set<std::string>( "/1", "array" );
		auto newArr = arrDoc.get<Document::Array>( "" ).value();

		nestedArrays.add<Document::Array>( std::move( newArr ) );
		EXPECT_EQ( nestedArrays.size(), originalSize + 1 );

		auto retrievedArr = nestedArrays.get<Document::Array>( originalSize );
		ASSERT_TRUE( retrievedArr.has_value() );
		EXPECT_EQ( retrievedArr.value().get<std::string>( 0 ).value_or( "" ), "added" );
		EXPECT_EQ( retrievedArr.value().get<std::string>( 1 ).value_or( "" ), "array" );
	}

	//----------------------------------------------
	// Array element insertion (insert<T>)
	//----------------------------------------------

	TEST_F( JSONArrayTest, InsertStringElements )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();
		size_t originalSize = stringsArray.size();
		std::string originalSecond = stringsArray.get<std::string>( 1 ).value_or( "" );

		stringsArray.insert<std::string>( 1, "inserted" );
		EXPECT_EQ( stringsArray.size(), originalSize + 1 );
		EXPECT_EQ( stringsArray.get<std::string>( 1 ).value_or( "" ), "inserted" );
		EXPECT_EQ( stringsArray.get<std::string>( 2 ).value_or( "" ), originalSecond ); // Shifted right
	}

	TEST_F( JSONArrayTest, InsertIntegerElements )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();
		size_t originalSize = numbersArray.size();

		numbersArray.insert<int64_t>( 0, 999 ); // Insert at beginning
		EXPECT_EQ( numbersArray.size(), originalSize + 1 );
		EXPECT_EQ( numbersArray.get<int64_t>( 0 ).value_or( 0 ), 999 );
		EXPECT_EQ( numbersArray.get<int64_t>( 1 ).value_or( 0 ), 1 ); // Original first element shifted
	}

	TEST_F( JSONArrayTest, InsertAtEnd )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" ).value();
		size_t originalSize = numbersArray.size();

		numbersArray.insert<int64_t>( originalSize, 888 ); // Insert at end (same as add)
		EXPECT_EQ( numbersArray.size(), originalSize + 1 );
		EXPECT_EQ( numbersArray.get<int64_t>( originalSize ).value_or( 0 ), 888 );
	}

	TEST_F( JSONArrayTest, InsertObjectElements )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();
		size_t originalSize = nestedObjects.size();

		Document objDoc;
		objDoc.set<std::string>( "name", "Inserted" );
		objDoc.set<int64_t>( "age", 99 );
		auto newObj = objDoc.get<Document::Object>( "" ).value();

		nestedObjects.insert<Document::Object>( 1, std::move( newObj ) );
		EXPECT_EQ( nestedObjects.size(), originalSize + 1 );

		auto insertedObj = nestedObjects.get<Document::Object>( 1 );
		ASSERT_TRUE( insertedObj.has_value() );
		EXPECT_EQ( insertedObj.value().get<std::string>( "name" ).value_or( "" ), "Inserted" );
	}

	//----------------------------------------------
	// Array utility methods
	//----------------------------------------------

	TEST_F( JSONArrayTest, HasElement )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		EXPECT_TRUE( stringsArray.hasElement( "0" ) );
		EXPECT_TRUE( stringsArray.hasElement( "1" ) );
		EXPECT_TRUE( stringsArray.hasElement( "2" ) );
		EXPECT_FALSE( stringsArray.hasElement( "10" ) );

		// Test JSON Pointer syntax
		EXPECT_TRUE( stringsArray.hasElement( "/0" ) );
		EXPECT_TRUE( stringsArray.hasElement( "/1" ) );
		EXPECT_FALSE( stringsArray.hasElement( "/10" ) );
	}

	TEST_F( JSONArrayTest, Size )
	{
		EXPECT_EQ( testDoc.get<Document::Array>( "numbers" ).value().size(), 5 );
		EXPECT_EQ( testDoc.get<Document::Array>( "strings" ).value().size(), 3 );
		EXPECT_EQ( testDoc.get<Document::Array>( "empty_array" ).value().size(), 0 );
	}

	TEST_F( JSONArrayTest, Clear )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();
		EXPECT_GT( stringsArray.size(), 0 );

		stringsArray.clear();
		EXPECT_EQ( stringsArray.size(), 0 );
	}

	TEST_F( JSONArrayTest, Remove )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();
		size_t originalSize = stringsArray.size();
		std::string originalLast = stringsArray.get<std::string>( 2 ).value_or( "" );

		bool removed = stringsArray.remove( 1 ); // Remove middle element
		EXPECT_TRUE( removed );
		EXPECT_EQ( stringsArray.size(), originalSize - 1 );
		EXPECT_EQ( stringsArray.get<std::string>( 1 ).value_or( "" ), originalLast ); // Last element shifted left

		// Try to remove out of bounds
		bool removedOOB = stringsArray.remove( 100 );
		EXPECT_FALSE( removedOOB );
	}

	//----------------------------------------------
	// Perfect forwarding
	//----------------------------------------------

	TEST_F( JSONArrayTest, PerfectForwardingMove )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		std::string movableString = "move_me";
		stringsArray.set<std::string>( 0, std::move( movableString ) );
		EXPECT_EQ( stringsArray.get<std::string>( 0 ).value_or( "" ), "move_me" );

		// Test with add
		std::string anotherMovable = "add_move";
		stringsArray.add<std::string>( std::move( anotherMovable ) );
		EXPECT_EQ( stringsArray.get<std::string>( stringsArray.size() - 1 ).value_or( "" ), "add_move" );

		// Test with insert
		std::string insertMovable = "insert_move";
		stringsArray.insert<std::string>( 1, std::move( insertMovable ) );
		EXPECT_EQ( stringsArray.get<std::string>( 1 ).value_or( "" ), "insert_move" );
	}

	//----------------------------------------------
	// Auto-detection and path support
	//----------------------------------------------

	TEST_F( JSONArrayTest, AutoDetectionWithJsonPointer )
	{
		// Test that Array methods work with both dot notation and JSON Pointer paths internally
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();

		// The auto-detection should work in nested path construction
		auto subArray = nestedArrays.get<Document::Array>( 0 );
		ASSERT_TRUE( subArray.has_value() );

		// Verify the sub-array has correct elements
		EXPECT_EQ( subArray.value().get<int64_t>( 0 ).value_or( 0 ), 1 );
		EXPECT_EQ( subArray.value().get<int64_t>( 1 ).value_or( 0 ), 2 );
	}

	//----------------------------------------------
	// Edge cases and error Handling
	//----------------------------------------------

	TEST_F( JSONArrayTest, OutOfBoundsAccess )
	{
		auto stringsArray = testDoc.get<Document::Array>( "strings" ).value();

		// Get out of bounds
		EXPECT_FALSE( stringsArray.get<std::string>( 100 ).has_value() );
		EXPECT_FALSE( stringsArray.get<int64_t>( 100 ).has_value() );
		EXPECT_FALSE( stringsArray.get<Document>( 100 ).has_value() );
	}

	TEST_F( JSONArrayTest, SetBeyondBounds )
	{
		auto emptyArray = testDoc.get<Document::Array>( "empty_array" ).value();
		EXPECT_EQ( emptyArray.size(), 0 );

		// Set at index 5 in empty array - should expand
		emptyArray.set<std::string>( 5, "expanded" );
		EXPECT_GE( emptyArray.size(), 6 ); // Array should have at least 6 elements now
		EXPECT_EQ( emptyArray.get<std::string>( 5 ).value_or( "" ), "expanded" );
	}

	TEST_F( JSONArrayTest, InsertBeyondBounds )
	{
		auto smallArray = testDoc.get<Document::Array>( "strings" ).value();
		size_t originalSize = smallArray.size();

		// Insert way beyond bounds - should append instead
		smallArray.insert<std::string>( 100, "appended" );
		EXPECT_EQ( smallArray.size(), originalSize + 1 );
		EXPECT_EQ( smallArray.get<std::string>( originalSize ).value_or( "" ), "appended" );
	}

	//----------------------------------------------
	// Complex nested operations
	//----------------------------------------------

	TEST_F( JSONArrayTest, DeepNestedOperations )
	{
		// Create complex nested structure
		Document complexDoc;

		Document level1ArrayDoc;
		level1ArrayDoc.set<Document::Array>( "" );
		complexDoc.set<Document>( "level1", level1ArrayDoc );

		auto level1Array = complexDoc.get<Document::Array>( "level1" ).value();

		Document level2ArrayDoc;
		level2ArrayDoc.set<Document::Array>( "" );
		level1Array.add<Document>( level2ArrayDoc );

		auto level2Array = level1Array.get<Document::Array>( 0 ).value();

		Document level3ObjectDoc;
		level2Array.add<Document>( level3ObjectDoc );

		auto level3Object = level2Array.get<Document::Object>( 0 ).value();
		level3Object.set<std::string>( "deep_value", "found_it" );

		// Verify deep access works
		EXPECT_EQ( level3Object.get<std::string>( "deep_value" ).value_or( "" ), "found_it" );
	}

	TEST_F( JSONArrayTest, MixedTypeOperations )
	{
		auto mixedArray = testDoc.get<Document::Array>( "mixed" ).value();

		// Verify we can handle mixed types correctly
		EXPECT_TRUE( mixedArray.get<int64_t>( 0 ).has_value() );	 // number
		EXPECT_TRUE( mixedArray.get<std::string>( 1 ).has_value() ); // string
		EXPECT_TRUE( mixedArray.get<bool>( 2 ).has_value() );		 // boolean
		EXPECT_TRUE( mixedArray.get<double>( 3 ).has_value() );		 // double

		// Verify type mismatches return nullopt
		EXPECT_FALSE( mixedArray.get<std::string>( 0 ).has_value() ); // number as string
		EXPECT_FALSE( mixedArray.get<int64_t>( 1 ).has_value() );	  // string as number
	}

	//----------------------------------------------
	// Nested access features
	//----------------------------------------------

	TEST_F( JSONArrayTest, NestedPathAccess )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test nested get with path syntax
		auto name1 = nestedObjects.get<std::string>( "0/name" );
		ASSERT_TRUE( name1.has_value() );
		EXPECT_EQ( name1.value(), "Alice" );

		auto age1 = nestedObjects.get<int64_t>( "0/age" );
		ASSERT_TRUE( age1.has_value() );
		EXPECT_EQ( age1.value(), 30 );

		auto name2 = nestedObjects.get<std::string>( "1/name" );
		ASSERT_TRUE( name2.has_value() );
		EXPECT_EQ( name2.value(), "Bob" );

		auto age2 = nestedObjects.get<int64_t>( "1/age" );
		ASSERT_TRUE( age2.has_value() );
		EXPECT_EQ( age2.value(), 25 );
	}

	TEST_F( JSONArrayTest, NestedPathAccessWithJsonPointer )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test with JSON Pointer syntax
		auto name1 = nestedObjects.get<std::string>( "/0/name" );
		ASSERT_TRUE( name1.has_value() );
		EXPECT_EQ( name1.value(), "Alice" );

		auto age2 = nestedObjects.get<int64_t>( "/1/age" );
		ASSERT_TRUE( age2.has_value() );
		EXPECT_EQ( age2.value(), 25 );
	}

	TEST_F( JSONArrayTest, NestedPathAccessInvalidPaths )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test invalid index
		auto invalid1 = nestedObjects.get<std::string>( "5/name" );
		EXPECT_FALSE( invalid1.has_value() );

		// Test invalid field
		auto invalid2 = nestedObjects.get<std::string>( "0/nonexistent" );
		EXPECT_FALSE( invalid2.has_value() );

		// Test empty path
		auto invalid3 = nestedObjects.get<std::string>( "" );
		EXPECT_FALSE( invalid3.has_value() );

		// Test type mismatch
		auto invalid4 = nestedObjects.get<int64_t>( "0/name" );
		EXPECT_FALSE( invalid4.has_value() );
	}

	TEST_F( JSONArrayTest, NestedPathModification )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test nested set with path syntax
		nestedObjects.set<std::string>( "0/name", "Modified Alice" );
		auto modifiedName = nestedObjects.get<std::string>( "0/name" );
		ASSERT_TRUE( modifiedName.has_value() );
		EXPECT_EQ( modifiedName.value(), "Modified Alice" );

		// Test modifying age
		nestedObjects.set<int64_t>( "1/age", 99 );
		auto modifiedAge = nestedObjects.get<int64_t>( "1/age" );
		ASSERT_TRUE( modifiedAge.has_value() );
		EXPECT_EQ( modifiedAge.value(), 99 );
	}

	TEST_F( JSONArrayTest, NestedPathModificationWithJsonPointer )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test with JSON Pointer syntax
		nestedObjects.set<std::string>( "/0/name", "JSON Pointer Alice" );
		auto modifiedName = nestedObjects.get<std::string>( "/0/name" );
		ASSERT_TRUE( modifiedName.has_value() );
		EXPECT_EQ( modifiedName.value(), "JSON Pointer Alice" );
	}

	TEST_F( JSONArrayTest, NestedPathCreation )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test creating new nested fields
		nestedObjects.set<std::string>( "0/address/city", "New York" );
		nestedObjects.set<std::string>( "0/address/country", "USA" );

		// Verify the nested fields were created
		auto city = nestedObjects.get<std::string>( "0/address/city" );
		ASSERT_TRUE( city.has_value() );
		EXPECT_EQ( city.value(), "New York" );

		auto country = nestedObjects.get<std::string>( "0/address/country" );
		ASSERT_TRUE( country.has_value() );
		EXPECT_EQ( country.value(), "USA" );
	}

	TEST_F( JSONArrayTest, NestedArrayAccess )
	{
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();

		// Test accessing elements within nested arrays
		auto firstElement = nestedArrays.get<int64_t>( "0/0" );
		ASSERT_TRUE( firstElement.has_value() );
		EXPECT_EQ( firstElement.value(), 1 );

		auto secondElement = nestedArrays.get<int64_t>( "0/1" );
		ASSERT_TRUE( secondElement.has_value() );
		EXPECT_EQ( secondElement.value(), 2 );

		// Test string elements in nested array
		auto stringElement = nestedArrays.get<std::string>( "2/0" );
		ASSERT_TRUE( stringElement.has_value() );
		EXPECT_EQ( stringElement.value(), "a" );
	}

	TEST_F( JSONArrayTest, NestedArrayModification )
	{
		auto nestedArrays = testDoc.get<Document::Array>( "nested_arrays" ).value();

		// Test modifying elements within nested arrays
		nestedArrays.set<int64_t>( "0/0", 999 );
		auto modifiedElement = nestedArrays.get<int64_t>( "0/0" );
		ASSERT_TRUE( modifiedElement.has_value() );
		EXPECT_EQ( modifiedElement.value(), 999 );

		// Test setting string in nested array
		nestedArrays.set<std::string>( "2/1", "modified" );
		auto modifiedString = nestedArrays.get<std::string>( "2/1" );
		ASSERT_TRUE( modifiedString.has_value() );
		EXPECT_EQ( modifiedString.value(), "modified" );
	}

	TEST_F( JSONArrayTest, DeepNestedPathAccess )
	{
		// Create a complex nested structure for testing
		Document complexDoc;
		complexDoc.set<Document::Array>( "" );

		complexDoc.set<Document::Object>( "/0" );
		complexDoc.set<Document::Array>( "/0/level1" );
		complexDoc.set<Document::Object>( "/0/level1/0" );
		complexDoc.set<std::string>( "/0/level1/0/deep_field", "deep_value" );

		auto rootArray = complexDoc.get<Document::Array>( "" ).value();

		// Test deep nested access
		auto deepValue = rootArray.get<std::string>( "0/level1/0/deep_field" );
		ASSERT_TRUE( deepValue.has_value() );
		EXPECT_EQ( deepValue.value(), "deep_value" );

		// Test deep nested modification
		rootArray.set<std::string>( "0/level1/0/deep_field", "modified_deep_value" );
		auto modifiedDeepValue = rootArray.get<std::string>( "0/level1/0/deep_field" );
		ASSERT_TRUE( modifiedDeepValue.has_value() );
		EXPECT_EQ( modifiedDeepValue.value(), "modified_deep_value" );
	}

	TEST_F( JSONArrayTest, NestedPathPerfectForwarding )
	{
		auto nestedObjects = testDoc.get<Document::Array>( "nested_objects" ).value();

		// Test perfect forwarding with move semantics
		std::string movableValue = "moved_value";
		nestedObjects.set<std::string>( "0/moved_field", std::move( movableValue ) );

		auto retrievedValue = nestedObjects.get<std::string>( "0/moved_field" );
		ASSERT_TRUE( retrievedValue.has_value() );
		EXPECT_EQ( retrievedValue.value(), "moved_value" );
	}

	//----------------------------------------------
	// Array serialization methods (toJsonString/toJsonBytes)
	//----------------------------------------------

	TEST_F( JSONArrayTest, ToJsonStringEmpty )
	{
		auto emptyArray = testDoc.get<Document::Array>( "empty_array" );
		ASSERT_TRUE( emptyArray.has_value() );

		std::string jsonStr = emptyArray.value().toJsonString();
		EXPECT_EQ( jsonStr, "[]" );
	}

	TEST_F( JSONArrayTest, ToJsonBytesEmpty )
	{
		auto emptyArray = testDoc.get<Document::Array>( "empty_array" );
		ASSERT_TRUE( emptyArray.has_value() );

		std::vector<uint8_t> jsonBytes = emptyArray.value().toJsonBytes();
		std::string jsonStr( jsonBytes.begin(), jsonBytes.end() );
		EXPECT_EQ( jsonStr, "[]" );
	}

	//----------------------------------------------
	// Array validation methods (isValid/lastError)
	//----------------------------------------------

	TEST_F( JSONArrayTest, IsValidForValidArray )
	{
		auto numbersArray = testDoc.get<Document::Array>( "numbers" );
		ASSERT_TRUE( numbersArray.has_value() );

		EXPECT_TRUE( numbersArray.value().isValid() );
		EXPECT_TRUE( numbersArray.value().lastError().empty() );
	}

	TEST_F( JSONArrayTest, IsValidForEmptyArray )
	{
		auto emptyArray = testDoc.get<Document::Array>( "empty_array" );
		ASSERT_TRUE( emptyArray.has_value() );

		EXPECT_TRUE( emptyArray.value().isValid() );
		EXPECT_TRUE( emptyArray.value().lastError().empty() );
	}
} // namespace nfx::serialization::json::test
