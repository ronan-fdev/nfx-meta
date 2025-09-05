/**
 * @file Sample_StringViewSplitter.cpp
 * @brief Demonstrates zero-allocation string splitting with StringViewSplitter
 * @details This sample shows how to use StringViewSplitter for high-performance
 *          string processing in real-world scenarios like CSV parsing, configuration
 *          files, log analysis, and path manipulation
 */

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <nfx/string/StringViewSplitter.h>

int main()
{
	std::cout << "=== NFX C++ Core - StringViewSplitter Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Basic string splitting - CSV data processing
	//=========================================================================

	std::cout << "--- CSV Data Processing ---" << std::endl;

	const std::string_view csv_line{ "John,Doe,30,Engineer,NewYork,75000" };
	auto csv_splitter{ nfx::string::splitView( csv_line, ',' ) };

	std::cout << "Original CSV line: " << csv_line << std::endl;
	std::cout << "Parsed fields:" << std::endl;

	// Using range-based for loop (most convenient)
	size_t field_index{};
	const std::array<const char*, 6> field_names{ "First Name", "Last Name", "Age", "Job", "City", "Salary" };

	for ( const auto field : csv_splitter )
	{
		const char* field_name = ( field_index < field_names.size() ) ? field_names.at( field_index ) : "Unknown";
		std::cout << "  " << field_name << ": " << field << std::endl;
		++field_index;
	}

	std::cout << "Total fields: " << field_index << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Configuration file processing
	//=========================================================================

	std::cout << "--- Configuration File Processing ---" << std::endl;

	const std::string_view config_data{ "server=localhost;port=8080;database=mydb;timeout=30;ssl=true;debug=false" };
	auto config_splitter{ nfx::string::splitView( config_data, ';' ) };

	std::cout << "Configuration string: " << config_data << std::endl;
	std::cout << "Configuration settings:" << std::endl;

	for ( const auto setting : config_splitter )
	{
		// Further split each setting by '=' to get key-value pairs
		auto kv_splitter{ nfx::string::splitView( setting, '=' ) };
		auto it{ kv_splitter.begin() };

		if ( it != kv_splitter.end() )
		{
			std::string_view key{ *it };
			++it;

			if ( it != kv_splitter.end() )
			{
				std::string_view value{ *it };
				std::cout << "  " << std::setw( 10 ) << key << " = " << value << std::endl;
			}
		}
	}

	std::cout << std::endl;

	//=========================================================================
	// Log file analysis
	//=========================================================================

	std::cout << "--- Log File Analysis ---" << std::endl;

	const std::string_view log_entries[] = {
		"2025-08-31 09:15:32 INFO Application started successfully",
		"2025-08-31 09:15:33 DEBUG Loading configuration from config.xml",
		"2025-08-31 09:15:34 INFO Database connection established",
		"2025-08-31 09:15:35 WARN Cache size limit reached, cleaning up",
		"2025-08-31 09:15:36 ERROR Failed to process request: timeout" };

	std::cout << "Analyzing log entries:" << std::endl;

	for ( const auto& log_entry : log_entries )
	{
		auto log_splitter{ nfx::string::splitView( log_entry, ' ' ) };
		auto it{ log_splitter.begin() };

		// Extract log components
		std::string_view date{};
		std::string_view time{};
		std::string_view level{};

		if ( it != log_splitter.end() )
		{
			date = *it;
			++it;
		}
		if ( it != log_splitter.end() )
		{
			time = *it;
			++it;
		}
		if ( it != log_splitter.end() )
		{
			level = *it;
			++it;
		}

		// Collect remaining parts as message
		std::string message;
		while ( it != log_splitter.end() )
		{
			if ( !message.empty() )
			{
				message += " ";
			}
			message += *it;
			++it;
		}

		std::cout << "  [" << level << "] " << date << " " << time << " - " << message << std::endl;
	}

	std::cout << std::endl;

	//=========================================================================
	// Path manipulation
	//=========================================================================

	std::cout << "--- Path Manipulation ---" << std::endl;

	const std::string_view paths[]{
		"/usr/local/bin/myapp",
		"C:\\Program Files\\MyApp\\bin\\app.exe",
		"../config/settings.ini",
		"./data/output.csv" };

	for ( const auto& path : paths )
	{
		std::cout << "Original path: " << path << std::endl;

		// Determine delimiter based on path style
		char delimiter{ ( path.find( '\\' ) != std::string_view::npos ) ? '\\' : '/' };
		auto path_splitter{ nfx::string::splitView( path, delimiter ) };

		std::cout << "  Components: ";
		bool first{ true };
		for ( const auto component : path_splitter )
		{
			if ( !first )
			{
				std::cout << " -> ";
			}
			if ( component.empty() )
			{
				std::cout << "[root]";
			}
			else
			{
				std::cout << component;
			}
			first = false;
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;

	//=========================================================================
	// Performance demonstration - Zero allocation
	//=========================================================================

	std::cout << "--- Performance Demonstration ---" << std::endl;

	// Large CSV-like data for performance testing
	std::string large_data;
	for ( int i{}; i < 1000; ++i )
	{
		if ( i > 0 )
		{
			large_data += ",";
		}
		large_data += "field" + std::to_string( i );
	}

	auto start_time{ std::chrono::high_resolution_clock::now() };

	// Process large data with zero allocations
	auto large_splitter{ nfx::string::splitView( large_data, ',' ) };
	size_t count{};
	for ( const auto field : large_splitter )
	{
		count++;
		(void)field;
	}

	auto end_time{ std::chrono::high_resolution_clock::now() };
	auto duration{ std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ) };

	std::cout << "Processed " << count << " fields from large dataset" << std::endl;
	std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
	std::cout << "Performance: " << ( static_cast<double>( count ) * 1000000.0 / static_cast<double>( duration.count() ) ) << " fields/second" << std::endl;
	std::cout << "Memory allocations: 0 (all string_views point to original data)" << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Edge cases and delimiter variations
	//=========================================================================

	std::cout << "--- Edge Cases and Different Delimiters ---" << std::endl;

	// Test various edge cases
	const std::string_view edge_cases[]{
		"",		  // Empty string
		",",	  // Single delimiter
		"hello,", // Trailing delimiter
		",world", // Leading delimiter
		"a,,b",	  // Consecutive delimiters
		"single"  // No delimiters
	};

	for ( const auto& test_case : edge_cases )
	{
		std::cout << "Input: \"" << test_case << "\"" << std::endl;
		std::cout << "  Segments: ";

		auto edge_splitter{ nfx::string::splitView( test_case, ',' ) };
		bool first{ true };
		for ( const auto segment : edge_splitter )
		{
			if ( !first )
			{
				std::cout << " | ";
			}
			std::cout << "\"" << segment << "\"";
			first = false;
		}
		if ( first )
			std::cout << "(no segments)";
		std::cout << std::endl;
	}

	std::cout << std::endl;

	// Different delimiter examples
	const struct
	{
		std::string_view data;
		char delimiter;
		const char* description;
	} delimiter_examples[]{
		{ "apple banana cherry date", ' ', "Space-separated words" },
		{ "line1\nline2\nline3", '\n', "Newline-separated lines" },
		{ "col1\tcol2\tcol3", '\t', "Tab-separated columns" },
		{ "key1=val1;key2=val2;key3=val3", ';', "Semicolon-separated pairs" },
		{ "192.168.1.1", '.', "Dot-separated IP address" } };

	std::cout << "--- Different Delimiter Examples ---" << std::endl;

	for ( const auto& example : delimiter_examples )
	{
		std::cout << example.description << ":" << std::endl;
		std::cout << "  Input: \"" << example.data << "\"" << std::endl;
		std::cout << "  Parts: ";

		auto delim_splitter{ nfx::string::splitView( example.data, example.delimiter ) };
		bool first{ true };
		for ( const auto part : delim_splitter )
		{
			if ( !first )
			{
				std::cout << " | ";
			}
			std::cout << "\"" << part << "\"";
			first = false;
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;

	//=========================================================================
	// Manual iterator usage
	//=========================================================================

	std::cout << "--- Manual Iterator Usage ---" << std::endl;

	const std::string_view manual_data{ "first,second,third,fourth,fifth" };
	auto manual_splitter{ nfx::string::splitView( manual_data, ',' ) };

	std::cout << "Manual iteration through: " << manual_data << std::endl;

	auto it{ manual_splitter.begin() };
	auto end{ manual_splitter.end() };
	size_t index{};

	while ( it != end )
	{
		std::cout << "  [" << index++ << "] = \"" << *it << "\"" << std::endl;
		++it;
	}

	// Demonstrate iterator comparison
	auto it1{ manual_splitter.begin() };
	auto it2{ manual_splitter.begin() };
	std::cout << "Two begin iterators are equal: " << ( it1 == it2 ? "Yes" : "No" ) << std::endl;

	++it1;
	std::cout << "After advancing first iterator: " << ( it1 == it2 ? "Equal" : "Different" ) << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Memory safety demonstration
	//=========================================================================

	std::cout << "--- Memory Safety Demonstration ---" << std::endl;

	const std::string original_string{ "memory,safety,test,data" };
	const std::string_view string_view{ original_string };

	auto safety_splitter{ nfx::string::splitView( string_view, ',' ) };
	auto safety_it{ safety_splitter.begin() };

	std::string_view first_segment{ *safety_it };

	std::cout << "Original string: \"" << original_string << "\"" << std::endl;
	std::cout << "First segment: \"" << first_segment << "\"" << std::endl;
	std::cout << "Segment points to original memory: " << ( first_segment.data() == original_string.data() ? "Yes" : "No" ) << std::endl;
	std::cout << "No additional memory allocated: StringViewSplitter uses zero-copy design" << std::endl;

	std::cout << std::endl;
	std::cout << "=== StringViewSplitter sample completed successfully ===" << std::endl;
	std::cout << "This demonstrates zero-allocation, high-performance string splitting" << std::endl;
	std::cout << "suitable for performance-critical applications and large-scale data processing." << std::endl;

	return 0;
}
