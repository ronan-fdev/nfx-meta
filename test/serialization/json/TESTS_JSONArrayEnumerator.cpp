/**
 * @file TESTS_JSONArrayEnumerator.cpp
 * @brief Comprehensive tests for JSON ArrayEnumerator functionality
 * @details Tests covering array navigation, element access, path validation,
 *          enumerator state management, and error handling scenarios.
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/ArrayEnumerator.h>
#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// JSON ArrayEnumerator tests
	//=====================================================================

	//----------------------------------------------
	// ArrayEnumerator test fixture
	//----------------------------------------------

	class ArrayEnumeratorTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create test JSON with various array types
			std::string testJson = R"({
				"users": [
					{"name": "Alice", "age": 30, "active": true},
					{"name": "Bob", "age": 25, "active": false},
					{"name": "Charlie", "age": 35, "active": true}
				],
				"scores": [100, 95, 87, 92, 78],
				"tags": ["important", "urgent", "review", "critical"],
				"empty": [],
				"mixed": [1, "text", true, null, {"nested": "object"}],
				"nested": {
					"levels": {
						"deep": {
							"array": ["deep1", "deep2", "deep3"]
						}
					}
				}
			})";

			auto docOpt = Document::fromJsonString( testJson );
			ASSERT_TRUE( docOpt.has_value() );
			document = std::move( docOpt.value() );
		}

		Document document;
	};

	//----------------------------------------------
	// Basic navigation
	//----------------------------------------------

	TEST_F( ArrayEnumeratorTest, ConstructorAndBasicNavigation )
	{
		ArrayEnumerator enumerator( document );

		// Test dot notation navigation
		EXPECT_TRUE( enumerator.setPath( "users" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 3 );
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );

		// Test JSON Pointer navigation
		EXPECT_TRUE( enumerator.setPointer( "/scores" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 5 );
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );
	}

	TEST_F( ArrayEnumeratorTest, InvalidPathHandling )
	{
		ArrayEnumerator enumerator( document );

		// Test invalid paths
		EXPECT_FALSE( enumerator.setPath( "nonexistent" ) );
		EXPECT_FALSE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 0 );

		// Test non-array path
		EXPECT_FALSE( enumerator.setPath( "users.0" ) );
		EXPECT_FALSE( enumerator.isValid() );

		// Test invalid JSON Pointer
		EXPECT_FALSE( enumerator.setPointer( "/invalid/path" ) );
		EXPECT_FALSE( enumerator.isValid() );

		// Test empty path
		EXPECT_FALSE( enumerator.setPath( "" ) );
		EXPECT_FALSE( enumerator.isValid() );
	}

	//----------------------------------------------
	// Element access
	//----------------------------------------------

	TEST_F( ArrayEnumeratorTest, PrimitiveElementAccess )
	{
		ArrayEnumerator enumerator( document );

		// Test integer array access
		ASSERT_TRUE( enumerator.setPath( "scores" ) );

		auto score0 = enumerator.currentInt();
		ASSERT_TRUE( score0.has_value() );
		EXPECT_EQ( *score0, 100 );

		EXPECT_TRUE( enumerator.next() );
		auto score1 = enumerator.currentInt();
		ASSERT_TRUE( score1.has_value() );
		EXPECT_EQ( *score1, 95 );

		// Test string array access
		ASSERT_TRUE( enumerator.setPath( "tags" ) );

		auto tag0 = enumerator.currentString();
		ASSERT_TRUE( tag0.has_value() );
		EXPECT_EQ( *tag0, "important" );

		EXPECT_TRUE( enumerator.next() );
		auto tag1 = enumerator.currentString();
		ASSERT_TRUE( tag1.has_value() );
		EXPECT_EQ( *tag1, "urgent" );
	}

	TEST_F( ArrayEnumeratorTest, ObjectElementAccess )
	{
		ArrayEnumerator enumerator( document );

		// Test object array access
		ASSERT_TRUE( enumerator.setPath( "users" ) );

		// Get first user as Document
		Document user0 = enumerator.currentElement();
		auto name = user0.getString( "name" );
		auto age = user0.getInt( "age" );
		auto active = user0.getBool( "active" );

		ASSERT_TRUE( name.has_value() );
		EXPECT_EQ( *name, "Alice" );
		ASSERT_TRUE( age.has_value() );
		EXPECT_EQ( *age, 30 );
		ASSERT_TRUE( active.has_value() );
		EXPECT_TRUE( *active );

		// Move to next user
		EXPECT_TRUE( enumerator.next() );
		Document user1 = enumerator.currentElement();
		name = user1.getString( "name" );
		age = user1.getInt( "age" );
		active = user1.getBool( "active" );

		ASSERT_TRUE( name.has_value() );
		EXPECT_EQ( *name, "Bob" );
		ASSERT_TRUE( age.has_value() );
		EXPECT_EQ( *age, 25 );
		ASSERT_TRUE( active.has_value() );
		EXPECT_FALSE( *active );
	}

	TEST_F( ArrayEnumeratorTest, WrongTypeAccess )
	{
		ArrayEnumerator enumerator( document );

		// Test accessing string array as int
		ASSERT_TRUE( enumerator.setPath( "tags" ) );

		auto intValue = enumerator.currentInt();
		EXPECT_FALSE( intValue.has_value() );

		auto stringValue = enumerator.currentString();
		EXPECT_TRUE( stringValue.has_value() );
		EXPECT_EQ( *stringValue, "important" );

		// Test accessing int array as string
		ASSERT_TRUE( enumerator.setPath( "scores" ) );

		auto stringFromInt = enumerator.currentString();
		EXPECT_FALSE( stringFromInt.has_value() );

		auto intFromInt = enumerator.currentInt();
		EXPECT_TRUE( intFromInt.has_value() );
		EXPECT_EQ( *intFromInt, 100 );
	}

	//----------------------------------------------
	// Enumerator movement
	//----------------------------------------------

	TEST_F( ArrayEnumeratorTest, ForwardEnumeration )
	{
		ArrayEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "scores" ) );

		std::vector<int> expectedScores = { 100, 95, 87, 92, 78 };
		std::vector<int> actualScores;

		// enumeratorate through all elements
		while ( !enumerator.isEnd() )
		{
			auto score = enumerator.currentInt();
			ASSERT_TRUE( score.has_value() );
			actualScores.push_back( *score );

			if ( !enumerator.next() )
				break;
		}

		EXPECT_EQ( actualScores, expectedScores );
		EXPECT_TRUE( enumerator.isEnd() );
		EXPECT_EQ( enumerator.index(), enumerator.size() );
	}

	TEST_F( ArrayEnumeratorTest, RandomAccess )
	{
		ArrayEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "tags" ) );

		// Test moveTo specific indices
		EXPECT_TRUE( enumerator.moveTo( 2 ) );
		EXPECT_EQ( enumerator.index(), 2 );
		auto tag = enumerator.currentString();
		ASSERT_TRUE( tag.has_value() );
		EXPECT_EQ( *tag, "review" );

		EXPECT_TRUE( enumerator.moveTo( 0 ) );
		EXPECT_EQ( enumerator.index(), 0 );
		tag = enumerator.currentString();
		ASSERT_TRUE( tag.has_value() );
		EXPECT_EQ( *tag, "important" );

		EXPECT_TRUE( enumerator.moveTo( 3 ) );
		EXPECT_EQ( enumerator.index(), 3 );
		tag = enumerator.currentString();
		ASSERT_TRUE( tag.has_value() );
		EXPECT_EQ( *tag, "critical" );

		// Test out of bounds
		EXPECT_FALSE( enumerator.moveTo( 10 ) );
		EXPECT_EQ( enumerator.index(), 3 ); // Should stay at last valid position
	}

	TEST_F( ArrayEnumeratorTest, BackwardMovement )
	{
		ArrayEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "scores" ) );

		// Move to end
		EXPECT_TRUE( enumerator.moveTo( 4 ) );
		EXPECT_EQ( enumerator.index(), 4 );

		// Move backwards
		EXPECT_TRUE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 3 );
		auto score = enumerator.currentInt();
		ASSERT_TRUE( score.has_value() );
		EXPECT_EQ( *score, 92 );

		EXPECT_TRUE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 2 );
		score = enumerator.currentInt();
		ASSERT_TRUE( score.has_value() );
		EXPECT_EQ( *score, 87 );

		// Try to move before beginning
		enumerator.reset();
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.previous() );
		EXPECT_EQ( enumerator.index(), 0 );
	}

	TEST_F( ArrayEnumeratorTest, ResetFunctionality )
	{
		ArrayEnumerator enumerator( document );
		ASSERT_TRUE( enumerator.setPath( "tags" ) );

		// Move to middle
		EXPECT_TRUE( enumerator.moveTo( 2 ) );
		EXPECT_EQ( enumerator.index(), 2 );

		// Reset to beginning
		enumerator.reset();
		EXPECT_EQ( enumerator.index(), 0 );
		EXPECT_FALSE( enumerator.isEnd() );

		auto tag = enumerator.currentString();
		ASSERT_TRUE( tag.has_value() );
		EXPECT_EQ( *tag, "important" );
	}

	//----------------------------------------------
	// Edge case
	//----------------------------------------------

	TEST_F( ArrayEnumeratorTest, EmptyArrayHandling )
	{
		ArrayEnumerator enumerator( document );

		ASSERT_TRUE( enumerator.setPath( "empty" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 0 );
		EXPECT_TRUE( enumerator.isEnd() );
		EXPECT_EQ( enumerator.index(), 0 );

		// Try to access element in empty array
		EXPECT_THROW( enumerator.currentElement(), std::runtime_error );
		EXPECT_FALSE( enumerator.currentString().has_value() );
		EXPECT_FALSE( enumerator.currentInt().has_value() );

		// Try to move in empty array
		EXPECT_FALSE( enumerator.next() );
		EXPECT_FALSE( enumerator.previous() );
		EXPECT_FALSE( enumerator.moveTo( 0 ) );
	}

	TEST_F( ArrayEnumeratorTest, MixedTypeArray )
	{
		ArrayEnumerator enumerator( document );

		ASSERT_TRUE( enumerator.setPath( "mixed" ) );
		EXPECT_EQ( enumerator.size(), 5 );

		// Element 0: integer
		auto intVal = enumerator.currentInt();
		EXPECT_TRUE( intVal.has_value() );
		EXPECT_EQ( *intVal, 1 );

		// Element 1: string
		EXPECT_TRUE( enumerator.next() );
		auto strVal = enumerator.currentString();
		EXPECT_TRUE( strVal.has_value() );
		EXPECT_EQ( *strVal, "text" );

		// Element 2: boolean
		EXPECT_TRUE( enumerator.next() );
		auto boolVal = enumerator.currentBool();
		EXPECT_TRUE( boolVal.has_value() );
		EXPECT_TRUE( *boolVal );

		// Element 3: null (should not be accessible as typed values)
		EXPECT_TRUE( enumerator.next() );
		EXPECT_FALSE( enumerator.currentString().has_value() );
		EXPECT_FALSE( enumerator.currentInt().has_value() );
		EXPECT_FALSE( enumerator.currentBool().has_value() );

		// Element 4: object
		EXPECT_TRUE( enumerator.next() );
		Document objDoc = enumerator.currentElement();
		auto nestedVal = objDoc.getString( "nested" );
		EXPECT_TRUE( nestedVal.has_value() );
		EXPECT_EQ( *nestedVal, "object" );
	}

	//----------------------------------------------
	// JSON Pointer specific tests
	//----------------------------------------------

	TEST_F( ArrayEnumeratorTest, NestedArrayAccess )
	{
		ArrayEnumerator enumerator( document );

		// Test deeply nested array access with JSON Pointer
		EXPECT_TRUE( enumerator.setPointer( "/nested/levels/deep/array" ) );
		EXPECT_TRUE( enumerator.isValid() );
		EXPECT_EQ( enumerator.size(), 3 );

		auto deep1 = enumerator.currentString();
		ASSERT_TRUE( deep1.has_value() );
		EXPECT_EQ( *deep1, "deep1" );

		EXPECT_TRUE( enumerator.next() );
		auto deep2 = enumerator.currentString();
		ASSERT_TRUE( deep2.has_value() );
		EXPECT_EQ( *deep2, "deep2" );
	}

	TEST_F( ArrayEnumeratorTest, EnumeratorStateConsistency )
	{
		ArrayEnumerator enumerator( document );

		// Test state consistency when switching between arrays
		ASSERT_TRUE( enumerator.setPath( "users" ) );
		EXPECT_EQ( enumerator.size(), 3 );
		EXPECT_TRUE( enumerator.moveTo( 1 ) );
		EXPECT_EQ( enumerator.index(), 1 );

		// Switch to different array
		ASSERT_TRUE( enumerator.setPath( "scores" ) );
		EXPECT_EQ( enumerator.size(), 5 );
		EXPECT_EQ( enumerator.index(), 0 ); // Should reset when changing arrays
		EXPECT_FALSE( enumerator.isEnd() );

		// Verify we're at the right array
		auto score = enumerator.currentInt();
		ASSERT_TRUE( score.has_value() );
		EXPECT_EQ( *score, 100 );
	}
} // namespace nfx::serialization::json::test
