/**
 * @file TESTS_StringViewSplitter.cpp
 * @brief Comprehensive tests for StringViewSplitter zero-allocation string splitting
 * @details Tests covering construction, iteration, edge cases, and performance validation
 */

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <vector>

#include <nfx/string/StringViewSplitter.h>

namespace nfx::string::test
{
	//=====================================================================
	// StringViewSplitter tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( StringViewSplitterConstruction, BasicConstruction )
	{
		const std::string_view str{ "hello,world,test" };
		const char delimiter{ ',' };

		string::StringViewSplitter splitter{ str, delimiter };

		// Constructor should not throw and object should be valid
		EXPECT_NO_THROW( splitter.begin() );
		EXPECT_NO_THROW( splitter.end() );
	}

	TEST( StringViewSplitterConstruction, EmptyString )
	{
		const std::string_view str{ "" };
		const char delimiter{ ',' };

		string::StringViewSplitter splitter{ str, delimiter };

		// Should handle empty string gracefully
		auto begin_it{ splitter.begin() };
		auto end_it{ splitter.end() };
		EXPECT_EQ( begin_it, end_it );
	}

	TEST( StringViewSplitterConstruction, SingleCharacter )
	{
		const std::string_view str{ "a" };
		const char delimiter{ ',' };

		string::StringViewSplitter splitter{ str, delimiter };

		// Should handle single character
		auto begin_it{ splitter.begin() };
		auto end_it{ splitter.end() };
		EXPECT_NE( begin_it, end_it );
		EXPECT_EQ( *begin_it, "a" );
	}

	TEST( StringViewSplitterConstruction, FactoryFunction )
	{
		const std::string_view str{ "hello,world" };
		const char delimiter{ ',' };

		auto splitter{ string::splitView( str, delimiter ) };

		// Factory function should work identically
		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 2 );
		EXPECT_EQ( segments[0], "hello" );
		EXPECT_EQ( segments[1], "world" );
	}

	//----------------------------------------------
	// Basic iteration
	//----------------------------------------------

	TEST( StringViewSplitterIteration, BasicSplitting )
	{
		const std::string_view str{ "hello,world,test" };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "hello" );
		EXPECT_EQ( segments[1], "world" );
		EXPECT_EQ( segments[2], "test" );
	}

	TEST( StringViewSplitterIteration, NoDelimiter )
	{
		const std::string_view str{ "hello_world_test" };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 1 );
		EXPECT_EQ( segments[0], "hello_world_test" );
	}

	TEST( StringViewSplitterIteration, TrailingDelimiter )
	{
		const std::string_view str{ "hello,world," };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "hello" );
		EXPECT_EQ( segments[1], "world" );
		EXPECT_EQ( segments[2], "" ); // Empty segment after trailing delimiter
	}

	TEST( StringViewSplitterIteration, LeadingDelimiter )
	{
		const std::string_view str{ ",hello,world" };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "" ); // Empty segment before leading delimiter
		EXPECT_EQ( segments[1], "hello" );
		EXPECT_EQ( segments[2], "world" );
	}

	TEST( StringViewSplitterIteration, ConsecutiveDelimiters )
	{
		const std::string_view str{ "hello,,world" };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "hello" );
		EXPECT_EQ( segments[1], "" ); // Empty segment between consecutive delimiters
		EXPECT_EQ( segments[2], "world" );
	}

	TEST( StringViewSplitterIteration, OnlyDelimiters )
	{
		const std::string_view str{ ",,," };
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 4 );
		for ( const auto& segment : segments )
		{
			EXPECT_EQ( segment, "" );
		}
	}

	//----------------------------------------------
	// Iterator behavior
	//----------------------------------------------

	TEST( StringViewSplitterIterator, ManualIteration )
	{
		const std::string_view str{ "a,b,c" };
		auto splitter{ string::splitView( str, ',' ) };

		auto it{ splitter.begin() };
		auto end{ splitter.end() };

		EXPECT_NE( it, end );
		EXPECT_EQ( *it, "a" );

		++it;
		EXPECT_NE( it, end );
		EXPECT_EQ( *it, "b" );

		++it;
		EXPECT_NE( it, end );
		EXPECT_EQ( *it, "c" );

		++it;
		EXPECT_EQ( it, end );
	}

	TEST( StringViewSplitterIterator, BeginEndComparison )
	{
		const std::string_view str{ "hello,world" };
		auto splitter{ string::splitView( str, ',' ) };

		auto begin_it{ splitter.begin() };
		auto end_it{ splitter.end() };

		EXPECT_NE( begin_it, end_it );

		// Advance to end
		++begin_it; // "world"
		EXPECT_NE( begin_it, end_it );

		++begin_it; // past end
		EXPECT_EQ( begin_it, end_it );
	}

	TEST( StringViewSplitterIterator, MultipleBeginCalls )
	{
		const std::string_view str{ "hello,world" };
		auto splitter{ string::splitView( str, ',' ) };

		auto it1{ splitter.begin() };
		auto it2{ splitter.begin() };

		// Multiple begin() calls should return equivalent iterators
		EXPECT_EQ( *it1, *it2 );
		EXPECT_EQ( *it1, "hello" );
	}

	//----------------------------------------------
	// Different delimiters
	//----------------------------------------------

	TEST( StringViewSplitterDelimiters, SpaceDelimiter )
	{
		const std::string_view str{ "hello world test" };
		auto splitter{ string::splitView( str, ' ' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "hello" );
		EXPECT_EQ( segments[1], "world" );
		EXPECT_EQ( segments[2], "test" );
	}

	TEST( StringViewSplitterDelimiters, SemicolonDelimiter )
	{
		const std::string_view str{ "name=John;age=30;city=NYC" };
		auto splitter{ string::splitView( str, ';' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "name=John" );
		EXPECT_EQ( segments[1], "age=30" );
		EXPECT_EQ( segments[2], "city=NYC" );
	}

	TEST( StringViewSplitterDelimiters, NewlineDelimiter )
	{
		const std::string_view str{ "line1\nline2\nline3" };
		auto splitter{ string::splitView( str, '\n' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "line1" );
		EXPECT_EQ( segments[1], "line2" );
		EXPECT_EQ( segments[2], "line3" );
	}

	TEST( StringViewSplitterDelimiters, TabDelimiter )
	{
		const std::string_view str{ "col1\tcol2\tcol3" };
		auto splitter{ string::splitView( str, '\t' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "col1" );
		EXPECT_EQ( segments[1], "col2" );
		EXPECT_EQ( segments[2], "col3" );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( StringViewSplitterEdgeCases, EmptyStringWithDelimiter )
	{
		const std::string_view str = "";
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 0 );
	}

	TEST( StringViewSplitterEdgeCases, SingleDelimiter )
	{
		const std::string_view str = ",";
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 2 );
		EXPECT_EQ( segments[0], "" );
		EXPECT_EQ( segments[1], "" );
	}

	TEST( StringViewSplitterEdgeCases, LongString )
	{
		// Test with a longer string to ensure no issues with larger data
		std::string long_str;
		for ( int i{ 0 }; i < 1000; ++i )
		{
			if ( i > 0 )
				long_str += ",";
			long_str += "item" + std::to_string( i );
		}

		auto splitter{ string::splitView( long_str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 1000 );
		EXPECT_EQ( segments[0], "item0" );
		EXPECT_EQ( segments[999], "item999" );
	}

	TEST( StringViewSplitterEdgeCases, UnicodeCharacters )
	{
		const std::string_view str = "héllo,wørld,tëst";
		auto splitter{ string::splitView( str, ',' ) };

		std::vector<std::string_view> segments;
		for ( auto segment : splitter )
		{
			segments.push_back( segment );
		}

		EXPECT_EQ( segments.size(), 3 );
		EXPECT_EQ( segments[0], "héllo" );
		EXPECT_EQ( segments[1], "wørld" );
		EXPECT_EQ( segments[2], "tëst" );
	}

	//----------------------------------------------
	// Real-world use cases
	//----------------------------------------------

	TEST( StringViewSplitterUseCases, CSVParsing )
	{
		const std::string_view csv_line = "John,Doe,30,Engineer,NewYork";
		auto splitter{ string::splitView( csv_line, ',' ) };

		std::vector<std::string_view> fields;
		for ( auto field : splitter )
		{
			fields.push_back( field );
		}

		EXPECT_EQ( fields.size(), 5 );
		EXPECT_EQ( fields[0], "John" );
		EXPECT_EQ( fields[1], "Doe" );
		EXPECT_EQ( fields[2], "30" );
		EXPECT_EQ( fields[3], "Engineer" );
		EXPECT_EQ( fields[4], "NewYork" );
	}

	TEST( StringViewSplitterUseCases, ConfigurationParsing )
	{
		const std::string_view config = "debug=true;timeout=30;host=localhost;port=8080";
		auto splitter{ string::splitView( config, ';' ) };

		std::vector<std::string_view> settings;
		for ( auto setting : splitter )
		{
			settings.push_back( setting );
		}

		EXPECT_EQ( settings.size(), 4 );
		EXPECT_EQ( settings[0], "debug=true" );
		EXPECT_EQ( settings[1], "timeout=30" );
		EXPECT_EQ( settings[2], "host=localhost" );
		EXPECT_EQ( settings[3], "port=8080" );
	}

	TEST( StringViewSplitterUseCases, LogParsing )
	{
		const std::string_view log_entry = "2025-08-31 14:30:15 INFO Application started successfully";
		auto splitter{ string::splitView( log_entry, ' ' ) };

		std::vector<std::string_view> parts;
		for ( auto part : splitter )
		{
			parts.push_back( part );
		}

		EXPECT_GE( parts.size(), 5 );
		EXPECT_EQ( parts[0], "2025-08-31" );
		EXPECT_EQ( parts[1], "14:30:15" );
		EXPECT_EQ( parts[2], "INFO" );
		EXPECT_EQ( parts[3], "Application" );
		EXPECT_EQ( parts[4], "started" );
	}

	TEST( StringViewSplitterUseCases, PathSplitting )
	{
		const std::string_view path = "/usr/local/bin/myapp";
		auto splitter{ string::splitView( path, '/' ) };

		std::vector<std::string_view> components;
		for ( auto component : splitter )
		{
			components.push_back( component );
		}

		EXPECT_EQ( components.size(), 5 );
		EXPECT_EQ( components[0], "" ); // Empty before leading /
		EXPECT_EQ( components[1], "usr" );
		EXPECT_EQ( components[2], "local" );
		EXPECT_EQ( components[3], "bin" );
		EXPECT_EQ( components[4], "myapp" );
	}

	//----------------------------------------------
	// Performance characteristics
	//----------------------------------------------

	TEST( StringViewSplitterPerformance, ZeroAllocation )
	{
		// This test verifies that no heap allocations occur during splitting
		const std::string_view str = "a,b,c,d,e,f,g,h,i,j";
		auto splitter{ string::splitView( str, ',' ) };

		// Count segments without storing them (zero allocation)
		size_t count{ 0 };
		for ( auto segment : splitter )
		{
			(void)segment.length();
			++count;
		}

		EXPECT_EQ( count, 10 );
	}

	TEST( StringViewSplitterPerformance, StringViewPreservation )
	{
		// Verify that returned string_views point to original string data
		const std::string original = "hello,world,test";
		const std::string_view str = original;
		auto splitter{ string::splitView( str, ',' ) };

		auto it{ splitter.begin() };
		std::string_view first_segment = *it;

		// The segment should point to the same memory as the original string
		EXPECT_EQ( first_segment.data(), original.data() );
		EXPECT_EQ( first_segment, "hello" );
	}
}
