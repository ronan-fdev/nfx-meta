/**
 * @file Sample_StringContainers.cpp
 * @brief Demonstration of string containers with zero-copy heterogeneous lookups
 * @details Shows real-world usage patterns, performance benefits, and migration examples
 *          for StringMap, StringSet, HashMap (Robin Hood hashing), and the underlying
 *          StringViewHash/StringViewEqual functors
 */

#include <iomanip>
#include <iostream>
#include <chrono>
#include <memory>
#include <thread>

#include <nfx/containers/HashMap.h>
#include <nfx/containers/StringMap.h>
#include <nfx/containers/StringSet.h>

using namespace nfx::containers;

//=====================================================================
// Configuration management demo
//=====================================================================

static void demoConfigurationManagement()
{
	std::cout << "\n=== Configuration Management Demo ===\n";

	StringMap<std::string> config;

	// Load configuration (simulated)
	config["database.host"] = "localhost";
	config["database.port"] = "5432";
	config["database.name"] = "production_db";
	config["api.timeout"] = "30";
	config["api.retries"] = "3";
	config["logging.level"] = "INFO";
	config["logging.file"] = "/var/log/app.log";

	// Zero-copy configuration lookup function
	auto getConfig = []( const StringMap<std::string>& cfg, std::string_view key, std::string_view default_value = "" ) -> std::string_view {
		auto it{ cfg.find( key ) };
		return it != cfg.end() ? it->second : default_value;
	};

	// Demonstrate zero-copy lookups with different key types
	auto dbHost{ getConfig( config, "database.host" ) };
	std::cout << "Database Host: " << dbHost << "\n";

	auto dbPort{ getConfig( config, std::string_view{ "database.port" } ) };
	std::cout << "Database Port: " << dbPort << "\n";

	const char* timeoutKey{ "api.timeout" };
	auto apiTimeout{ getConfig( config, timeoutKey ) };
	std::cout << "API Timeout: " << apiTimeout << "\n";

	// Non-existent key with default
	auto cacheTtl{ getConfig( config, "cache.ttl", "3600" ) };
	std::cout << "Cache TTL: " << cacheTtl << "\n";

	std::cout << "Total config entries: " << config.size() << "\n";
}

//=====================================================================
// HTTP Headers processing demo
//=====================================================================

static void demoHttpHeaders()
{
	std::cout << "\n=== HTTP Headers Processing Demo ===\n";

	StringMap<std::string> headers;

	// Simulate incoming HTTP headers
	headers["Content-Type"] = "application/json";
	headers["Authorization"] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";
	headers["User-Agent"] = "agent/0.0.7";
	headers["Accept"] = "application/json, */*";
	headers["Connection"] = "keep-alive";
	headers["Cache-Control"] = "no-cache";

	// Zero-copy header processing
	auto getHeader = []( const StringMap<std::string>& hdrs, std::string_view name ) -> std::string_view {
		auto it{ hdrs.find( name ) };
		return it != hdrs.end() ? it->second : std::string_view{};
	};

	// Process headers with different key types (zero allocations)
	std::string_view contentType{ getHeader( headers, "Content-Type" ) };
	std::string_view userAgent{ getHeader( headers, std::string_view{ "User-Agent" } ) };

	const char* authHeader{ "Authorization" };
	std::string_view authValue{ getHeader( headers, authHeader ) };

	std::cout << "Content-Type: " << contentType << "\n";
	std::cout << "User-Agent: " << userAgent << "\n";
	std::cout << "Has Authorization: " << ( !authValue.empty() ? "Yes" : "No" ) << "\n";

	auto missingHeader{ getHeader( headers, "X-Custom-Header" ) };
	std::cout << "Missing Header: '" << missingHeader << "'\n";

	// Check for security headers
	StringSet requiredSecurityHeaders{ "X-Frame-Options", "X-Content-Type-Options", "X-XSS-Protection" };

	std::cout << "\nSecurity headers check:\n";
	for ( const auto& header : requiredSecurityHeaders )
	{
		bool present{ headers.contains( std::string_view{ header } ) };
		std::cout << "  " << header << ": " << ( present ? "✓" : "✗" ) << "\n";
	}
}

//=====================================================================
// Cache implementation demo
//=====================================================================

static void demoCachingSystem()
{
	std::cout << "\n=== Caching System Demo ===\n";

	StringMap<std::shared_ptr<std::string>> cache;
	size_t cacheHits{ 0 };
	size_t cacheMisses{ 0 };

	// Expensive computation simulator
	auto expensiveComputation = []( std::string_view input ) -> std::shared_ptr<std::string> {
		// Simulate work
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		return std::make_shared<std::string>( "computed_" + std::string{ input } );
	};

	// Cache with zero-copy lookup
	auto getOrCompute = [&]( std::string_view key ) -> std::shared_ptr<std::string> {
		// Zero-copy cache lookup
		auto it{ cache.find( key ) };
		if ( it != cache.end() )
		{
			++cacheHits;
			return it->second;
		}

		// Cache miss - compute and store
		++cacheMisses;
		auto result{ expensiveComputation( key ) };
		cache[std::string{ key }] = result; // Only allocation is for the key storage
		return result;
	};

	// Demonstrate caching with different key types
	std::vector<std::string_view> testKeys{
		"user_123", "user_456", "user_123",		 // Repeat for cache hit
		"product_abc", "product_xyz", "user_456" // Another repeat
	};

	auto start{ std::chrono::high_resolution_clock::now() };

	for ( const auto& key : testKeys )
	{
		auto result{ getOrCompute( key ) };
		std::cout << "Key: " << key << " -> " << *result << "\n";
	}

	auto end{ std::chrono::high_resolution_clock::now() };
	auto duration{ std::chrono::duration_cast<std::chrono::microseconds>( end - start ) };

	std::cout << "\nCache Statistics:\n";
	std::cout << "  Cache Hits: " << cacheHits << "\n";
	std::cout << "  Cache Misses: " << cacheMisses << "\n";
	std::cout << "  Hit Ratio: " << std::fixed << std::setprecision( 1 )
			  << ( 100.0 * static_cast<double>( cacheHits ) / static_cast<double>( cacheHits + cacheMisses ) ) << "%\n";
	std::cout << "  Total Time: " << duration.count() << "μs\n";
	std::cout << "  Cache Size: " << cache.size() << " entries\n";
}

//=====================================================================
// Performance comparison demo
//=====================================================================

static void demoPerformanceComparison()
{
	std::cout << "\n=== Performance Comparison Demo ===\n";

	const size_t iterations{ 100000 };
	std::vector<std::string> keys;

	// Generate test keys
	for ( size_t i = 0; i < 1000; ++i )
	{
		keys.push_back( "key_" + std::to_string( i ) );
	}

	// Setup containers
	std::unordered_map<std::string, int> stdMap;
	StringMap<int> nfxMap;

	// Populate both containers
	for ( size_t i = 0; i < keys.size(); ++i )
	{
		stdMap[keys[i]] = static_cast<int>( i );
		nfxMap[keys[i]] = static_cast<int>( i );
	}

	std::cout << "Lookup performance test (" << iterations << " iterations):\n";

	// Test std::unordered_map with string construction
	auto start{ std::chrono::high_resolution_clock::now() };
	volatile int sum1{ 0 };
	for ( size_t i = 0; i < iterations; ++i )
	{
		std::string_view svKey{ keys[i % keys.size()] };
		// This creates a temporary std::string for lookup
		auto it{ stdMap.find( std::string{ svKey } ) };
		if ( it != stdMap.end() )
		{
			sum1 = sum1 + it->second;
		}
	}
	auto end{ std::chrono::high_resolution_clock::now() };
	auto stdTime{ std::chrono::duration_cast<std::chrono::microseconds>( end - start ) };

	// Test StringMap with zero-copy lookup
	start = std::chrono::high_resolution_clock::now();
	volatile int sum2{ 0 };
	for ( size_t i = 0; i < iterations; ++i )
	{
		std::string_view svKey{ keys[i % keys.size()] };
		// Zero-copy lookup - no temporary string created
		auto it{ nfxMap.find( svKey ) };
		if ( it != nfxMap.end() )
		{
			sum2 = sum2 + it->second;
		}
	}
	end = std::chrono::high_resolution_clock::now();
	auto nfxTime{ std::chrono::duration_cast<std::chrono::microseconds>( end - start ) };

	std::cout << "  std::unordered_map: " << stdTime.count() << "μs\n";
	std::cout << "  StringMap:          " << nfxTime.count() << "μs\n";
	std::cout << "  Performance gain:   " << std::fixed << std::setprecision( 1 )
			  << ( 100.0 * static_cast<double>( stdTime.count() - nfxTime.count() ) / static_cast<double>( stdTime.count() ) ) << "%\n";
	std::cout << "  (Both sums: " << sum1 << " vs " << sum2 << ")\n";
}

//=====================================================================
// StringSet usage demo
//=====================================================================

static void demoStringSetUsage()
{
	std::cout << "\n=== StringSet Usage Demo ===\n";

	StringSet allowedMethods{ "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS" };
	StringSet adminUsers{ "admin", "root", "superuser" };

	// Request validation function with zero-copy lookups
	auto validateRequest = [&]( std::string_view method, std::string_view user ) -> bool {
		bool validMethod{ allowedMethods.contains( method ) };
		bool isAdmin{ adminUsers.contains( user ) };

		std::cout << "Method: " << method << " (" << ( validMethod ? "valid" : "invalid" ) << "), ";
		std::cout << "User: " << user << " (" << ( isAdmin ? "admin" : "regular" ) << ")\n";

		return validMethod;
	};

	// Test various requests
	std::vector<std::pair<std::string_view, std::string_view>> requests{
		{ "GET", "admin" },
		{ "POST", "user123" },
		{ "PATCH", "admin" }, // Invalid method
		{ "DELETE", "superuser" },
		{ "INVALID", "hacker" } // Invalid method
	};

	std::cout << "Request validation:\n";
	for ( const auto& [method, user] : requests )
	{
		std::cout << "  ";
		bool valid{ validateRequest( method, user ) };
		std::cout << "  -> " << ( valid ? "✓ Allowed" : "✗ Rejected" ) << "\n";
	}

	std::cout << "\nAllowed methods: ";
	for ( const auto& method : allowedMethods )
	{
		std::cout << method << " ";
	}
	std::cout << "\n";
}

//=====================================================================
// HashMap with Robin Hood Algorithm demo
//=====================================================================

static void demoHashMap()
{
	std::cout << "\n=== HashMap with Robin Hood Algorithm ===\n";

	// The HashMap uses Robin Hood hashing which provides better worst-case performance
	// by keeping all probe distances bounded and redistributing "rich" entries
	nfx::containers::HashMap<std::string, int> scores;

	// Standard insertions using insertOrAssign - Robin Hood algorithm minimizes clustering
	scores.insertOrAssign( "Alice", 95 );
	scores.insertOrAssign( "Bob", 87 );
	scores.insertOrAssign( "Charlie", 92 );

	// Insert or assign - updates if key exists, inserts if new
	scores.insertOrAssign( "Diana", 89 );
	scores.insertOrAssign( "Bob", 91 ); // Updates Bob's score

	std::cout << "Initial scores (Robin Hood maintains optimal probe distances):\n";
	// Note: HashMap doesn't provide iterators, so we demonstrate key-based access
	std::vector<std::string> names{ "Alice", "Bob", "Charlie", "Diana" };
	for ( const auto& name : names )
	{
		int* score = nullptr;
		if ( scores.tryGetValue( name, score ) && score )
		{
			std::cout << "  " << name << ": " << *score << "\n";
		}
	}

	// Heterogeneous lookup with string_view - no temporary string construction
	std::string_view lookup{ "Alice" };
	int* score = nullptr;
	if ( scores.tryGetValue( lookup, score ) && score )
	{
		std::cout << "\nFound " << lookup << " with score: " << *score << "\n";
	}

	// tryGetValue - efficient lookup without iterator overhead
	int* charlieScore = nullptr;
	if ( scores.tryGetValue( "Charlie", charlieScore ) && charlieScore )
	{
		std::cout << "Charlie's score: " << *charlieScore << "\n";
	}

	// Demonstrate HashMap state information
	std::cout << "\nHashMap capacity: " << scores.capacity()
			  << ", size: " << scores.size() << "\n";

	// Erase operations maintain Robin Hood invariants
	bool erased = scores.erase( "Bob" );
	std::cout << "\nAfter erasing Bob (" << ( erased ? "success" : "not found" ) << "):\n";
	for ( const auto& name : names )
	{
		int* nameScore = nullptr;
		if ( scores.tryGetValue( name, nameScore ) && nameScore )
		{
			std::cout << "  " << name << ": " << *nameScore << "\n";
		}
		else
		{
			std::cout << "  " << name << ": <not found>\n";
		}
	}

	// Reserve capacity to prevent rehashing during batch operations
	scores.reserve( 100 );
	std::cout << "\nAfter reserve(100), capacity: " << scores.capacity() << "\n";

	// Bulk operations with optimal performance
	std::vector<std::pair<std::string, int>> newScores{
		{ "Eve", 94 },
		{ "Frank", 88 },
		{ "Grace", 96 },
		{ "Henry", 83 } };

	for ( const auto& [name, scoreValue] : newScores )
	{
		scores.insertOrAssign( name, scoreValue );
	}

	std::cout << "\nFinal scores count: " << scores.size() << "\n";
	std::cout << "Empty check: " << ( scores.isEmpty() ? "empty" : "not empty" ) << "\n";

	// Demonstrate heterogeneous lookup performance
	std::cout << "\n--- Robin Hood vs Standard Hashing Benefits ---\n";
	std::cout << "• Bounded probe distances (max displacement tracked)\n";
	std::cout << "• Better cache locality through displacement optimization\n";
	std::cout << "• Reduced variance in lookup times\n";
	std::cout << "• Automatic load balancing during insertions\n";
	std::cout << "• Zero-copy heterogeneous lookups (string_view -> string)\n";
}

//=====================================================================
// Main Demonstration
//=====================================================================

int main()
{
	std::cout << "NFX C++ Core - String Containers Demo\n";
	std::cout << "==========================================\n";
	std::cout << "Demonstrating zero-copy heterogeneous string containers:\n";
	std::cout << "- StringMap<T>: Enhanced unordered_map with string_view lookups\n";
	std::cout << "- StringSet: Enhanced unordered_set with string_view lookups\n";
	std::cout << "- HashMap<K,V>: Robin Hood hashing with bounded probe distances\n";
	std::cout << "- StringViewHash/Equal: Transparent functors for any container\n";

	try
	{
		demoConfigurationManagement();
		demoHttpHeaders();
		demoCachingSystem();
		demoHashMap();
		demoPerformanceComparison();
		demoStringSetUsage();

		std::cout << "\n=== Demo Complete ===\n";
		std::cout << "Key benefits demonstrated:\n";
		std::cout << "✓ Zero-copy string_view lookups\n";
		std::cout << "✓ Heterogeneous key operations\n";
		std::cout << "✓ Robin Hood hashing performance\n";
		std::cout << "✓ Performance improvements\n";
		std::cout << "✓ Drop-in STL compatibility\n";
		std::cout << "✓ Real-world usage patterns\n";
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
