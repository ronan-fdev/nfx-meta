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

	TEST( StringSetHeterogeneousLookup, ComprehensiveContains )
	{
		StringSet set;
		set.insert( "test_key" );
		set.insert( "another_key" );
		set.insert( "" ); // Empty string

		// Test all contains() overloads
		const char* cstr = "test_key";
		char mutable_str[] = "test_key";
		std::string_view sv = "test_key";
		std::string str = "test_key";

		// All should find the key
		EXPECT_TRUE( set.contains( cstr ) );
		EXPECT_TRUE( set.contains( mutable_str ) );
		EXPECT_TRUE( set.contains( sv ) );
		EXPECT_TRUE( set.contains( str ) );

		// Test missing keys
		EXPECT_FALSE( set.contains( "missing_key" ) );
		EXPECT_FALSE( set.contains( std::string_view{ "missing_key" } ) );

		// Test empty string
		EXPECT_TRUE( set.contains( "" ) );
		EXPECT_TRUE( set.contains( std::string_view{} ) );
	}

	TEST( StringSetHeterogeneousLookup, FindOperations )
	{
		StringSet set{ "find_me", "also_find_me", "third_item" };

		// Test heterogeneous find operations
		auto it1 = set.find( "find_me" );						   // const char*
		auto it2 = set.find( std::string_view{ "also_find_me" } ); // string_view
		auto it3 = set.find( std::string{ "third_item" } );		   // std::string

		EXPECT_NE( it1, set.end() );
		EXPECT_NE( it2, set.end() );
		EXPECT_NE( it3, set.end() );

		EXPECT_EQ( *it1, "find_me" );
		EXPECT_EQ( *it2, "also_find_me" );
		EXPECT_EQ( *it3, "third_item" );

		// Test missing key
		auto it4 = set.find( "not_found" );
		EXPECT_EQ( it4, set.end() );
	}

	TEST( StringSetHeterogeneousLookup, PerformanceComparison )
	{
		// Create a set with many entries
		StringSet nfx_set;
		std::unordered_set<std::string> std_set;

		for ( int i = 0; i < 1000; ++i )
		{
			std::string key = "key_" + std::to_string( i );
			nfx_set.insert( key );
			std_set.insert( key );
		}

		const char* lookup_key = "key_500";
		std::string_view sv_key{ lookup_key };

		// Both should find the item
		EXPECT_TRUE( nfx_set.contains( lookup_key ) );				// No temporary string
		EXPECT_EQ( std_set.count( std::string{ lookup_key } ), 1 ); // Creates temporary string

		EXPECT_TRUE( nfx_set.contains( sv_key ) );				// No temporary string
		EXPECT_EQ( std_set.count( std::string{ sv_key } ), 1 ); // Creates temporary string
	}

	TEST( StringSetHeterogeneousLookup, EdgeCases )
	{
		StringSet set;

		// Empty string handling
		set.insert( "" );
		EXPECT_TRUE( set.contains( "" ) );
		EXPECT_TRUE( set.contains( std::string_view{} ) );
		EXPECT_TRUE( set.contains( std::string{} ) );

		// Very long strings
		std::string long_key( 1000, 'x' );
		set.insert( long_key );
		EXPECT_TRUE( set.contains( long_key ) );
		EXPECT_TRUE( set.contains( std::string_view{ long_key } ) );

		// Null termination issues
		char buffer[] = "testXXX";
		buffer[4] = '\0'; // Truncate to "test"
		set.insert( buffer );
		EXPECT_TRUE( set.contains( "test" ) );
		EXPECT_TRUE( set.contains( std::string_view{ "test" } ) );
	}

	//----------------------------------------------
	// Insertion
	//----------------------------------------------

	TEST( StringSetInsertion, BasicOperations )
	{
		StringSet set;

		// Insert with different string types
		auto result1 = set.insert( "set_item1" );
		auto result2 = set.insert( std::string{ "set_item2" } );
		auto result3 = set.insert( std::string_view{ "set_item3" } );

		EXPECT_TRUE( result1.second ); // New insertion
		EXPECT_TRUE( result2.second ); // New insertion
		EXPECT_TRUE( result3.second ); // New insertion
		EXPECT_EQ( set.size(), 3 );

		// Try to insert duplicates
		auto result4 = set.insert( "set_item1" );
		auto result5 = set.insert( std::string_view{ "set_item2" } );

		EXPECT_FALSE( result4.second ); // Duplicate
		EXPECT_FALSE( result5.second ); // Duplicate
		EXPECT_EQ( set.size(), 3 );		// Size unchanged

		// Heterogeneous lookups
		EXPECT_TRUE( set.contains( "set_item1" ) );
		EXPECT_TRUE( set.contains( std::string_view{ "set_item2" } ) );
		EXPECT_TRUE( set.contains( std::string{ "set_item3" } ) );
		EXPECT_FALSE( set.contains( "missing_item" ) );
	}

	TEST( StringSetInsertion, HeterogeneousInsert )
	{
		StringSet set;

		// Test all heterogeneous insert overloads
		const char* cstr = "const_char_key";
		char mutable_str[] = "mutable_char_key";
		std::string_view sv = "string_view_key";
		std::string str = "string_key";

		auto [it1, inserted1] = set.insert( cstr );
		auto [it2, inserted2] = set.insert( mutable_str );
		auto [it3, inserted3] = set.insert( sv );
		auto [it4, inserted4] = set.insert( str );

		EXPECT_TRUE( inserted1 );
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );
		EXPECT_TRUE( inserted4 );
		EXPECT_EQ( set.size(), 4 );

		// Verify all were inserted correctly
		EXPECT_EQ( *it1, "const_char_key" );
		EXPECT_EQ( *it2, "mutable_char_key" );
		EXPECT_EQ( *it3, "string_view_key" );
		EXPECT_EQ( *it4, "string_key" );
	}

	TEST( StringSetInsertion, HeterogeneousEmplace )
	{
		StringSet set;

		// Test all heterogeneous emplace overloads
		const char* cstr = "emplace_const_char";
		char mutable_str[] = "emplace_mutable_char";
		std::string_view sv = "emplace_string_view";

		auto [it1, inserted1] = set.emplace( cstr );
		auto [it2, inserted2] = set.emplace( mutable_str );
		auto [it3, inserted3] = set.emplace( sv );

		EXPECT_TRUE( inserted1 );
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );
		EXPECT_EQ( set.size(), 3 );

		// Verify emplace worked correctly
		EXPECT_TRUE( set.contains( "emplace_const_char" ) );
		EXPECT_TRUE( set.contains( "emplace_mutable_char" ) );
		EXPECT_TRUE( set.contains( "emplace_string_view" ) );
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
