/**
 * @file Sample_StringUtils.cpp
 * @brief Comprehensive demonstration of NFX StringUtils library
 * @details This sample shows how to use StringUtils for high-performance string operations
 *          including validation, parsing, matching, comparison, and case conversion with
 *          zero-allocation design for maximum efficiency.
 */

#include <iostream>
#include <vector>
#include <chrono>

#include <nfx/string/Utils.h>

using namespace nfx::string;

int main()
{
	std::cout << "=== NFX C++ Core - StringUtils Library Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// String Validation - Fast length and emptiness checks
	//=========================================================================

	std::cout << "--- String Validation ---" << std::endl;

	// Length validation examples
	std::string_view password{ "MySecurePassword123!" };
	std::cout << "Password validation:" << std::endl;
	std::cout << "  Password: \"" << password << "\"" << std::endl;
	std::cout << "  Has exactly 20 characters: " << ( hasExactLength( password, 20 ) ? "Yes" : "No" ) << std::endl;
	std::cout << "  Has exactly 8 characters: " << ( hasExactLength( password, 8 ) ? "Yes" : "No" ) << std::endl;

	// Empty string checks
	std::vector<std::string_view> inputs{ "", "   ", "content", "\t", "a" };
	std::cout << std::endl
			  << "Empty string detection:" << std::endl;

	for ( const auto& input : inputs )
	{
		std::cout << "  \"" << input << "\" is empty: " << ( isEmpty( input ) ? "Yes" : "No" ) << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Boolean Parsing - Comprehensive format support
	//=========================================================================

	std::cout << "--- Boolean Parsing ---" << std::endl;

	// Test various boolean formats
	std::vector<std::string_view> boolInputs{
		"true", "TRUE", "True",
		"1", "t", "T", "yes",
		"YES", "on", "ON",
		"false", "FALSE", "False",
		"0", "f", "F", "no",
		"NO", "off", "OFF",
		"maybe", "2", "",
		"invalid" };

	std::cout << "Boolean parsing results:" << std::endl;
	for ( const auto& input : boolInputs )
	{
		bool result{};
		bool success{ tryParseBool( input, result ) };
		std::cout << "  \"" << input << "\" -> ";
		if ( success )
		{
			std::cout << ( result ? "true" : "false" );
		}
		else
		{
			std::cout << "invalid";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Numeric Parsing - Safe conversion with error handling
	//=========================================================================

	std::cout << "--- Numeric Parsing ---" << std::endl;

	// Integer parsing examples
	std::vector<std::string_view> intInputs{ "123", "-456", "0", "2147483647", "invalid", "12.34", "" };
	std::cout << "Integer parsing:" << std::endl;
	for ( const auto& input : intInputs )
	{
		int result{};
		bool success{ tryParseInt( input, result ) };
		std::cout << "  \"" << input << "\" -> ";
		if ( success )
		{
			std::cout << result;
		}
		else
		{
			std::cout << "invalid";
		}
		std::cout << std::endl;
	}

	// Double parsing examples
	std::vector<std::string_view> doubleInputs{ "123.456", "-78.9", "1.23e-4", "0.0", "invalid", "" };
	std::cout << std::endl
			  << "Double parsing:" << std::endl;
	for ( const auto& input : doubleInputs )
	{
		double result{};
		bool success{ tryParseDouble( input, result ) };
		std::cout << "  \"" << input << "\" -> ";
		if ( success )
		{
			std::cout << result;
		}
		else
		{
			std::cout << "invalid";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// String Operations - Fast matching and comparison
	//=========================================================================

	std::cout << "--- String Operations ---" << std::endl;

	// File extension checking
	std::vector<std::string_view> filenames{ "document.pdf", "image.PNG", "script.js", "data.CSV", "readme" };
	std::cout << "File extension detection:" << std::endl;
	for ( const auto& filename : filenames )
	{
		std::cout << "  " << filename << ":" << std::endl;

		bool isPdf{ iequals( filename.substr( filename.find_last_of( '.' ) + 1 ), "pdf" ) };
		bool isImage{ endsWith( filename, ".png" ) || endsWith( filename, ".PNG" ) };
		bool hasExtension{ contains( filename, "." ) };

		std::cout << "    PDF: " << ( isPdf ? "Yes" : "No" ) << std::endl;
		std::cout << "    Image: " << ( isImage ? "Yes" : "No" ) << std::endl;
		std::cout << "    Has extension: " << ( hasExtension ? "Yes" : "No" ) << std::endl;
	}

	// URL validation example
	std::cout << std::endl
			  << "URL validation:" << std::endl;

	std::vector<std::string_view> urls{
		"https://api.example.com/v1/data",
		"http://localhost:8080/test",
		"ftp://files.server.com/download",
		"invalid-url",
		"https://secure.bank.com/api" };

	for ( const auto& url : urls )
	{
		std::cout << "  " << url << ":" << std::endl;

		bool isHttps{ startsWith( url, "https://" ) };
		bool isApiEndpoint{ contains( url, "/api" ) };
		bool isLocal{ contains( url, "localhost" ) };

		std::cout << "    HTTPS: " << ( isHttps ? "Yes" : "No" ) << std::endl;
		std::cout << "    API endpoint: " << ( isApiEndpoint ? "Yes" : "No" ) << std::endl;
		std::cout << "    Local: " << ( isLocal ? "Yes" : "No" ) << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Case Conversion - ASCII-optimized transformations
	//=========================================================================

	std::cout << "--- Case Conversion ---" << std::endl;

	// Character conversion examples
	std::cout << "Character conversion:" << std::endl;
	std::string alphabet{ "AaBbCcDdEeFf123!@#" };
	std::cout << "  Original: " << alphabet << std::endl;

	std::string lowerResult, upperResult;
	for ( char c : alphabet )
	{
		lowerResult += toLower( c );
		upperResult += toUpper( c );
	}
	std::cout << "  Lowercase: " << lowerResult << std::endl;
	std::cout << "  Uppercase: " << upperResult << std::endl;

	// String conversion examples
	std::cout << std::endl
			  << "String conversion:" << std::endl;
	std::vector<std::string_view> testStrings{
		"Hello World",
		"MixedCASE123!",
		"ALREADY_UPPER",
		"already_lower",
		"Café_Unicode_Ñoël", // Unicode should remain unchanged
		"" };

	for ( const auto& str : testStrings )
	{
		std::cout << "  Original:  \"" << str << "\"" << std::endl;
		std::cout << "  Lowercase: \"" << toLower( str ) << "\"" << std::endl;
		std::cout << "  Uppercase: \"" << toUpper( str ) << "\"" << std::endl;
		std::cout << std::endl;
	}

	//=========================================================================
	// Real-World Use Cases
	//=========================================================================

	std::cout << "--- Real-World Use Cases ---" << std::endl;

	// Configuration parsing
	std::cout << "Configuration file parsing:" << std::endl;
	std::vector<std::string_view> configLines{
		"debug=true",
		"port=8080",
		"ssl=false",
		"timeout=30.5",
		"retries=3" };

	for ( const auto& line : configLines )
	{
		auto pos{ line.find( '=' ) };
		if ( pos != std::string_view::npos )
		{
			auto key{ line.substr( 0, pos ) };
			auto value{ line.substr( pos + 1 ) };

			std::cout << "  " << key << " = " << value;

			// Try parsing as different types
			bool boolVal{};
			int intVal{};
			double doubleVal{};

			if ( tryParseBool( value, boolVal ) )
			{
				std::cout << " (boolean: " << ( boolVal ? "true" : "false" ) << ")";
			}
			else if ( tryParseInt( value, intVal ) )
			{
				std::cout << " (integer: " << intVal << ")";
			}
			else if ( tryParseDouble( value, doubleVal ) )
			{
				std::cout << " (double: " << doubleVal << ")";
			}
			else
			{
				std::cout << " (string)";
			}
			std::cout << std::endl;
		}
	}

	// Command validation
	std::cout << std::endl
			  << "Command validation:" << std::endl;
	std::vector<std::string_view> commands{ "start", "STOP", "restart", "Status", "invalid" };
	std::vector<std::string_view> validCommands{ "start", "stop", "restart", "status" };

	for ( const auto& cmd : commands )
	{
		std::cout << "  Command \"" << cmd << "\": ";
		bool valid{ false };
		for ( const auto& validCmd : validCommands )
		{
			if ( iequals( cmd, validCmd ) )
			{
				valid = true;
				break;
			}
		}
		std::cout << ( valid ? "Valid" : "Invalid" ) << std::endl;
	}
	std::cout << std::endl;

	//=========================================================================
	// Performance Demonstration
	//=========================================================================

	std::cout << "--- Performance Demonstration ---" << std::endl;

	// Large string processing
	const std::string largeText( 10000, 'X' );
	const std::string searchPattern{ "XXXX" };

	auto start{ std::chrono::high_resolution_clock::now() };

	// Perform multiple operations
	bool hasLength{ hasExactLength( largeText, 10000 ) };
	bool containsPattern{ contains( largeText, searchPattern ) };
	bool startsWithX{ startsWith( largeText, "XXX" ) };
	bool endsWithX{ endsWith( largeText, "XXX" ) };
	std::string lowerCase{ toLower( largeText ) };

	auto end{ std::chrono::high_resolution_clock::now() };
	auto duration{ std::chrono::duration_cast<std::chrono::microseconds>( end - start ) };

	std::cout << "Large string operations (10,000 characters):" << std::endl;
	std::cout << "  Length check: " << ( hasLength ? "Passed" : "Failed" ) << std::endl;
	std::cout << "  Contains pattern: " << ( containsPattern ? "Yes" : "No" ) << std::endl;
	std::cout << "  Starts with XXX: " << ( startsWithX ? "Yes" : "No" ) << std::endl;
	std::cout << "  Ends with XXX: " << ( endsWithX ? "Yes" : "No" ) << std::endl;
	std::cout << "  Case conversion: " << ( lowerCase.size() == 10000 ? "Success" : "Failed" ) << std::endl;
	std::cout << "  Total time: " << duration.count() << " microseconds" << std::endl;
	std::cout << std::endl;

	return 0;
}
