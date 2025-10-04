/**
 * @file BM_StringUtilities.cpp
 * @brief Benchmark nfx::string::Utils performance vs standard library implementations
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <charconv>
#include <cctype>
#include <string>
#include <string_view>
#include <regex>
#include <vector>

#include <nfx/string/Utils.h>

namespace nfx::string::benchmark
{
	//=====================================================================
	// StringUtilities benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Test data
	//----------------------------------------------

	static const std::vector<std::string_view> test_strings = {
		"",
		"a",
		"Hello",
		"Hello World",
		"The quick brown fox jumps over the lazy dog",
		"   whitespace test   ",
		"12345",
		"abc123def",
		"UPPERCASE",
		"lowercase",
		"MixedCase",
		"https://example.com/path?query=value",
		"true",
		"false",
		"123456789",
		"-987654321",
		"3.14159",
		"not_a_number" };

	static const std::vector<char> test_chars = {
		'a', 'Z', '5', ' ', '\t', '\n', '!', '@', '#', '_', '-', '.', '~' };

	//----------------------------------------------
	// Character classification
	//----------------------------------------------

	//----------------------------
	// Spaces
	//----------------------------

	static void BM_Std_isspace( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = static_cast<bool>( std::isspace( static_cast<unsigned char>( c ) ) );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isWhitespace( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = nfx::string::isWhitespace( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Digit
	//----------------------------

	static void BM_Std_isdigit( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = static_cast<bool>( std::isdigit( static_cast<unsigned char>( c ) ) );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isDigit( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = nfx::string::isDigit( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Alpha
	//----------------------------

	static void BM_Std_isalpha( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = static_cast<bool>( std::isalpha( static_cast<unsigned char>( c ) ) );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isAlpha( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = nfx::string::isAlpha( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// String validation
	//----------------------------------------------

	//----------------------------
	// Empty
	//----------------------------

	static void BM_Std_empty( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = str.empty();
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isEmpty( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::isEmpty( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Null or whitespace
	//----------------------------

	static void BM_Manual_isNullOrWhiteSpace( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = str.empty() || std::all_of( str.begin(), str.end(),
												 []( char c ) { return std::isspace( static_cast<unsigned char>( c ) ); } );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isNullOrWhiteSpace( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::isNullOrWhiteSpace( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// All digits
	//----------------------------

	static void BM_Manual_isAllDigits( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = !str.empty() && std::all_of( str.begin(), str.end(),
												  []( char c ) { return std::isdigit( static_cast<unsigned char>( c ) ); } );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isAllDigits( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::isAllDigits( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// String operations
	//----------------------------------------------

	//----------------------------
	// Starts with
	//----------------------------

	static void BM_Std_starts_with( ::benchmark::State& state )
	{
		const std::string_view prefix = "Hello";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = str.starts_with( prefix );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_startsWith( ::benchmark::State& state )
	{
		const std::string_view prefix = "Hello";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::startsWith( str, prefix );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Ends with
	//----------------------------

	static void BM_Std_ends_with( ::benchmark::State& state )
	{
		const std::string_view suffix = "dog";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = str.ends_with( suffix );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_endsWith( ::benchmark::State& state )
	{
		const std::string_view suffix = "dog";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::endsWith( str, suffix );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Contains
	//----------------------------

	static void BM_Std_contains( ::benchmark::State& state )
	{
		const std::string_view substr = "fox";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = str.find( substr ) != std::string_view::npos;
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_contains( ::benchmark::State& state )
	{
		const std::string_view substr = "fox";
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				bool result = nfx::string::contains( str, substr );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// String trimming
	//----------------------------------------------

	static void BM_Manual_trim( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				auto start = str.find_first_not_of( " \t\n\r\f\v" );
				if ( start == std::string_view::npos )
				{
					auto result = std::string_view{};
					::benchmark::DoNotOptimize( result );
					continue;
				}
				auto end = str.find_last_not_of( " \t\n\r\f\v" );
				auto result = str.substr( start, end - start + 1 );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_trim( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				auto result = nfx::string::trim( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// Case conversion
	//----------------------------------------------

	//----------------------------
	// To lower
	//----------------------------

	static void BM_Std_tolower( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				char result = static_cast<char>( std::tolower( static_cast<unsigned char>( c ) ) );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_Std_transform_tolower( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				std::string result{ str };
				std::transform( result.begin(), result.end(), result.begin(),
					[]( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_toLower_char( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				char result = nfx::string::toLower( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_toLower_string( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				auto result = nfx::string::toLower( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// To upper
	//----------------------------

	static void BM_Std_toupper( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				char result = static_cast<char>( std::toupper( static_cast<unsigned char>( c ) ) );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_Std_transform_toupper( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				std::string result{ str };
				std::transform( result.begin(), result.end(), result.begin(),
					[]( unsigned char c ) { return static_cast<char>( std::toupper( c ) ); } );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_toUpper_char( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				char result = nfx::string::toUpper( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_toUpper_string( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( const auto& str : test_strings )
			{
				auto result = nfx::string::toUpper( str );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	//----------------------------
	// Int
	//----------------------------

	static void BM_Std_from_chars_int( ::benchmark::State& state )
	{
		const std::vector<std::string_view> int_strings = { "123", "-456", "0", "999999", "not_a_number" };
		for ( auto _ : state )
		{
			for ( const auto& str : int_strings )
			{
				int result;
				auto [ptr, ec] = std::from_chars( str.data(), str.data() + str.size(), result );
				bool success = ( ec == std::errc{} );
				::benchmark::DoNotOptimize( success );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_tryParseInt( ::benchmark::State& state )
	{
		const std::vector<std::string_view> int_strings = { "123", "-456", "0", "999999", "not_a_number" };
		for ( auto _ : state )
		{
			for ( const auto& str : int_strings )
			{
				int result;
				bool success = nfx::string::tryParseInt( str, result );
				::benchmark::DoNotOptimize( success );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Double
	//----------------------------

	static void BM_Std_from_chars_double( ::benchmark::State& state )
	{
		const std::vector<std::string_view> double_strings = { "3.14", "-2.718", "0.0", "1e6", "not_a_number" };
		for ( auto _ : state )
		{
			for ( const auto& str : double_strings )
			{
				double result;
				auto [ptr, ec] = std::from_chars( str.data(), str.data() + str.size(), result );
				bool success = ( ec == std::errc{} );
				::benchmark::DoNotOptimize( success );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_tryParseDouble( ::benchmark::State& state )
	{
		const std::vector<std::string_view> double_strings = { "3.14", "-2.718", "0.0", "1e6", "not_a_number" };
		for ( auto _ : state )
		{
			for ( const auto& str : double_strings )
			{
				double result;
				bool success = nfx::string::tryParseDouble( str, result );
				::benchmark::DoNotOptimize( success );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------------------------
	// URI character classification
	//----------------------------------------------

	//----------------------------
	// Reserved
	//----------------------------

	static void BM_Manual_isURIReserved( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = ( c == ':' || c == '/' || c == '?' || c == '#' || c == '[' ||
								c == ']' || c == '@' || c == '!' || c == '$' || c == '&' ||
								c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' ||
								c == ',' || c == ';' || c == '=' );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isURIReserved( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = nfx::string::isURIReserved( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	//----------------------------
	// Unreserved
	//----------------------------

	static void BM_Manual_isURIUnreserved( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) ||
								( c >= '0' && c <= '9' ) || c == '-' || c == '.' ||
								c == '_' || c == '~' );
				::benchmark::DoNotOptimize( result );
			}
		}
	}

	static void BM_NFX_isURIUnreserved( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			for ( char c : test_chars )
			{
				bool result = nfx::string::isURIUnreserved( c );
				::benchmark::DoNotOptimize( result );
			}
		}
	}
} // namespace nfx::string::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Character Classification
//----------------------------------------------

//----------------------------
// Spaces
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_isspace )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isWhitespace )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Digit
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_isdigit )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isDigit )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Alpha
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_isalpha )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isAlpha )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// String Validation
//----------------------------------------------

//----------------------------
// Empty
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_empty )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isEmpty )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Null or whitespace
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Manual_isNullOrWhiteSpace )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isNullOrWhiteSpace )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// All digits
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Manual_isAllDigits )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isAllDigits )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// String Operations
//----------------------------------------------

//----------------------------
// Starts with
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_starts_with )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_startsWith )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Ends with
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_ends_with )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_endsWith )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Contains
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_contains )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_contains )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// String Trimming
//----------------------------------------------

BENCHMARK( nfx::string::benchmark::BM_Manual_trim )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_trim )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// Case Conversion
//----------------------------------------------

//----------------------------
// To lower
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_tolower )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_Std_transform_tolower )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_toLower_char )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_toLower_string )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// To upper
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_toupper )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_Std_transform_toupper )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_toUpper_char )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_toUpper_string )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// Parsing
//----------------------------------------------

//----------------------------
// Int
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_from_chars_int )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_tryParseInt )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Double
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Std_from_chars_double )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_tryParseDouble )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------------------------
// URI Character Classification
//----------------------------------------------

//----------------------------
// Reserved
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Manual_isURIReserved )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isURIReserved )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

//----------------------------
// Unreserved
//----------------------------

BENCHMARK( nfx::string::benchmark::BM_Manual_isURIUnreserved )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK( nfx::string::benchmark::BM_NFX_isURIUnreserved )
	->MinTime( 1.0 )
	->Unit( benchmark::kNanosecond );

BENCHMARK_MAIN();
