/**
 * @file TESTS_StringMap.cpp
 * @brief Unit tests for StringMap heterogeneous string container
 * @details Comprehensive test suite validating zero-copy string_view lookups,
 *          heterogeneous operations, and STL compatibility in StringMap
 */

#include <algorithm>

#include <gtest/gtest.h>

#include <nfx/containers/StringMap.h>

namespace nfx::containers::test
{
	//=====================================================================
	// StringMap tests
	//=====================================================================

	//----------------------------------------------
	// Basic operations
	//----------------------------------------------

	TEST( StringMapBasic, BasicConstruction )
	{
		StringMap<int> map;

		EXPECT_TRUE( map.empty() );
		EXPECT_EQ( map.size(), 0 );
	}

	TEST( StringMapBasic, InitializerListConstruction )
	{
		StringMap<int> map{
			{ "key1", 1 },
			{ "key2", 2 },
			{ "key3", 3 } };

		EXPECT_EQ( map.size(), 3 );
		EXPECT_EQ( map["key1"], 1 );
		EXPECT_EQ( map["key2"], 2 );
		EXPECT_EQ( map["key3"], 3 );
	}

	TEST( StringMapBasic, HeterogeneousInsertion )
	{
		StringMap<std::string> map;

		// Insert with different string types
		map["std_string_key"] = "value1";
		map[std::string_view{ "string_view_key" }] = "value2";
		map["c_string_key"] = "value3";

		// Test mutable char* support
		char mutable_key[] = "mutable_key";
		map[mutable_key] = "value4";

		EXPECT_EQ( map.size(), 4 );
		EXPECT_EQ( map["std_string_key"], "value1" );
		EXPECT_EQ( map["string_view_key"], "value2" );
		EXPECT_EQ( map["c_string_key"], "value3" );
		EXPECT_EQ( map["mutable_key"], "value4" );
	}

	TEST( StringMapBasic, HeterogeneousLookup )
	{
		StringMap<int> map{
			{ "lookup_test", 42 } };

		std::string strKey{ "lookup_test" };
		std::string_view svKey{ strKey };
		const char* cstrKey{ strKey.c_str() };
		char mutableKey[] = "lookup_test";

		// All lookup methods should work
		EXPECT_EQ( map[strKey], 42 );
		EXPECT_EQ( map[svKey], 42 );
		EXPECT_EQ( map[cstrKey], 42 );
		EXPECT_EQ( map[mutableKey], 42 );

		// find() should work with all types
		EXPECT_NE( map.find( strKey ), map.end() );
		EXPECT_NE( map.find( svKey ), map.end() );
		EXPECT_NE( map.find( cstrKey ), map.end() );
		EXPECT_NE( map.find( mutableKey ), map.end() );
	}

	TEST( StringMapBasic, TryEmplaceHeterogeneous )
	{
		StringMap<std::string> map;

		// try_emplace with different key types
		auto [iter1, inserted1] = map.try_emplace( "key1", "value1" );
		auto [iter2, inserted2] = map.try_emplace( std::string_view{ "key2" }, "value2" );
		auto [iter3, inserted3] = map.try_emplace( std::string{ "key3" }, "value3" );

		// Test mutable char* support
		char mutable_key[] = "key4";
		auto [iter4, inserted4] = map.try_emplace( mutable_key, "value4" );

		EXPECT_TRUE( inserted1 );
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );
		EXPECT_TRUE( inserted4 );

		EXPECT_EQ( iter1->second, "value1" );
		EXPECT_EQ( iter2->second, "value2" );
		EXPECT_EQ( iter3->second, "value3" );
		EXPECT_EQ( iter4->second, "value4" );

		// Try to emplace existing keys
		auto [iter5, inserted5] = map.try_emplace( "key1", "new_value1" );
		auto [iter6, inserted6] = map.try_emplace( std::string_view{ "key2" }, "new_value2" );

		char existing_key[] = "key4";
		auto [iter7, inserted7] = map.try_emplace( existing_key, "new_value4" );

		EXPECT_FALSE( inserted5 );
		EXPECT_FALSE( inserted6 );
		EXPECT_FALSE( inserted7 );

		// Values should remain unchanged
		EXPECT_EQ( iter5->second, "value1" );
		EXPECT_EQ( iter6->second, "value2" );
		EXPECT_EQ( iter7->second, "value4" );
	}

	TEST( StringMapBasic, InsertOrAssignHeterogeneous )
	{
		StringMap<std::string> map;

		// insert_or_assign with different key types - new keys
		auto [iter1, inserted1] = map.insert_or_assign( "key1", "value1" );
		auto [iter2, inserted2] = map.insert_or_assign( std::string_view{ "key2" }, "value2" );
		auto [iter3, inserted3] = map.insert_or_assign( std::string{ "key3" }, "value3" );

		// Test mutable char* support - new key
		char mutable_key[] = "key4";
		auto [iter4, inserted4] = map.insert_or_assign( mutable_key, "value4" );

		EXPECT_TRUE( inserted1 ); // All should be insertions
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );
		EXPECT_TRUE( inserted4 );

		EXPECT_EQ( iter1->second, "value1" );
		EXPECT_EQ( iter2->second, "value2" );
		EXPECT_EQ( iter3->second, "value3" );
		EXPECT_EQ( iter4->second, "value4" );

		// insert_or_assign existing keys - should assign (update)
		auto [iter5, inserted5] = map.insert_or_assign( "key1", "updated_value1" );
		auto [iter6, inserted6] = map.insert_or_assign( std::string_view{ "key2" }, "updated_value2" );

		char existing_key[] = "key4";
		auto [iter7, inserted7] = map.insert_or_assign( existing_key, "updated_value4" );

		EXPECT_FALSE( inserted5 ); // All should be assignments
		EXPECT_FALSE( inserted6 );
		EXPECT_FALSE( inserted7 );

		// Values should be updated
		EXPECT_EQ( iter5->second, "updated_value1" );
		EXPECT_EQ( iter6->second, "updated_value2" );
		EXPECT_EQ( iter7->second, "updated_value4" );

		// Verify map size hasn't changed
		EXPECT_EQ( map.size(), 4 );

		// Verify all keys have updated values
		EXPECT_EQ( map["key1"], "updated_value1" );
		EXPECT_EQ( map["key2"], "updated_value2" );
		EXPECT_EQ( map["key3"], "value3" ); // This one wasn't updated
		EXPECT_EQ( map["key4"], "updated_value4" );
	}

	//----------------------------------------------
	// Advanced operations
	//----------------------------------------------

	TEST( StringMapAdvanced, TryEmplaceVsInsertOrAssignComparison )
	{
		StringMap<std::string> map1, map2;

		// Initial state: both maps have the same key
		map1["comparison_key"] = "original_value";
		map2["comparison_key"] = "original_value";

		// try_emplace on existing key - should NOT change value
		auto [iter1, inserted1] = map1.try_emplace( "comparison_key", "try_emplace_value" );
		EXPECT_FALSE( inserted1 );					  // Should not insert
		EXPECT_EQ( iter1->second, "original_value" ); // Value unchanged
		EXPECT_EQ( map1["comparison_key"], "original_value" );

		// insert_or_assign on existing key - should change value
		auto [iter2, inserted2] = map2.insert_or_assign( "comparison_key", "insert_or_assign_value" );
		EXPECT_FALSE( inserted2 );							  // Should not insert (was assignment)
		EXPECT_EQ( iter2->second, "insert_or_assign_value" ); // Value changed
		EXPECT_EQ( map2["comparison_key"], "insert_or_assign_value" );

		// Both methods on new keys - should behave identically
		auto [iter3, inserted3] = map1.try_emplace( "new_key", "new_value" );
		auto [iter4, inserted4] = map2.insert_or_assign( "new_key", "new_value" );

		EXPECT_TRUE( inserted3 );
		EXPECT_TRUE( inserted4 );
		EXPECT_EQ( iter3->second, "new_value" );
		EXPECT_EQ( iter4->second, "new_value" );
		EXPECT_EQ( map1["new_key"], "new_value" );
		EXPECT_EQ( map2["new_key"], "new_value" );
	}

	TEST( StringMapAdvanced, InsertOrAssignWithDifferentValueTypes )
	{
		StringMap<int> map;

		// Test with different value types that are convertible to int
		auto [iter1, inserted1] = map.insert_or_assign( "int_key", 42 );
		auto [iter2, inserted2] = map.insert_or_assign( "short_key", short( 100 ) );
		auto [iter3, inserted3] = map.insert_or_assign( "long_key", 200L );

		EXPECT_TRUE( inserted1 );
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );
		EXPECT_EQ( map["int_key"], 42 );
		EXPECT_EQ( map["short_key"], 100 );
		EXPECT_EQ( map["long_key"], 200 );

		// Update with different convertible types
		auto [iter4, inserted4] = map.insert_or_assign( "int_key", 999L );						// long to int
		auto [iter5, inserted5] = map.insert_or_assign( std::string_view{ "short_key" }, 500 ); // int to int via string_view key

		EXPECT_FALSE( inserted4 );
		EXPECT_FALSE( inserted5 );
		EXPECT_EQ( map["int_key"], 999 );
		EXPECT_EQ( map["short_key"], 500 );
	}

	TEST( StringMapAdvanced, MutableCharPointerSupport )
	{
		StringMap<int> map;

		// Test that mutable char* works correctly
		char key1[] = "mutable_key_1";
		char key2[] = "mutable_key_2";
		char key3[] = "mutable_key_3";

		// Insert using mutable char*
		map[key1] = 100;
		map[key2] = 200;

		// try_emplace using mutable char*
		auto [iter, inserted] = map.try_emplace( key3, 300 );
		EXPECT_TRUE( inserted );
		EXPECT_EQ( iter->second, 300 );

		// Verify all insertions worked
		EXPECT_EQ( map.size(), 3 );
		EXPECT_EQ( map["mutable_key_1"], 100 );
		EXPECT_EQ( map["mutable_key_2"], 200 );
		EXPECT_EQ( map["mutable_key_3"], 300 );

		// Test that modifying the original char array doesn't affect the map
		// (because the key is copied as std::string)
		key1[0] = 'X';								  // Modify original array
		EXPECT_EQ( map["mutable_key_1"], 100 );		  // Map should still work with original key
		EXPECT_EQ( map.count( "Xutable_key_1" ), 0 ); // Modified key shouldn't exist

		// Test lookup with different mutable char* arrays containing same content
		char lookup_key1[] = "mutable_key_1";
		char lookup_key2[] = "mutable_key_2";

		EXPECT_EQ( map[lookup_key1], 100 );
		EXPECT_EQ( map[lookup_key2], 200 );
		EXPECT_EQ( map.count( lookup_key1 ), 1 );
		EXPECT_EQ( map.count( lookup_key2 ), 1 );
	}

	TEST( StringMapAdvanced, EraseOperations )
	{
		// NOTE: std::unordered_map::erase() does not support heterogeneous lookup
		// Only find(), count(), contains() etc. support it through is_transparent

		StringMap<int> map{
			{ "erase1", 1 },
			{ "erase2", 2 },
			{ "erase3", 3 } };

		EXPECT_EQ( map.size(), 3 );

		// Erase with exact key type (std::string or const char*)
		EXPECT_EQ( map.erase( "erase1" ), 1 );
		EXPECT_EQ( map.erase( std::string{ "erase2" } ), 1 );

		// For string_view, we need to find first, then erase the iterator
		std::string_view svKey{ "erase3" };
		auto it = map.find( svKey );
		ASSERT_NE( it, map.end() );
		map.erase( it );

		EXPECT_TRUE( map.empty() );
	}

	TEST( StringMapAdvanced, CountOperations )
	{
		StringMap<double> map{
			{ "count_key", 3.14 } };

		// count() with different key types
		EXPECT_EQ( map.count( "count_key" ), 1 );
		EXPECT_EQ( map.count( std::string_view{ "count_key" } ), 1 );
		EXPECT_EQ( map.count( std::string{ "count_key" } ), 1 );
		EXPECT_EQ( map.count( "missing_key" ), 0 );
	}

	TEST( StringMapAdvanced, ContainsOperations )
	{
		StringMap<bool> map{
			{ "contains_key", true } };

		// contains() with different key types (C++20 feature)
		EXPECT_TRUE( map.contains( "contains_key" ) );
		EXPECT_TRUE( map.contains( std::string_view{ "contains_key" } ) );
		EXPECT_TRUE( map.contains( std::string{ "contains_key" } ) );
		EXPECT_FALSE( map.contains( "missing_key" ) );
	}

	TEST( StringMapAdvanced, AtMethodBasicFunctionality )
	{
		StringMap<int> map{
			{ "at_test_key", 42 } };

		// Test const at() with different string types
		const auto& const_map = map;
		EXPECT_EQ( const_map.at( "at_test_key" ), 42 );
		EXPECT_EQ( const_map.at( std::string_view{ "at_test_key" } ), 42 );

		char mutable_key[] = "at_test_key";
		EXPECT_EQ( const_map.at( mutable_key ), 42 );

		// Test non-const at() with different string types
		EXPECT_EQ( map.at( "at_test_key" ), 42 );
		EXPECT_EQ( map.at( std::string_view{ "at_test_key" } ), 42 );
		EXPECT_EQ( map.at( mutable_key ), 42 );

		// Test that non-const at() allows modification
		map.at( "at_test_key" ) = 100;
		EXPECT_EQ( map.at( "at_test_key" ), 100 );
	}

	TEST( StringMapAdvanced, AtMethodExceptionSafety )
	{
		StringMap<std::string> map{
			{ "existing_key", "value" } };

		// Test exception throwing with const char*
		EXPECT_THROW( map.at( "missing_key" ), std::out_of_range );

		// Test exception throwing with string_view
		std::string_view missing_sv{ "missing_key_sv" };
		EXPECT_THROW( map.at( missing_sv ), std::out_of_range );

		// Test exception throwing with char*
		char missing_key[] = "missing_key_char";
		EXPECT_THROW( map.at( missing_key ), std::out_of_range );

		// Test const version exception throwing
		const auto& const_map = map;
		EXPECT_THROW( const_map.at( "missing_key" ), std::out_of_range );

		// Verify exception message contains useful information
		try
		{
			map.at( "nonexistent" );
			FAIL() << "Expected std::out_of_range exception";
		}
		catch ( const std::out_of_range& e )
		{
			std::string message = e.what();
			EXPECT_TRUE( message.find( "StringMap::at" ) != std::string::npos );
		}
	}

	TEST( StringMapAdvanced, AtMethodConstCorrectness )
	{
		StringMap<std::string> map{
			{ "const_test", "original" } };

		// Test const version returns const reference
		const auto& const_map = map;
		const std::string& const_ref = const_map.at( "const_test" );
		EXPECT_EQ( const_ref, "original" );

		// Test non-const version allows modification
		map.at( "const_test" ) = "modified";
		EXPECT_EQ( map.at( "const_test" ), "modified" );
		EXPECT_EQ( const_map.at( "const_test" ), "modified" );
	}

	//----------------------------------------------
	// Performance
	//----------------------------------------------

	TEST( StringMapPerformance, NoUnnecessaryAllocations )
	{
		StringMap<int> map{
			{ "performance_key", 100 } };

		// These operations should not create temporary std::string objects
		// when using string_view, const char*, or char* keys
		std::string_view svKey{ "performance_key" };
		const char* cstrKey{ "performance_key" };
		char mutableKey[] = "performance_key";

		// Lookups should be zero-copy
		auto it1 = map.find( svKey );
		auto it2 = map.find( cstrKey );
		auto it3 = map.find( mutableKey );

		EXPECT_NE( it1, map.end() );
		EXPECT_NE( it2, map.end() );
		EXPECT_NE( it3, map.end() );
		EXPECT_EQ( it1->second, 100 );
		EXPECT_EQ( it2->second, 100 );
		EXPECT_EQ( it3->second, 100 );

		// count and contains should also be zero-copy
		EXPECT_EQ( map.count( svKey ), 1 );
		EXPECT_EQ( map.count( cstrKey ), 1 );
		EXPECT_EQ( map.count( mutableKey ), 1 );
		EXPECT_TRUE( map.contains( svKey ) );
		EXPECT_TRUE( map.contains( cstrKey ) );
		EXPECT_TRUE( map.contains( mutableKey ) );
	}

	TEST( StringMapPerformance, LargeDataHandling )
	{
		StringMap<size_t> map;

		// Insert many items
		for ( size_t i = 0; i < 1000; ++i )
		{
			std::string key{ "key_" + std::to_string( i ) };
			map[key] = i;
		}

		EXPECT_EQ( map.size(), 1000 );

		// Verify all items are accessible
		for ( size_t i = 0; i < 1000; ++i )
		{
			std::string key{ "key_" + std::to_string( i ) };
			EXPECT_EQ( map[key], i );

			// Also test with string_view
			std::string_view svKey{ key };
			EXPECT_EQ( map.count( svKey ), 1 );
		}
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( StringMapEdgeCases, EmptyStringKey )
	{
		StringMap<std::string> map;

		// Empty string as key
		map[""] = "empty_key_value";
		map[std::string{}] = "empty_string_value";
		map[std::string_view{}] = "empty_string_view_value";

		// All should map to the same key
		EXPECT_EQ( map.size(), 1 );
		EXPECT_EQ( map[""], "empty_string_view_value" ); // Last one wins
	}

	TEST( StringMapEdgeCases, SpecialCharacterKeys )
	{
		StringMap<int> map;

		// Keys with special characters
		map["key with spaces"] = 1;
		map["key\twith\ttabs"] = 2;
		map["key\nwith\nnewlines"] = 3;
		map["key\"with\"quotes"] = 4;
		map["key\\with\\backslashes"] = 5;

		EXPECT_EQ( map.size(), 5 );
		EXPECT_EQ( map["key with spaces"], 1 );
		EXPECT_EQ( map["key\twith\ttabs"], 2 );
		EXPECT_EQ( map["key\nwith\nnewlines"], 3 );
		EXPECT_EQ( map["key\"with\"quotes"], 4 );
		EXPECT_EQ( map["key\\with\\backslashes"], 5 );
	}

	TEST( StringMapEdgeCases, UnicodeKeys )
	{
		StringMap<std::string> map;

		// Unicode keys
		map["🔑"] = "key_emoji";
		map["clé"] = "french_key";
		map["键"] = "chinese_key";
		map["ключ"] = "russian_key";

		EXPECT_EQ( map.size(), 4 );
		EXPECT_EQ( map["🔑"], "key_emoji" );
		EXPECT_EQ( map["clé"], "french_key" );
		EXPECT_EQ( map["键"], "chinese_key" );
		EXPECT_EQ( map["ключ"], "russian_key" );
	}

	//----------------------------------------------
	// STL integration
	//----------------------------------------------

	TEST( StringMapSTLIntegration, IteratorSupport )
	{
		StringMap<int> map{
			{ "iter1", 10 },
			{ "iter2", 20 },
			{ "iter3", 30 } };

		// Range-based for loop
		int sum = 0;
		for ( const auto& [key, value] : map )
		{
			sum += value;
		}
		EXPECT_EQ( sum, 60 );

		// STL algorithms
		auto count = std::count_if( map.begin(), map.end(),
			[]( const auto& pair ) { return pair.second > 15; } );
		EXPECT_EQ( count, 2 );
	}

	TEST( StringMapSTLIntegration, StdMapCompatibility )
	{
		// StringMap should be a drop-in replacement for std::unordered_map
		StringMap<int> nfxMap{
			{ "compat1", 1 },
			{ "compat2", 2 } };

		std::unordered_map<std::string, int> stdMap{
			{ "compat1", 1 },
			{ "compat2", 2 } };

		// Same operations should work on both
		EXPECT_EQ( nfxMap.size(), stdMap.size() );
		EXPECT_EQ( nfxMap["compat1"], stdMap["compat1"] );
		EXPECT_EQ( nfxMap["compat2"], stdMap["compat2"] );

		// StringMap has additional heterogeneous lookup capabilities
		std::string_view svKey{ "compat1" };
		EXPECT_EQ( nfxMap.count( svKey ), 1 ); // This works
											   // stdMap.count(svKey) would not compile without heterogeneous support
	}

	//----------------------------------------------
	// Real-world scenarios
	//----------------------------------------------

	TEST( StringMapRealWorld, ConfigurationMap )
	{
		StringMap<std::string> config;

		// Simulate loading configuration
		config["database.host"] = "localhost";
		config["database.port"] = "5432";
		config["api.timeout"] = "30";
		config["logging.level"] = "INFO";
		config["default"] = "default"; // Add default as actual entry for zero-copy demo

		// Function that demonstrates zero-copy lookup with fallback
		auto getConfig = []( const StringMap<std::string>& cfg, std::string_view key ) -> std::string_view {
			auto it = cfg.find( key );
			if ( it != cfg.end() )
			{
				return it->second; // Zero-copy: string_view into existing std::string
			}

			// For demo purposes, fallback to actual "default" entry
			auto defaultIt = cfg.find( "default" );
			return defaultIt != cfg.end() ? defaultIt->second : std::string_view{};
		};

		// Zero-copy lookups
		EXPECT_EQ( getConfig( config, "database.host" ), "localhost" );
		EXPECT_EQ( getConfig( config, "database.port" ), "5432" );
		EXPECT_EQ( getConfig( config, "missing.key" ), "default" );

		// Test with const char* and string_view keys (truly zero-copy)
		EXPECT_EQ( getConfig( config, "database.host" ), "localhost" );
		std::string_view keyView{ "database.port" };
		EXPECT_EQ( getConfig( config, keyView ), "5432" );
	}

	TEST( StringMapRealWorld, CacheScenario )
	{
		StringMap<std::shared_ptr<std::string>> cache;

		// Simulate caching expensive computations
		auto getOrCompute = [&cache]( std::string_view key ) -> std::shared_ptr<std::string> {
			// Zero-copy lookup
			auto it = cache.find( key );
			if ( it != cache.end() )
			{
				return it->second;
			}

			// Compute and cache (this creates the std::string key)
			auto result = std::make_shared<std::string>( "computed_" + std::string{ key } );
			cache[std::string{ key }] = result;
			return result;
		};

		// First access computes
		auto result1 = getOrCompute( "expensive_key" );
		EXPECT_EQ( *result1, "computed_expensive_key" );
		EXPECT_EQ( cache.size(), 1 );

		// Second access hits cache (zero-copy lookup)
		auto result2 = getOrCompute( "expensive_key" );
		EXPECT_EQ( result1, result2 ); // Same shared_ptr
		EXPECT_EQ( cache.size(), 1 );
	}

	TEST( StringMapRealWorld, HttpHeadersScenario )
	{
		StringMap<std::string> headers;

		// Case-insensitive headers would require custom hash/equal functors
		// This test shows the basic functionality
		headers["Content-Type"] = "application/json";
		headers["Authorization"] = "Bearer token123";
		headers["User-Agent"] = "agent/0.7.0";

		// Function processing HTTP headers
		auto getHeader = []( const StringMap<std::string>& hdrs, std::string_view name ) -> std::string_view {
			auto it = hdrs.find( name );
			return it != hdrs.end() ? std::string_view{ it->second } : std::string_view{};
		};

		// Zero-copy header lookups
		EXPECT_EQ( getHeader( headers, "Content-Type" ), "application/json" );
		EXPECT_EQ( getHeader( headers, "Authorization" ), "Bearer token123" );
		EXPECT_EQ( getHeader( headers, "Missing-Header" ), "" );
	}

	TEST( StringMapRealWorld, ConfigurationManagementWithInsertOrAssign )
	{
		StringMap<std::string> config;

		// Simulate loading default configuration
		config.insert_or_assign( "timeout", "30" );
		config.insert_or_assign( "retries", "3" );
		config.insert_or_assign( "host", "localhost" );
		config.insert_or_assign( "port", "8080" );

		EXPECT_EQ( config.size(), 4 );

		// Simulate loading user overrides using insert_or_assign
		// This demonstrates the "upsert" pattern
		auto updateConfig = [&config]( std::string_view key, std::string_view value ) {
			auto [iter, inserted] = config.insert_or_assign( key, std::string{ value } );
			return inserted; // true if new setting, false if override
		};

		// Override existing settings
		bool timeoutWasNew = updateConfig( "timeout", "60" );		// Override
		bool hostWasNew = updateConfig( "host", "production.com" ); // Override
		bool debugWasNew = updateConfig( "debug", "true" );			// New setting

		EXPECT_FALSE( timeoutWasNew ); // Was an override
		EXPECT_FALSE( hostWasNew );	   // Was an override
		EXPECT_TRUE( debugWasNew );	   // Was a new setting

		// Verify final configuration
		EXPECT_EQ( config["timeout"], "60" );
		EXPECT_EQ( config["retries"], "3" ); // Unchanged
		EXPECT_EQ( config["host"], "production.com" );
		EXPECT_EQ( config["port"], "8080" );  // Unchanged
		EXPECT_EQ( config["debug"], "true" ); // New
		EXPECT_EQ( config.size(), 5 );

		// Demonstrate heterogeneous key usage in real scenario
		char dynamic_key[] = "dynamic_setting";
		config.insert_or_assign( dynamic_key, "dynamic_value" );
		EXPECT_EQ( config[std::string_view{ "dynamic_setting" }], "dynamic_value" );
	}
}
