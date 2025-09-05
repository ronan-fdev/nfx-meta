/**
 * @file TESTS_MemoryCache.cpp
 * @brief Comprehensive tests for MemoryCache thread-safe caching with expiration policies
 * @details Tests covering cache operations, LRU eviction, expiration policies,
 *          thread safety, and enterprise-grade caching scenarios
 */

#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <nfx/memory/MemoryCache.h>

namespace nfx::memory::test
{
	//=====================================================================
	// MemoryCache Tests
	//=====================================================================

	//----------------------------------------------
	// Basic construction
	//----------------------------------------------

	TEST( MemoryCacheConstruction, DefaultConstruction )
	{
		MemoryCache<std::string, int> cache;

		EXPECT_TRUE( cache.isEmpty() );
		EXPECT_EQ( cache.size(), 0 );
	}

	TEST( MemoryCacheConstruction, OptionsConstruction )
	{
		MemoryCacheOptions options{ 100, std::chrono::minutes( 30 ) };

		MemoryCache<std::string, std::string> cache( options );

		EXPECT_TRUE( cache.isEmpty() );
		EXPECT_EQ( cache.size(), 0 );
	}

	TEST( MemoryCacheConstruction, GetOrCreateBasic )
	{
		MemoryCache<std::string, std::string> cache;

		// Create new entry
		auto& value = cache.getOrCreate( "key1", []() { return std::string{ "value1" }; } );
		EXPECT_EQ( value, "value1" );
		EXPECT_EQ( cache.size(), 1 );
		EXPECT_FALSE( cache.isEmpty() );

		// Get existing entry
		auto& value2 = cache.getOrCreate( "key1", []() { return std::string{ "should_not_create" }; } );
		EXPECT_EQ( value2, "value1" );
		EXPECT_EQ( cache.size(), 1 );
	}

	//----------------------------------------------
	// Basic operations
	//----------------------------------------------

	TEST( MemoryCacheOperations, TryGetOperations )
	{
		MemoryCache<std::string, int> cache;

		// Try get non-existent
		auto result1 = cache.tryGet( "missing_key" );
		EXPECT_FALSE( result1.has_value() );

		// Add entry and try get
		cache.getOrCreate( "existing_key", []() { return 42; } );
		auto result2 = cache.tryGet( "existing_key" );
		ASSERT_TRUE( result2.has_value() );
		EXPECT_EQ( result2->get(), 42 );
	}

	TEST( MemoryCacheOperations, RemoveOperations )
	{
		MemoryCache<std::string, std::string> cache;

		// Remove non-existent
		EXPECT_FALSE( cache.remove( "missing_key" ) );

		// Add and remove
		cache.getOrCreate( "remove_key", []() { return std::string{ "remove_value" }; } );
		EXPECT_EQ( cache.size(), 1 );

		EXPECT_TRUE( cache.remove( "remove_key" ) );
		EXPECT_EQ( cache.size(), 0 );
		EXPECT_TRUE( cache.isEmpty() );

		// Try get after remove
		auto result = cache.tryGet( "remove_key" );
		EXPECT_FALSE( result.has_value() );
	}

	TEST( MemoryCacheOperations, ClearOperations )
	{
		MemoryCache<std::string, int> cache;

		// Add multiple entries
		cache.getOrCreate( "key1", []() { return 1; } );
		cache.getOrCreate( "key2", []() { return 2; } );
		cache.getOrCreate( "key3", []() { return 3; } );
		EXPECT_EQ( cache.size(), 3 );

		// Clear all
		cache.clear();
		EXPECT_EQ( cache.size(), 0 );
		EXPECT_TRUE( cache.isEmpty() );

		// Verify all entries are gone
		EXPECT_FALSE( cache.tryGet( "key1" ).has_value() );
		EXPECT_FALSE( cache.tryGet( "key2" ).has_value() );
		EXPECT_FALSE( cache.tryGet( "key3" ).has_value() );
	}

	//----------------------------------------------
	// Expiration policies
	//----------------------------------------------

	TEST( MemoryCacheExpiration, SlidingExpirationDefault )
	{
		MemoryCacheOptions options{ 0, std::chrono::milliseconds( 50 ) };

		MemoryCache<std::string, std::string> cache( options );

		// Add entry
		cache.getOrCreate( "expire_key", []() { return std::string{ "expire_value" }; } );
		EXPECT_EQ( cache.size(), 1 );

		// Should be available immediately
		auto result1 = cache.tryGet( "expire_key" );
		EXPECT_TRUE( result1.has_value() );

		// Wait for expiration
		std::this_thread::sleep_for( std::chrono::milliseconds( 60 ) );

		// Should be expired and removed
		auto result2 = cache.tryGet( "expire_key" );
		EXPECT_FALSE( result2.has_value() );
		EXPECT_EQ( cache.size(), 0 );
	}

	TEST( MemoryCacheExpiration, CustomExpirationPerEntry )
	{
		MemoryCache<std::string, int> cache;

		// Add entry with custom short expiration
		cache.getOrCreate( "short_expire", []() { return 100; }, []( CacheEntry& entry ) { entry.slidingExpiration = { std::chrono::milliseconds( 30 ) }; } );

		// Add entry with custom long expiration
		cache.getOrCreate( "long_expire", []() { return 200; }, []( CacheEntry& entry ) { entry.slidingExpiration = { std::chrono::minutes( 10 ) }; } );

		EXPECT_EQ( cache.size(), 2 );

		// Wait for short expiration
		std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );

		// Short should be expired, long should remain
		EXPECT_FALSE( cache.tryGet( "short_expire" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "long_expire" ).has_value() );
		EXPECT_EQ( cache.size(), 1 );
	}

	TEST( MemoryCacheExpiration, SlidingExpirationRenewal )
	{
		MemoryCacheOptions options{ 0, std::chrono::milliseconds( 100 ) };

		MemoryCache<std::string, std::string> cache( options );

		cache.getOrCreate( "sliding_key", []() { return std::string{ "sliding_value" }; } );

		// Access periodically to keep it alive
		for ( int i{ 0 }; i < 5; ++i )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
			auto result = cache.tryGet( "sliding_key" );
			EXPECT_TRUE( result.has_value() );
		}

		// Stop accessing and wait for expiration
		std::this_thread::sleep_for( std::chrono::milliseconds( 120 ) );
		auto result = cache.tryGet( "sliding_key" );
		EXPECT_FALSE( result.has_value() );
	}

	TEST( MemoryCacheExpiration, ManualCleanupExpired )
	{
		MemoryCacheOptions options{ 0, std::chrono::milliseconds( 30 ) };

		MemoryCache<std::string, int> cache( options );

		// Add multiple entries
		cache.getOrCreate( "key1", []() { return 1; } );
		cache.getOrCreate( "key2", []() { return 2; } );
		cache.getOrCreate( "key3", []() { return 3; } );
		EXPECT_EQ( cache.size(), 3 );

		// Wait for expiration
		std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );

		// Size should still be 3 until cleanup
		EXPECT_EQ( cache.size(), 3 );

		// Manual cleanup
		cache.cleanupExpired();
		EXPECT_EQ( cache.size(), 0 );
	}

	//----------------------------------------------
	// Size limits and LRU eviction
	//----------------------------------------------

	TEST( MemoryCacheLRU, SizeLimitEnforcement )
	{
		MemoryCacheOptions options{ 3, std::chrono::hours{ 1 } };

		MemoryCache<std::string, std::string> cache( options );

		// Fill to capacity
		cache.getOrCreate( "key1", []() { return std::string{ "value1" }; } );
		cache.getOrCreate( "key2", []() { return std::string{ "value2" }; } );
		cache.getOrCreate( "key3", []() { return std::string{ "value3" }; } );
		EXPECT_EQ( cache.size(), 3 );

		// All entries should be present
		EXPECT_TRUE( cache.tryGet( "key1" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "key2" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "key3" ).has_value() );

		// Add fourth entry - should evict LRU (key1)
		cache.getOrCreate( "key4", []() { return std::string{ "value4" }; } );
		EXPECT_EQ( cache.size(), 3 );

		// key1 should be evicted, others should remain
		EXPECT_FALSE( cache.tryGet( "key1" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "key2" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "key3" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "key4" ).has_value() );
	}

	TEST( MemoryCacheLRU, LRUOrderingWithAccess )
	{
		MemoryCacheOptions options{ 3, std::chrono::hours{ 1 } };

		MemoryCache<std::string, int> cache( options );

		// Fill cache
		cache.getOrCreate( "oldest", []() { return 1; } );
		cache.getOrCreate( "middle", []() { return 2; } );
		cache.getOrCreate( "newest", []() { return 3; } );

		// Access oldest to make it most recent
		cache.tryGet( "oldest" );

		// Add new entry - should evict middle (now LRU)
		cache.getOrCreate( "fourth", []() { return 4; } );

		// middle should be evicted
		EXPECT_TRUE( cache.tryGet( "oldest" ).has_value() );
		EXPECT_FALSE( cache.tryGet( "middle" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "newest" ).has_value() );
		EXPECT_TRUE( cache.tryGet( "fourth" ).has_value() );
	}

	TEST( MemoryCacheLRU, NoSizeLimit )
	{
		MemoryCacheOptions options{ 0, std::chrono::hours{ 1 } }; // Unlimited

		MemoryCache<std::string, int> cache( options );

		// Add many entries
		for ( int i{ 0 }; i < 1000; ++i )
		{
			std::string key{ "key" + std::to_string( i ) };
			cache.getOrCreate( key, [i]() { return i; } );
		}

		EXPECT_EQ( cache.size(), 1000 );

		// All entries should still be present
		for ( int i{ 0 }; i < 1000; ++i )
		{
			std::string key{ "key" + std::to_string( i ) };
			auto result = cache.tryGet( key );
			ASSERT_TRUE( result.has_value() );
			EXPECT_EQ( result->get(), i );
		}
	}

	//----------------------------------------------
	// Factory function and configuration
	//----------------------------------------------

	TEST( MemoryCacheFactory, FactoryFunctionCalls )
	{
		MemoryCache<std::string, std::string> cache;

		int factoryCallCount{ 0 };
		auto factory = [&factoryCallCount]() {
			++factoryCallCount;
			return std::string{ "factory_value" };
		};

		// First call should invoke factory
		auto& value1 = cache.getOrCreate( "factory_key", factory );
		EXPECT_EQ( value1, "factory_value" );
		EXPECT_EQ( factoryCallCount, 1 );

		// Second call should not invoke factory
		auto& value2 = cache.getOrCreate( "factory_key", factory );
		EXPECT_EQ( value2, "factory_value" );
		EXPECT_EQ( factoryCallCount, 1 );
	}

	TEST( MemoryCacheFactory, ConfigurationFunction )
	{
		MemoryCache<std::string, std::string> cache;

		bool configCalled{ false };
		auto configFunc = [&configCalled]( CacheEntry& entry ) {
			configCalled = true;
			entry.slidingExpiration = { std::chrono::minutes( 5 ) };
			entry.size = { 100 };
		};

		cache.getOrCreate( "config_key", []() { return std::string{ "config_value" }; }, configFunc );

		EXPECT_TRUE( configCalled );
	}

	//----------------------------------------------
	// Value type tests
	//----------------------------------------------

	TEST( MemoryCacheValueTypes, ComplexValues )
	{
		struct ComplexValue
		{
			std::string name;
			std::vector<int> data;
			double weight;

			ComplexValue( std::string n, std::vector<int> d, double w )
				: name{ std::move( n ) }, data{ std::move( d ) }, weight{ w } {}
		};

		MemoryCache<std::string, ComplexValue> cache;

		auto factory = []() {
			return ComplexValue{ "test", { 1, 2, 3, 4, 5 }, 3.14 };
		};

		auto& value = cache.getOrCreate( "complex_key", factory );
		EXPECT_EQ( value.name, "test" );
		EXPECT_EQ( value.data.size(), 5 );
		EXPECT_DOUBLE_EQ( value.weight, 3.14 );
	}

	TEST( MemoryCacheValueTypes, MoveSemantics )
	{
		MemoryCache<std::string, std::unique_ptr<std::string>> cache;

		auto factory = []() {
			return std::make_unique<std::string>( "unique_value" );
		};

		auto& ptr = cache.getOrCreate( "unique_key", factory );
		ASSERT_NE( ptr, nullptr );
		EXPECT_EQ( *ptr, "unique_value" );
	}

	//----------------------------------------------
	// Thread safety
	//----------------------------------------------

	TEST( MemoryCacheThreadSafety, ConcurrentAccess )
	{
		MemoryCache<int, std::string> cache;

		const int numThreads{ 10 };
		const int itemsPerThread{ 100 };
		std::vector<std::thread> threads;

		// Concurrent insertions
		for ( int t{ 0 }; t < numThreads; ++t )
		{
			threads.emplace_back( [&cache, t]() {
				for ( int i{ 0 }; i < itemsPerThread; ++i )
				{
					int key{ t * itemsPerThread + i };
					cache.getOrCreate( key, [key]() {
						return std::string{ "value_" + std::to_string( key ) };
					} );
				}
			} );
		}

		for ( auto& thread : threads )
		{
			thread.join();
		}

		// Verify all entries were created
		EXPECT_EQ( cache.size(), numThreads * itemsPerThread );

		// Verify all values are accessible
		for ( int t{ 0 }; t < numThreads; ++t )
		{
			for ( int i{ 0 }; i < itemsPerThread; ++i )
			{
				int key{ t * itemsPerThread + i };
				auto result = cache.tryGet( key );
				ASSERT_TRUE( result.has_value() );
				EXPECT_EQ( result->get(), "value_" + std::to_string( key ) );
			}
		}
	}

	//----------------------------------------------
	// Performance characteristics
	//----------------------------------------------

	TEST( MemoryCachePerformance, LargeDataHandling )
	{
		MemoryCache<std::string, std::vector<int>> cache;

		const std::size_t numEntries{ 1000 };
		const std::size_t vectorSize{ 1000 };

		// Add large data structures
		auto start = std::chrono::high_resolution_clock::now();

		for ( std::size_t i{ 0 }; i < numEntries; ++i )
		{
			std::string key{ "large_key_" + std::to_string( i ) };
			cache.getOrCreate( key, [i]() {
				std::vector<int> data( vectorSize, static_cast<int>( i ) );
				return data;
			} );
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );

		EXPECT_EQ( cache.size(), numEntries );
		EXPECT_LT( duration.count(), 1000 ); // Should complete within 1 second

		// Verify random access performance
		start = std::chrono::high_resolution_clock::now();

		for ( std::size_t i{ 0 }; i < numEntries; ++i )
		{
			std::string key{ "large_key_" + std::to_string( i ) };
			auto result = cache.tryGet( key );
			EXPECT_TRUE( result.has_value() );
		}

		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );

		EXPECT_LT( duration.count(), 100 ); // Lookups should be very fast
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( MemoryCacheEdgeCases, EmptyStringKeys )
	{
		MemoryCache<std::string, int> cache;

		cache.getOrCreate( "", []() { return 42; } );
		EXPECT_EQ( cache.size(), 1 );

		auto result = cache.tryGet( "" );
		ASSERT_TRUE( result.has_value() );
		EXPECT_EQ( result->get(), 42 );
	}

	TEST( MemoryCacheEdgeCases, NumericKeys )
	{
		MemoryCache<int, std::string> cache;

		cache.getOrCreate( 0, []() { return std::string{ "zero" }; } );
		cache.getOrCreate( -1, []() { return std::string{ "negative" }; } );
		cache.getOrCreate( INT_MAX, []() { return std::string{ "max_int" }; } );

		EXPECT_EQ( cache.size(), 3 );
		EXPECT_EQ( cache.tryGet( 0 )->get(), "zero" );
		EXPECT_EQ( cache.tryGet( -1 )->get(), "negative" );
		EXPECT_EQ( cache.tryGet( INT_MAX )->get(), "max_int" );
	}
}
