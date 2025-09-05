/**
 * @file Sample_StringBuilder.cpp
 * @brief Demonstrates high-performance string building with StringBuilderPool
 * @details This sample shows how to use StringBuilderPool for zero-allocation string operations,
 *          including pooled buffers, efficient concatenation, streaming operations,
 *          and enterprise-grade string building patterns for maximum performance
 */

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <nfx/string/StringBuilderPool.h>

using namespace nfx::string;

int main()
{
	std::cout << "=== NFX C++ Core - StringBuilderPool Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Basic StringBuilderPool usage
	//=========================================================================

	std::cout << "--- Basic StringBuilder Operations ---" << std::endl;

	// Acquire a lease from the pool
	auto lease{ StringBuilderPool::lease() };
	auto builder{ lease.builder() };

	// Basic string building
	builder.append( "Hello" );
	builder.append( ", " );
	builder.append( "World" );
	builder.push_back( '!' );

	std::cout << "Basic concatenation: " << lease.toString() << std::endl;

	// Clear and reuse the same buffer
	lease.buffer().clear();
	std::cout << "Buffer is empty after clear: " << ( lease.buffer().isEmpty() ? "Yes" : "No" ) << std::endl;

	// Stream operators for fluent interface
	builder << "Stream " << "operators " << "are " << "convenient!";
	std::cout << "Stream operators: " << lease.toString() << std::endl;

	std::cout << "Buffer capacity: " << lease.buffer().capacity() << " characters" << std::endl;
	std::cout << "Buffer size: " << lease.buffer().size() << " characters" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Performance comparison with std::string
	//=========================================================================

	std::cout << "--- Performance Comparison ---" << std::endl;

	const int iterations{ 1000 };
	const std::string segment{ "Performance test segment " };

	// StringBuilder performance test
	auto start_sb{ std::chrono::high_resolution_clock::now() };
	{
		auto perf_lease{ StringBuilderPool::lease() };
		auto perf_builder{ perf_lease.builder() };

		// Reserve capacity for better performance
		perf_lease.buffer().reserve( iterations * segment.size() );

		for ( int i{ 0 }; i < iterations; ++i )
		{
			perf_builder << segment << std::to_string( i ) << " ";
		}

		auto result{ perf_lease.toString() };
		std::cout << "StringBuilder result length: " << result.size() << " characters" << std::endl;
	}
	auto end_sb{ std::chrono::high_resolution_clock::now() };
	auto duration_sb{ std::chrono::duration_cast<std::chrono::microseconds>( end_sb - start_sb ) };

	// std::string performance test
	auto start_str{ std::chrono::high_resolution_clock::now() };
	{
		std::string result;
		result.reserve( iterations * ( segment.size() + 10 ) );

		for ( int i{ 0 }; i < iterations; ++i )
		{
			result += segment + std::to_string( i ) + " ";
		}

		std::cout << "std::string result length: " << result.size() << " characters" << std::endl;
	}
	auto end_str{ std::chrono::high_resolution_clock::now() };
	auto duration_str{ std::chrono::duration_cast<std::chrono::microseconds>( end_str - start_str ) };

	// std::ostringstream performance test
	auto start_oss{ std::chrono::high_resolution_clock::now() };
	{
		std::ostringstream oss;

		for ( int i{ 0 }; i < iterations; ++i )
		{
			oss << segment << i << " ";
		}

		auto result{ oss.str() };
		std::cout << "std::ostringstream result length: " << result.size() << " characters" << std::endl;
	}
	auto end_oss{ std::chrono::high_resolution_clock::now() };
	auto duration_oss{ std::chrono::duration_cast<std::chrono::microseconds>( end_oss - start_oss ) };

	std::cout << std::endl
			  << "Performance Results (" << iterations << " iterations):" << std::endl;
	std::cout << "  StringBuilder:     " << std::setw( 6 ) << duration_sb.count() << " μs" << std::endl;
	std::cout << "  std::string:       " << std::setw( 6 ) << duration_str.count() << " μs" << std::endl;
	std::cout << "  std::ostringstream:" << std::setw( 6 ) << duration_oss.count() << " μs" << std::endl;

	if ( duration_sb.count() > 0 )
	{
		std::cout << "  StringBuilder speedup vs std::string: "
				  << std::fixed << std::setprecision( 1 )
				  << ( static_cast<double>( duration_str.count() ) / static_cast<double>( duration_sb.count() ) ) << "x" << std::endl;
		std::cout << "  StringBuilder speedup vs ostringstream: "
				  << std::fixed << std::setprecision( 1 )
				  << ( static_cast<double>( duration_oss.count() ) / static_cast<double>( duration_sb.count() ) ) << "x" << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Pool statistics and reuse demonstration
	//=========================================================================

	std::cout << "--- Pool Statistics and Reuse ---" << std::endl;

	// Reset statistics for clean measurement
	StringBuilderPool::resetStats();

	// Create several leases to demonstrate pooling
	{
		auto lease1{ StringBuilderPool::lease() };
		lease1.builder().append( "First lease content" );
		std::cout << "Lease 1: " << lease1.toString() << std::endl;
	} // lease1 returns to pool

	{
		auto lease2{ StringBuilderPool::lease() };
		lease2.builder().append( "Second lease content" );
		std::cout << "Lease 2: " << lease2.toString() << std::endl;
	} // lease2 returns to pool

	{
		auto lease3{ StringBuilderPool::lease() };
		lease3.builder().append( "Third lease content" );
		std::cout << "Lease 3: " << lease3.toString() << std::endl;
	} // lease3 returns to pool

	// Display pool statistics
	auto stats{ StringBuilderPool::stats() };
	std::cout << std::endl
			  << "Pool Statistics:" << std::endl;
	std::cout << "  Total requests: " << stats.totalRequests << std::endl;
	std::cout << "  Thread-local hits: " << stats.threadLocalHits << std::endl;
	std::cout << "  Shared pool hits: " << stats.dynamicStringBufferPoolHits << std::endl;
	std::cout << "  New allocations: " << stats.newAllocations << std::endl;
	std::cout << "  Hit rate: " << std::fixed << std::setprecision( 1 ) << ( stats.hitRate * 100.0 ) << "%" << std::endl;
	std::cout << "  Current pool size: " << StringBuilderPool::size() << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Advanced string building patterns
	//=========================================================================

	std::cout << "--- Advanced String Building Patterns ---" << std::endl;

	// JSON-like object construction
	{
		auto json_lease{ StringBuilderPool::lease() };
		auto json_builder{ json_lease.builder() };

		json_builder << "{\n";
		json_builder << "  \"name\": \"StringBuilderPool\",\n";
		json_builder << "  \"version\": \"1.0\",\n";
		json_builder << "  \"performance\": {\n";
		json_builder << "    \"fast\": true,\n";
		json_builder << "    \"memory_efficient\": true\n";
		json_builder << "  },\n";
		json_builder << "  \"features\": [\"pooling\", \"streaming\", \"zero-copy\"]\n";
		json_builder << "}";

		std::cout << "JSON construction:" << std::endl;
		std::cout << json_lease.toString() << std::endl;
	}
	std::cout << std::endl;

	// SQL query building
	{
		auto sql_lease{ StringBuilderPool::lease() };
		auto sql_builder{ sql_lease.builder() };

		std::vector<std::string> columns{ "id", "name", "email", "created_at" };
		std::vector<std::string> conditions{ "active = 1", "age > 18", "country = 'US'" };

		sql_builder << "SELECT ";
		for ( size_t i{ 0 }; i < columns.size(); ++i )
		{
			if ( i > 0 )
				sql_builder << ", ";
			sql_builder << columns[i];
		}

		sql_builder << " FROM users WHERE ";
		for ( size_t i{ 0 }; i < conditions.size(); ++i )
		{
			if ( i > 0 )
				sql_builder << " AND ";
			sql_builder << conditions[i];
		}

		sql_builder << " ORDER BY created_at DESC LIMIT 100";

		std::cout << "SQL query building:" << std::endl;
		std::cout << sql_lease.toString() << std::endl;
	}
	std::cout << std::endl;

	// Log message formatting
	{
		auto log_lease{ StringBuilderPool::lease() };
		auto log_builder{ log_lease.builder() };

		log_builder << "[2025-08-31 14:30:00 UTC] ";
		log_builder << "INFO: StringBuilderPool sample running successfully. ";
		log_builder << "Memory usage optimized, performance enhanced.";

		std::cout << "Log message formatting:" << std::endl;
		std::cout << log_lease.toString() << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Iterator and enumeration examples
	//=========================================================================

	std::cout << "--- Iterator and Enumeration Examples ---" << std::endl;

	{
		auto iter_lease{ StringBuilderPool::lease() };
		auto iterBuilder{ iter_lease.builder() };
		iterBuilder.append( "Iterator Demo" );

		std::cout << "Original content: " << iter_lease.toString() << std::endl;

		// Range-based for loop
		std::cout << "Characters via range-based for: ";
		for ( char c : iterBuilder )
		{
			std::cout << c << " ";
		}
		std::cout << std::endl;

		std::cout << "Characters via manual iteration: ";
		std::for_each( iterBuilder.begin(), iterBuilder.end(), []( char c ) {
			std::cout << c << " ";
		} );
		std::cout << std::endl;

		// Enumerator pattern
		std::cout << "Characters via enumerator: ";
		StringBuilder::Enumerator enumerator{ iterBuilder };
		while ( enumerator.next() )
		{
			std::cout << enumerator.current() << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Memory management and capacity demonstration
	//=========================================================================

	std::cout << "--- Memory Management and Capacity ---" << std::endl;

	{
		auto mem_lease{ StringBuilderPool::lease() };
		auto& mem_buffer{ mem_lease.buffer() };
		auto mem_builder{ mem_lease.builder() };

		std::cout << "Initial capacity: " << mem_buffer.capacity() << std::endl;

		// Reserve larger capacity
		mem_buffer.reserve( 2048 );
		std::cout << "After reserve(2048): " << mem_buffer.capacity() << std::endl;

		// Add content
		for ( int i{ 0 }; i < 10; ++i )
		{
			mem_builder << "Content block " << std::to_string( i ) << " - ";
		}

		std::cout << "Content size: " << mem_buffer.size() << std::endl;
		std::cout << "Capacity after content: " << mem_buffer.capacity() << std::endl;
		std::cout << "Content preview: " << mem_lease.toString().substr( 0, 50 ) << "..." << std::endl;

		// Array access
		if ( mem_buffer.size() > 10 )
		{
			std::cout << "Character at position 8: '" << mem_builder[8] << "'" << std::endl;
		}

		// Resize operations
		size_t original_size{ mem_buffer.size() };
		mem_builder.resize( 20 );
		std::cout << "After resize to 20: \"" << mem_lease.toString() << "\"" << std::endl;

		mem_builder.resize( original_size );
		std::cout << "After resize back to original: size = " << mem_buffer.size() << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Thread safety demonstration
	//=========================================================================

	std::cout << "--- Thread Safety Demonstration ---" << std::endl;

	std::vector<std::thread> threads;
	std::vector<std::string> results{ 4 };

	// Reset pool statistics
	StringBuilderPool::resetStats();

	for ( size_t t{ 0 }; t < 4; ++t )
	{
		threads.emplace_back( [t, &results]() {
			auto thread_lease{ StringBuilderPool::lease() };
			auto thread_builder{ thread_lease.builder() };

			thread_builder << "Thread " << std::to_string( t ) << " processing: ";
			for ( int i{ 0 }; i < 10; ++i )
			{
				thread_builder << "[" << std::to_string( i ) << "]";
			}

			results[t] = thread_lease.toString();
		} );
	}

	// Wait for all threads to complete
	for ( auto& thread : threads )
	{
		thread.join();
	}

	std::cout << "Thread results:" << std::endl;
	for ( size_t i{ 0 }; i < results.size(); ++i )
	{
		std::cout << "  " << results[i] << std::endl;
	}

	auto thread_stats{ StringBuilderPool::stats() };
	std::cout << std::endl
			  << "Multi-threaded statistics:" << std::endl;
	std::cout << "  Total requests: " << thread_stats.totalRequests << std::endl;
	std::cout << "  Hit rate: " << std::fixed << std::setprecision( 1 ) << ( thread_stats.hitRate * 100.0 ) << "%" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Real-world use case: CSV generation
	//=========================================================================

	std::cout << "--- Real-World Use Case: CSV Generation ---" << std::endl;

	struct Product
	{
		std::string name;
		double price;
		int quantity;
		std::string category;
	};

	std::vector<Product> products{
		{ "Laptop", 999.99, 50, "Electronics" },
		{ "Mouse", 29.99, 200, "Electronics" },
		{ "Keyboard", 79.99, 100, "Electronics" },
		{ "Monitor", 299.99, 75, "Electronics" },
		{ "Desk Chair", 199.99, 25, "Furniture" } };

	auto csv_lease{ StringBuilderPool::lease() };
	auto csv_builder{ csv_lease.builder() };

	// CSV header
	csv_builder << "Name,Price,Quantity,Category,Total Value\n";

	// CSV data rows
	double grand_total{ 0.0 };
	for ( const auto& product : products )
	{
		double total_value{ product.price * product.quantity };
		grand_total += total_value;

		csv_builder << product.name << ",";
		csv_builder << std::to_string( product.price ) << ",";
		csv_builder << std::to_string( product.quantity ) << ",";
		csv_builder << product.category << ",";
		csv_builder << std::to_string( total_value ) << "\n";
	}

	// Summary row
	csv_builder << "TOTAL,,,," << std::to_string( grand_total );

	std::cout << "Generated CSV:" << std::endl;
	std::cout << csv_lease.toString() << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Clean up and final statistics
	//=========================================================================

	std::cout << "--- Final Pool Statistics ---" << std::endl;

	auto final_stats{ StringBuilderPool::stats() };
	std::cout << "Session summary:" << std::endl;
	std::cout << "  Total pool requests: " << final_stats.totalRequests << std::endl;
	std::cout << "  Cache efficiency: " << std::fixed << std::setprecision( 1 ) << ( final_stats.hitRate * 100.0 ) << "%" << std::endl;
	std::cout << "  Current pool size: " << StringBuilderPool::size() << std::endl;

	// Clear the pool
	size_t cleared{ StringBuilderPool::clear() };
	std::cout << "  Cleared " << cleared << " buffers from pool" << std::endl;
	std::cout << "  Final pool size: " << StringBuilderPool::size() << std::endl;

	std::cout << std::endl;
	std::cout << "=== StringBuilderPool sample completed successfully ===" << std::endl;
	std::cout << "This demonstrates high-performance, zero-allocation string building" << std::endl;
	std::cout << "with automatic memory management and thread-safe pooling!" << std::endl;

	return 0;
}
