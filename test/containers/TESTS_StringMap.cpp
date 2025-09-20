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

		EXPECT_EQ( map.size(), 3 );
		EXPECT_EQ( map["std_string_key"], "value1" );
		EXPECT_EQ( map["string_view_key"], "value2" );
		EXPECT_EQ( map["c_string_key"], "value3" );
	}

	TEST( StringMapBasic, HeterogeneousLookup )
	{
		StringMap<int> map{
			{ "lookup_test", 42 } };

		std::string strKey{ "lookup_test" };
		std::string_view svKey{ strKey };
		const char* cstrKey{ strKey.c_str() };

		// All lookup methods should work
		EXPECT_EQ( map[strKey], 42 );
		EXPECT_EQ( map[svKey], 42 );
		EXPECT_EQ( map[cstrKey], 42 );

		// find() should work with all types
		EXPECT_NE( map.find( strKey ), map.end() );
		EXPECT_NE( map.find( svKey ), map.end() );
		EXPECT_NE( map.find( cstrKey ), map.end() );
	}

	TEST( StringMapBasic, TryEmplaceHeterogeneous )
	{
		StringMap<std::string> map;

		// try_emplace with different key types
		auto [iter1, inserted1] = map.try_emplace( "key1", "value1" );
		auto [iter2, inserted2] = map.try_emplace( std::string_view{ "key2" }, "value2" );
		auto [iter3, inserted3] = map.try_emplace( std::string{ "key3" }, "value3" );

		EXPECT_TRUE( inserted1 );
		EXPECT_TRUE( inserted2 );
		EXPECT_TRUE( inserted3 );

		EXPECT_EQ( iter1->second, "value1" );
		EXPECT_EQ( iter2->second, "value2" );
		EXPECT_EQ( iter3->second, "value3" );

		// Try to emplace existing keys
		auto [iter4, inserted4] = map.try_emplace( "key1", "new_value1" );
		auto [iter5, inserted5] = map.try_emplace( std::string_view{ "key2" }, "new_value2" );

		EXPECT_FALSE( inserted4 );
		EXPECT_FALSE( inserted5 );

		// Values should remain unchanged
		EXPECT_EQ( iter4->second, "value1" );
		EXPECT_EQ( iter5->second, "value2" );
	}

	//----------------------------------------------
	// Advanced operations
	//----------------------------------------------

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

	//----------------------------------------------
	// Performance
	//----------------------------------------------

	TEST( StringMapPerformance, NoUnnecessaryAllocations )
	{
		StringMap<int> map{
			{ "performance_key", 100 } };

		// These operations should not create temporary std::string objects
		// when using string_view or const char* keys
		std::string_view svKey{ "performance_key" };
		const char* cstrKey{ "performance_key" };

		// Lookups should be zero-copy
		auto it1 = map.find( svKey );
		auto it2 = map.find( cstrKey );

		EXPECT_NE( it1, map.end() );
		EXPECT_NE( it2, map.end() );
		EXPECT_EQ( it1->second, 100 );
		EXPECT_EQ( it2->second, 100 );

		// count and contains should also be zero-copy
		EXPECT_EQ( map.count( svKey ), 1 );
		EXPECT_EQ( map.count( cstrKey ), 1 );
		EXPECT_TRUE( map.contains( svKey ) );
		EXPECT_TRUE( map.contains( cstrKey ) );
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
}
