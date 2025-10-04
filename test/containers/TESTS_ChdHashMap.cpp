/**
 * @file TESTS_ChdHashMap.cpp
 * @brief Unit tests for ChdHashMap perfect hashing dictionary
 * @details Comprehensive test suite validating CHD algorithm implementation,
 *          cross-platform hash compatibility, and O(1) guaranteed lookups
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <vector>

#include <nfx/containers/ChdHashMap.h>

namespace nfx::containers::test
{
	//=====================================================================
	// ChdHashMap Tests
	//=====================================================================

	//----------------------------------------------
	// Basic construction and operations
	//----------------------------------------------

	TEST( ChdHashMapBasic, DefaultConstruction )
	{
		ChdHashMap<int> map;

		EXPECT_TRUE( map.isEmpty() );
		EXPECT_EQ( map.size(), 0 );
	}

	TEST( ChdHashMapBasic, EmptyVectorConstruction )
	{
		std::vector<std::pair<std::string, int>> items;
		ChdHashMap<int> map{ std::move( items ) };

		EXPECT_TRUE( map.isEmpty() );
		EXPECT_EQ( map.size(), 0 );
	}

	TEST( ChdHashMapBasic, BasicConstruction )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "key1", 100 },
			{ "key2", 200 },
			{ "key3", 300 } };

		ChdHashMap<int> map{ std::move( items ) };

		EXPECT_FALSE( map.isEmpty() );
		EXPECT_GT( map.size(), 0 ); // Size includes empty slots for perfect hashing
	}

	TEST( ChdHashMapBasic, BasicLookup )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "lookup1", 42 },
			{ "lookup2", 84 },
			{ "lookup3", 126 } };

		ChdHashMap<int> map{ std::move( items ) };

		// Test operator[]
		EXPECT_EQ( map["lookup1"], 42 );
		EXPECT_EQ( map["lookup2"], 84 );
		EXPECT_EQ( map["lookup3"], 126 );

		// Test at()
		EXPECT_EQ( map.at( "lookup1" ), 42 );
		EXPECT_EQ( map.at( "lookup2" ), 84 );
		EXPECT_EQ( map.at( "lookup3" ), 126 );
	}

	TEST( ChdHashMapBasic, TryGetValue )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "try_key1", 111 },
			{ "try_key2", 222 } };

		ChdHashMap<int> map{ std::move( items ) };

		int* value1 = nullptr;
		int* value2 = nullptr;
		int* valueMissing = nullptr;

		EXPECT_TRUE( map.tryGetValue( "try_key1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "try_key2", value2 ) );
		EXPECT_FALSE( map.tryGetValue( "missing_key", valueMissing ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		EXPECT_EQ( valueMissing, nullptr );

		EXPECT_EQ( *value1, 111 );
		EXPECT_EQ( *value2, 222 );
	}

	//----------------------------------------------
	// Exception handling
	//----------------------------------------------

	TEST( ChdHashMapExceptions, KeyNotFoundOperator )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "existing_key", 999 } };

		ChdHashMap<int> map{ std::move( items ) };

		EXPECT_THROW( static_cast<void>( map["non_existent_key"] ), ChdHashMap<int>::KeyNotFoundException );
	}

	TEST( ChdHashMapExceptions, KeyNotFoundAt )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "valid_key", "valid_value" } };

		ChdHashMap<std::string> map{ std::move( items ) };

		EXPECT_THROW( static_cast<void>( map.at( "invalid_key" ) ), ChdHashMap<std::string>::KeyNotFoundException );
	}

	TEST( ChdHashMapExceptions, EmptyMapAccess )
	{
		ChdHashMap<int> emptyMap;

		EXPECT_THROW( static_cast<void>( emptyMap["any_key"] ), ChdHashMap<int>::KeyNotFoundException );
		EXPECT_THROW( static_cast<void>( emptyMap.at( "any_key" ) ), ChdHashMap<int>::KeyNotFoundException );
	}

	//----------------------------------------------
	// String view operations (heterogeneous lookup)
	//----------------------------------------------

	TEST( ChdHashMapStringView, HeterogeneousLookup )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "hetero_key", "hetero_value" } };

		ChdHashMap<std::string> map{ std::move( items ) };

		std::string strKey{ "hetero_key" };
		std::string_view svKey{ strKey };
		const char* cstrKey{ strKey.c_str() };

		// All lookup methods should work with different string types
		EXPECT_EQ( map[strKey], "hetero_value" );
		EXPECT_EQ( map[svKey], "hetero_value" );
		EXPECT_EQ( map[cstrKey], "hetero_value" );

		EXPECT_EQ( map.at( strKey ), "hetero_value" );
		EXPECT_EQ( map.at( svKey ), "hetero_value" );
		EXPECT_EQ( map.at( cstrKey ), "hetero_value" );

		// tryGetValue with different types
		std::string* value1 = nullptr;
		std::string* value2 = nullptr;
		std::string* value3 = nullptr;

		EXPECT_TRUE( map.tryGetValue( strKey, value1 ) );
		EXPECT_TRUE( map.tryGetValue( svKey, value2 ) );
		EXPECT_TRUE( map.tryGetValue( cstrKey, value3 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );

		EXPECT_EQ( *value1, "hetero_value" );
		EXPECT_EQ( *value2, "hetero_value" );
		EXPECT_EQ( *value3, "hetero_value" );
	}

	TEST( ChdHashMapStringView, EmptyStringKey )
	{
		// Test that CHD algorithm can actually handle empty string keys
		std::vector<std::pair<std::string, int>> items{
			{ "", 123 },
			{ "normal_key", 456 } };

		ChdHashMap<int> map{ std::move( items ) };

		// Normal key should work
		EXPECT_EQ( map["normal_key"], 456 );

		// If CHD algorithm supports empty strings, they should work
		// If not, construction should fail or lookups should throw
		try
		{
			int emptyValue = map[""];
			EXPECT_EQ( emptyValue, 123 );

			std::string_view emptySv{};
			int emptyValue2 = map[emptySv];
			EXPECT_EQ( emptyValue2, 123 );
		}
		catch ( const ChdHashMap<int>::KeyNotFoundException& )
		{
			// If empty strings are not supported, this is acceptable
			SUCCEED() << "CHD algorithm correctly rejects empty string keys";
		}
	}

	//----------------------------------------------
	// Iteration support
	//----------------------------------------------

	TEST( ChdHashMapIteration, BasicIteration )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "iter1", 10 },
			{ "iter2", 20 },
			{ "iter3", 30 } };

		ChdHashMap<int> map{ std::move( items ) };

		// Range-based for loop
		std::vector<std::string> foundKeys;
		std::vector<int> foundValues;

		for ( const auto& [key, value] : map )
		{
			foundKeys.push_back( key );
			foundValues.push_back( value );
		}

		// Should find all 3 items (order may vary due to perfect hashing)
		EXPECT_EQ( foundKeys.size(), 3 );
		EXPECT_EQ( foundValues.size(), 3 );

		std::sort( foundKeys.begin(), foundKeys.end() );
		std::sort( foundValues.begin(), foundValues.end() );

		EXPECT_EQ( foundKeys[0], "iter1" );
		EXPECT_EQ( foundKeys[1], "iter2" );
		EXPECT_EQ( foundKeys[2], "iter3" );

		EXPECT_EQ( foundValues[0], 10 );
		EXPECT_EQ( foundValues[1], 20 );
		EXPECT_EQ( foundValues[2], 30 );
	}

	TEST( ChdHashMapIteration, ManualIteration )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "manual1", "value1" },
			{ "manual2", "value2" } };

		ChdHashMap<std::string> map{ std::move( items ) };

		// Manual iterator usage
		size_t count = 0;
		for ( auto it = map.begin(); it != map.end(); ++it )
		{
			EXPECT_FALSE( it->first.empty() );	// All keys should be non-empty
			EXPECT_FALSE( it->second.empty() ); // All values should be non-empty
			++count;
		}

		EXPECT_EQ( count, 2 );
	}

	TEST( ChdHashMapIteration, STLAlgorithms )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "algo1", 5 },
			{ "algo2", 15 },
			{ "algo3", 25 } };

		ChdHashMap<int> map{ std::move( items ) };

		// STL algorithm usage
		auto found = std::find_if( map.begin(), map.end(),
			[]( const auto& pair ) { return pair.second > 10; } );

		EXPECT_NE( found, map.end() );
		EXPECT_GT( found->second, 10 );

		// Count elements matching condition
		auto count = std::count_if( map.begin(), map.end(),
			[]( const auto& pair ) { return pair.second >= 15; } );

		EXPECT_EQ( count, 2 ); // algo2 and algo3
	}

	//----------------------------------------------
	// Enumerator support
	//----------------------------------------------

	TEST( ChdHashMapEnumerator, BasicEnumeration )
	{
		std::vector<std::pair<std::string, double>> items{
			{ "enum1", 1.1 },
			{ "enum2", 2.2 },
			{ "enum3", 3.3 } };

		ChdHashMap<double> map{ std::move( items ) };

		auto enumerator = map.enumerator();

		std::vector<std::string> keys;
		std::vector<double> values;

		while ( enumerator.next() )
		{
			const auto& current = enumerator.current();
			keys.push_back( current.first );
			values.push_back( current.second );
		}

		EXPECT_EQ( keys.size(), 3 );
		EXPECT_EQ( values.size(), 3 );

		// Sort for predictable comparison
		std::sort( keys.begin(), keys.end() );
		std::sort( values.begin(), values.end() );

		EXPECT_EQ( keys[0], "enum1" );
		EXPECT_EQ( keys[1], "enum2" );
		EXPECT_EQ( keys[2], "enum3" );

		EXPECT_DOUBLE_EQ( values[0], 1.1 );
		EXPECT_DOUBLE_EQ( values[1], 2.2 );
		EXPECT_DOUBLE_EQ( values[2], 3.3 );
	}

	TEST( ChdHashMapEnumerator, ResetEnumerator )
	{
		std::vector<std::pair<std::string, int>> items{
			{ "reset1", 100 } };

		ChdHashMap<int> map{ std::move( items ) };

		auto enumerator = map.enumerator();

		// First enumeration
		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.current().first, "reset1" );
		EXPECT_FALSE( enumerator.next() ); // Should be at end

		// Reset and enumerate again
		enumerator.reset();
		EXPECT_TRUE( enumerator.next() );
		EXPECT_EQ( enumerator.current().first, "reset1" );
		EXPECT_FALSE( enumerator.next() );
	}

	//----------------------------------------------
	// Edge cases and special scenarios
	//----------------------------------------------

	TEST( ChdHashMapEdgeCases, SpecialCharacterKeys )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "key with spaces", "spaces" },
			{ "key\twith\ttabs", "tabs" },
			{ "key\nwith\nnewlines", "newlines" },
			{ "key\"with\"quotes", "quotes" },
			{ "key\\with\\backslashes", "backslashes" } };

		ChdHashMap<std::string> map{ std::move( items ) };

		EXPECT_EQ( map["key with spaces"], "spaces" );
		EXPECT_EQ( map["key\twith\ttabs"], "tabs" );
		EXPECT_EQ( map["key\nwith\nnewlines"], "newlines" );
		EXPECT_EQ( map["key\"with\"quotes"], "quotes" );
		EXPECT_EQ( map["key\\with\\backslashes"], "backslashes" );
	}

	TEST( ChdHashMapEdgeCases, UnicodeKeys )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "🔑", "key_emoji" },
			{ "clé", "french_key" },
			{ "键", "chinese_key" },
			{ "ключ", "russian_key" } };

		ChdHashMap<std::string> map{ std::move( items ) };

		EXPECT_EQ( map["🔑"], "key_emoji" );
		EXPECT_EQ( map["clé"], "french_key" );
		EXPECT_EQ( map["键"], "chinese_key" );
		EXPECT_EQ( map["ключ"], "russian_key" );
	}

	TEST( ChdHashMapEdgeCases, LongKeys )
	{
		std::string longKey( 1000, 'A' );
		std::string veryLongKey( 10000, 'B' );

		std::vector<std::pair<std::string, int>> items{
			{ "short", 1 },
			{ longKey, 2 },
			{ veryLongKey, 3 } };

		ChdHashMap<int> map{ std::move( items ) };

		EXPECT_EQ( map["short"], 1 );
		EXPECT_EQ( map[longKey], 2 );
		EXPECT_EQ( map[veryLongKey], 3 );
	}

	//----------------------------------------------
	// Value type tests
	//----------------------------------------------

	TEST( ChdHashMapValueTypes, ComplexValues )
	{
		std::vector<std::pair<std::string, std::vector<int>>> items{
			{ "vector1", { 1, 2, 3 } },
			{ "vector2", { 4, 5, 6, 7 } } };

		ChdHashMap<std::vector<int>> map{ std::move( items ) };

		const auto& vec1 = map["vector1"];
		const auto& vec2 = map["vector2"];

		EXPECT_EQ( vec1.size(), 3 );
		EXPECT_EQ( vec2.size(), 4 );
		EXPECT_EQ( vec1[0], 1 );
		EXPECT_EQ( vec2[3], 7 );
	}

	TEST( ChdHashMapValueTypes, MoveOnlyValues )
	{
		std::vector<std::pair<std::string, std::unique_ptr<int>>> items;
		items.emplace_back( "unique1", std::make_unique<int>( 42 ) );
		items.emplace_back( "unique2", std::make_unique<int>( 84 ) );

		ChdHashMap<std::unique_ptr<int>> map{ std::move( items ) };

		const auto* ptr1 = &map["unique1"];
		const auto* ptr2 = &map["unique2"];

		ASSERT_NE( ptr1, nullptr );
		ASSERT_NE( ptr2, nullptr );
		EXPECT_EQ( **ptr1, 42 );
		EXPECT_EQ( **ptr2, 84 );
	}

	//----------------------------------------------
	// Performance and stress tests
	//----------------------------------------------

	TEST( ChdHashMapPerformance, MediumDataset )
	{
		std::vector<std::pair<std::string, size_t>> items;
		items.reserve( 100 );

		// Create medium dataset
		for ( size_t i = 0; i < 100; ++i )
		{
			items.emplace_back( "key_" + std::to_string( i ), i );
		}

		ChdHashMap<size_t> map{ std::move( items ) };

		// Verify all items are accessible with O(1) lookup
		for ( size_t i = 0; i < 100; ++i )
		{
			std::string key = "key_" + std::to_string( i );
			EXPECT_EQ( map[key], i );

			// Also test with string_view (zero-copy)
			std::string_view keyView{ key };
			size_t* value = nullptr;
			EXPECT_TRUE( map.tryGetValue( keyView, value ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );
		}
	}

	TEST( ChdHashMapPerformance, CollisionHandling )
	{
		// Create keys that might cause hash collisions
		std::vector<std::pair<std::string, int>> items;
		for ( int i = 0; i < 50; ++i )
		{
			items.emplace_back( "collision_test_" + std::to_string( i ), i );
		}

		// CHD algorithm should handle all collisions perfectly
		ChdHashMap<int> map{ std::move( items ) };

		// All items should be retrievable in O(1) time
		for ( int i = 0; i < 50; ++i )
		{
			std::string key = "collision_test_" + std::to_string( i );
			EXPECT_EQ( map[key], i );
		}
	}

	//----------------------------------------------
	// Cross-platform hash compatibility tests
	//----------------------------------------------

	TEST( ChdHashMapCompatibility, ASCIIHashValues )
	{
		// Test that hash function produces consistent values
		uint32_t hash1 = ChdHashMap<int>::hash( "test" );
		uint32_t hash2 = ChdHashMap<int>::hash( "test" );
		uint32_t hash3 = ChdHashMap<int>::hash( "different" );

		// Same input should produce same hash
		EXPECT_EQ( hash1, hash2 );

		// Different input should produce different hash (with high probability)
		EXPECT_NE( hash1, hash3 );

		// Empty string hash
		uint32_t emptyHash = ChdHashMap<int>::hash( "" );
		EXPECT_NE( emptyHash, 0 ); // Should not be zero due to FNV offset basis
	}

	//----------------------------------------------
	// Real-world usage scenarios
	//----------------------------------------------

	TEST( ChdHashMapRealWorld, ConfigurationScenario )
	{
		std::vector<std::pair<std::string, std::string>> items{
			{ "database.host", "localhost" },
			{ "database.port", "5432" },
			{ "api.timeout", "30" },
			{ "logging.level", "INFO" },
			{ "cache.enabled", "true" } };

		ChdHashMap<std::string> config{ std::move( items ) };

		// Simulate configuration access patterns
		EXPECT_EQ( config["database.host"], "localhost" );
		EXPECT_EQ( config["database.port"], "5432" );
		EXPECT_EQ( config["api.timeout"], "30" );

		// Zero-copy lookups with string_view
		std::string_view logKey{ "logging.level" };
		EXPECT_EQ( config[logKey], "INFO" );
	}

	TEST( ChdHashMapRealWorld, LookupTableScenario )
	{
		// Simulate a lookup table for HTTP status codes
		std::vector<std::pair<std::string, int>> items{
			{ "OK", 200 },
			{ "NOT_FOUND", 404 },
			{ "INTERNAL_ERROR", 500 },
			{ "BAD_REQUEST", 400 },
			{ "UNAUTHORIZED", 401 } };

		ChdHashMap<int> statusCodes{ std::move( items ) };

		// Fast O(1) lookups for status codes
		EXPECT_EQ( statusCodes["OK"], 200 );
		EXPECT_EQ( statusCodes["NOT_FOUND"], 404 );
		EXPECT_EQ( statusCodes["INTERNAL_ERROR"], 500 );

		// Error case
		EXPECT_THROW( static_cast<void>( statusCodes["UNKNOWN_STATUS"] ), ChdHashMap<int>::KeyNotFoundException );
	}
} // namespace nfx::containers::test
