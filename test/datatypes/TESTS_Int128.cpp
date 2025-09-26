/**
 * @file TESTS_Int128.cpp
 * @brief Comprehensive tests for Int128 128-bit signed integer class
 * @details Tests covering construction, arithmetic, comparison, and edge cases
 */

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <limits>

#include <nfx/datatypes/constants/Int128Constants.h>
#include <nfx/datatypes/Decimal.h>
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

	TEST( Int128Construction, ConstructionFromFloat )
	{
		// Test basic truncation behavior (matches C++ static_cast<int>(float))
		float f1 = 42.9f;
		datatypes::Int128 i1{ f1 };
		EXPECT_EQ( i1.toString(), "42" ); // Truncated toward zero
		EXPECT_FALSE( i1.isNegative() );
		EXPECT_FALSE( i1.isZero() );

		// Test negative truncation
		float f2 = -42.9f;
		datatypes::Int128 i2{ f2 };
		EXPECT_EQ( i2.toString(), "-42" ); // Truncated toward zero
		EXPECT_TRUE( i2.isNegative() );
		EXPECT_FALSE( i2.isZero() );

		// Test exact integer values
		float f3 = 123.0f;
		datatypes::Int128 i3{ f3 };
		EXPECT_EQ( i3.toString(), "123" );
		EXPECT_FALSE( i3.isNegative() );

		// Test zero
		float f4 = 0.0f;
		datatypes::Int128 i4{ f4 };
		EXPECT_TRUE( i4.isZero() );
		EXPECT_FALSE( i4.isNegative() );

		// Test negative zero
		float f5 = -0.0f;
		datatypes::Int128 i5{ f5 };
		EXPECT_TRUE( i5.isZero() );
		EXPECT_FALSE( i5.isNegative() );

		// Test large positive float
		float f6 = 1234567.8f;
		datatypes::Int128 i6{ f6 };
		EXPECT_EQ( i6.toString(), "1234567" ); // Truncated
		EXPECT_FALSE( i6.isNegative() );

		// Test large negative float
		float f7 = -9876543.2f;
		datatypes::Int128 i7{ f7 };
		EXPECT_EQ( i7.toString(), "-9876543" ); // Truncated
		EXPECT_TRUE( i7.isNegative() );

		// Test special values - NaN should become zero
		float f8 = std::numeric_limits<float>::quiet_NaN();
		datatypes::Int128 i8{ f8 };
		EXPECT_TRUE( i8.isZero() );

		// Test small fractional values
		float f11 = 0.9f;
		datatypes::Int128 i11{ f11 };
		EXPECT_TRUE( i11.isZero() ); // Truncated to zero

		float f12 = -0.9f;
		datatypes::Int128 i12{ f12 };
		EXPECT_TRUE( i12.isZero() ); // Truncated to zero
	}

	TEST( Int128Construction, ConstructionFromDouble )
	{
		// Test basic truncation behavior (matches C++ static_cast<int>(double))
		double d1 = 42.7;
		datatypes::Int128 i1{ d1 };
		EXPECT_EQ( i1.toString(), "42" ); // Truncated toward zero
		EXPECT_FALSE( i1.isNegative() );
		EXPECT_FALSE( i1.isZero() );

		// Test negative truncation
		double d2 = -42.7;
		datatypes::Int128 i2{ d2 };
		EXPECT_EQ( i2.toString(), "-42" ); // Truncated toward zero
		EXPECT_TRUE( i2.isNegative() );
		EXPECT_FALSE( i2.isZero() );

		// Test exact integer values
		double d3 = 123456.0;
		datatypes::Int128 i3{ d3 };
		EXPECT_EQ( i3.toString(), "123456" );
		EXPECT_FALSE( i3.isNegative() );

		// Test zero
		double d4 = 0.0;
		datatypes::Int128 i4{ d4 };
		EXPECT_TRUE( i4.isZero() );
		EXPECT_FALSE( i4.isNegative() );

		// Test negative zero
		double d5 = -0.0;
		datatypes::Int128 i5{ d5 };
		EXPECT_TRUE( i5.isZero() );
		EXPECT_FALSE( i5.isNegative() );

		// Test large positive double within range
		double d6 = 123456789012345.6;
		datatypes::Int128 i6{ d6 };
		EXPECT_EQ( i6.toString(), "123456789012345" ); // Truncated
		EXPECT_FALSE( i6.isNegative() );

		// Test large negative double within range
		double d7 = -987654321098765.4;
		datatypes::Int128 i7{ d7 };
		EXPECT_EQ( i7.toString(), "-987654321098765" ); // Truncated
		EXPECT_TRUE( i7.isNegative() );

		// Test very large double (should use string conversion for accuracy)
		double d8 = 1.23456789012345e20; // 20 digits
		datatypes::Int128 i8{ d8 };
		EXPECT_FALSE( i8.isZero() );
		EXPECT_FALSE( i8.isNegative() );
		// Result should be close to 123456789012345000000 (truncated)

		// Test special values - NaN should become zero
		double d9 = std::numeric_limits<double>::quiet_NaN();
		datatypes::Int128 i9{ d9 };
		EXPECT_TRUE( i9.isZero() );

		// Test small fractional values
		double d12 = 0.9999;
		datatypes::Int128 i12{ d12 };
		EXPECT_TRUE( i12.isZero() ); // Truncated to zero

		double d13 = -0.9999;
		datatypes::Int128 i13{ d13 };
		EXPECT_TRUE( i13.isZero() ); // Truncated to zero

		// Test edge case: exactly 1.0 - epsilon
		double d14 = 1.0 - std::numeric_limits<double>::epsilon();
		datatypes::Int128 i14{ d14 };
		EXPECT_TRUE( i14.isZero() ); // Should truncate to 0

		// Test edge case: exactly 1.0
		double d15 = 1.0;
		datatypes::Int128 i15{ d15 };
		EXPECT_EQ( i15.toString(), "1" );
		EXPECT_FALSE( i15.isNegative() );

		// Test consistency with C++ behavior
		double testValue = 12.6;
		int cpp_result = static_cast<int>( testValue );
		datatypes::Int128 int128_result{ testValue };
		EXPECT_EQ( int128_result.toString(), std::to_string( cpp_result ) );
	}

	TEST( Int128Construction, ConstructionFromDecimal )
	{
		// Test construction from simple positive Decimal
		datatypes::Decimal d1{ 42 };
		datatypes::Int128 i1{ d1 };
		EXPECT_EQ( 42ULL, i1.toLow() );
		EXPECT_EQ( 0ULL, i1.toHigh() );
		EXPECT_FALSE( i1.isNegative() );
		EXPECT_FALSE( i1.isZero() );

		// Test construction from simple negative Decimal
		datatypes::Decimal d2{ -123 };
		datatypes::Int128 i2{ d2 };
		EXPECT_TRUE( i2.isNegative() );
		EXPECT_FALSE( i2.isZero() );
		EXPECT_EQ( i2.toString(), "-123" );

		// Test construction from zero Decimal
		datatypes::Decimal d3{ 0 };
		datatypes::Int128 i3{ d3 };
		EXPECT_TRUE( i3.isZero() );
		EXPECT_FALSE( i3.isNegative() );
		EXPECT_EQ( 0ULL, i3.toLow() );
		EXPECT_EQ( 0ULL, i3.toHigh() );

		// Test construction from large positive Decimal (within Int128 range)
		datatypes::Decimal d4{ "12345678901234567890" };
		datatypes::Int128 i4{ d4 };
		EXPECT_FALSE( i4.isZero() );
		EXPECT_FALSE( i4.isNegative() );
		EXPECT_EQ( i4.toString(), "12345678901234567890" );

		// Test construction from large negative Decimal
		datatypes::Decimal d5{ "-9876543210987654321" };
		datatypes::Int128 i5{ d5 };
		EXPECT_FALSE( i5.isZero() );
		EXPECT_TRUE( i5.isNegative() );
		EXPECT_EQ( i5.toString(), "-9876543210987654321" );

		// Test construction from Decimal with trailing zeros (should work)
		datatypes::Decimal d6{ "42.0000" }; // Will be normalized to integer
		datatypes::Int128 i6{ d6 };
		EXPECT_FALSE( i6.isZero() );
		EXPECT_FALSE( i6.isNegative() );
		EXPECT_EQ( 42ULL, i6.toLow() );

		// Test that construction from Decimal with fractional part truncates (like C++ cast)
		datatypes::Decimal d7{ "42.5" };
		datatypes::Int128 i7{ d7 };
		EXPECT_EQ( i7.toString(), "42" ); // Truncated toward zero

		// Test that construction from Decimal with small fractional part truncates
		datatypes::Decimal d8{ "123.001" };
		datatypes::Int128 i8{ d8 };
		EXPECT_EQ( i8.toString(), "123" ); // Truncated toward zero

		// Test that construction from negative Decimal with fractional part truncates
		datatypes::Decimal d9{ "-456.789" };
		datatypes::Int128 i9{ d9 };
		EXPECT_EQ( i9.toString(), "-456" ); // Truncated toward zero

		// Test construction from very large Decimal (within both types' limits)
		datatypes::Decimal d10{ "1234567890123456789012345678" }; // 28 digits (Decimal limit)
		datatypes::Int128 i10{ d10 };
		EXPECT_FALSE( i10.isZero() );
		EXPECT_FALSE( i10.isNegative() );
		EXPECT_EQ( i10.toString(), "1234567890123456789012345678" );

		// Test roundtrip conversion consistency
		datatypes::Decimal original{ "987654321098765432109876" };
		datatypes::Int128 converted{ original };
		datatypes::Decimal backToDecimal{ converted.toString() };
		EXPECT_TRUE( original == backToDecimal );
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
		datatypes::Int128 large{ static_cast<std::uint64_t>( 0xFFFFFFFFULL ) };
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
	// Comparison with built-in integer types
	//----------------------------------------------

	TEST( Int128ComparisonBuiltinTypes, ComparisonWithInt64 )
	{
		datatypes::Int128 int128_42{ 42 };
		datatypes::Int128 int128_negative{ -123 };
		datatypes::Int128 int128_zero{ 0 };

		std::int64_t int64_42 = 42;
		std::int64_t int64_100 = 100;
		std::int64_t int64_negative = -123;
		std::int64_t int64_zero = 0;

		// Equality tests
		EXPECT_TRUE( int128_42 == int64_42 );
		EXPECT_FALSE( int128_42 == int64_100 );
		EXPECT_TRUE( int128_negative == int64_negative );
		EXPECT_TRUE( int128_zero == int64_zero );

		// Inequality tests
		EXPECT_FALSE( int128_42 != int64_42 );
		EXPECT_TRUE( int128_42 != int64_100 );
		EXPECT_FALSE( int128_negative != int64_negative );

		// Less than tests
		EXPECT_TRUE( int128_42 < int64_100 );
		EXPECT_FALSE( int128_42 < int64_42 );
		EXPECT_TRUE( int128_negative < int64_42 );
		EXPECT_TRUE( int128_negative < int64_zero );

		// Less than or equal tests
		EXPECT_TRUE( int128_42 <= int64_42 );
		EXPECT_TRUE( int128_42 <= int64_100 );
		EXPECT_FALSE( int128_42 <= int64_negative );

		// Greater than tests
		EXPECT_FALSE( int128_42 > int64_100 );
		EXPECT_FALSE( int128_42 > int64_42 );
		EXPECT_TRUE( int128_42 > int64_negative );
		EXPECT_TRUE( int128_zero > int64_negative );

		// Greater than or equal tests
		EXPECT_TRUE( int128_42 >= int64_42 );
		EXPECT_FALSE( int128_42 >= int64_100 );
		EXPECT_TRUE( int128_42 >= int64_negative );
	}

	TEST( Int128ComparisonBuiltinTypes, ComparisonWithUint64 )
	{
		datatypes::Int128 int128_42{ 42 };
		datatypes::Int128 int128_large{ static_cast<std::uint64_t>( 0x123456789ABCDEFULL ) };
		datatypes::Int128 int128_negative{ -123 };
		datatypes::Int128 int128_zero{ 0 };

		std::uint64_t uint64_42 = 42;
		std::uint64_t uint64_100 = 100;
		std::uint64_t uint64_large = 0x123456789ABCDEFULL;
		std::uint64_t uint64_zero = 0;

		// Equality tests
		EXPECT_TRUE( int128_42 == uint64_42 );
		EXPECT_FALSE( int128_42 == uint64_100 );
		EXPECT_TRUE( int128_large == uint64_large );
		EXPECT_TRUE( int128_zero == uint64_zero );
		EXPECT_FALSE( int128_negative == uint64_42 ); // Negative Int128 never equals positive uint64_t

		// Inequality tests
		EXPECT_FALSE( int128_42 != uint64_42 );
		EXPECT_TRUE( int128_42 != uint64_100 );
		EXPECT_TRUE( int128_negative != uint64_42 );

		// Less than tests with negative Int128
		EXPECT_TRUE( int128_negative < uint64_zero ); // Negative is always less than any positive uint64_t
		EXPECT_TRUE( int128_negative < uint64_42 );
		EXPECT_TRUE( int128_negative < uint64_100 );

		// Less than tests with positive Int128
		EXPECT_TRUE( int128_42 < uint64_100 );
		EXPECT_FALSE( int128_42 < uint64_42 );
		EXPECT_FALSE( int128_large < uint64_42 );

		// Less than or equal tests
		EXPECT_TRUE( int128_42 <= uint64_42 );
		EXPECT_TRUE( int128_42 <= uint64_100 );
		EXPECT_TRUE( int128_negative <= uint64_zero );

		// Greater than tests with negative Int128
		EXPECT_FALSE( int128_negative > uint64_zero ); // Negative is never greater than positive uint64_t
		EXPECT_FALSE( int128_negative > uint64_42 );

		// Greater than tests with positive Int128
		EXPECT_FALSE( int128_42 > uint64_100 );
		EXPECT_FALSE( int128_42 > uint64_42 );
		EXPECT_TRUE( int128_large > uint64_42 );

		// Greater than or equal tests
		EXPECT_TRUE( int128_42 >= uint64_42 );
		EXPECT_FALSE( int128_42 >= uint64_100 );
		EXPECT_FALSE( int128_negative >= uint64_zero );
	}

	TEST( Int128ComparisonBuiltinTypes, ComparisonWithInt )
	{
		datatypes::Int128 int128_42{ 42 };
		datatypes::Int128 int128_negative{ -123 };
		datatypes::Int128 int128_zero{ 0 };

		int int_42 = 42;
		int int_100 = 100;
		int int_negative = -123;
		int int_zero = 0;

		// Equality tests
		EXPECT_TRUE( int128_42 == int_42 );
		EXPECT_FALSE( int128_42 == int_100 );
		EXPECT_TRUE( int128_negative == int_negative );
		EXPECT_TRUE( int128_zero == int_zero );

		// Inequality tests
		EXPECT_FALSE( int128_42 != int_42 );
		EXPECT_TRUE( int128_42 != int_100 );
		EXPECT_FALSE( int128_negative != int_negative );

		// Less than tests
		EXPECT_TRUE( int128_42 < int_100 );
		EXPECT_FALSE( int128_42 < int_42 );
		EXPECT_TRUE( int128_negative < int_42 );
		EXPECT_TRUE( int128_negative < int_zero );

		// Less than or equal tests
		EXPECT_TRUE( int128_42 <= int_42 );
		EXPECT_TRUE( int128_42 <= int_100 );
		EXPECT_FALSE( int128_42 <= int_negative );

		// Greater than tests
		EXPECT_FALSE( int128_42 > int_100 );
		EXPECT_FALSE( int128_42 > int_42 );
		EXPECT_TRUE( int128_42 > int_negative );
		EXPECT_TRUE( int128_zero > int_negative );

		// Greater than or equal tests
		EXPECT_TRUE( int128_42 >= int_42 );
		EXPECT_FALSE( int128_42 >= int_100 );
		EXPECT_TRUE( int128_42 >= int_negative );
	}

	TEST( Int128ComparisonBuiltinTypes, ComparisonWithLargeValues )
	{
		// Test Int128 values that exceed the range of smaller integer types
		datatypes::Int128 very_large{ static_cast<std::uint64_t>( 0xFFFFFFFFFFFFFFFFULL ) }; // 2^64-1
		datatypes::Int128 int128_max_int64{ std::numeric_limits<std::int64_t>::max() };
		datatypes::Int128 int128_min_int64{ std::numeric_limits<std::int64_t>::min() };

		std::int64_t max_int64 = std::numeric_limits<std::int64_t>::max();
		std::int64_t min_int64 = std::numeric_limits<std::int64_t>::min();
		std::uint64_t max_uint64 = std::numeric_limits<std::uint64_t>::max();

		// Test boundary values
		EXPECT_TRUE( int128_max_int64 == max_int64 );
		EXPECT_TRUE( int128_min_int64 == min_int64 );
		EXPECT_TRUE( very_large == max_uint64 );

		// Test that large Int128 is greater than smaller type maximums
		EXPECT_TRUE( very_large > max_int64 );
		EXPECT_FALSE( very_large < max_int64 );
	}

	TEST( Int128ComparisonBuiltinTypes, ComparisonEdgeCases )
	{
		// Test edge cases around zero and sign boundaries
		datatypes::Int128 positive_one{ 1 };
		datatypes::Int128 negative_one{ -1 };
		datatypes::Int128 zero{ 0 };

		// Test comparisons with zero
		EXPECT_TRUE( positive_one > 0 );
		EXPECT_TRUE( negative_one < 0 );
		EXPECT_TRUE( zero == 0 );
		EXPECT_FALSE( zero != 0 );

		// Test with different integer types representing the same value
		EXPECT_TRUE( positive_one == std::int64_t{ 1 } );
		EXPECT_TRUE( positive_one == std::int64_t{ 1 } );
		EXPECT_TRUE( positive_one == std::int64_t{ 1 } );

		// Test negative comparisons with unsigned types
		EXPECT_TRUE( negative_one < std::uint64_t{ 0 } );
		EXPECT_FALSE( negative_one > std::uint64_t{ 0 } );
		EXPECT_FALSE( negative_one == std::uint64_t{ 2 } ); // Should never be equal to positive uint64_t
	}

	TEST( Int128ComparisonBuiltinTypes, ComparisonSymmetry )
	{
		// Test that Int128 comparison with built-in types works correctly
		// Note: We're testing Int128 op builtin_type, not builtin_type op Int128
		datatypes::Int128 int128_val{ 42 };
		std::int64_t int64_val = 42;
		std::uint64_t uint64_val = 42;
		int int_val = 42;

		// All these should be true for equality
		EXPECT_TRUE( int128_val == int64_val );
		EXPECT_TRUE( int128_val == uint64_val );
		EXPECT_TRUE( int128_val == int_val );

		// Test inequality
		EXPECT_FALSE( int128_val != int64_val );
		EXPECT_FALSE( int128_val != uint64_val );
		EXPECT_FALSE( int128_val != int_val );

		// Test with different values
		datatypes::Int128 int128_different{ 99 };
		EXPECT_FALSE( int128_different == int64_val );
		EXPECT_FALSE( int128_different == uint64_val );
		EXPECT_FALSE( int128_different == int_val );
		EXPECT_TRUE( int128_different != int64_val );
		EXPECT_TRUE( int128_different != uint64_val );
		EXPECT_TRUE( int128_different != int_val );
	}

	//----------------------------------------------
	// Comparison with built-in floating point types
	//----------------------------------------------

	TEST( Int128FloatingPointComparison, FloatingPointComparison )
	{
		datatypes::Int128 int128_42{ 42 };
		datatypes::Int128 int128_negative{ -123 };
		datatypes::Int128 int128_zero{ 0 };
		datatypes::Int128 int128_large{ 1000000 };

		// Test equality with double
		EXPECT_TRUE( int128_42 == 42.0 );
		EXPECT_TRUE( int128_negative == -123.0 );
		EXPECT_TRUE( int128_zero == 0.0 );
		EXPECT_TRUE( int128_large == 1000000.0 );
		EXPECT_FALSE( int128_42 == 42.1 );
		EXPECT_FALSE( int128_42 == 43.0 );

		// Test inequality with double
		EXPECT_FALSE( int128_42 != 42.0 );
		EXPECT_TRUE( int128_42 != 42.1 );
		EXPECT_TRUE( int128_negative != 42.0 );

		// Test less than with double
		EXPECT_TRUE( int128_42 < 42.1 );
		EXPECT_FALSE( int128_42 < 42.0 );
		EXPECT_FALSE( int128_42 < 41.9 );
		EXPECT_TRUE( int128_negative < 0.0 );
		EXPECT_TRUE( int128_negative < -122.9 );

		// Test less than or equal with double
		EXPECT_TRUE( int128_42 <= 42.1 );
		EXPECT_TRUE( int128_42 <= 42.0 );
		EXPECT_FALSE( int128_42 <= 41.9 );
		EXPECT_TRUE( int128_negative <= -123.0 );

		// Test greater than with double
		EXPECT_FALSE( int128_42 > 42.1 );
		EXPECT_FALSE( int128_42 > 42.0 );
		EXPECT_TRUE( int128_42 > 41.9 );
		EXPECT_FALSE( int128_negative > 0.0 );
		EXPECT_TRUE( int128_negative > -123.1 );

		// Test greater than or equal with double
		EXPECT_FALSE( int128_42 >= 42.1 );
		EXPECT_TRUE( int128_42 >= 42.0 );
		EXPECT_TRUE( int128_42 >= 41.9 );
		EXPECT_TRUE( int128_negative >= -123.0 );

		// Test with float (should delegate to double)
		EXPECT_TRUE( int128_42 == 42.0f );
		EXPECT_TRUE( int128_42 < 42.1f );
		EXPECT_TRUE( int128_42 > 41.9f );
	}

	TEST( Int128FloatingPointComparison, FloatingPointSpecialValues )
	{
		datatypes::Int128 int128_val{ 123 };

		// Test with NaN - should always return false for equality, no ordering
		double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_FALSE( int128_val == nan );
		EXPECT_TRUE( int128_val != nan );
		EXPECT_FALSE( int128_val < nan );
		EXPECT_FALSE( int128_val <= nan );
		EXPECT_FALSE( int128_val > nan );
		EXPECT_FALSE( int128_val >= nan );
	}

	TEST( Int128FloatingPointComparison, FloatingPointPrecisionBehavior )
	{
		// Test that demonstrates floating-point precision with integers

		// Small integers should compare exactly
		datatypes::Int128 small_int{ 42 };
		EXPECT_TRUE( small_int == 42.0 );
		EXPECT_TRUE( small_int == 42.0f );

		// Large integers within double precision range
		datatypes::Int128 medium_int{ 1000000 };
		EXPECT_TRUE( medium_int == 1000000.0 );
		EXPECT_TRUE( medium_int == 1000000.0f );

		// Very large integers may have precision issues with float but not double
		datatypes::Int128 large_int{ static_cast<std::int64_t>( 123456789012345LL ) };
		EXPECT_TRUE( large_int == 123456789012345.0 ); // Double should handle this

		// Test fractional values - integers should never equal non-integer values
		EXPECT_FALSE( small_int == 42.1 );
		EXPECT_FALSE( small_int == 42.000001 );
		EXPECT_FALSE( small_int == 41.999999 );

		// Test ordering with fractional values
		EXPECT_TRUE( small_int > 41.9 );
		EXPECT_TRUE( small_int < 42.1 );
		EXPECT_TRUE( small_int >= 42.0 );
		EXPECT_TRUE( small_int <= 42.0 );
	}

	TEST( Int128FloatingPointComparison, LargeNumberComparisons )
	{
		// Test comparisons with numbers near the limits of floating-point precision

		// Large positive Int128
		datatypes::Int128 large_positive{ static_cast<std::uint64_t>( 0x1FFFFFFFFFFFFFULL ) }; // Large but within double precision
		double large_double = static_cast<double>( 0x1FFFFFFFFFFFFFULL );
		EXPECT_TRUE( large_positive == large_double );

		// Large negative Int128
		datatypes::Int128 large_negative{ -static_cast<std::int64_t>( 0x1FFFFFFFFFFFFFLL ) };
		double large_negative_double = -static_cast<double>( 0x1FFFFFFFFFFFFFULL );
		EXPECT_TRUE( large_negative == large_negative_double );

		// Test ordering with large numbers
		EXPECT_TRUE( large_positive > large_negative_double );
		EXPECT_TRUE( large_negative < large_double );

		// Test boundary conditions
		datatypes::Int128 max_int64{ std::numeric_limits<std::int64_t>::max() };
		double max_int64_as_double = static_cast<double>( std::numeric_limits<std::int64_t>::max() );
		EXPECT_TRUE( max_int64 <= max_int64_as_double ); // May not be exactly equal due to precision
	}

	TEST( Int128FloatingPointComparison, EdgeCaseComparisons )
	{
		datatypes::Int128 positive_one{ 1 };
		datatypes::Int128 negative_one{ -1 };
		datatypes::Int128 zero{ 0 };

		// Test comparisons around zero
		EXPECT_TRUE( positive_one > 0.0 );
		EXPECT_TRUE( negative_one < 0.0 );
		EXPECT_TRUE( zero == 0.0 );
		EXPECT_TRUE( zero == -0.0 ); // IEEE 754: +0.0 == -0.0

		// Test with very small floating-point values
		EXPECT_TRUE( zero > -0.000001 );
		EXPECT_TRUE( zero < 0.000001 );
		EXPECT_TRUE( positive_one > 0.999999 );
		EXPECT_TRUE( positive_one < 1.000001 );

		// Test negative comparisons
		EXPECT_TRUE( negative_one < -0.999999 );
		EXPECT_TRUE( negative_one > -1.000001 );
	}

	TEST( Int128FloatingPointComparison, ComparisonSymmetry )
	{
		// Test that Int128 floating-point comparisons work consistently
		datatypes::Int128 int128_val{ 42 };
		double double_val = 42.0;
		float float_val = 42.0f;

		// Test equality consistency
		EXPECT_EQ( int128_val == double_val, true );
		EXPECT_EQ( int128_val == float_val, true );
		EXPECT_EQ( int128_val != double_val, false );
		EXPECT_EQ( int128_val != float_val, false );

		// Test ordering consistency
		datatypes::Int128 int128_larger{ 50 };
		EXPECT_EQ( int128_larger > double_val, true );
		EXPECT_EQ( int128_larger > float_val, true );
		EXPECT_EQ( int128_val < 50.0, true );
		EXPECT_EQ( int128_val < 50.0f, true );

		// Test with different values
		EXPECT_EQ( int128_val == 43.0, false );
		EXPECT_EQ( int128_val != 43.0, true );
		EXPECT_EQ( int128_val < 43.0, true );
		EXPECT_EQ( int128_val > 41.0, true );
	}

	//----------------------------------------------
	// Comparison with nfx Decimal
	//----------------------------------------------

	TEST( Int128DecimalComparison, EqualityComparison )
	{
		// Test equality with positive values
		datatypes::Int128 i1{ 42 };
		datatypes::Decimal d1{ 42 };
		EXPECT_TRUE( i1 == d1 );
		EXPECT_FALSE( i1 != d1 );

		// Test equality with negative values
		datatypes::Int128 i2{ -123 };
		datatypes::Decimal d2{ -123 };
		EXPECT_TRUE( i2 == d2 );
		EXPECT_FALSE( i2 != d2 );

		// Test equality with zero
		datatypes::Int128 i3{ 0 };
		datatypes::Decimal d3{ 0 };
		EXPECT_TRUE( i3 == d3 );
		EXPECT_FALSE( i3 != d3 );

		// Test inequality with different values
		datatypes::Int128 i4{ 100 };
		datatypes::Decimal d4{ 200 };
		EXPECT_FALSE( i4 == d4 );
		EXPECT_TRUE( i4 != d4 );

		// Test inequality when Decimal has fractional part
		datatypes::Int128 i5{ 42 };
		datatypes::Decimal d5{ 42.5 };
		EXPECT_FALSE( i5 == d5 );
		EXPECT_TRUE( i5 != d5 );

		// Test inequality with different signs
		datatypes::Int128 i6{ 42 };
		datatypes::Decimal d6{ -42 };
		EXPECT_FALSE( i6 == d6 );
		EXPECT_TRUE( i6 != d6 );
	}

	TEST( Int128DecimalComparison, OrderingComparison )
	{
		// Test less than with positive values
		datatypes::Int128 i1{ 100 };
		datatypes::Decimal d1{ 200 };
		EXPECT_TRUE( i1 < d1 );
		EXPECT_TRUE( i1 <= d1 );
		EXPECT_FALSE( i1 > d1 );
		EXPECT_FALSE( i1 >= d1 );

		// Test greater than with positive values
		datatypes::Int128 i2{ 300 };
		datatypes::Decimal d2{ 200 };
		EXPECT_FALSE( i2 < d2 );
		EXPECT_FALSE( i2 <= d2 );
		EXPECT_TRUE( i2 > d2 );
		EXPECT_TRUE( i2 >= d2 );

		// Test equal values
		datatypes::Int128 i3{ 150 };
		datatypes::Decimal d3{ 150 };
		EXPECT_FALSE( i3 < d3 );
		EXPECT_TRUE( i3 <= d3 );
		EXPECT_FALSE( i3 > d3 );
		EXPECT_TRUE( i3 >= d3 );

		// Test with negative vs positive
		datatypes::Int128 i4{ -50 };
		datatypes::Decimal d4{ 50 };
		EXPECT_TRUE( i4 < d4 );
		EXPECT_TRUE( i4 <= d4 );
		EXPECT_FALSE( i4 > d4 );
		EXPECT_FALSE( i4 >= d4 );

		// Test with positive vs negative
		datatypes::Int128 i5{ 50 };
		datatypes::Decimal d5{ -50 };
		EXPECT_FALSE( i5 < d5 );
		EXPECT_FALSE( i5 <= d5 );
		EXPECT_TRUE( i5 > d5 );
		EXPECT_TRUE( i5 >= d5 );
	}

	TEST( Int128DecimalComparison, NegativeValueComparison )
	{
		// Test negative vs negative - less negative is greater
		datatypes::Int128 i1{ -100 };
		datatypes::Decimal d1{ -200 };
		EXPECT_FALSE( i1 < d1 );
		EXPECT_FALSE( i1 <= d1 );
		EXPECT_TRUE( i1 > d1 );
		EXPECT_TRUE( i1 >= d1 );

		// Test negative vs negative - more negative is smaller
		datatypes::Int128 i2{ -300 };
		datatypes::Decimal d2{ -200 };
		EXPECT_TRUE( i2 < d2 );
		EXPECT_TRUE( i2 <= d2 );
		EXPECT_FALSE( i2 > d2 );
		EXPECT_FALSE( i2 >= d2 );

		// Test equal negative values
		datatypes::Int128 i3{ -150 };
		datatypes::Decimal d3{ -150 };
		EXPECT_FALSE( i3 < d3 );
		EXPECT_TRUE( i3 <= d3 );
		EXPECT_FALSE( i3 > d3 );
		EXPECT_TRUE( i3 >= d3 );
	}

	TEST( Int128DecimalComparison, FractionalPartHandling )
	{
		// Integer vs decimal with fractional part
		datatypes::Int128 i1{ 42 };
		datatypes::Decimal d1{ 42.7 };
		EXPECT_FALSE( i1 == d1 ); // Not equal due to fractional part
		EXPECT_TRUE( i1 != d1 );
		EXPECT_TRUE( i1 < d1 ); // 42 < 42.7
		EXPECT_TRUE( i1 <= d1 );
		EXPECT_FALSE( i1 > d1 );
		EXPECT_FALSE( i1 >= d1 );

		// Integer vs decimal with fractional part (next integer)
		datatypes::Int128 i2{ 43 };
		datatypes::Decimal d2{ 42.3 };
		EXPECT_FALSE( i2 == d2 );
		EXPECT_TRUE( i2 != d2 );
		EXPECT_FALSE( i2 < d2 ); // 43 > 42.3
		EXPECT_FALSE( i2 <= d2 );
		EXPECT_TRUE( i2 > d2 );
		EXPECT_TRUE( i2 >= d2 );

		// Negative integer vs negative decimal with fractional part
		datatypes::Int128 i3{ -42 };
		datatypes::Decimal d3{ -42.3 };
		EXPECT_FALSE( i3 == d3 );
		EXPECT_TRUE( i3 != d3 );
		EXPECT_FALSE( i3 < d3 ); // -42 > -42.3
		EXPECT_FALSE( i3 <= d3 );
		EXPECT_TRUE( i3 > d3 );
		EXPECT_TRUE( i3 >= d3 );

		// Negative integer vs more negative decimal with fractional part
		datatypes::Int128 i4{ -43 };
		datatypes::Decimal d4{ -42.7 };
		EXPECT_FALSE( i4 == d4 );
		EXPECT_TRUE( i4 != d4 );
		EXPECT_TRUE( i4 < d4 ); // -43 < -42.7
		EXPECT_TRUE( i4 <= d4 );
		EXPECT_FALSE( i4 > d4 );
		EXPECT_FALSE( i4 >= d4 );
	}

	TEST( Int128DecimalComparison, ZeroComparison )
	{
		// Zero integer vs positive decimal
		datatypes::Int128 i1{ 0 };
		datatypes::Decimal d1{ 0.001 };
		EXPECT_FALSE( i1 == d1 );
		EXPECT_TRUE( i1 != d1 );
		EXPECT_TRUE( i1 < d1 );
		EXPECT_TRUE( i1 <= d1 );
		EXPECT_FALSE( i1 > d1 );
		EXPECT_FALSE( i1 >= d1 );

		// Zero integer vs negative decimal
		datatypes::Int128 i2{ 0 };
		datatypes::Decimal d2{ -0.001 };
		EXPECT_FALSE( i2 == d2 );
		EXPECT_TRUE( i2 != d2 );
		EXPECT_FALSE( i2 < d2 );
		EXPECT_FALSE( i2 <= d2 );
		EXPECT_TRUE( i2 > d2 );
		EXPECT_TRUE( i2 >= d2 );

		// Positive integer vs zero decimal
		datatypes::Int128 i3{ 1 };
		datatypes::Decimal d3{ 0 };
		EXPECT_FALSE( i3 == d3 );
		EXPECT_TRUE( i3 != d3 );
		EXPECT_FALSE( i3 < d3 );
		EXPECT_FALSE( i3 <= d3 );
		EXPECT_TRUE( i3 > d3 );
		EXPECT_TRUE( i3 >= d3 );

		// Negative integer vs zero decimal
		datatypes::Int128 i4{ -1 };
		datatypes::Decimal d4{ 0 };
		EXPECT_FALSE( i4 == d4 );
		EXPECT_TRUE( i4 != d4 );
		EXPECT_TRUE( i4 < d4 );
		EXPECT_TRUE( i4 <= d4 );
		EXPECT_FALSE( i4 > d4 );
		EXPECT_FALSE( i4 >= d4 );
	}

	TEST( Int128DecimalComparison, LargeValueComparison )
	{
		// Test with large positive values (within both Int128 and Decimal limits)
		datatypes::Int128 i1{ "1234567890123456789012345678" };
		datatypes::Decimal d1{ "1234567890123456789012345678" };
		EXPECT_TRUE( i1 == d1 );
		EXPECT_FALSE( i1 != d1 );

		// Test with large negative values
		datatypes::Int128 i2{ "-1234567890123456789012345678" };
		datatypes::Decimal d2{ "-1234567890123456789012345678" };
		EXPECT_TRUE( i2 == d2 );
		EXPECT_FALSE( i2 != d2 );

		// Test ordering with large values
		datatypes::Int128 i3{ "1234567890123456789012345" };
		datatypes::Decimal d3{ "1234567890123456789012346" };
		EXPECT_FALSE( i3 == d3 );
		EXPECT_TRUE( i3 != d3 );
		EXPECT_TRUE( i3 < d3 );
		EXPECT_TRUE( i3 <= d3 );
		EXPECT_FALSE( i3 > d3 );
		EXPECT_FALSE( i3 >= d3 );
	}

	TEST( Int128DecimalComparison, PrecisionEdgeCases )
	{
		// Test integer vs decimal with meaningful fractional part (won't be normalized away)
		datatypes::Int128 i1{ 42 };
		datatypes::Decimal d1{ "42.1" };
		EXPECT_FALSE( i1 == d1 ); // Fractional part makes them not equal
		EXPECT_TRUE( i1 != d1 );
		EXPECT_TRUE( i1 < d1 );
		EXPECT_TRUE( i1 <= d1 );
		EXPECT_FALSE( i1 > d1 );
		EXPECT_FALSE( i1 >= d1 );

		// Test integer vs decimal that equals integer after normalization
		datatypes::Int128 i2{ 42 };
		datatypes::Decimal d2{ "42.0000" }; // Trailing zeros will be normalized away
		// This should be exactly equal after normalization
		EXPECT_TRUE( i2 == d2 );
		EXPECT_FALSE( i2 != d2 );

		// Test zero vs small but significant decimal
		datatypes::Int128 i3{ 0 };
		datatypes::Decimal d3{ "0.001" }; // Small but won't be normalized to zero
		EXPECT_FALSE( i3 == d3 );
		EXPECT_TRUE( i3 != d3 );
		EXPECT_TRUE( i3 < d3 );
		EXPECT_TRUE( i3 <= d3 );
		EXPECT_FALSE( i3 > d3 );
		EXPECT_FALSE( i3 >= d3 );
	}

	TEST( Int128DecimalComparison, ScalingComparison )
	{
		// Test that Int128 is properly scaled when comparing to Decimal with scale
		datatypes::Int128 i1{ 42 };
		datatypes::Decimal d1{ "420" }; // 10 times larger
		EXPECT_FALSE( i1 == d1 );
		EXPECT_TRUE( i1 != d1 );
		EXPECT_TRUE( i1 < d1 );
		EXPECT_TRUE( i1 <= d1 );
		EXPECT_FALSE( i1 > d1 );
		EXPECT_FALSE( i1 >= d1 );

		// Test with decimal having scale
		datatypes::Int128 i2{ 420 };
		datatypes::Decimal d2{ "42.0" }; // Same value, different representation
		EXPECT_FALSE( i2 == d2 );		 // 420 != 42.0
		EXPECT_TRUE( i2 != d2 );
		EXPECT_FALSE( i2 < d2 );
		EXPECT_FALSE( i2 <= d2 );
		EXPECT_TRUE( i2 > d2 );
		EXPECT_TRUE( i2 >= d2 );

		// Test with matching scaled values
		datatypes::Int128 i3{ 42 };
		datatypes::Decimal d3{ "4.2" };
		EXPECT_FALSE( i3 == d3 ); // 42 != 4.2
		EXPECT_TRUE( i3 != d3 );
		EXPECT_FALSE( i3 < d3 );
		EXPECT_FALSE( i3 <= d3 );
		EXPECT_TRUE( i3 > d3 );
		EXPECT_TRUE( i3 >= d3 );
	}

	TEST( Int128DecimalComparison, ConsistencyWithReversedOperands )
	{
		// Test that Decimal vs Int128 gives consistent results with Int128 vs Decimal
		// This tests the symmetry of the comparison operators

		datatypes::Int128 i1{ 100 };
		datatypes::Decimal d1{ 200 };

		// Test int128 < decimal vs decimal > int128
		EXPECT_TRUE( i1 < d1 );
		EXPECT_TRUE( d1 > i1 );

		// Test int128 > decimal vs decimal < int128
		datatypes::Int128 i2{ 300 };
		datatypes::Decimal d2{ 200 };
		EXPECT_TRUE( i2 > d2 );
		EXPECT_TRUE( d2 < i2 );

		// Test int128 == decimal vs decimal == int128
		datatypes::Int128 i3{ 150 };
		datatypes::Decimal d3{ 150 };
		EXPECT_TRUE( i3 == d3 );
		EXPECT_TRUE( d3 == i3 );

		// Test with fractional decimal (should not equal integer)
		datatypes::Int128 i4{ 42 };
		datatypes::Decimal d4{ 42.5 };
		EXPECT_FALSE( i4 == d4 );
		EXPECT_FALSE( d4 == i4 );
		EXPECT_TRUE( i4 < d4 );
		EXPECT_TRUE( d4 > i4 );
	}

	TEST( Int128DecimalComparison, EdgeCases )
	{
		// Test boundary values and edge cases within practical limits

		// Test with large values within Decimal's 28-digit precision limit
		datatypes::Int128 largeInt128{ "1234567890123456789012345678" }; // 28 digits
		datatypes::Decimal largeDecimal{ "1234567890123456789012345678" };
		EXPECT_TRUE( largeInt128 == largeDecimal );

		// Test with negative large values
		datatypes::Int128 largeNegInt128{ "-1234567890123456789012345678" };
		datatypes::Decimal largeNegDecimal{ "-1234567890123456789012345678" };
		EXPECT_TRUE( largeNegInt128 == largeNegDecimal );

		// Test comparison near large value boundaries
		datatypes::Int128 nearLarge{ "1234567890123456789012345677" };
		datatypes::Decimal largePlusOne{ "1234567890123456789012345678" };
		EXPECT_TRUE( nearLarge < largePlusOne );
		EXPECT_FALSE( nearLarge >= largePlusOne );

		// Test with meaningful decimal differences (won't be normalized away)
		datatypes::Int128 exactInt{ 1000000000 };
		datatypes::Decimal slightlyLarger{ "1000000000.5" }; // Clear fractional difference
		EXPECT_FALSE( exactInt == slightlyLarger );
		EXPECT_TRUE( exactInt < slightlyLarger );
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
