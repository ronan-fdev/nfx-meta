/**
 * @file Sample_LruCache.cpp
 * @brief Demonstrates comprehensive usage of NFX C++ Core LruCache
 * @details This sample shows how to use LruCache for high-performance caching
 *          with LRU eviction, configurable expiration policies, factory functions,
 *          and thread-safe operations for real-world applications
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <nfx/memory/LruCache.h>

using namespace nfx::memory;

//=====================================================================
// Sample data structures for demonstration
//=====================================================================

struct UserProfile
{
	std::string userId;
	std::string name;
	std::string email;
	std::chrono::system_clock::time_point lastLogin;

	UserProfile( std::string id, std::string userName, std::string userEmail )
		: userId{ std::move( id ) },
		  name{ std::move( userName ) },
		  email{ std::move( userEmail ) },
		  lastLogin{ std::chrono::system_clock::now() }
	{
	}
};

struct DatabaseResult
{
	std::vector<std::string> data;
	std::chrono::steady_clock::time_point queryTime;

	DatabaseResult( std::vector<std::string> resultData )
		: data{ std::move( resultData ) },
		  queryTime{ std::chrono::steady_clock::now() }
	{
	}
};

//=====================================================================
// Utility functions for demonstrations
//=====================================================================

// Simulate expensive database lookup
static UserProfile simulateUserLookup( const std::string& userId )
{
	// Simulate database latency
	std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

	return UserProfile{
		userId,
		"User_" + userId,
		"user" + userId + "@example.com" };
}

// Simulate expensive computation
static std::vector<std::string> simulateExpensiveQuery( const std::string& query )
{
	// Simulate processing time
	std::this_thread::sleep_for( std::chrono::milliseconds( 25 ) );

	std::vector<std::string> results;
	for ( int i{ 0 }; i < 100; ++i )
	{
		results.push_back( query + "_result_" + std::to_string( i ) );
	}

	return results;
}

int main()
{
	std::cout << "=== NFX C++ Core - LruCache Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Basic LruCache usage - User profile caching
	//=========================================================================

	std::cout << "--- Basic User Profile Caching ---" << std::endl;

	// Create cache with default options
	LruCache<std::string, UserProfile> userCache{};

	// Cache user profiles with factory function
	auto getUser = [&]( const std::string& userId ) -> UserProfile& {
		return userCache.getOrCreate( userId, [&userId]() {
			std::cout << "  Loading user " << userId << " from database..." << std::endl;
			return simulateUserLookup( userId );
		} );
	};

	// Demonstrate caching behavior
	std::cout << "First access (cache miss):" << std::endl;
	auto& user1{ getUser( "12345" ) };
	std::cout << "  User: " << user1.name << " (" << user1.email << ")" << std::endl;

	std::cout << "Second access (cache hit):" << std::endl;
	auto& user1_cached{ getUser( "12345" ) };
	std::cout << "  User: " << user1_cached.name << " (from cache)" << std::endl;

	std::cout << "Cache size: " << userCache.size() << " entries" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Configured cache with size limits and expiration
	//=========================================================================

	std::cout << "--- Configured Cache with Size Limits ---" << std::endl;

	LruCacheOptions options{ 3, std::chrono::seconds( 2 ) }; // Maximum 3 entries, 2-second expiration

	LruCache<std::string, DatabaseResult> queryCache{ options };

	// Factory function for database queries
	auto executeQuery = [&]( const std::string& query ) -> DatabaseResult& {
		return queryCache.getOrCreate( query, [&query]() {
			std::cout << "  Executing expensive query: " << query << std::endl;
			return DatabaseResult{ simulateExpensiveQuery( query ) };
		} );
	};

	// Fill cache beyond size limit to demonstrate LRU eviction
	std::cout << "Filling cache beyond size limit:" << std::endl;

	[[maybe_unused]] auto& result1{ executeQuery( "SELECT * FROM users" ) };
	std::cout << "  Query 1 cached, size: " << queryCache.size() << std::endl;

	[[maybe_unused]] auto& result2{ executeQuery( "SELECT * FROM orders" ) };
	std::cout << "  Query 2 cached, size: " << queryCache.size() << std::endl;

	[[maybe_unused]] auto& result3{ executeQuery( "SELECT * FROM products" ) };
	std::cout << "  Query 3 cached, size: " << queryCache.size() << std::endl;

	// This should evict the first query (LRU)
	[[maybe_unused]] auto& result4{ executeQuery( "SELECT * FROM inventory" ) };
	std::cout << "  Query 4 cached, size: " << queryCache.size() << " (should be 3)" << std::endl;

	// Try to access first query again (should be cache miss)
	std::cout << "Accessing first query again (should be evicted):" << std::endl;
	[[maybe_unused]] auto& result1_again{ executeQuery( "SELECT * FROM users" ) };
	std::cout << "  Query 5 cached, size: " << queryCache.size() << " (should be 3)" << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Custom expiration configuration per entry
	//=========================================================================

	std::cout << "--- Custom Expiration Configuration ---" << std::endl;

	LruCache<std::string, std::string> customCache{};

	// Cache with short expiration
	customCache.getOrCreate( "short_lived", []() { return std::string{ "This expires quickly" }; }, []( CacheEntry& entry ) { entry.slidingExpiration = std::chrono::milliseconds( 500 ); } );

	// Cache with long expiration
	customCache.getOrCreate( "long_lived", []() { return std::string{ "This lasts longer" }; }, []( CacheEntry& entry ) { entry.slidingExpiration = std::chrono::seconds( 10 ); } );

	std::cout << "Created entries with different expiration times" << std::endl;
	std::cout << "Cache size: " << customCache.size() << std::endl;

	// Wait for short-lived entry to expire
	std::cout << "Waiting 600ms for short-lived entry to expire..." << std::endl;
	std::this_thread::sleep_for( std::chrono::milliseconds( 600 ) );

	// Try to access both entries
	auto shortResult{ customCache.tryGet( "short_lived" ) };
	auto longResult{ customCache.tryGet( "long_lived" ) };

	std::cout << "Short-lived entry found: " << ( shortResult.has_value() ? "Yes" : "No" ) << std::endl;
	std::cout << "Long-lived entry found: " << ( longResult.has_value() ? "Yes" : "No" ) << std::endl;

	// Manual cleanup of expired entries
	customCache.cleanupExpired();
	std::cout << "After cleanup, cache size: " << customCache.size() << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Performance demonstration
	//=========================================================================

	std::cout << "--- Performance Demonstration ---" << std::endl;

	LruCacheOptions perfOptions{ 1000, std::chrono::hours{ 1 } };
	LruCache<int, std::vector<int>> perfCache{ perfOptions };

	const std::size_t iterations{ 10000 };
	const std::size_t unique_keys{ 100 };

	std::cout << "Performance test: " << iterations << " lookups across " << unique_keys << " unique keys" << std::endl;

	auto start{ std::chrono::high_resolution_clock::now() };

	std::size_t cache_hits{ 0 };
	std::size_t cache_misses{ 0 };

	for ( std::size_t i{ 0 }; i < iterations; ++i )
	{
		int key{ static_cast<int>( i % unique_keys ) };

		// Try cache first
		auto cached{ perfCache.tryGet( key ) };
		if ( cached.has_value() )
		{
			++cache_hits;
		}
		else
		{
			// Cache miss - create entry
			perfCache.getOrCreate( key, [key]() {
				std::vector<int> data( 1000, key );
				return data;
			} );
			++cache_misses;
		}
	}

	auto end{ std::chrono::high_resolution_clock::now() };
	auto duration{ std::chrono::duration_cast<std::chrono::microseconds>( end - start ) };

	std::cout << "Results:" << std::endl;
	std::cout << "  Total operations: " << iterations << std::endl;
	std::cout << "  Cache hits: " << cache_hits << std::endl;
	std::cout << "  Cache misses: " << cache_misses << std::endl;
	std::cout << "  Hit ratio: " << std::fixed << std::setprecision( 1 )
			  << ( 100.0 * static_cast<double>( cache_hits ) / static_cast<double>( iterations ) ) << "%" << std::endl;
	std::cout << "  Total time: " << duration.count() << " microseconds" << std::endl;
	std::cout << "  Average per operation: " << std::setprecision( 2 )
			  << ( static_cast<double>( duration.count() ) / static_cast<double>( iterations ) ) << " μs" << std::endl;
	std::cout << "  Final cache size: " << perfCache.size() << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Thread safety demonstration
	//=========================================================================

	std::cout << "--- Thread Safety Demonstration ---" << std::endl;

	LruCache<std::string, std::string> threadSafeCache{};
	const std::size_t num_threads{ 4 };
	const std::size_t ops_per_thread{ 1000 };

	std::vector<std::thread> threads;
	std::vector<std::size_t> hits_per_thread{ num_threads, 0 };

	auto start_threads{ std::chrono::high_resolution_clock::now() };

	for ( std::size_t t{ 0 }; t < num_threads; ++t )
	{
		threads.emplace_back( [&threadSafeCache, &hits_per_thread, t]() {
			std::size_t local_hits{ 0 };

			for ( std::size_t i{ 0 }; i < ops_per_thread; ++i )
			{
				std::string key{ "thread_" + std::to_string( t ) + "_key_" + std::to_string( i % 10 ) };

				auto result{ threadSafeCache.tryGet( key ) };
				if ( result.has_value() )
				{
					++local_hits;
				}
				else
				{
					threadSafeCache.getOrCreate( key, [&key]() {
						return std::string{ "value_for_" + key };
					} );
				}
			}

			hits_per_thread[t] = local_hits;
		} );
	}

	// Wait for all threads to complete
	for ( auto& thread : threads )
	{
		thread.join();
	}

	auto end_threads{ std::chrono::high_resolution_clock::now() };
	auto thread_duration{ std::chrono::duration_cast<std::chrono::milliseconds>( end_threads - start_threads ) };

	std::size_t total_hits{ 0 };
	for ( std::size_t hits : hits_per_thread )
	{
		total_hits += hits;
	}

	std::cout << "Concurrent operations completed:" << std::endl;
	std::cout << "  Threads: " << num_threads << std::endl;
	std::cout << "  Operations per thread: " << ops_per_thread << std::endl;
	std::cout << "  Total operations: " << ( num_threads * ops_per_thread ) << std::endl;
	std::cout << "  Total cache hits: " << total_hits << std::endl;
	std::cout << "  Final cache size: " << threadSafeCache.size() << std::endl;
	std::cout << "  Execution time: " << thread_duration.count() << " ms" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Cache manipulation operations
	//=========================================================================

	std::cout << "--- Cache Manipulation Operations ---" << std::endl;

	LruCache<std::string, std::string> manipCache{};

	// Add some entries
	manipCache.getOrCreate( "key1", []() { return std::string{ "value1" }; } );
	manipCache.getOrCreate( "key2", []() { return std::string{ "value2" }; } );
	manipCache.getOrCreate( "key3", []() { return std::string{ "value3" }; } );

	std::cout << "Initial cache size: " << manipCache.size() << std::endl;
	std::cout << "Cache is empty: " << ( manipCache.isEmpty() ? "Yes" : "No" ) << std::endl;

	// Remove specific entry
	bool removed{ manipCache.remove( "key2" ) };
	std::cout << "Removed key2: " << ( removed ? "Success" : "Failed" ) << std::endl;
	std::cout << "Cache size after removal: " << manipCache.size() << std::endl;

	// Try to remove non-existent entry
	bool removedNonExistent{ manipCache.remove( "nonexistent" ) };
	std::cout << "Removed non-existent key: " << ( removedNonExistent ? "Success" : "Failed" ) << std::endl;

	// Clear entire cache
	manipCache.clear();
	std::cout << "Cache size after clear: " << manipCache.size() << std::endl;
	std::cout << "Cache is empty: " << ( manipCache.isEmpty() ? "Yes" : "No" ) << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Real-world use case: Web application caching
	//=========================================================================

	std::cout << "--- Real-World Use Case: Web Application Caching ---" << std::endl;

	LruCacheOptions webOptions{ 50, std::chrono::minutes( 5 ) }; // 50 entries max, 5-minute default expiration

	LruCache<std::string, std::string> webCache{ webOptions };

	// Simulate web requests
	auto processRequest = [&]( const std::string& endpoint ) -> std::string {
		// Check if it's already cached
		auto existing = webCache.tryGet( endpoint );
		if ( existing.has_value() )
		{
			std::cout << "  " << endpoint << " (cache hit)" << std::endl;
			return existing.value().get();
		}
		else
		{
			auto result = webCache.getOrCreate( endpoint, [&endpoint]() {
				std::cout << "  " << endpoint << " (cache miss) - processing..." << std::endl;
				// Simulate expensive operation (database query, API call, etc.)
				std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
				return std::string{ "Response for " + endpoint };
			} );
			return result;
		}
	};

	std::vector<std::string> endpoints{
		"/api/users",
		"/api/products",
		"/api/orders",
		"/api/users",	 // Repeat for cache hit
		"/api/products", // Repeat for cache hit
		"/api/analytics",
		"/api/reports",
		"/api/users" // Another repeat
	};

	auto web_start{ std::chrono::high_resolution_clock::now() };

	std::cout << "Simulating web requests:" << std::endl;
	for ( const auto& endpoint : endpoints )
	{
		auto response{ processRequest( endpoint ) };
	}

	auto web_end{ std::chrono::high_resolution_clock::now() };
	auto web_duration{ std::chrono::duration_cast<std::chrono::milliseconds>( web_end - web_start ) };

	std::cout << "Web application caching results:" << std::endl;
	std::cout << "  Total requests: " << endpoints.size() << std::endl;
	std::cout << "  Unique endpoints cached: " << webCache.size() << std::endl;
	std::cout << "  Total processing time: " << web_duration.count() << " ms" << std::endl;
	std::cout << "  (Without caching, this would take much longer)" << std::endl;

	std::cout << std::endl;
	std::cout << "=== LruCache demonstration completed ===" << std::endl;

	return 0;
}
