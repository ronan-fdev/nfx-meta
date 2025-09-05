/**
 * @file TESTS_Int128.cpp
 * @brief Comprehensive tests for Int128 128-bit signed integer class
 * @details Tests covering construction, arithmetic, comparison, and edge cases
 */

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include <nfx/datatypes/constants/Int128Constants.h>
#include <nfx/datatypes/Int128.h>

namespace nfx::datatypes::test
{
	//=====================================================================
	// Int128 type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( Int128Construction, DefaultConstruction )
	{
		datatypes::Int128 zero;
		EXPECT_TRUE( zero.isZero() );
		EXPECT_FALSE( zero.isNegative() );
		EXPECT_EQ( 0ULL, zero.toLow() );
		EXPECT_EQ( 0ULL, zero.toHigh() );
	}

	TEST( Int128Construction, ConstructionFromUint64 )
	{
		const std::uint64_t value{ 0x123456789ABCDEFULL };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( value, num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST( Int128Construction, ConstructionFromInt64Positive )
	{
		const std::int64_t value{ 0x123456789ABCDEFLL };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST( Int128Construction, ConstructionFromInt64Negative )
	{
		const std::int64_t value{ -0x123456789ABCDEFLL };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, num.toHigh() ); // Sign extension
	}

	TEST( Int128Construction, ConstructionFromUint32 )
	{
		const std::uint32_t value{ 0x12345678U };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST( Int128Construction, ConstructionFromIntPositive )
	{
		const int value{ 0x12345678 };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_FALSE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( 0ULL, num.toHigh() );
	}

	TEST( Int128Construction, ConstructionFromIntNegative )
	{
		const int value{ -0x12345678 };
		datatypes::Int128 num{ value };

		EXPECT_FALSE( num.isZero() );
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( static_cast<std::uint64_t>( value ), num.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, num.toHigh() ); // Sign extension
	}

	TEST( Int128Construction, ConstructionFromLowHigh )
	{
		const std::uint64_t low{ 0x123456789ABCDEFULL };
		const std::uint64_t high{ 0xFEDCBA9876543210ULL };
		datatypes::Int128 num{ low, high };

		EXPECT_FALSE( num.isZero() );
		EXPECT_TRUE( num.isNegative() );
		EXPECT_EQ( low, num.toLow() );
		EXPECT_EQ( high, num.toHigh() );
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	TEST( Int128Conversion, ToString )
	{
		// Basic cases
		EXPECT_EQ( datatypes::Int128{ 123 }.toString(), "123" );
		EXPECT_EQ( datatypes::Int128{ -456 }.toString(), "-456" );
		EXPECT_EQ( datatypes::Int128{ 0 }.toString(), "0" );

		// String constructor with valid integer strings
		datatypes::Int128 i1{ "123" };
		EXPECT_EQ( i1.toString(), "123" );

		datatypes::Int128 i2{ "-456" };
		EXPECT_EQ( i2.toString(), "-456" );

		// Large integer string
		datatypes::Int128 i3{ "123456789012345678901234567890" };
		EXPECT_FALSE( i3.isZero() );

		// Decimal strings should throw (Int128 is integer type, not decimal)
		EXPECT_THROW( datatypes::Int128{ "123.456" }, std::invalid_argument );
		EXPECT_THROW( datatypes::Int128{ "-0.001" }, std::invalid_argument );
	}

	TEST( Int128Conversion, ToBits )
	{
		// Test zero
		datatypes::Int128 zero{};
		auto zeroBits{ zero.toBits() };
		EXPECT_EQ( zeroBits.size(), 4 );
		EXPECT_EQ( zeroBits[0], 0 );
		EXPECT_EQ( zeroBits[1], 0 );
		EXPECT_EQ( zeroBits[2], 0 );
		EXPECT_EQ( zeroBits[3], 0 );

		// Test simple positive value
		datatypes::Int128 simple{ 123456 };
		auto simpleBits{ simple.toBits() };
		EXPECT_EQ( simpleBits[0], 123456 );
		EXPECT_EQ( simpleBits[1], 0 );
		EXPECT_EQ( simpleBits[2], 0 );
		EXPECT_EQ( simpleBits[3], 0 );

		// Test negative value (sign extension)
		datatypes::Int128 negative{ -1 };
		auto negativeBits{ negative.toBits() };
		EXPECT_EQ( negativeBits[0], -1 );
		EXPECT_EQ( negativeBits[1], -1 );
		EXPECT_EQ( negativeBits[2], -1 );
		EXPECT_EQ( negativeBits[3], -1 );

		// Test specific negative value
		datatypes::Int128 negativeValue{ -123456 };
		auto negBits{ negativeValue.toBits() };
		EXPECT_EQ( negBits[0], -123456 );
		EXPECT_EQ( negBits[1], -1 ); // Sign extension
		EXPECT_EQ( negBits[2], -1 ); // Sign extension
		EXPECT_EQ( negBits[3], -1 ); // Sign extension

		// Test that toBits() returns 4 elements for any value
		datatypes::Int128 large{ static_cast<std::uint64_t>(0xFFFFFFFFULL) };
		auto largeBits{ large.toBits() };
		EXPECT_EQ( largeBits.size(), 4 );
		// First element should contain the lower 32 bits
		EXPECT_EQ( largeBits[0], static_cast<std::int32_t>( 0xFFFFFFFF ) );
		EXPECT_EQ( largeBits[1], 0 ); // Should be zero for values fitting in 32 bits
		EXPECT_EQ( largeBits[2], 0 );
		EXPECT_EQ( largeBits[3], 0 );
	}

	//----------------------------------------------
	// Arithmetic
	//----------------------------------------------

	TEST( Int128Arithmetic, Addition )
	{
		datatypes::Int128 a{ 100 };
		datatypes::Int128 b{ 200 };
		datatypes::Int128 result{ a + b };

		EXPECT_EQ( 300ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, AdditionWithCarry )
	{
		// Test overflow from low to high word
		datatypes::Int128 a{ constants::int128::MAX_POSITIVE_LOW, 0 };
		datatypes::Int128 b{ 1 };
		datatypes::Int128 result{ a + b };

		EXPECT_EQ( 0ULL, result.toLow() );
		EXPECT_EQ( 1ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, Subtraction )
	{
		datatypes::Int128 a{ 300 };
		datatypes::Int128 b{ 100 };
		datatypes::Int128 result{ a - b };

		EXPECT_EQ( 200ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, SubtractionWithBorrow )
	{
		// Test borrow from high to low word
		datatypes::Int128 a{ 0, 1 };
		datatypes::Int128 b{ 1 };
		datatypes::Int128 result{ a - b };

		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, Multiplication )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 456 };
		datatypes::Int128 result{ a * b };

		EXPECT_EQ( 123ULL * 456ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, MultiplicationLarge )
	{
		// Test multiplication that requires high word
		const std::uint64_t a_val{ 0x123456789ABCDEFULL };
		const std::uint64_t b_val{ 0x100000000ULL }; // 2^32

		datatypes::Int128 a{ a_val };
		datatypes::Int128 b{ b_val };
		datatypes::Int128 result{ a * b };

		// Result should be a_val shifted left by 32 bits
		EXPECT_EQ( ( a_val << 32 ) & constants::int128::MAX_POSITIVE_LOW, result.toLow() );
		EXPECT_EQ( a_val >> 32, result.toHigh() );
	}

	TEST( Int128Arithmetic, Division )
	{
		datatypes::Int128 a{ 456 };
		datatypes::Int128 b{ 123 };
		datatypes::Int128 result{ a / b };

		EXPECT_EQ( 3ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, DivisionByZero )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 zero{};

		EXPECT_THROW( a / zero, std::overflow_error );
	}

	TEST( Int128Arithmetic, Modulo )
	{
		datatypes::Int128 a{ 456 };
		datatypes::Int128 b{ 123 };
		datatypes::Int128 result{ a % b };

		// 456 % 123 = 87
		EXPECT_EQ( 87ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128Arithmetic, ModuloByZero )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 zero{};

		EXPECT_THROW( a % zero, std::overflow_error );
	}

	TEST( Int128Arithmetic, UnaryMinus )
	{
		datatypes::Int128 positive{ 123 };
		datatypes::Int128 negative{ -positive };

		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( negative.isZero() );

		// Two's complement: ~123 + 1
		EXPECT_EQ( static_cast<std::uint64_t>( -123 ), negative.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, negative.toHigh() );
	}

	TEST( Int128Arithmetic, UnaryMinusZero )
	{
		datatypes::Int128 zero{};
		datatypes::Int128 negated{ -zero };

		EXPECT_TRUE( negated.isZero() );
		EXPECT_FALSE( negated.isNegative() );
	}

	TEST( Int128Arithmetic, AbsoluteValue )
	{
		datatypes::Int128 positive{ 123 };
		datatypes::Int128 negative{ -123 };
		datatypes::Int128 zero{};

		EXPECT_EQ( positive, positive.abs() );
		EXPECT_EQ( positive, negative.abs() );
		EXPECT_EQ( zero, zero.abs() );
	}

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	TEST( Int128Comparison, Equality )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 123 };
		datatypes::Int128 c{ 456 };

		EXPECT_TRUE( a == b );
		EXPECT_FALSE( a == c );
	}

	TEST( Int128Comparison, Inequality )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 123 };
		datatypes::Int128 c{ 456 };

		EXPECT_FALSE( a != b );
		EXPECT_TRUE( a != c );
	}

	TEST( Int128Comparison, LessThan )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 456 };

		EXPECT_TRUE( a < b );
		EXPECT_FALSE( b < a );
		EXPECT_FALSE( a < a );
	}

	TEST( Int128Comparison, LessThanSignedComparison )
	{
		datatypes::Int128 positive{ 123 };
		datatypes::Int128 negative{ -456 };

		EXPECT_TRUE( negative < positive );
		EXPECT_FALSE( positive < negative );
	}

	TEST( Int128Comparison, LessEqual )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 456 };
		datatypes::Int128 c{ 123 };

		EXPECT_TRUE( a <= b );
		EXPECT_TRUE( a <= c );
		EXPECT_FALSE( b <= a );
	}

	TEST( Int128Comparison, GreaterThan )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 456 };

		EXPECT_FALSE( a > b );
		EXPECT_TRUE( b > a );
		EXPECT_FALSE( a > a );
	}

	TEST( Int128Comparison, GreaterEqual )
	{
		datatypes::Int128 a{ 123 };
		datatypes::Int128 b{ 456 };
		datatypes::Int128 c{ 123 };

		EXPECT_FALSE( a >= b );
		EXPECT_TRUE( a >= c );
		EXPECT_TRUE( b >= a );
	}

	TEST( Int128Comparison, ComparisonHighWord )
	{
		// Test comparison when high words differ
		datatypes::Int128 a{ constants::int128::MAX_POSITIVE_LOW, 0 };
		datatypes::Int128 b{ 0, 1 };

		EXPECT_TRUE( a < b );
		EXPECT_FALSE( b < a );
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	TEST( Int128StateChecking, IsZero )
	{
		datatypes::Int128 zero{};
		datatypes::Int128 nonZero{ 1 };

		EXPECT_TRUE( zero.isZero() );
		EXPECT_FALSE( nonZero.isZero() );
	}

	TEST( Int128StateChecking, IsNegative )
	{
		datatypes::Int128 positive{ 123 };
		datatypes::Int128 negative{ -123 };
		datatypes::Int128 zero{};

		EXPECT_FALSE( positive.isNegative() );
		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( zero.isNegative() );
	}

	TEST( Int128StateChecking, IsNegativeHighBit )
	{
		// Test negativity based on high bit
		datatypes::Int128 negative{ 0, constants::int128::MIN_NEGATIVE_HIGH };
		datatypes::Int128 positive{ 0, constants::int128::MAX_POSITIVE_HIGH };

		EXPECT_TRUE( negative.isNegative() );
		EXPECT_FALSE( positive.isNegative() );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( Int128StringParsing, TryParseMethod )
	{
		datatypes::Int128 result;

		// Valid positive parsing
		EXPECT_TRUE( datatypes::Int128::tryParse( "12345", result ) );
		EXPECT_EQ( 12345ULL, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
		EXPECT_FALSE( result.isNegative() );

		// Valid negative parsing
		EXPECT_TRUE( datatypes::Int128::tryParse( "-9876543210", result ) );
		EXPECT_TRUE( result.isNegative() );

		// Zero parsing
		EXPECT_TRUE( datatypes::Int128::tryParse( "0", result ) );
		EXPECT_TRUE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Large positive number
		EXPECT_TRUE( datatypes::Int128::tryParse( "123456789012345678901234567890", result ) );
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Large negative number
		EXPECT_TRUE( datatypes::Int128::tryParse( "-123456789012345678901234567890", result ) );
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		// Positive sign
		EXPECT_TRUE( datatypes::Int128::tryParse( "+42", result ) );
		EXPECT_EQ( 42ULL, result.toLow() );
		EXPECT_FALSE( result.isNegative() );

		// Invalid strings
		EXPECT_FALSE( datatypes::Int128::tryParse( "", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "123abc", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "12.34", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "+", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "-", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "123 456", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "1e10", result ) );
	}

	TEST( Int128StringParsing, ParseMethod )
	{
		// Valid positive parsing
		auto result1{ datatypes::Int128::parse( "12345" ) };
		EXPECT_EQ( 12345ULL, result1.toLow() );
		EXPECT_FALSE( result1.isNegative() );

		// Valid negative parsing
		auto result2{ datatypes::Int128::parse( "-9876543210" ) };
		EXPECT_TRUE( result2.isNegative() );

		// Zero parsing
		auto result3{ datatypes::Int128::parse( "0" ) };
		EXPECT_TRUE( result3.isZero() );

		// Large number parsing
		auto result4{ datatypes::Int128::parse( "123456789012345678901234567890" ) };
		EXPECT_FALSE( result4.isZero() );
		EXPECT_FALSE( result4.isNegative() );

		// Positive sign
		auto result5{ datatypes::Int128::parse( "+42" ) };
		EXPECT_EQ( 42ULL, result5.toLow() );
		EXPECT_FALSE( result5.isNegative() );

		// Invalid parsing should throw
		EXPECT_THROW( (void)datatypes::Int128::parse( "" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "abc" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "123abc" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "12.34" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "+" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "-" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "123 456" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Int128::parse( "1e10" ), std::invalid_argument );
	}

	//----------------------------------------------
	// Stream
	//----------------------------------------------

	TEST( Int128Stream, StreamOutput )
	{
		datatypes::Int128 i1{ "123456" };
		std::ostringstream oss;
		oss << i1;
		EXPECT_EQ( oss.str(), "123456" );

		datatypes::Int128 i2{ "-789123" };
		oss.str( "" );
		oss << i2;
		EXPECT_EQ( oss.str(), "-789123" );
	}

	TEST( Int128Stream, StreamInput )
	{
		datatypes::Int128 i1;
		std::istringstream iss( "456789" );
		iss >> i1;
		EXPECT_FALSE( iss.fail() );
		EXPECT_EQ( i1.toString(), "456789" );

		// Test invalid input
		datatypes::Int128 i2;
		std::istringstream iss2( "invalid" );
		iss2 >> i2;
		EXPECT_TRUE( iss2.fail() );

		// Test decimal input (should fail for integer type)
		datatypes::Int128 i3;
		std::istringstream iss3( "123.456" );
		iss3 >> i3;
		EXPECT_TRUE( iss3.fail() );
	}

	//----------------------------------------------
	// Edge case and overflow
	//----------------------------------------------

	TEST( Int128EdgeCaseAndOverflow, MaxValues )
	{
		// Test with maximum possible values
		datatypes::Int128 maxVal{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };

		EXPECT_FALSE( maxVal.isZero() );
		EXPECT_FALSE( maxVal.isNegative() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, maxVal.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_HIGH, maxVal.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, MinValue )
	{
		// Test with minimum possible value (most negative)
		datatypes::Int128 minVal{ 0, constants::int128::MIN_NEGATIVE_HIGH };

		EXPECT_FALSE( minVal.isZero() );
		EXPECT_TRUE( minVal.isNegative() );
		EXPECT_EQ( 0ULL, minVal.toLow() );
		EXPECT_EQ( constants::int128::MIN_NEGATIVE_HIGH, minVal.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, OverflowAddition )
	{
		// Test addition overflow
		datatypes::Int128 maxPositive{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };
		datatypes::Int128 one{ 1 };
		datatypes::Int128 result{ maxPositive + one };

		// Should wrap to minimum negative value
		EXPECT_TRUE( result.isNegative() );
		EXPECT_EQ( 0ULL, result.toLow() );
		EXPECT_EQ( constants::int128::MIN_NEGATIVE_HIGH, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, OverflowSubtraction )
	{
		// Test subtraction underflow
		datatypes::Int128 minNegative{ 0, constants::int128::MIN_NEGATIVE_HIGH };
		datatypes::Int128 one{ 1 };
		datatypes::Int128 result{ minNegative - one };

		// Should wrap to maximum positive value
		EXPECT_FALSE( result.isNegative() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, result.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_HIGH, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, OverflowMultiplication )
	{
		// Test multiplication that overflows 128 bits
		datatypes::Int128 large1{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };
		datatypes::Int128 two{ 2 };
		datatypes::Int128 result{ large1 * two };

		// Should wrap around due to overflow
		EXPECT_EQ( 0xFFFFFFFFFFFFFFFEULL, result.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, DivisionLargeNumbers )
	{
		// Test division with large 128-bit numbers
		datatypes::Int128 dividend{ 0, 1 }; // 2^64
		datatypes::Int128 divisor{ 2 };
		datatypes::Int128 result{ dividend / divisor };

		// Should be 2^63
		EXPECT_EQ( constants::int128::MIN_NEGATIVE_HIGH, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, ModuloLargeNumbers )
	{
		// Test modulo with large numbers
		datatypes::Int128 dividend{ constants::int128::MAX_POSITIVE_LOW, 1 }; // 2^64 + (2^64-1)
		datatypes::Int128 divisor{ 0, 1 };									  // 2^64 (constructed as {0, 1})
		datatypes::Int128 result{ dividend % divisor };

		// Should be 2^64 - 1
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, result.toLow() );
		EXPECT_EQ( 0ULL, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, SignBitBoundary )
	{
		// Test numbers right at the sign bit boundary
		datatypes::Int128 justPositive{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };
		datatypes::Int128 justNegative{ 0, constants::int128::MIN_NEGATIVE_HIGH };

		EXPECT_FALSE( justPositive.isNegative() );
		EXPECT_TRUE( justNegative.isNegative() );

		// Adding 1 to max positive should make it negative
		datatypes::Int128 wrapped{ justPositive + datatypes::Int128{ 1 } };
		EXPECT_TRUE( wrapped.isNegative() );
		EXPECT_EQ( wrapped, justNegative );
	}

	TEST( Int128EdgeCaseAndOverflow, ParseVeryLargeNumbers )
	{
		// Test parsing numbers near 128-bit limits
		datatypes::Int128 result;

		// Test maximum positive 128-bit signed integer
		EXPECT_TRUE( datatypes::Int128::tryParse( "170141183460469231731687303715884105727", result ) );
		EXPECT_FALSE( result.isNegative() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_LOW, result.toLow() );
		EXPECT_EQ( constants::int128::MAX_POSITIVE_HIGH, result.toHigh() );

		// Test minimum negative 128-bit signed integer
		EXPECT_TRUE( datatypes::Int128::tryParse( "-170141183460469231731687303715884105728", result ) );
		EXPECT_TRUE( result.isNegative() );
		EXPECT_EQ( 0ULL, result.toLow() );
		EXPECT_EQ( constants::int128::MIN_NEGATIVE_HIGH, result.toHigh() );
	}

	TEST( Int128EdgeCaseAndOverflow, ParseOverflowNumbers )
	{
		// Test parsing numbers that exceed 128-bit range
		datatypes::Int128 result;

		// Should fail for numbers too large
		EXPECT_FALSE( datatypes::Int128::tryParse( "170141183460469231731687303715884105728", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "-170141183460469231731687303715884105729", result ) );
		EXPECT_FALSE( datatypes::Int128::tryParse( "999999999999999999999999999999999999999", result ) );
	}

	TEST( Int128EdgeCaseAndOverflow, ComparisonHighWordEdges )
	{
		// Test comparison edge cases with high word differences
		datatypes::Int128 a{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };
		datatypes::Int128 b{ 0, constants::int128::MIN_NEGATIVE_HIGH };

		// Max positive vs min negative
		EXPECT_TRUE( a > b );
		EXPECT_FALSE( a < b );
		EXPECT_TRUE( b < a );
		EXPECT_FALSE( b > a );
	}

	TEST( Int128EdgeCaseAndOverflow, NegationEdgeCases )
	{
		// Test negation of boundary values
		datatypes::Int128 maxPositive{ constants::int128::MAX_POSITIVE_LOW, constants::int128::MAX_POSITIVE_HIGH };
		datatypes::Int128 minNegative{ 0, constants::int128::MIN_NEGATIVE_HIGH };

		// Negating max positive should give min negative + 1
		datatypes::Int128 negated{ -maxPositive };
		datatypes::Int128 expected{ minNegative + datatypes::Int128{ 1 } };
		EXPECT_EQ( expected, negated );

		// Negating min negative should overflow back to itself
		datatypes::Int128 negatedMin{ -minNegative };
		EXPECT_EQ( minNegative, negatedMin );
	}
}
