/**
 * @file TESTS_StringSet.cpp
 * @brief Unit tests for StringSet heterogeneous string container
 * @details Test suite validating zero-copy string_view lookups and
 *          heterogeneous operations in StringSet
 */

#include <gtest/gtest.h>

#include <algorithm>

#include <nfx/containers/StringSet.h>

namespace nfx::containers::test
{
	//=====================================================================
	// StringSet Tests
	//=====================================================================

	//----------------------------------------------
	// Heterogeneous lookup operations
	//----------------------------------------------

	TEST( StringSetHeterogeneousLookup, NoUnnecessaryStringCreation )
	{
		StringSet set{ "lookup_item" };

		// These lookups should not create temporary std::string objects
		std::string_view sv{ "lookup_item" };
		const char* cstr{ "lookup_item" };

		EXPECT_EQ( set.count( sv ), 1 );
		EXPECT_EQ( set.count( cstr ), 1 );
		EXPECT_TRUE( set.contains( sv ) );
		EXPECT_TRUE( set.contains( cstr ) );

		auto it1{ set.find( sv ) };
		auto it2{ set.find( cstr ) };

		EXPECT_NE( it1, set.end() );
		EXPECT_NE( it2, set.end() );
	}

	//----------------------------------------------
	// Insertion
	//----------------------------------------------

	TEST( StringSetInsertion, BasicOperations )
	{
		// NOTE: std::unordered_set::insert() does not support heterogeneous insertion
		// Only find(), count(), contains() etc. support it through is_transparent

		StringSet set;

		// Insert with different string types
		set.insert( "set_item1" );
		set.insert( std::string{ "set_item2" } );
		set.insert( std::string{ std::string_view{ "set_item3" } } ); // Convert string_view to string for insertion

		EXPECT_EQ( set.size(), 3 );

		// Heterogeneous lookups
		EXPECT_TRUE( set.contains( "set_item1" ) );
		EXPECT_TRUE( set.contains( std::string_view{ "set_item2" } ) );
		EXPECT_TRUE( set.contains( std::string{ "set_item3" } ) );
		EXPECT_FALSE( set.contains( "missing_item" ) );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( StringSetEdgeCases, EmptyAndSpecialStrings )
	{
		StringSet set;

		// Empty string
		set.insert( "" );
		EXPECT_TRUE( set.contains( "" ) );
		EXPECT_TRUE( set.contains( std::string_view{} ) );

		// Special characters
		set.insert( "string with spaces" );
		set.insert( "string\nwith\nnewlines" );

		EXPECT_EQ( set.size(), 3 );
		EXPECT_TRUE( set.contains( std::string_view{ "string with spaces" } ) );
	}

	//----------------------------------------------
	// STL compatibility
	//----------------------------------------------

	TEST( StringSetSTLCompatibility, IteratorAndAlgorithms )
	{
		StringSet set{ "item1", "item2", "item3" };

		// Range-based for loop
		size_t count{ 0 };
		for ( const auto& item : set )
		{
			(void)item;
			++count;
		}
		EXPECT_EQ( count, 3 );

		// STL algorithm
		auto found{ std::find( set.begin(), set.end(), "item2" ) };
		EXPECT_NE( found, set.end() );
	}
}
