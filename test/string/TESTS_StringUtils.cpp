/**
 * @file TESTS_StringUtils.cpp
 * @brief Comprehensive tests for StringUtils high-performance string library
 * @details Tests covering validation, parsing, string operations, character classification,
 *          trimming, case conversion, edge cases, and performance validation for both
 *          zero-allocation (string_view) and allocating (std::string) functions
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <limits>
#include <string_view>

#include <nfx/string/Utils.h>

namespace nfx::string::test
{
	//=====================================================================
	// String utils tests
	//=====================================================================

	//----------------------------------------------
	// String validation
	//----------------------------------------------

	TEST( StringUtilsValidation, HasExactLength )
	{
		// Basic functionality
		EXPECT_TRUE( hasExactLength( "hello", 5 ) );
		EXPECT_FALSE( hasExactLength( "hello", 4 ) );
		EXPECT_FALSE( hasExactLength( "hello", 6 ) );

		// Edge cases
		EXPECT_TRUE( hasExactLength( "", 0 ) );
		EXPECT_FALSE( hasExactLength( "", 1 ) );
		EXPECT_TRUE( hasExactLength( "a", 1 ) );

		// Long strings
		const std::string longStr( 1000, 'x' );
		EXPECT_TRUE( hasExactLength( longStr, 1000 ) );
		EXPECT_FALSE( hasExactLength( longStr, 999 ) );
		EXPECT_FALSE( hasExactLength( longStr, 1001 ) );
	}

	TEST( StringUtilsValidation, IsEmpty )
	{
		// Basic functionality
		EXPECT_TRUE( isEmpty( "" ) );
		EXPECT_FALSE( isEmpty( "a" ) );
		EXPECT_FALSE( isEmpty( "hello" ) );

		// String view from various sources
		std::string str = "test";
		EXPECT_FALSE( isEmpty( str ) );
		EXPECT_TRUE( isEmpty( std::string_view{} ) );

		// Whitespace is not empty
		EXPECT_FALSE( isEmpty( " " ) );
		EXPECT_FALSE( isEmpty( "\t" ) );
		EXPECT_FALSE( isEmpty( "\n" ) );
	}

	TEST( StringUtilsValidation, IsNullOrWhiteSpace )
	{
		EXPECT_TRUE( isNullOrWhiteSpace( "" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( std::string_view{} ) );

		// Single whitespace characters
		EXPECT_TRUE( isNullOrWhiteSpace( " " ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\t" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\n" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\r" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\f" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\v" ) );

		// Multiple whitespace characters
		EXPECT_TRUE( isNullOrWhiteSpace( "   " ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\t\t\t" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( "\n\r\n" ) );
		EXPECT_TRUE( isNullOrWhiteSpace( " \t\n\r\f\v " ) );

		// Non-whitespace strings
		EXPECT_FALSE( isNullOrWhiteSpace( "a" ) );
		EXPECT_FALSE( isNullOrWhiteSpace( "hello" ) );
		EXPECT_FALSE( isNullOrWhiteSpace( "hello world" ) );

		// Strings with mixed content (containing non-whitespace)
		EXPECT_FALSE( isNullOrWhiteSpace( " a " ) );
		EXPECT_FALSE( isNullOrWhiteSpace( "\ta\t" ) );
		EXPECT_FALSE( isNullOrWhiteSpace( "\n hello \n" ) );
		EXPECT_FALSE( isNullOrWhiteSpace( "   .   " ) );

		// String view from various sources
		std::string whitespaceStr = "   \t\n   ";
		std::string normalStr = "test";
		EXPECT_TRUE( isNullOrWhiteSpace( whitespaceStr ) );
		EXPECT_FALSE( isNullOrWhiteSpace( normalStr ) );
	}

	TEST( StringUtilsValidation, IsAllDigits )
	{
		// Valid digit strings
		EXPECT_TRUE( isAllDigits( "0" ) );
		EXPECT_TRUE( isAllDigits( "1" ) );
		EXPECT_TRUE( isAllDigits( "9" ) );
		EXPECT_TRUE( isAllDigits( "123" ) );
		EXPECT_TRUE( isAllDigits( "0123456789" ) );
		EXPECT_TRUE( isAllDigits( "999999999" ) );

		// Large digit string
		const std::string largeDigits( 1000, '5' );
		EXPECT_TRUE( isAllDigits( largeDigits ) );

		// Mixed digit string
		EXPECT_TRUE( isAllDigits( "1234567890" ) );

		// Invalid cases - empty string
		EXPECT_FALSE( isAllDigits( "" ) );
		EXPECT_FALSE( isAllDigits( std::string_view{} ) );

		// Invalid cases - contains non-digits
		EXPECT_FALSE( isAllDigits( "123a" ) );
		EXPECT_FALSE( isAllDigits( "a123" ) );
		EXPECT_FALSE( isAllDigits( "12a34" ) );
		EXPECT_FALSE( isAllDigits( "123 " ) );
		EXPECT_FALSE( isAllDigits( " 123" ) );
		EXPECT_FALSE( isAllDigits( "123.456" ) );
		EXPECT_FALSE( isAllDigits( "123-456" ) );
		EXPECT_FALSE( isAllDigits( "+123" ) );
		EXPECT_FALSE( isAllDigits( "-123" ) );

		// Invalid cases - only non-digits
		EXPECT_FALSE( isAllDigits( "abc" ) );
		EXPECT_FALSE( isAllDigits( "hello" ) );
		EXPECT_FALSE( isAllDigits( "!@#$" ) );
		EXPECT_FALSE( isAllDigits( "   " ) );
		EXPECT_FALSE( isAllDigits( "\t\n\r" ) );

		// Invalid cases - single non-digit characters
		EXPECT_FALSE( isAllDigits( "a" ) );
		EXPECT_FALSE( isAllDigits( "Z" ) );
		EXPECT_FALSE( isAllDigits( " " ) );
		EXPECT_FALSE( isAllDigits( "!" ) );
		EXPECT_FALSE( isAllDigits( "." ) );
		EXPECT_FALSE( isAllDigits( "-" ) );
		EXPECT_FALSE( isAllDigits( "+" ) );

		// Edge cases - ASCII characters around digits
		EXPECT_FALSE( isAllDigits( "/" ) ); // ASCII 47 (before '0')
		EXPECT_FALSE( isAllDigits( ":" ) ); // ASCII 58 (after '9')

		// Unicode digits should return false (ASCII-only)
		EXPECT_FALSE( isAllDigits( "１２３" ) ); // Full-width digits
		EXPECT_FALSE( isAllDigits( "۱۲۳" ) );	 // Arabic-Indic digits

		// String view from various sources
		std::string digitStr = "98765";
		std::string mixedStr = "123abc";
		EXPECT_TRUE( isAllDigits( digitStr ) );
		EXPECT_FALSE( isAllDigits( mixedStr ) );

		// Performance test with very long digit string
		const std::string veryLargeDigits( 10000, '7' );
		EXPECT_TRUE( isAllDigits( veryLargeDigits ) );

		// Performance test with very long mixed string
		std::string veryLargeMixed( 9999, '8' );
		veryLargeMixed += "a"; // Add one non-digit at the end
		EXPECT_FALSE( isAllDigits( veryLargeMixed ) );
	}

	//----------------------------------------------
	// Character classification
	//----------------------------------------------

	TEST( StringUtilsCharacterClassification, IsWhitespace )
	{
		// Whitespace characters
		EXPECT_TRUE( isWhitespace( ' ' ) );
		EXPECT_TRUE( isWhitespace( '\t' ) );
		EXPECT_TRUE( isWhitespace( '\n' ) );
		EXPECT_TRUE( isWhitespace( '\r' ) );
		EXPECT_TRUE( isWhitespace( '\f' ) );
		EXPECT_TRUE( isWhitespace( '\v' ) );

		// Non-whitespace characters
		EXPECT_FALSE( isWhitespace( 'a' ) );
		EXPECT_FALSE( isWhitespace( 'Z' ) );
		EXPECT_FALSE( isWhitespace( '0' ) );
		EXPECT_FALSE( isWhitespace( '9' ) );
		EXPECT_FALSE( isWhitespace( '!' ) );
		EXPECT_FALSE( isWhitespace( '@' ) );
		EXPECT_FALSE( isWhitespace( '\0' ) );
	}

	TEST( StringUtilsCharacterClassification, IsDigit )
	{
		// Digits
		EXPECT_TRUE( isDigit( '0' ) );
		EXPECT_TRUE( isDigit( '1' ) );
		EXPECT_TRUE( isDigit( '5' ) );
		EXPECT_TRUE( isDigit( '9' ) );

		// Non-digits
		EXPECT_FALSE( isDigit( 'a' ) );
		EXPECT_FALSE( isDigit( 'Z' ) );
		EXPECT_FALSE( isDigit( ' ' ) );
		EXPECT_FALSE( isDigit( '!' ) );
		EXPECT_FALSE( isDigit( '\t' ) );
		EXPECT_FALSE( isDigit( '\0' ) );
	}

	TEST( StringUtilsCharacterClassification, IsAlpha )
	{
		// Lowercase letters
		EXPECT_TRUE( isAlpha( 'a' ) );
		EXPECT_TRUE( isAlpha( 'm' ) );
		EXPECT_TRUE( isAlpha( 'z' ) );

		// Uppercase letters
		EXPECT_TRUE( isAlpha( 'A' ) );
		EXPECT_TRUE( isAlpha( 'M' ) );
		EXPECT_TRUE( isAlpha( 'Z' ) );

		// Non-alphabetic
		EXPECT_FALSE( isAlpha( '0' ) );
		EXPECT_FALSE( isAlpha( '9' ) );
		EXPECT_FALSE( isAlpha( ' ' ) );
		EXPECT_FALSE( isAlpha( '!' ) );
		EXPECT_FALSE( isAlpha( '\t' ) );
		EXPECT_FALSE( isAlpha( '\0' ) );
	}

	TEST( StringUtilsCharacterClassification, IsAlphaNumeric )
	{
		// Letters
		EXPECT_TRUE( isAlphaNumeric( 'a' ) );
		EXPECT_TRUE( isAlphaNumeric( 'Z' ) );
		EXPECT_TRUE( isAlphaNumeric( 'm' ) );

		// Digits
		EXPECT_TRUE( isAlphaNumeric( '0' ) );
		EXPECT_TRUE( isAlphaNumeric( '5' ) );
		EXPECT_TRUE( isAlphaNumeric( '9' ) );

		// Non-alphanumeric
		EXPECT_FALSE( isAlphaNumeric( ' ' ) );
		EXPECT_FALSE( isAlphaNumeric( '!' ) );
		EXPECT_FALSE( isAlphaNumeric( '@' ) );
		EXPECT_FALSE( isAlphaNumeric( '\t' ) );
		EXPECT_FALSE( isAlphaNumeric( '\0' ) );
	}

	//----------------------------------------------
	// URI character classification
	//----------------------------------------------

	TEST( StringUtilsCharacterClassification, isURIReserved_char )
	{
		// RFC 3986 Section 2.2 - Reserved Characters
		// gen-delims: : / ? # [ ] @
		EXPECT_TRUE( isURIReserved( ':' ) );
		EXPECT_TRUE( isURIReserved( '/' ) );
		EXPECT_TRUE( isURIReserved( '?' ) );
		EXPECT_TRUE( isURIReserved( '#' ) );
		EXPECT_TRUE( isURIReserved( '[' ) );
		EXPECT_TRUE( isURIReserved( ']' ) );
		EXPECT_TRUE( isURIReserved( '@' ) );

		// sub-delims: ! $ & ' ( ) * + , ; =
		EXPECT_TRUE( isURIReserved( '!' ) );
		EXPECT_TRUE( isURIReserved( '$' ) );
		EXPECT_TRUE( isURIReserved( '&' ) );
		EXPECT_TRUE( isURIReserved( '\'' ) );
		EXPECT_TRUE( isURIReserved( '(' ) );
		EXPECT_TRUE( isURIReserved( ')' ) );
		EXPECT_TRUE( isURIReserved( '*' ) );
		EXPECT_TRUE( isURIReserved( '+' ) );
		EXPECT_TRUE( isURIReserved( ',' ) );
		EXPECT_TRUE( isURIReserved( ';' ) );
		EXPECT_TRUE( isURIReserved( '=' ) );

		// Non-reserved characters
		EXPECT_FALSE( isURIReserved( 'a' ) );
		EXPECT_FALSE( isURIReserved( 'Z' ) );
		EXPECT_FALSE( isURIReserved( '0' ) );
		EXPECT_FALSE( isURIReserved( '9' ) );
		EXPECT_FALSE( isURIReserved( '-' ) );
		EXPECT_FALSE( isURIReserved( '.' ) );
		EXPECT_FALSE( isURIReserved( '_' ) );
		EXPECT_FALSE( isURIReserved( '~' ) );
		EXPECT_FALSE( isURIReserved( ' ' ) );
		EXPECT_FALSE( isURIReserved( '\t' ) );
		EXPECT_FALSE( isURIReserved( '\n' ) );
		EXPECT_FALSE( isURIReserved( '\0' ) );

		// Edge ASCII characters
		EXPECT_FALSE( isURIReserved( '"' ) );  // ASCII 34
		EXPECT_FALSE( isURIReserved( '%' ) );  // ASCII 37
		EXPECT_FALSE( isURIReserved( '<' ) );  // ASCII 60
		EXPECT_FALSE( isURIReserved( '>' ) );  // ASCII 62
		EXPECT_FALSE( isURIReserved( '\\' ) ); // ASCII 92
		EXPECT_FALSE( isURIReserved( '^' ) );  // ASCII 94
		EXPECT_FALSE( isURIReserved( '`' ) );  // ASCII 96
		EXPECT_FALSE( isURIReserved( '|' ) );  // ASCII 124
	}

	TEST( StringUtilsCharacterClassification, isURIReserved_string )
	{
		// Valid reserved character strings
		EXPECT_TRUE( isURIReserved( ":" ) );
		EXPECT_TRUE( isURIReserved( "/" ) );
		EXPECT_TRUE( isURIReserved( "?" ) );
		EXPECT_TRUE( isURIReserved( "#" ) );
		EXPECT_TRUE( isURIReserved( "[]" ) );
		EXPECT_TRUE( isURIReserved( "@" ) );
		EXPECT_TRUE( isURIReserved( "!$&'()*+,;=" ) );
		EXPECT_TRUE( isURIReserved( ":/?#[]@" ) );
		EXPECT_TRUE( isURIReserved( "!$&'()*+,;=" ) );
		EXPECT_TRUE( isURIReserved( ":/?#[]@!$&'()*+,;=" ) ); // All reserved characters

		// Mixed strings with reserved and unreserved characters
		EXPECT_FALSE( isURIReserved( ":a" ) );
		EXPECT_FALSE( isURIReserved( "a:" ) );
		EXPECT_FALSE( isURIReserved( ":a:" ) );
		EXPECT_FALSE( isURIReserved( ":a?" ) );				// Contains unreserved char 'a'
		EXPECT_FALSE( isURIReserved( "test:" ) );			// Text + reserved
		EXPECT_FALSE( isURIReserved( ":test" ) );			// Reserved + text
		EXPECT_FALSE( isURIReserved( "test:123" ) );		// Text + reserved + digits
		EXPECT_FALSE( isURIReserved( "a/b" ) );				// Unreserved + reserved + unreserved
		EXPECT_FALSE( isURIReserved( "hello@world.com" ) ); // Mixed realistic scenario
		EXPECT_FALSE( isURIReserved( "user:pass" ) );		// Common auth format
		EXPECT_FALSE( isURIReserved( "file.txt" ) );		// Filename with unreserved chars
		EXPECT_FALSE( isURIReserved( "test" ) );
		EXPECT_FALSE( isURIReserved( "123" ) );
		EXPECT_FALSE( isURIReserved( "abc123" ) );

		// Invalid cases - empty string
		EXPECT_FALSE( isURIReserved( "" ) );
		EXPECT_FALSE( isURIReserved( std::string_view{} ) );

		// Strings with only unreserved characters
		EXPECT_FALSE( isURIReserved( "abcdefghijklmnopqrstuvwxyz" ) );
		EXPECT_FALSE( isURIReserved( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) );
		EXPECT_FALSE( isURIReserved( "0123456789" ) );
		EXPECT_FALSE( isURIReserved( "-._~" ) );
		EXPECT_FALSE( isURIReserved( "abc123-._~XYZ" ) );

		// Strings with whitespace and control characters
		EXPECT_FALSE( isURIReserved( " " ) );
		EXPECT_FALSE( isURIReserved( "\t\n\r" ) );
		EXPECT_FALSE( isURIReserved( ": " ) ); // Contains space

		// Large string tests
		const std::string largeReserved( 1000, ':' );
		EXPECT_TRUE( isURIReserved( largeReserved ) );

		const std::string largeMixed = std::string( 999, ':' ) + "a";
		EXPECT_FALSE( isURIReserved( largeMixed ) );
	}

	TEST( StringUtilsCharacterClassification, isURIUnreserved_char )
	{
		// RFC 3986 Section 2.3 - Unreserved Characters
		// ALPHA (uppercase)
		EXPECT_TRUE( isURIUnreserved( 'A' ) );
		EXPECT_TRUE( isURIUnreserved( 'M' ) );
		EXPECT_TRUE( isURIUnreserved( 'Z' ) );

		// ALPHA (lowercase)
		EXPECT_TRUE( isURIUnreserved( 'a' ) );
		EXPECT_TRUE( isURIUnreserved( 'm' ) );
		EXPECT_TRUE( isURIUnreserved( 'z' ) );

		// DIGIT
		EXPECT_TRUE( isURIUnreserved( '0' ) );
		EXPECT_TRUE( isURIUnreserved( '5' ) );
		EXPECT_TRUE( isURIUnreserved( '9' ) );

		// Special unreserved characters: - . _ ~
		EXPECT_TRUE( isURIUnreserved( '-' ) );
		EXPECT_TRUE( isURIUnreserved( '.' ) );
		EXPECT_TRUE( isURIUnreserved( '_' ) );
		EXPECT_TRUE( isURIUnreserved( '~' ) );

		// Reserved characters should return false
		EXPECT_FALSE( isURIUnreserved( ':' ) );
		EXPECT_FALSE( isURIUnreserved( '/' ) );
		EXPECT_FALSE( isURIUnreserved( '?' ) );
		EXPECT_FALSE( isURIUnreserved( '#' ) );
		EXPECT_FALSE( isURIUnreserved( '[' ) );
		EXPECT_FALSE( isURIUnreserved( ']' ) );
		EXPECT_FALSE( isURIUnreserved( '@' ) );
		EXPECT_FALSE( isURIUnreserved( '!' ) );
		EXPECT_FALSE( isURIUnreserved( '$' ) );
		EXPECT_FALSE( isURIUnreserved( '&' ) );
		EXPECT_FALSE( isURIUnreserved( '\'' ) );
		EXPECT_FALSE( isURIUnreserved( '(' ) );
		EXPECT_FALSE( isURIUnreserved( ')' ) );
		EXPECT_FALSE( isURIUnreserved( '*' ) );
		EXPECT_FALSE( isURIUnreserved( '+' ) );
		EXPECT_FALSE( isURIUnreserved( ',' ) );
		EXPECT_FALSE( isURIUnreserved( ';' ) );
		EXPECT_FALSE( isURIUnreserved( '=' ) );

		// Other characters should return false
		EXPECT_FALSE( isURIUnreserved( ' ' ) );
		EXPECT_FALSE( isURIUnreserved( '\t' ) );
		EXPECT_FALSE( isURIUnreserved( '\n' ) );
		EXPECT_FALSE( isURIUnreserved( '\r' ) );
		EXPECT_FALSE( isURIUnreserved( '\0' ) );
		EXPECT_FALSE( isURIUnreserved( '"' ) );
		EXPECT_FALSE( isURIUnreserved( '%' ) );
		EXPECT_FALSE( isURIUnreserved( '<' ) );
		EXPECT_FALSE( isURIUnreserved( '>' ) );
		EXPECT_FALSE( isURIUnreserved( '\\' ) );
		EXPECT_FALSE( isURIUnreserved( '^' ) );
		EXPECT_FALSE( isURIUnreserved( '`' ) );
		EXPECT_FALSE( isURIUnreserved( '|' ) );

		// Edge ASCII boundaries
		EXPECT_FALSE( isURIUnreserved( '@' ) ); // ASCII 64 (before 'A')
		EXPECT_FALSE( isURIUnreserved( '[' ) ); // ASCII 91 (after 'Z')
		EXPECT_FALSE( isURIUnreserved( '`' ) ); // ASCII 96 (before 'a')
		EXPECT_FALSE( isURIUnreserved( '{' ) ); // ASCII 123 (after 'z')
		EXPECT_FALSE( isURIUnreserved( '/' ) ); // ASCII 47 (before '0')
		EXPECT_FALSE( isURIUnreserved( ':' ) ); // ASCII 58 (after '9')
	}

	TEST( StringUtilsCharacterClassification, isURIUnreserved_string )
	{
		// Valid unreserved character strings
		EXPECT_TRUE( isURIUnreserved( "a" ) );
		EXPECT_TRUE( isURIUnreserved( "Z" ) );
		EXPECT_TRUE( isURIUnreserved( "0" ) );
		EXPECT_TRUE( isURIUnreserved( "9" ) );
		EXPECT_TRUE( isURIUnreserved( "-" ) );
		EXPECT_TRUE( isURIUnreserved( "." ) );
		EXPECT_TRUE( isURIUnreserved( "_" ) );
		EXPECT_TRUE( isURIUnreserved( "~" ) );

		// Multiple unreserved characters
		EXPECT_TRUE( isURIUnreserved( "abc" ) );
		EXPECT_TRUE( isURIUnreserved( "XYZ" ) );
		EXPECT_TRUE( isURIUnreserved( "123" ) );
		EXPECT_TRUE( isURIUnreserved( "-._~" ) );
		EXPECT_TRUE( isURIUnreserved( "abcdefghijklmnopqrstuvwxyz" ) );
		EXPECT_TRUE( isURIUnreserved( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) );
		EXPECT_TRUE( isURIUnreserved( "0123456789" ) );
		EXPECT_TRUE( isURIUnreserved( "abc123XYZ-._~" ) ); // All unreserved types

		// Valid URL-like strings with only unreserved characters
		EXPECT_TRUE( isURIUnreserved( "example" ) );
		EXPECT_TRUE( isURIUnreserved( "test123" ) );
		EXPECT_TRUE( isURIUnreserved( "my-file.txt" ) );
		EXPECT_TRUE( isURIUnreserved( "user_name" ) );
		EXPECT_TRUE( isURIUnreserved( "version~1" ) );

		// Mixed strings with reserved characters
		EXPECT_FALSE( isURIUnreserved( "a:" ) );
		EXPECT_FALSE( isURIUnreserved( ":a" ) );
		EXPECT_FALSE( isURIUnreserved( "a:b" ) );
		EXPECT_FALSE( isURIUnreserved( "test/" ) );
		EXPECT_FALSE( isURIUnreserved( "file?name" ) );
		EXPECT_FALSE( isURIUnreserved( "user@domain" ) );
		EXPECT_FALSE( isURIUnreserved( "path[0]" ) );
		EXPECT_FALSE( isURIUnreserved( "hello:world" ) );  // Text + reserved + text
		EXPECT_FALSE( isURIUnreserved( "test.txt?" ) );	   // Unreserved + reserved
		EXPECT_FALSE( isURIUnreserved( "!important" ) );   // Reserved + unreserved
		EXPECT_FALSE( isURIUnreserved( "data&more" ) );	   // Mixed realistic scenario
		EXPECT_FALSE( isURIUnreserved( "key=value" ) );	   // Common query format
		EXPECT_FALSE( isURIUnreserved( "path/to/file" ) ); // Path with slashes

		// Invalid cases - empty string
		EXPECT_FALSE( isURIUnreserved( "" ) );
		EXPECT_FALSE( isURIUnreserved( std::string_view{} ) );

		// Strings with whitespace and control characters
		EXPECT_FALSE( isURIUnreserved( " " ) );
		EXPECT_FALSE( isURIUnreserved( "\t" ) );
		EXPECT_FALSE( isURIUnreserved( "\n" ) );
		EXPECT_FALSE( isURIUnreserved( "a " ) );  // Contains space
		EXPECT_FALSE( isURIUnreserved( " a" ) );  // Contains space
		EXPECT_FALSE( isURIUnreserved( "a b" ) ); // Contains space

		// Strings with other non-unreserved characters
		EXPECT_FALSE( isURIUnreserved( "\"" ) );
		EXPECT_FALSE( isURIUnreserved( "%" ) );
		EXPECT_FALSE( isURIUnreserved( "<>" ) );
		EXPECT_FALSE( isURIUnreserved( "\\" ) );
		EXPECT_FALSE( isURIUnreserved( "^" ) );
		EXPECT_FALSE( isURIUnreserved( "`" ) );
		EXPECT_FALSE( isURIUnreserved( "|" ) );

		// Large string tests
		const std::string largeUnreserved( 1000, 'a' );
		EXPECT_TRUE( isURIUnreserved( largeUnreserved ) );

		const std::string largeMixed = std::string( 999, 'a' ) + ":";
		EXPECT_FALSE( isURIUnreserved( largeMixed ) );

		// Performance test with mixed unreserved types
		const std::string mixedUnreserved = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~";
		EXPECT_TRUE( isURIUnreserved( mixedUnreserved ) );

		// String view from various sources
		std::string unreservedStr = "test123";
		std::string mixedStr = "test:123";
		EXPECT_TRUE( isURIUnreserved( unreservedStr ) );
		EXPECT_FALSE( isURIUnreserved( mixedStr ) );
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	TEST( StringUtilsOperations, EndsWith )
	{
		// Basic functionality
		EXPECT_TRUE( endsWith( "hello world", "world" ) );
		EXPECT_TRUE( endsWith( "hello world", "d" ) );
		EXPECT_FALSE( endsWith( "hello world", "hello" ) );
		EXPECT_FALSE( endsWith( "hello world", "World" ) ); // Case sensitive

		// Edge cases
		EXPECT_TRUE( endsWith( "test", "" ) );	   // Empty suffix
		EXPECT_TRUE( endsWith( "test", "test" ) ); // Exact match
		EXPECT_FALSE( endsWith( "", "test" ) );	   // Empty string, non-empty suffix
		EXPECT_TRUE( endsWith( "", "" ) );		   // Both empty

		// Suffix longer than string
		EXPECT_FALSE( endsWith( "hi", "hello" ) );

		// Single characters
		EXPECT_TRUE( endsWith( "a", "a" ) );
		EXPECT_FALSE( endsWith( "a", "b" ) );
	}

	TEST( StringUtilsOperations, StartsWith )
	{
		// Basic functionality
		EXPECT_TRUE( startsWith( "hello world", "hello" ) );
		EXPECT_TRUE( startsWith( "hello world", "h" ) );
		EXPECT_FALSE( startsWith( "hello world", "world" ) );
		EXPECT_FALSE( startsWith( "hello world", "Hello" ) ); // Case sensitive

		// Edge cases
		EXPECT_TRUE( startsWith( "test", "" ) );	 // Empty prefix
		EXPECT_TRUE( startsWith( "test", "test" ) ); // Exact match
		EXPECT_FALSE( startsWith( "", "test" ) );	 // Empty string, non-empty prefix
		EXPECT_TRUE( startsWith( "", "" ) );		 // Both empty

		// Prefix longer than string
		EXPECT_FALSE( startsWith( "hi", "hello" ) );

		// Single characters
		EXPECT_TRUE( startsWith( "a", "a" ) );
		EXPECT_FALSE( startsWith( "a", "b" ) );
	}

	TEST( StringUtilsOperations, Contains )
	{
		// Basic functionality
		EXPECT_TRUE( contains( "hello world", "hello" ) );
		EXPECT_TRUE( contains( "hello world", "world" ) );
		EXPECT_TRUE( contains( "hello world", "o w" ) );
		EXPECT_TRUE( contains( "hello world", "l" ) );
		EXPECT_FALSE( contains( "hello world", "Hello" ) ); // Case sensitive
		EXPECT_FALSE( contains( "hello world", "xyz" ) );

		// Edge cases
		EXPECT_TRUE( contains( "test", "" ) );	   // Empty substring
		EXPECT_TRUE( contains( "test", "test" ) ); // Exact match
		EXPECT_FALSE( contains( "", "test" ) );	   // Empty string, non-empty substring
		EXPECT_TRUE( contains( "", "" ) );		   // Both empty

		// Substring longer than string
		EXPECT_FALSE( contains( "hi", "hello" ) );

		// Multiple occurrences
		EXPECT_TRUE( contains( "hello hello", "hello" ) );
		EXPECT_TRUE( contains( "abcabcabc", "abc" ) );
	}

	TEST( StringUtilsOperations, Equals )
	{
		// Basic functionality
		EXPECT_TRUE( equals( "hello", "hello" ) );
		EXPECT_FALSE( equals( "hello", "world" ) );
		EXPECT_FALSE( equals( "hello", "Hello" ) ); // Case sensitive

		// Edge cases
		EXPECT_TRUE( equals( "", "" ) );	  // Both empty
		EXPECT_FALSE( equals( "", "test" ) ); // One empty
		EXPECT_FALSE( equals( "test", "" ) ); // One empty

		// Different lengths
		EXPECT_FALSE( equals( "hello", "hello world" ) );
		EXPECT_FALSE( equals( "hello world", "hello" ) );

		// Single characters
		EXPECT_TRUE( equals( "a", "a" ) );
		EXPECT_FALSE( equals( "a", "b" ) );
	}

	TEST( StringUtilsOperations, IEquals )
	{
		// Basic functionality
		EXPECT_TRUE( iequals( "hello", "hello" ) );
		EXPECT_TRUE( iequals( "hello", "HELLO" ) );
		EXPECT_TRUE( iequals( "hello", "Hello" ) );
		EXPECT_TRUE( iequals( "HELLO", "hello" ) );
		EXPECT_TRUE( iequals( "HeLLo", "hEllO" ) );
		EXPECT_FALSE( iequals( "hello", "world" ) );

		// Edge cases
		EXPECT_TRUE( iequals( "", "" ) );	   // Both empty
		EXPECT_FALSE( iequals( "", "test" ) ); // One empty
		EXPECT_FALSE( iequals( "test", "" ) ); // One empty

		// Different lengths
		EXPECT_FALSE( iequals( "hello", "hello world" ) );
		EXPECT_FALSE( iequals( "hello world", "hello" ) );

		// Mixed case with numbers and special chars
		EXPECT_TRUE( iequals( "Test123!", "TEST123!" ) );
		EXPECT_TRUE( iequals( "Test123!", "test123!" ) );
		EXPECT_FALSE( iequals( "Test123!", "Test124!" ) );

		// ASCII-only case conversion
		EXPECT_TRUE( iequals( "ASCII", "ascii" ) );
		EXPECT_TRUE( iequals( "Test", "TEST" ) );
	}

	//----------------------------------------------
	// String trimming
	//----------------------------------------------

	//----------------------------
	// Non-allocating
	//----------------------------

	TEST( StringUtilsStringTrimming, TrimStart )
	{
		// No leading whitespace
		EXPECT_EQ( trimStart( "hello" ), "hello" );
		EXPECT_EQ( trimStart( "hello world" ), "hello world" );

		// Leading whitespace
		EXPECT_EQ( trimStart( " hello" ), "hello" );
		EXPECT_EQ( trimStart( "\thello" ), "hello" );
		EXPECT_EQ( trimStart( "\n\r hello" ), "hello" );
		EXPECT_EQ( trimStart( "   hello world   " ), "hello world   " );

		// Only whitespace
		EXPECT_EQ( trimStart( "   " ), "" );
		EXPECT_EQ( trimStart( "\t\n\r" ), "" );

		// Empty string
		EXPECT_EQ( trimStart( "" ), "" );

		// Mixed whitespace types
		EXPECT_EQ( trimStart( " \t\n\r\f\v hello" ), "hello" );
	}

	TEST( StringUtilsStringTrimming, TrimEnd )
	{
		// No trailing whitespace
		EXPECT_EQ( trimEnd( "hello" ), "hello" );
		EXPECT_EQ( trimEnd( "hello world" ), "hello world" );

		// Trailing whitespace
		EXPECT_EQ( trimEnd( "hello " ), "hello" );
		EXPECT_EQ( trimEnd( "hello\t" ), "hello" );
		EXPECT_EQ( trimEnd( "hello \n\r" ), "hello" );
		EXPECT_EQ( trimEnd( "   hello world   " ), "   hello world" );

		// Only whitespace
		EXPECT_EQ( trimEnd( "   " ), "" );
		EXPECT_EQ( trimEnd( "\t\n\r" ), "" );

		// Empty string
		EXPECT_EQ( trimEnd( "" ), "" );

		// Mixed whitespace types
		EXPECT_EQ( trimEnd( "hello \t\n\r\f\v " ), "hello" );
	}

	TEST( StringUtilsStringTrimming, Trim )
	{
		// No whitespace
		EXPECT_EQ( trim( "hello" ), "hello" );
		EXPECT_EQ( trim( "hello world" ), "hello world" );

		// Leading and trailing whitespace
		EXPECT_EQ( trim( " hello " ), "hello" );
		EXPECT_EQ( trim( "\thello\t" ), "hello" );
		EXPECT_EQ( trim( "\n\r hello world \n\r" ), "hello world" );
		EXPECT_EQ( trim( "   hello world   " ), "hello world" );

		// Only leading whitespace
		EXPECT_EQ( trim( " hello" ), "hello" );

		// Only trailing whitespace
		EXPECT_EQ( trim( "hello " ), "hello" );

		// Only whitespace
		EXPECT_EQ( trim( "   " ), "" );
		EXPECT_EQ( trim( "\t\n\r\f\v" ), "" );

		// Empty string
		EXPECT_EQ( trim( "" ), "" );

		// Internal whitespace preserved
		EXPECT_EQ( trim( "  hello  world  " ), "hello  world" );
	}

	//----------------------------------------------
	// String case conversion
	//----------------------------------------------

	TEST( StringUtilsCaseConversion, ToLowerString )
	{
		// Basic ASCII conversion
		EXPECT_EQ( "hello world", toLower( "HELLO WORLD" ) );
		EXPECT_EQ( "hello world", toLower( "Hello World" ) );
		EXPECT_EQ( "hello world", toLower( "HeLLo WoRLd" ) );

		// Already lowercase
		EXPECT_EQ( "hello world", toLower( "hello world" ) );

		// Mixed case with numbers and symbols
		EXPECT_EQ( "test123!@#", toLower( "TEST123!@#" ) );
		EXPECT_EQ( "test123!@#", toLower( "Test123!@#" ) );

		// Empty string
		EXPECT_EQ( "", toLower( "" ) );

		// Single characters
		EXPECT_EQ( "a", toLower( "A" ) );
		EXPECT_EQ( "z", toLower( "Z" ) );
		EXPECT_EQ( "1", toLower( "1" ) );
		EXPECT_EQ( "!", toLower( "!" ) );

		// All uppercase alphabet
		EXPECT_EQ( "abcdefghijklmnopqrstuvwxyz", toLower( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) );

		// Unicode characters should remain unchanged (ASCII-only conversion)
		EXPECT_EQ( "café", toLower( "café" ) ); // Already has lowercase é
		EXPECT_EQ( "cafÉ", toLower( "CAFÉ" ) ); // Only ASCII 'C', 'A', 'F' converted, É unchanged

		// Large string test
		const std::string largeUpper( 1000, 'X' );
		const std::string largeLower( 1000, 'x' );
		EXPECT_EQ( largeLower, toLower( largeUpper ) );
	}

	TEST( StringUtilsCaseConversion, ToUpperString )
	{
		// Basic ASCII conversion
		EXPECT_EQ( "HELLO WORLD", toUpper( "hello world" ) );
		EXPECT_EQ( "HELLO WORLD", toUpper( "Hello World" ) );
		EXPECT_EQ( "HELLO WORLD", toUpper( "HeLLo WoRLd" ) );

		// Already uppercase
		EXPECT_EQ( "HELLO WORLD", toUpper( "HELLO WORLD" ) );

		// Mixed case with numbers and symbols
		EXPECT_EQ( "TEST123!@#", toUpper( "test123!@#" ) );
		EXPECT_EQ( "TEST123!@#", toUpper( "Test123!@#" ) );

		// Empty string
		EXPECT_EQ( "", toUpper( "" ) );

		// Single characters
		EXPECT_EQ( "A", toUpper( "a" ) );
		EXPECT_EQ( "Z", toUpper( "z" ) );
		EXPECT_EQ( "1", toUpper( "1" ) );
		EXPECT_EQ( "!", toUpper( "!" ) );

		// All lowercase alphabet
		EXPECT_EQ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ", toUpper( "abcdefghijklmnopqrstuvwxyz" ) );

		// Unicode characters should remain unchanged (ASCII-only conversion)
		EXPECT_EQ( "CAFÉ", toUpper( "CAFÉ" ) ); // Already has uppercase É
		EXPECT_EQ( "CAFé", toUpper( "café" ) ); // Only ASCII 'c', 'a', 'f' converted, é unchanged

		// Large string test
		const std::string largeLower( 1000, 'x' );
		const std::string largeUpper( 1000, 'X' );
		EXPECT_EQ( largeUpper, toUpper( largeLower ) );
	}

	//----------------------------------------------
	// Character case conversion
	//----------------------------------------------

	TEST( StringUtilsCaseConversion, ToLowerChar )
	{
		// ASCII uppercase to lowercase
		EXPECT_EQ( 'a', toLower( 'A' ) );
		EXPECT_EQ( 'z', toLower( 'Z' ) );
		EXPECT_EQ( 'm', toLower( 'M' ) );

		// Already lowercase - no change
		EXPECT_EQ( 'a', toLower( 'a' ) );
		EXPECT_EQ( 'z', toLower( 'z' ) );
		EXPECT_EQ( 'm', toLower( 'm' ) );

		// Non-alphabetic characters - no change
		EXPECT_EQ( '0', toLower( '0' ) );
		EXPECT_EQ( '9', toLower( '9' ) );
		EXPECT_EQ( ' ', toLower( ' ' ) );
		EXPECT_EQ( '!', toLower( '!' ) );
		EXPECT_EQ( '@', toLower( '@' ) );

		// Edge ASCII values
		EXPECT_EQ( '@', toLower( '@' ) ); // ASCII 64 (before 'A')
		EXPECT_EQ( '[', toLower( '[' ) ); // ASCII 91 (after 'Z')
		EXPECT_EQ( '`', toLower( '`' ) ); // ASCII 96 (before 'a')
		EXPECT_EQ( '{', toLower( '{' ) ); // ASCII 123 (after 'z')
	}

	TEST( StringUtilsCaseConversion, ToUpperChar )
	{
		// ASCII lowercase to uppercase
		EXPECT_EQ( 'A', toUpper( 'a' ) );
		EXPECT_EQ( 'Z', toUpper( 'z' ) );
		EXPECT_EQ( 'M', toUpper( 'm' ) );

		// Already uppercase - no change
		EXPECT_EQ( 'A', toUpper( 'A' ) );
		EXPECT_EQ( 'Z', toUpper( 'Z' ) );
		EXPECT_EQ( 'M', toUpper( 'M' ) );

		// Non-alphabetic characters - no change
		EXPECT_EQ( '0', toUpper( '0' ) );
		EXPECT_EQ( '9', toUpper( '9' ) );
		EXPECT_EQ( ' ', toUpper( ' ' ) );
		EXPECT_EQ( '!', toUpper( '!' ) );
		EXPECT_EQ( '@', toUpper( '@' ) );

		// Edge ASCII values
		EXPECT_EQ( '@', toUpper( '@' ) ); // ASCII 64 (before 'A')
		EXPECT_EQ( '[', toUpper( '[' ) ); // ASCII 91 (after 'Z')
		EXPECT_EQ( '`', toUpper( '`' ) ); // ASCII 96 (before 'a')
		EXPECT_EQ( '{', toUpper( '{' ) ); // ASCII 123 (after 'z')
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	//----------------------------
	// Bool
	//----------------------------

	TEST( StringUtilsBoolParsing, TryParseBool_TrueValues )
	{
		bool result{ false };

		// Single character true values
		EXPECT_TRUE( tryParseBool( "1", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "t", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "T", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "y", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "Y", result ) );
		EXPECT_TRUE( result );

		// Two character true values
		EXPECT_TRUE( tryParseBool( "on", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "ON", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "On", result ) );
		EXPECT_TRUE( result );

		// Three character true values
		EXPECT_TRUE( tryParseBool( "yes", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "YES", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "Yes", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "YeS", result ) );
		EXPECT_TRUE( result );

		// Four character true values
		EXPECT_TRUE( tryParseBool( "true", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "TRUE", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "True", result ) );
		EXPECT_TRUE( result );

		EXPECT_TRUE( tryParseBool( "TrUe", result ) );
		EXPECT_TRUE( result );
	}

	TEST( StringUtilsBoolParsing, TryParseBool_FalseValues )
	{
		bool result{ true };

		// Single character false values
		EXPECT_TRUE( tryParseBool( "0", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "f", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "F", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "n", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "N", result ) );
		EXPECT_FALSE( result );

		// Two character false values
		EXPECT_TRUE( tryParseBool( "no", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "NO", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "No", result ) );
		EXPECT_FALSE( result );

		// Three character false values
		EXPECT_TRUE( tryParseBool( "off", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "OFF", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "Off", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "OfF", result ) );
		EXPECT_FALSE( result );

		// Five character false values
		EXPECT_TRUE( tryParseBool( "false", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "FALSE", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "False", result ) );
		EXPECT_FALSE( result );

		EXPECT_TRUE( tryParseBool( "FaLsE", result ) );
		EXPECT_FALSE( result );
	}

	TEST( StringUtilsBoolParsing, TryParseBool_InvalidValues )
	{
		bool result{ false };

		// Empty string
		EXPECT_FALSE( tryParseBool( "", result ) );

		// Invalid single characters
		EXPECT_FALSE( tryParseBool( "a", result ) );
		EXPECT_FALSE( tryParseBool( "2", result ) );
		EXPECT_FALSE( tryParseBool( "x", result ) );

		// Invalid multi-character strings
		EXPECT_FALSE( tryParseBool( "maybe", result ) );
		EXPECT_FALSE( tryParseBool( "truee", result ) );
		EXPECT_FALSE( tryParseBool( "falsee", result ) );
		EXPECT_FALSE( tryParseBool( "tr", result ) );
		EXPECT_FALSE( tryParseBool( "fal", result ) );

		// Numbers other than 0/1
		EXPECT_FALSE( tryParseBool( "2", result ) );
		EXPECT_FALSE( tryParseBool( "-1", result ) );
		EXPECT_FALSE( tryParseBool( "10", result ) );

		// Whitespace variations
		EXPECT_FALSE( tryParseBool( " true", result ) );
		EXPECT_FALSE( tryParseBool( "true ", result ) );
		EXPECT_FALSE( tryParseBool( " true ", result ) );
	}

	//----------------------------------------------
	// Numeric
	//----------------------------------------------

	TEST( StringUtilsNumericParsing, TryParseInt )
	{
		int result{ 0 };

		// Valid positive numbers
		EXPECT_TRUE( tryParseInt( "123", result ) );
		EXPECT_EQ( 123, result );

		EXPECT_TRUE( tryParseInt( "0", result ) );
		EXPECT_EQ( 0, result );

		EXPECT_TRUE( tryParseInt( "1", result ) );
		EXPECT_EQ( 1, result );

		// Valid negative numbers
		EXPECT_TRUE( tryParseInt( "-123", result ) );
		EXPECT_EQ( -123, result );

		EXPECT_TRUE( tryParseInt( "-1", result ) );
		EXPECT_EQ( -1, result );

		// Boundary values
		EXPECT_TRUE( tryParseInt( "2147483647", result ) );
		EXPECT_EQ( std::numeric_limits<int>::max(), result );

		EXPECT_TRUE( tryParseInt( "-2147483648", result ) );
		EXPECT_EQ( std::numeric_limits<int>::min(), result );

		// Invalid cases
		EXPECT_FALSE( tryParseInt( "", result ) );
		EXPECT_FALSE( tryParseInt( "abc", result ) );
		EXPECT_FALSE( tryParseInt( "123abc", result ) );
		EXPECT_FALSE( tryParseInt( "abc123", result ) );
		EXPECT_FALSE( tryParseInt( "12.34", result ) );
		EXPECT_FALSE( tryParseInt( " 123", result ) );
		EXPECT_FALSE( tryParseInt( "123 ", result ) );
		EXPECT_FALSE( tryParseInt( "+123", result ) );

		// Overflow cases
		EXPECT_FALSE( tryParseInt( "2147483648", result ) );  // max + 1
		EXPECT_FALSE( tryParseInt( "-2147483649", result ) ); // min - 1
		EXPECT_FALSE( tryParseInt( "999999999999999999999", result ) );
	}

	TEST( StringUtilsNumericParsing, TryParseUInt )
	{
		std::uint32_t result{ 0 };

		// Valid positive numbers
		EXPECT_TRUE( tryParseUInt( "123", result ) );
		EXPECT_EQ( 123u, result );

		EXPECT_TRUE( tryParseUInt( "0", result ) );
		EXPECT_EQ( 0u, result );

		EXPECT_TRUE( tryParseUInt( "1", result ) );
		EXPECT_EQ( 1u, result );

		// Boundary values
		EXPECT_TRUE( tryParseUInt( "4294967295", result ) );
		EXPECT_EQ( std::numeric_limits<std::uint32_t>::max(), result );

		// Invalid cases
		EXPECT_FALSE( tryParseUInt( "", result ) );
		EXPECT_FALSE( tryParseUInt( "-1", result ) );	// Negative not allowed
		EXPECT_FALSE( tryParseUInt( "-123", result ) ); // Negative not allowed
		EXPECT_FALSE( tryParseUInt( "abc", result ) );
		EXPECT_FALSE( tryParseUInt( "123abc", result ) );
		EXPECT_FALSE( tryParseUInt( "abc123", result ) );
		EXPECT_FALSE( tryParseUInt( "12.34", result ) );
		EXPECT_FALSE( tryParseUInt( " 123", result ) );
		EXPECT_FALSE( tryParseUInt( "123 ", result ) );
		EXPECT_FALSE( tryParseUInt( "+123", result ) );

		// Overflow cases
		EXPECT_FALSE( tryParseUInt( "4294967296", result ) ); // max + 1
		EXPECT_FALSE( tryParseUInt( "999999999999999999999", result ) );
	}

	TEST( StringUtilsNumericParsing, TryParseLong )
	{
		std::int64_t result{ 0 };

		// Valid positive numbers
		EXPECT_TRUE( tryParseLong( "123", result ) );
		EXPECT_EQ( 123LL, result );

		EXPECT_TRUE( tryParseLong( "0", result ) );
		EXPECT_EQ( 0LL, result );

		// Valid negative numbers
		EXPECT_TRUE( tryParseLong( "-123", result ) );
		EXPECT_EQ( -123LL, result );

		// Large numbers
		EXPECT_TRUE( tryParseLong( "9223372036854775807", result ) );
		EXPECT_EQ( std::numeric_limits<std::int64_t>::max(), result );

		EXPECT_TRUE( tryParseLong( "-9223372036854775808", result ) );
		EXPECT_EQ( std::numeric_limits<std::int64_t>::min(), result );

		// Invalid cases
		EXPECT_FALSE( tryParseLong( "", result ) );
		EXPECT_FALSE( tryParseLong( "abc", result ) );
		EXPECT_FALSE( tryParseLong( "123abc", result ) );
		EXPECT_FALSE( tryParseLong( "12.34", result ) );
		EXPECT_FALSE( tryParseLong( " 123", result ) );
		EXPECT_FALSE( tryParseLong( "+123", result ) );
	}

	TEST( StringUtilsNumericParsing, TryParseDouble )
	{
		double result{ 0.0 };

		// Valid integers
		EXPECT_TRUE( tryParseDouble( "123", result ) );
		EXPECT_DOUBLE_EQ( 123.0, result );

		EXPECT_TRUE( tryParseDouble( "0", result ) );
		EXPECT_DOUBLE_EQ( 0.0, result );

		EXPECT_TRUE( tryParseDouble( "-123", result ) );
		EXPECT_DOUBLE_EQ( -123.0, result );

		// Valid decimals
		EXPECT_TRUE( tryParseDouble( "123.456", result ) );
		EXPECT_DOUBLE_EQ( 123.456, result );

		EXPECT_TRUE( tryParseDouble( "-123.456", result ) );
		EXPECT_DOUBLE_EQ( -123.456, result );

		EXPECT_TRUE( tryParseDouble( "0.123", result ) );
		EXPECT_DOUBLE_EQ( 0.123, result );

		// Scientific notation
		EXPECT_TRUE( tryParseDouble( "1.23e2", result ) );
		EXPECT_DOUBLE_EQ( 123.0, result );

		EXPECT_TRUE( tryParseDouble( "1.23e-2", result ) );
		EXPECT_DOUBLE_EQ( 0.0123, result );

		// Invalid cases
		EXPECT_FALSE( tryParseDouble( "", result ) );
		EXPECT_FALSE( tryParseDouble( "abc", result ) );
		EXPECT_FALSE( tryParseDouble( "123abc", result ) );
		EXPECT_FALSE( tryParseDouble( " 123.456", result ) );
		EXPECT_FALSE( tryParseDouble( "123.456 ", result ) );
		EXPECT_FALSE( tryParseDouble( "+123.456", result ) );
	}

	//----------------------------------------------
	// PerformanceAndEdgeCase
	//----------------------------------------------

	TEST( StringUtilsPerformanceAndEdgeCase, Unicode )
	{
		// Basic Unicode string handling (UTF-8 encoded)
		const std::string utf8_string = "café";
		const std::string utf8_upper = "CAFÉ";
		const std::string ascii_cafe = "cafe";

		// Test basic string operations with Unicode
		EXPECT_TRUE( equals( utf8_string, "café" ) );
		EXPECT_FALSE( equals( utf8_string, utf8_upper ) ); // Case sensitive
		EXPECT_FALSE( equals( utf8_string, ascii_cafe ) ); // é ≠ e

		// String length tests with Unicode (UTF-8 byte count vs character count)
		EXPECT_TRUE( hasExactLength( utf8_string, 5 ) );  // "café" = 5 bytes in UTF-8
		EXPECT_FALSE( hasExactLength( utf8_string, 4 ) ); // Not 4 characters visually

		// StartsWith/EndsWith with Unicode
		EXPECT_TRUE( startsWith( utf8_string, "ca" ) );
		EXPECT_TRUE( endsWith( utf8_string, "é" ) );  // Single Unicode character
		EXPECT_FALSE( endsWith( utf8_string, "e" ) ); // ASCII 'e' ≠ Unicode 'é'

		// Contains with Unicode
		EXPECT_TRUE( contains( utf8_string, "af" ) );
		EXPECT_TRUE( contains( utf8_string, "é" ) );
		EXPECT_FALSE( contains( utf8_string, "e" ) ); // ASCII 'e' not in "café"

		// Case-insensitive comparison limitations with Unicode
		// Note: std::tolower only works correctly for ASCII characters
		EXPECT_FALSE( iequals( utf8_string, utf8_upper ) ); // Expected failure for non-ASCII
		EXPECT_TRUE( iequals( "ASCII", "ascii" ) );			// Works for ASCII only

		// Test with various Unicode characters
		const std::string emoji = "Hello 🌍 World";
		const std::string chinese = "你好世界";
		const std::string russian = "Привет мир";
		const std::string arabic = "مرحبا بالعالم";

		// Basic operations should work with any UTF-8 encoded string
		EXPECT_FALSE( isEmpty( emoji ) );
		EXPECT_FALSE( isEmpty( chinese ) );
		EXPECT_FALSE( isEmpty( russian ) );
		EXPECT_FALSE( isEmpty( arabic ) );

		EXPECT_TRUE( startsWith( emoji, "Hello" ) );
		EXPECT_TRUE( endsWith( emoji, "World" ) );
		EXPECT_TRUE( contains( emoji, "🌍" ) );
		EXPECT_TRUE( equals( chinese, chinese ) );

		// Mixed ASCII and Unicode
		const std::string mixed = "ASCII混合テキストMIXED";
		EXPECT_TRUE( startsWith( mixed, "ASCII" ) );
		EXPECT_TRUE( endsWith( mixed, "MIXED" ) );
		EXPECT_TRUE( contains( mixed, "混合" ) );
		EXPECT_TRUE( contains( mixed, "テキスト" ) );

		// Unicode normalization considerations (different byte representations)
		// Note: This library treats strings as byte sequences, not normalized Unicode
		const std::string composed = "é";	// Single composed character U+00E9
		const std::string decomposed = "é"; // e + combining acute accent U+0065 + U+0301
		// These might be visually identical but have different byte representations
		// The library correctly treats them as different byte sequences

		// Boolean parsing with Unicode characters should fail gracefully
		bool boolResult{ false };
		EXPECT_FALSE( tryParseBool( "真", boolResult ) );	// Chinese "true"
		EXPECT_FALSE( tryParseBool( "はい", boolResult ) ); // Japanese "yes"
		EXPECT_FALSE( tryParseBool( "да", boolResult ) );	// Russian "yes"
		EXPECT_FALSE( tryParseBool( "نعم", boolResult ) );	// Arabic "yes"

		// Numeric parsing with Unicode should also fail gracefully
		int intResult{ 0 };
		EXPECT_FALSE( tryParseInt( "１２３", intResult ) ); // Full-width numbers
		EXPECT_FALSE( tryParseInt( "۱۲۳", intResult ) );	// Arabic-Indic digits

		// Performance test with Unicode strings
		const std::string rocket_emoji{ "🚀" };				   // Single rocket emoji
		const std::size_t rocket_bytes{ rocket_emoji.size() }; // Should be 4 bytes for 🚀

		// Create string with multiple emojis using string concatenation
		std::string longUnicode;
		for ( int i{ 0 }; i < 100; ++i )
		{
			longUnicode += rocket_emoji;
		}

		EXPECT_FALSE( isEmpty( longUnicode ) );
		EXPECT_TRUE( hasExactLength( longUnicode, 100 * rocket_bytes ) ); // 100 × bytes per emoji
		EXPECT_TRUE( contains( longUnicode, rocket_emoji ) );

		// Test with Latin-1 accented characters (safer for MSVC)
		std::string accented;
		for ( int i{ 0 }; i < 50; ++i )
		{
			accented += "é"; // Each é is 2 bytes in UTF-8
		}
		EXPECT_TRUE( hasExactLength( accented, 100 ) ); // 50 × 2 bytes per é
		EXPECT_TRUE( contains( accented, "é" ) );

		// Test basic Unicode awareness without complex character construction
		const std::string simple_unicode = "Héllo Wörld";
		EXPECT_TRUE( contains( simple_unicode, "é" ) );
		EXPECT_TRUE( contains( simple_unicode, "ö" ) );
		EXPECT_FALSE( contains( simple_unicode, "e" ) ); // 'e' ≠ 'é'
	}

	TEST( StringUtilsPerformanceAndEdgeCase, LargeStrings )
	{
		// Test with large strings to ensure no performance issues
		const std::string largeStr( 10000, 'x' );
		const std::string largeSuffix( 1000, 'x' );
		const std::string largePrefix( 1000, 'x' );

		EXPECT_TRUE( endsWith( largeStr, largeSuffix ) );
		EXPECT_TRUE( startsWith( largeStr, largePrefix ) );
		EXPECT_TRUE( contains( largeStr, largeSuffix ) );
		EXPECT_TRUE( equals( largeStr, largeStr ) );
		EXPECT_TRUE( iequals( largeStr, largeStr ) );
		EXPECT_TRUE( hasExactLength( largeStr, 10000 ) );
		EXPECT_FALSE( isEmpty( largeStr ) );
	}

	TEST( StringUtilsPerformanceAndEdgeCase, SpecialCharacters )
	{
		// Test with special characters
		const std::string special = "!@#$%^&*()_+-=[]{}|;':\",./<>?";

		EXPECT_TRUE( startsWith( special, "!@#" ) );
		EXPECT_TRUE( endsWith( special, "<>?" ) );
		EXPECT_TRUE( contains( special, "[]{}|" ) );
		EXPECT_TRUE( equals( special, special ) );
		EXPECT_TRUE( iequals( special, special ) );

		// Unicode characters (basic test)
		EXPECT_TRUE( equals( "café", "café" ) );
		EXPECT_FALSE( equals( "café", "cafe" ) );
	}

	TEST( StringUtilsPerformanceAndEdgeCase, NullTerminatedStrings )
	{
		// Test with null-terminated strings to ensure string_view handles them correctly
		const char* cstr = "hello world";

		EXPECT_TRUE( startsWith( cstr, "hello" ) );
		EXPECT_TRUE( endsWith( cstr, "world" ) );
		EXPECT_TRUE( contains( cstr, "o w" ) );
		EXPECT_TRUE( hasExactLength( cstr, 11 ) );

		bool result;
		EXPECT_TRUE( tryParseBool( "true", result ) );
		EXPECT_TRUE( result );

		int intResult;
		EXPECT_TRUE( tryParseInt( "123", intResult ) );
		EXPECT_EQ( 123, intResult );
	}

	//----------------------------------------------
	// Round-trip conversion tests
	//----------------------------------------------

	TEST( StringUtilsCaseConversion, RoundTripConversion )
	{
		// Test that toLower(toUpper(x)) == toLower(x) and vice versa for ASCII
		const std::string original = "Hello World 123!";
		const std::string lower = toLower( original );
		const std::string upper = toUpper( original );

		// Round-trip conversions
		EXPECT_EQ( lower, toLower( toUpper( lower ) ) );
		EXPECT_EQ( upper, toUpper( toLower( upper ) ) );

		// ASCII alphabet round-trip
		const std::string ascii = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
		EXPECT_EQ( toLower( ascii ), toLower( toUpper( toLower( ascii ) ) ) );
		EXPECT_EQ( toUpper( ascii ), toUpper( toLower( toUpper( ascii ) ) ) );
	}

	TEST( StringUtilsCaseConversion, EdgeCases )
	{
		// Edge ASCII boundaries for case conversion
		std::string edgeCases = "@[`{";				  // Characters around A-Z and a-z ranges
		EXPECT_EQ( edgeCases, toLower( edgeCases ) ); // Should remain unchanged
		EXPECT_EQ( edgeCases, toUpper( edgeCases ) ); // Should remain unchanged

		// Null character (should handle gracefully)
		EXPECT_EQ( '\0', toLower( '\0' ) );
		EXPECT_EQ( '\0', toUpper( '\0' ) );

		// Whitespace characters
		EXPECT_EQ( ' ', toLower( ' ' ) );
		EXPECT_EQ( '\t', toLower( '\t' ) );
		EXPECT_EQ( '\n', toLower( '\n' ) );
		EXPECT_EQ( '\r', toLower( '\r' ) );

		EXPECT_EQ( ' ', toUpper( ' ' ) );
		EXPECT_EQ( '\t', toUpper( '\t' ) );
		EXPECT_EQ( '\n', toUpper( '\n' ) );
		EXPECT_EQ( '\r', toUpper( '\r' ) );
	}
}
