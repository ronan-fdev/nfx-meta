/**
 * @file TESTS_Decimal.cpp
 * @brief Comprehensive tests for cross-platform Decimal implementation
 * @details Validates Decimal compatibility and cross-platform behavior
 */

#include <limits>

#include <gtest/gtest.h>

#include <nfx/datatypes/Decimal.h>
#include <nfx/datatypes/Int128.h>

namespace nfx::datatypes::test
{
	//=====================================================================
	// Decimal type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( DecimalConstruction, DefaultConstruction )
	{
		datatypes::Decimal d;
		EXPECT_TRUE( d.isZero() );
		EXPECT_FALSE( d.isNegative() );
		EXPECT_EQ( d.scale(), 0 );
		EXPECT_EQ( d.toString(), "0" );
	}

	TEST( DecimalConstruction, IntegerConstruction )
	{
		// Positive integers
		datatypes::Decimal d1{ 42 };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		EXPECT_EQ( d1.toString(), "42" );

		// Negative integers
		datatypes::Decimal d2{ -123 };
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_EQ( d2.toString(), "-123" );

		// Zero
		datatypes::Decimal d3{ 0 };
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );
		EXPECT_EQ( d3.toString(), "0" );

		// Large integers
		datatypes::Decimal d4{ std::numeric_limits<std::int64_t>::max() };
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
	}

	TEST( DecimalConstruction, FloatConstruction )
	{
		// Simple positive float
		datatypes::Decimal d1{ 123.456f };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		// Note: Due to float precision, exact comparison may not work
		// We verify it's approximately correct
		EXPECT_TRUE( d1 > 123.0 );
		EXPECT_TRUE( d1 < 124.0 );

		// Negative float
		datatypes::Decimal d2{ -123.456f };
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_TRUE( d2 < -123.0 );
		EXPECT_TRUE( d2 > -124.0 );

		// Zero float
		datatypes::Decimal d3{ 0.0f };
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );

		// Small positive float
		datatypes::Decimal d4{ 0.001f };
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
		EXPECT_TRUE( d4 > 0.0 );
		EXPECT_TRUE( d4 < 0.01 );

		// Large float
		datatypes::Decimal d5{ 123456789.0f };
		EXPECT_FALSE( d5.isZero() );
		EXPECT_FALSE( d5.isNegative() );

		// Integer float (exact representation)
		datatypes::Decimal d6{ 42.0f };
		EXPECT_FALSE( d6.isZero() );
		EXPECT_FALSE( d6.isNegative() );
		EXPECT_EQ( d6.toString(), "42" ); // Should be exact for integer values

		// NaN should be handled (likely by setting to zero or throwing)
		datatypes::Decimal d9{ std::numeric_limits<float>::quiet_NaN() };

		// Very small float (near zero)
		datatypes::Decimal d10{ 1e-10f };
		EXPECT_FALSE( d10.isZero() );
		EXPECT_FALSE( d10.isNegative() );

		// Float precision edge case - values that can't be represented exactly
		float imprecise = 0.1f; // 0.1 cannot be represented exactly in float
		datatypes::Decimal d11{ imprecise };
		EXPECT_FALSE( d11.isZero() );
		EXPECT_FALSE( d11.isNegative() );
		// The result should be close to 0.1 but may have precision artifacts
	}

	TEST( DecimalConstruction, DoubleConstruction )
	{
		// Simple double
		datatypes::Decimal d1{ 123.456 };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );

		// Negative double
		datatypes::Decimal d2{ -123.456 };
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );

		// Zero double
		datatypes::Decimal d3{ 0.0 };
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );

		// Small Decimal
		datatypes::Decimal d4{ 0.001 };
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
	}

	TEST( DecimalConstruction, Int128Construction )
	{
		// Test construction from positive Int128
		datatypes::Int128 positive{ 42 };
		datatypes::Decimal d1{ positive };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		EXPECT_EQ( d1.toString(), "42" );
		EXPECT_EQ( d1.scale(), 0 ); // Integer values should have scale 0

		// Test construction from negative Int128
		datatypes::Int128 negative{ -123 };
		datatypes::Decimal d2{ negative };
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_EQ( d2.toString(), "-123" );
		EXPECT_EQ( d2.scale(), 0 );

		// Test construction from zero Int128
		datatypes::Int128 zero{ 0 };
		datatypes::Decimal d3{ zero };
		EXPECT_TRUE( d3.isZero() );
		EXPECT_FALSE( d3.isNegative() );
		EXPECT_EQ( d3.toString(), "0" );
		EXPECT_EQ( d3.scale(), 0 );

		// Test construction from large positive Int128
		datatypes::Int128 large{ "1234567890123456789" };
		datatypes::Decimal d4{ large };
		EXPECT_FALSE( d4.isZero() );
		EXPECT_FALSE( d4.isNegative() );
		EXPECT_EQ( d4.toString(), "1234567890123456789" );

		// Test construction from large negative Int128
		datatypes::Int128 largenegative{ "-9876543210987654321" };
		datatypes::Decimal d5{ largenegative };
		EXPECT_FALSE( d5.isZero() );
		EXPECT_TRUE( d5.isNegative() );
		EXPECT_EQ( d5.toString(), "-9876543210987654321" );

		// Test construction from maximum positive Int128 value - should clamp to Decimal max
		datatypes::Int128 maxInt128{ "170141183460469231731687303715884105727" }; // 2^127 - 1
		datatypes::Decimal d6{ maxInt128 };
		EXPECT_FALSE( d6.isZero() );
		EXPECT_FALSE( d6.isNegative() );
		// Should be clamped to maximum 96-bit value: 2^96 - 1 = 79228162514264337593543950335
		EXPECT_EQ( d6.toString(), "79228162514264337593543950335" );

		// Test construction from minimum negative Int128 value - should clamp to Decimal min
		datatypes::Int128 minInt128{ "-170141183460469231731687303715884105728" }; // -2^127
		datatypes::Decimal d7{ minInt128 };
		EXPECT_FALSE( d7.isZero() );
		EXPECT_TRUE( d7.isNegative() );
		// Should be clamped to minimum (negative of maximum 96-bit value)
		EXPECT_EQ( d7.toString(), "-79228162514264337593543950335" );

		// Test that the resulting Decimal can be converted back to Int128 for smaller values
		datatypes::Int128 original{ 98765 };
		datatypes::Decimal converted{ original };
		datatypes::Int128 backConverted{ converted }; // This should work for integer Decimals
		EXPECT_TRUE( original == backConverted );

		// Test bidirectional conversion consistency for zero
		datatypes::Int128 zeroOriginal{ 0 };
		datatypes::Decimal zeroConverted{ zeroOriginal };
		datatypes::Int128 zeroBackConverted{ zeroConverted };
		EXPECT_TRUE( zeroOriginal == zeroBackConverted );
		EXPECT_TRUE( zeroConverted.isZero() );

		// Test bidirectional conversion consistency for negative values
		datatypes::Int128 negativeOriginal{ -54321 };
		datatypes::Decimal negativeConverted{ negativeOriginal };
		datatypes::Int128 negativeBackConverted{ negativeConverted };
		EXPECT_TRUE( negativeOriginal == negativeBackConverted );
		EXPECT_TRUE( negativeConverted.isNegative() );

		// Test edge case: values at the boundary of Decimal's capacity
		// Maximum value that fits exactly in Decimal (2^96 - 1)
		datatypes::Int128 decimalMax{ "79228162514264337593543950335" };
		datatypes::Decimal d8{ decimalMax };
		EXPECT_FALSE( d8.isZero() );
		EXPECT_FALSE( d8.isNegative() );
		EXPECT_EQ( d8.toString(), "79228162514264337593543950335" );

		// Minimum value that fits exactly in Decimal (-(2^96 - 1))
		datatypes::Int128 decimalMin{ "-79228162514264337593543950335" };
		datatypes::Decimal d9{ decimalMin };
		EXPECT_FALSE( d9.isZero() );
		EXPECT_TRUE( d9.isNegative() );
		EXPECT_EQ( d9.toString(), "-79228162514264337593543950335" );

		// Value just over Decimal's capacity - should be clamped
		datatypes::Int128 justOverMax{ "79228162514264337593543950336" }; // 2^96
		datatypes::Decimal d10{ justOverMax };
		EXPECT_FALSE( d10.isZero() );
		EXPECT_FALSE( d10.isNegative() );
		EXPECT_EQ( d10.toString(), "79228162514264337593543950335" ); // Clamped to max
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	TEST( DecimalAssignment, CompoundAssignment )
	{
		datatypes::Decimal d1{ "100" };
		datatypes::Decimal d2{ "25" };

		// Addition assignment
		d1 += d2;
		EXPECT_FALSE( d1.isZero() );
		// Should be 125

		// Subtraction assignment
		d1 -= datatypes::Decimal{ "25" };
		// Should be back to 100

		// Multiplication assignment
		d1 *= datatypes::Decimal{ "2" };
		// Should be 200

		// Division assignment
		d1 /= datatypes::Decimal{ "4" };

		// Should be 50
		EXPECT_EQ( d1.toString(), "50" );
		EXPECT_FALSE( d1.isZero() );
	}

	TEST( DecimalAssignment, UnaryMinus )
	{
		datatypes::Decimal d1{ "123.45" };
		datatypes::Decimal d2{ -d1 };

		EXPECT_FALSE( d1.isNegative() );
		EXPECT_TRUE( d2.isNegative() );
		EXPECT_EQ( d1.toString(), "123.45" );

		// Double negation
		datatypes::Decimal d3{ -d2 };
		EXPECT_FALSE( d3.isNegative() );
		EXPECT_EQ( d3.toString(), "123.45" );
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	TEST( DecimalConversion, ToDouble )
	{
		datatypes::Decimal d1{ 123.456 };
		double converted{ d1.toDouble() };
		EXPECT_NEAR( converted, 123.456, 0.001 );

		datatypes::Decimal d2{ -789.123 };
		converted = d2.toDouble();
		EXPECT_NEAR( converted, -789.123, 0.001 );

		datatypes::Decimal d3{ 0 };
		converted = d3.toDouble();
		EXPECT_EQ( converted, 0.0 );
	}

	TEST( DecimalConversion, ToString )
	{
		// Basic cases
		EXPECT_EQ( datatypes::Decimal{ 123 }.toString(), "123" );
		EXPECT_EQ( datatypes::Decimal{ -456 }.toString(), "-456" );
		EXPECT_EQ( datatypes::Decimal{ 0 }.toString(), "0" );

		// Decimal cases
		datatypes::Decimal d1{ "123.456" };
		EXPECT_EQ( d1.toString(), "123.456" );

		datatypes::Decimal d2{ "-0.001" };
		EXPECT_EQ( d2.toString(), "-0.001" );
	}

	TEST( DecimalConversion, ToBits )
	{
		// Test zero value
		datatypes::Decimal zero{ 0 };
		auto zeroBits{ zero.toBits() };
		EXPECT_EQ( zeroBits.size(), 4 );
		EXPECT_EQ( zeroBits[0], 0 ); // Low mantissa
		EXPECT_EQ( zeroBits[1], 0 ); // Mid mantissa
		EXPECT_EQ( zeroBits[2], 0 ); // High mantissa
		EXPECT_EQ( zeroBits[3], 0 ); // Flags (scale=0, positive)

		// Test simple positive integer
		datatypes::Decimal simpleInt{ 123 };
		auto intBits{ simpleInt.toBits() };
		EXPECT_EQ( intBits[0], 123 ); // Low mantissa should contain 123
		EXPECT_EQ( intBits[1], 0 );	  // Mid mantissa should be 0
		EXPECT_EQ( intBits[2], 0 );	  // High mantissa should be 0
		EXPECT_EQ( intBits[3], 0 );	  // Flags: scale=0, positive

		// Test positive decimal with scale
		datatypes::Decimal decimal{ "123.45" };
		auto decimalBits{ decimal.toBits() };
		EXPECT_EQ( decimalBits[0], 12345 ); // Mantissa should be 12345 (123.45 * 10^2)
		EXPECT_EQ( decimalBits[1], 0 );		// Mid mantissa should be 0
		EXPECT_EQ( decimalBits[2], 0 );		// High mantissa should be 0
		// Flags should contain scale=2, positive (scale in bits 16-23)
		EXPECT_EQ( ( decimalBits[3] >> 16 ) & 0xFF, 2 ); // Scale should be 2

		// Test negative value
		datatypes::Decimal negative{ "-456.789" };
		auto negativeBits{ negative.toBits() };
		EXPECT_EQ( negativeBits[0], 456789 ); // Mantissa should be 456789
		EXPECT_EQ( negativeBits[1], 0 );	  // Mid mantissa should be 0
		EXPECT_EQ( negativeBits[2], 0 );	  // High mantissa should be 0
		// Sign bit (bit 31) should be set in flags
		EXPECT_NE( static_cast<std::uint32_t>( negativeBits[3] ) & constants::decimal::SIGN_MASK, 0u );

		// Scale should be 3
		EXPECT_EQ( ( negativeBits[3] >> 16 ) & 0xFF, 3u );

		// Test large value requiring multiple mantissa words
		datatypes::Decimal large{ "123456789012345.678" };
		auto largeBits{ large.toBits() };
		EXPECT_NE( largeBits[0], 0 ); // Low mantissa should be non-zero
		// For large numbers, we might use higher mantissa words
		// The exact values depend on internal representation

		// Test very small decimal
		datatypes::Decimal small{ "0.001" };
		auto smallBits{ small.toBits() };
		EXPECT_EQ( smallBits[0], 1 ); // Mantissa should be 1 (0.001 * 10^3)
		EXPECT_EQ( smallBits[1], 0 ); // Mid mantissa should be 0
		EXPECT_EQ( smallBits[2], 0 ); // High mantissa should be 0
		// Scale should be 3
		EXPECT_EQ( ( smallBits[3] >> 16 ) & 0xFF, 3u );
		// Should be positive
		EXPECT_EQ( static_cast<std::uint32_t>( smallBits[3] ) & constants::decimal::SIGN_MASK, 0u );
	}

	//----------------------------------------------
	// Arithmetic
	//----------------------------------------------

	TEST( DecimalArithmetic, Addition )
	{
		datatypes::Decimal d1{ "123.45" };
		datatypes::Decimal d2{ "67.89" };
		auto result{ d1 + d2 };

		// Should be close to 191.34
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Test with different signs
		datatypes::Decimal d3{ "100" };
		datatypes::Decimal d4{ "-50" };
		result = d3 + d4;
		// Should be 50
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Test adding zero
		result = d1 + datatypes::Decimal{ 0 };
		EXPECT_EQ( result.toString(), d1.toString() );
	}

	TEST( DecimalArithmetic, Subtraction )
	{
		datatypes::Decimal d1{ "100" };
		datatypes::Decimal d2{ "30" };
		datatypes::Decimal result{ d1 - d2 };

		// Should be 70
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Test negative result
		result = d2 - d1;
		// Should be -70
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		// Test subtracting self
		result = d1 - d1;
		EXPECT_TRUE( result.isZero() );
	}

	TEST( DecimalArithmetic, Multiplication )
	{
		datatypes::Decimal d1{ "12.5" };
		datatypes::Decimal d2{ "8" };
		datatypes::Decimal result{ d1 * d2 };

		// Should be 100
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Test with negative
		datatypes::Decimal d3{ "-5" };
		result = d1 * d3;
		// Should be -62.5
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		// Test multiply by zero
		result = d1 * datatypes::Decimal{ 0 };
		EXPECT_TRUE( result.isZero() );
	}

	TEST( DecimalArithmetic, Division )
	{
		datatypes::Decimal d1{ "100" };
		datatypes::Decimal d2{ "4" };
		datatypes::Decimal result{ d1 / d2 };

		// Should be 25
		EXPECT_FALSE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Test with negative
		datatypes::Decimal d3{ "-20" };
		result = d1 / d3;
		// Should be -5
		EXPECT_FALSE( result.isZero() );
		EXPECT_TRUE( result.isNegative() );

		// Test division by zero
		EXPECT_THROW( d1 / datatypes::Decimal{ 0 }, std::overflow_error );
	}

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	TEST( DecimalComparison, Equality )
	{
		datatypes::Decimal d1{ "123.45" };
		datatypes::Decimal d2{ "123.45" };
		datatypes::Decimal d3{ "123.46" };

		EXPECT_TRUE( d1 == d2 );
		EXPECT_FALSE( d1 == d3 );
		EXPECT_FALSE( d1 != d2 );
		EXPECT_TRUE( d1 != d3 );

		// Test with zero
		datatypes::Decimal zero1{ 0 };
		datatypes::Decimal zero2{ "0.0" };
		EXPECT_TRUE( zero1 == zero2 );
	}

	TEST( DecimalComparison, Comparison )
	{
		datatypes::Decimal d1{ "100" };
		datatypes::Decimal d2{ "200" };
		datatypes::Decimal d3{ "-50" };

		// Less than
		EXPECT_TRUE( d1 < d2 );
		EXPECT_FALSE( d2 < d1 );
		EXPECT_TRUE( d3 < d1 );

		// Greater than
		EXPECT_TRUE( d2 > d1 );
		EXPECT_FALSE( d1 > d2 );
		EXPECT_TRUE( d1 > d3 );

		// Less than or equal
		EXPECT_TRUE( d1 <= d2 );
		EXPECT_TRUE( d1 <= datatypes::Decimal{ "100" } );
		EXPECT_FALSE( d2 <= d1 );

		// Greater than or equal
		EXPECT_TRUE( d2 >= d1 );
		EXPECT_TRUE( d1 >= datatypes::Decimal{ "100" } );
		EXPECT_FALSE( d1 >= d2 );
	}

	//----------------------------------------------
	// Comparison with built-in floating-point types
	//----------------------------------------------

	TEST( DecimalBuiltinComparison, FloatingPointComparison )
	{
		datatypes::Decimal d1{ 123.456 };
		datatypes::Decimal d2{ -123.456 };
		datatypes::Decimal zero{};

		// Test equality with double
		EXPECT_TRUE( d1 == 123.456 );
		EXPECT_TRUE( d2 == -123.456 );
		EXPECT_TRUE( zero == 0.0 );
		EXPECT_FALSE( d1 == 123.457 );

		// Test inequality with double
		EXPECT_FALSE( d1 != 123.456 );
		EXPECT_TRUE( d1 != 123.457 );
		EXPECT_TRUE( d2 != 123.456 );

		// Test less than with double
		EXPECT_TRUE( d1 < 123.457 );
		EXPECT_FALSE( d1 < 123.456 );
		EXPECT_FALSE( d1 < 123.455 );
		EXPECT_TRUE( d2 < 0.0 );

		// Test less than or equal with double
		EXPECT_TRUE( d1 <= 123.457 );
		EXPECT_TRUE( d1 <= 123.456 );
		EXPECT_FALSE( d1 <= 123.455 );

		// Test greater than with double
		EXPECT_FALSE( d1 > 123.457 );
		EXPECT_FALSE( d1 > 123.456 );
		EXPECT_TRUE( d1 > 123.455 );
		EXPECT_FALSE( d2 > 0.0 );

		// Test greater than or equal with double
		EXPECT_FALSE( d1 >= 123.457 );
		EXPECT_TRUE( d1 >= 123.456 );
		EXPECT_TRUE( d1 >= 123.455 );

		// Test with float - demonstrates IEEE 754 precision limitations
		// 123.456 cannot be represented exactly in either float or double
		float test_float = 123.456f; // Stored as ~123.45600128... (float precision)
		// double is ~123.45600000... (double precision) - different from float!

		// When Decimal is constructed from the float's double-cast value,
		// it should compare equal to that same float value
		datatypes::Decimal d_from_float{ static_cast<double>( test_float ) };
		EXPECT_TRUE( d_from_float == test_float ); // Same precision source

		// But a Decimal from exact double won't equal the float due to different precision errors
		datatypes::Decimal d_from_double{ 123.456 }; // From double literal
		EXPECT_FALSE( d_from_double == test_float ); // Different precision errors

		// Test ordering with float values
		EXPECT_TRUE( d1 < 123.457f );
		EXPECT_TRUE( d1 > 123.455f );
	}

	TEST( DecimalBuiltinComparison, FloatingPointSpecialValues )
	{
		datatypes::Decimal d{ 123.456 };

		// Test with NaN - should always return false for equality, no ordering
		double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_FALSE( d == nan );
		EXPECT_TRUE( d != nan );
		EXPECT_FALSE( d < nan );
		EXPECT_FALSE( d <= nan );
		EXPECT_FALSE( d > nan );
		EXPECT_FALSE( d >= nan );
	}

	TEST( DecimalBuiltinComparison, FloatingPointPrecisionBehavior )
	{
		// Test that demonstrates IEEE 754 floating-point precision limitations

		// Values that can be represented exactly in float
		datatypes::Decimal d_exact{ 123.5 }; // 123.5 is exactly representable
		EXPECT_TRUE( d_exact == 123.5f );
		EXPECT_TRUE( d_exact == 123.5 );

		// Values that cannot be represented exactly in float
		float imprecise_float = 123.456f;
		double imprecise_double = 123.456;

		// The float and double have different precision errors
		EXPECT_FALSE( static_cast<double>( imprecise_float ) == imprecise_double );

		// But each Decimal compares equal to its source type due to consistent precision handling
		datatypes::Decimal d_from_float{ static_cast<double>( imprecise_float ) };
		datatypes::Decimal d_from_double{ imprecise_double };

		EXPECT_TRUE( d_from_float == imprecise_float );	  // Consistent with float precision
		EXPECT_TRUE( d_from_double == imprecise_double ); // Consistent with double precision

		// For 123.456, float and double typically have different precision errors,
		// but we verify this rather than assume it
		if ( static_cast<double>( imprecise_float ) != imprecise_double )
		{
			EXPECT_FALSE( d_from_float == d_from_double ); // Different precision sources
		}

		// This behavior matches IEEE 754 semantics and is mathematically correct
	}

	//----------------------------------------------
	// Comparison with built-in integer types
	//----------------------------------------------

	TEST( DecimalBuiltinComparison, SignedIntegerComparison )
	{
		datatypes::Decimal d1{ 42 };
		datatypes::Decimal d2{ -42 };
		datatypes::Decimal d3{ 42.5 };
		datatypes::Decimal zero{};

		// Test equality with int64_t
		EXPECT_TRUE( d1 == std::int64_t{ 42 } );
		EXPECT_TRUE( d2 == std::int64_t{ -42 } );
		EXPECT_TRUE( zero == std::int64_t{ 0 } );
		EXPECT_FALSE( d1 == std::int64_t{ 43 } );
		EXPECT_FALSE( d3 == std::int64_t{ 42 } ); // Fractional part means not equal to integer

		// Test inequality with int64_t
		EXPECT_FALSE( d1 != std::int64_t{ 42 } );
		EXPECT_TRUE( d1 != std::int64_t{ 43 } );
		EXPECT_TRUE( d3 != std::int64_t{ 42 } );

		// Test less than with int64_t
		EXPECT_TRUE( d1 < std::int64_t{ 43 } );
		EXPECT_FALSE( d1 < std::int64_t{ 42 } );
		EXPECT_FALSE( d1 < std::int64_t{ 41 } );
		EXPECT_TRUE( d2 < std::int64_t{ 0 } );

		// Test less than or equal with int64_t
		EXPECT_TRUE( d1 <= std::int64_t{ 43 } );
		EXPECT_TRUE( d1 <= std::int64_t{ 42 } );
		EXPECT_FALSE( d1 <= std::int64_t{ 41 } );

		// Test greater than with int64_t
		EXPECT_FALSE( d1 > std::int64_t{ 43 } );
		EXPECT_FALSE( d1 > std::int64_t{ 42 } );
		EXPECT_TRUE( d1 > std::int64_t{ 41 } );
		EXPECT_FALSE( d2 > std::int64_t{ 0 } );

		// Test greater than or equal with int64_t
		EXPECT_FALSE( d1 >= std::int64_t{ 43 } );
		EXPECT_TRUE( d1 >= std::int64_t{ 42 } );
		EXPECT_TRUE( d1 >= std::int64_t{ 41 } );

		// Test with int32_t (should delegate to int64_t)
		EXPECT_TRUE( d1 == 42 );
		EXPECT_TRUE( d1 < 43 );
		EXPECT_TRUE( d1 > 41 );
	}

	TEST( DecimalBuiltinComparison, UnsignedIntegerComparison )
	{
		datatypes::Decimal d1{ 42 };
		datatypes::Decimal d2{ -42 };
		datatypes::Decimal d3{ 42.5 };
		datatypes::Decimal zero{};

		// Test equality with uint64_t
		EXPECT_TRUE( d1 == std::uint64_t{ 42 } );
		EXPECT_TRUE( zero == std::uint64_t{ 0 } );
		EXPECT_FALSE( d1 == std::uint64_t{ 43 } );
		EXPECT_FALSE( d2 == std::uint64_t{ 42 } ); // Negative decimal can't equal positive uint64_t
		EXPECT_FALSE( d3 == std::uint64_t{ 42 } ); // Fractional part means not equal to integer

		// Test inequality with uint64_t
		EXPECT_FALSE( d1 != std::uint64_t{ 42 } );
		EXPECT_TRUE( d1 != std::uint64_t{ 43 } );
		EXPECT_TRUE( d2 != std::uint64_t{ 42 } );
		EXPECT_TRUE( d3 != std::uint64_t{ 42 } );

		// Test less than with uint64_t
		EXPECT_TRUE( d1 < std::uint64_t{ 43 } );
		EXPECT_FALSE( d1 < std::uint64_t{ 42 } );
		EXPECT_FALSE( d1 < std::uint64_t{ 41 } );
		EXPECT_TRUE( d2 < std::uint64_t{ 42 } ); // Negative is always less than positive uint64_t

		// Test less than or equal with uint64_t
		EXPECT_TRUE( d1 <= std::uint64_t{ 43 } );
		EXPECT_TRUE( d1 <= std::uint64_t{ 42 } );
		EXPECT_FALSE( d1 <= std::uint64_t{ 41 } );
		EXPECT_TRUE( d2 <= std::uint64_t{ 42 } );

		// Test greater than with uint64_t
		EXPECT_FALSE( d1 > std::uint64_t{ 43 } );
		EXPECT_FALSE( d1 > std::uint64_t{ 42 } );
		EXPECT_TRUE( d1 > std::uint64_t{ 41 } );
		EXPECT_FALSE( d2 > std::uint64_t{ 42 } ); // Negative is never greater than positive uint64_t

		// Test greater than or equal with uint64_t
		EXPECT_FALSE( d1 >= std::uint64_t{ 43 } );
		EXPECT_TRUE( d1 >= std::uint64_t{ 42 } );
		EXPECT_TRUE( d1 >= std::uint64_t{ 41 } );
		EXPECT_FALSE( d2 >= std::uint64_t{ 42 } );
	}

	TEST( DecimalBuiltinComparison, ComparisonEdgeCases )
	{
		datatypes::Decimal large{ "999999999999999999999999" };
		datatypes::Decimal small{ "0.000000000001" };
		datatypes::Decimal zero{};

		// Test with maximum values
		EXPECT_FALSE( large == std::numeric_limits<std::int64_t>::max() );
		EXPECT_TRUE( large > std::numeric_limits<std::int64_t>::max() );

		// Test with minimum values
		datatypes::Decimal negative_large{ "-999999999999999999999999" };
		EXPECT_FALSE( negative_large == std::numeric_limits<std::int64_t>::min() );
		EXPECT_TRUE( negative_large < std::numeric_limits<std::int64_t>::min() );

		// Test very small values
		EXPECT_FALSE( small == 0 );
		EXPECT_TRUE( small > 0 );
		EXPECT_TRUE( small != std::uint64_t{ 0 } );
		EXPECT_FALSE( small == std::uint64_t{ 0 } );

		// Test zero comparisons
		EXPECT_TRUE( zero == 0 );
		EXPECT_TRUE( zero == std::int64_t{ 0 } );
		EXPECT_TRUE( zero == std::uint64_t{ 0 } );
		EXPECT_TRUE( zero == 0.0 );
		EXPECT_TRUE( zero == 0.0f );
	}

	TEST( DecimalBuiltinComparison, ComparisonSymmetry )
	{
		datatypes::Decimal d{ 42.5 };

		// Test that comparison operators work consistently
		// For floating-point comparisons
		EXPECT_EQ( d == 42.5, true );
		EXPECT_EQ( d != 42.6, true );
		EXPECT_EQ( d < 42.6, true );
		EXPECT_EQ( d <= 42.5, true );
		EXPECT_EQ( d > 42.4, true );
		EXPECT_EQ( d >= 42.5, true );

		// For integer comparisons with fractional decimal
		EXPECT_EQ( d == 42, false ); // Has fractional part
		EXPECT_EQ( d != 42, true );
		EXPECT_EQ( d > 42, true );
		EXPECT_EQ( d >= 42, true );
		EXPECT_EQ( d < 43, true );
		EXPECT_EQ( d <= 43, true );
	}

	//----------------------------------------------
	// Comparison with nfx Int128
	//----------------------------------------------

	TEST( DecimalInt128Comparison, EqualityComparison )
	{
		// Test equality with positive values
		datatypes::Decimal d1{ 42 };
		datatypes::Int128 i1{ 42 };
		EXPECT_TRUE( d1 == i1 );
		EXPECT_FALSE( d1 != i1 );

		// Test equality with negative values
		datatypes::Decimal d2{ -123 };
		datatypes::Int128 i2{ -123 };
		EXPECT_TRUE( d2 == i2 );
		EXPECT_FALSE( d2 != i2 );

		// Test equality with zero
		datatypes::Decimal d3{ 0 };
		datatypes::Int128 i3{ 0 };
		EXPECT_TRUE( d3 == i3 );
		EXPECT_FALSE( d3 != i3 );

		// Test inequality with different values
		datatypes::Decimal d4{ 100 };
		datatypes::Int128 i4{ 200 };
		EXPECT_FALSE( d4 == i4 );
		EXPECT_TRUE( d4 != i4 );

		// Test inequality when Decimal has fractional part
		datatypes::Decimal d5{ 42.5 };
		datatypes::Int128 i5{ 42 };
		EXPECT_FALSE( d5 == i5 );
		EXPECT_TRUE( d5 != i5 );

		// Test inequality with different signs
		datatypes::Decimal d6{ 42 };
		datatypes::Int128 i6{ -42 };
		EXPECT_FALSE( d6 == i6 );
		EXPECT_TRUE( d6 != i6 );
	}

	TEST( DecimalInt128Comparison, OrderingComparison )
	{
		// Test less than with positive values
		datatypes::Decimal d1{ 100 };
		datatypes::Int128 i1{ 200 };
		EXPECT_TRUE( d1 < i1 );
		EXPECT_TRUE( d1 <= i1 );
		EXPECT_FALSE( d1 > i1 );
		EXPECT_FALSE( d1 >= i1 );

		// Test greater than with positive values
		datatypes::Decimal d2{ 300 };
		datatypes::Int128 i2{ 200 };
		EXPECT_FALSE( d2 < i2 );
		EXPECT_FALSE( d2 <= i2 );
		EXPECT_TRUE( d2 > i2 );
		EXPECT_TRUE( d2 >= i2 );

		// Test equal values
		datatypes::Decimal d3{ 150 };
		datatypes::Int128 i3{ 150 };
		EXPECT_FALSE( d3 < i3 );
		EXPECT_TRUE( d3 <= i3 );
		EXPECT_FALSE( d3 > i3 );
		EXPECT_TRUE( d3 >= i3 );

		// Test with negative vs positive
		datatypes::Decimal d4{ -50 };
		datatypes::Int128 i4{ 50 };
		EXPECT_TRUE( d4 < i4 );
		EXPECT_TRUE( d4 <= i4 );
		EXPECT_FALSE( d4 > i4 );
		EXPECT_FALSE( d4 >= i4 );

		// Test with positive vs negative
		datatypes::Decimal d5{ 50 };
		datatypes::Int128 i5{ -50 };
		EXPECT_FALSE( d5 < i5 );
		EXPECT_FALSE( d5 <= i5 );
		EXPECT_TRUE( d5 > i5 );
		EXPECT_TRUE( d5 >= i5 );
	}

	TEST( DecimalInt128Comparison, NegativeValueComparison )
	{
		// Test negative vs negative - less negative is greater
		datatypes::Decimal d1{ -100 };
		datatypes::Int128 i1{ -200 };
		EXPECT_FALSE( d1 < i1 );
		EXPECT_FALSE( d1 <= i1 );
		EXPECT_TRUE( d1 > i1 );
		EXPECT_TRUE( d1 >= i1 );

		// Test negative vs negative - more negative is smaller
		datatypes::Decimal d2{ -300 };
		datatypes::Int128 i2{ -200 };
		EXPECT_TRUE( d2 < i2 );
		EXPECT_TRUE( d2 <= i2 );
		EXPECT_FALSE( d2 > i2 );
		EXPECT_FALSE( d2 >= i2 );

		// Test equal negative values
		datatypes::Decimal d3{ -150 };
		datatypes::Int128 i3{ -150 };
		EXPECT_FALSE( d3 < i3 );
		EXPECT_TRUE( d3 <= i3 );
		EXPECT_FALSE( d3 > i3 );
		EXPECT_TRUE( d3 >= i3 );
	}

	TEST( DecimalInt128Comparison, FractionalPartHandling )
	{
		// Decimal with fractional part vs integer
		datatypes::Decimal d1{ 42.7 };
		datatypes::Int128 i1{ 42 };
		EXPECT_FALSE( d1 == i1 ); // Not equal due to fractional part
		EXPECT_TRUE( d1 != i1 );
		EXPECT_FALSE( d1 < i1 );
		EXPECT_FALSE( d1 <= i1 );
		EXPECT_TRUE( d1 > i1 );
		EXPECT_TRUE( d1 >= i1 );

		// Decimal with fractional part vs next integer
		datatypes::Decimal d2{ 42.3 };
		datatypes::Int128 i2{ 43 };
		EXPECT_FALSE( d2 == i2 );
		EXPECT_TRUE( d2 != i2 );
		EXPECT_TRUE( d2 < i2 );
		EXPECT_TRUE( d2 <= i2 );
		EXPECT_FALSE( d2 > i2 );
		EXPECT_FALSE( d2 >= i2 );

		// Negative decimal with fractional part
		datatypes::Decimal d3{ -42.3 };
		datatypes::Int128 i3{ -42 };
		EXPECT_FALSE( d3 == i3 );
		EXPECT_TRUE( d3 != i3 );
		EXPECT_TRUE( d3 < i3 ); // -42.3 < -42
		EXPECT_TRUE( d3 <= i3 );
		EXPECT_FALSE( d3 > i3 );
		EXPECT_FALSE( d3 >= i3 );

		// Negative decimal with fractional part vs more negative integer
		datatypes::Decimal d4{ -42.7 };
		datatypes::Int128 i4{ -43 };
		EXPECT_FALSE( d4 == i4 );
		EXPECT_TRUE( d4 != i4 );
		EXPECT_FALSE( d4 < i4 ); // -42.7 > -43
		EXPECT_FALSE( d4 <= i4 );
		EXPECT_TRUE( d4 > i4 );
		EXPECT_TRUE( d4 >= i4 );
	}

	TEST( DecimalInt128Comparison, ZeroComparison )
	{
		// Positive decimal vs zero
		datatypes::Decimal d1{ 0.001 };
		datatypes::Int128 i1{ 0 };
		EXPECT_FALSE( d1 == i1 );
		EXPECT_TRUE( d1 != i1 );
		EXPECT_FALSE( d1 < i1 );
		EXPECT_FALSE( d1 <= i1 );
		EXPECT_TRUE( d1 > i1 );
		EXPECT_TRUE( d1 >= i1 );

		// Negative decimal vs zero
		datatypes::Decimal d2{ -0.001 };
		datatypes::Int128 i2{ 0 };
		EXPECT_FALSE( d2 == i2 );
		EXPECT_TRUE( d2 != i2 );
		EXPECT_TRUE( d2 < i2 );
		EXPECT_TRUE( d2 <= i2 );
		EXPECT_FALSE( d2 > i2 );
		EXPECT_FALSE( d2 >= i2 );

		// Zero decimal vs positive integer
		datatypes::Decimal d3{ 0 };
		datatypes::Int128 i3{ 1 };
		EXPECT_FALSE( d3 == i3 );
		EXPECT_TRUE( d3 != i3 );
		EXPECT_TRUE( d3 < i3 );
		EXPECT_TRUE( d3 <= i3 );
		EXPECT_FALSE( d3 > i3 );
		EXPECT_FALSE( d3 >= i3 );

		// Zero decimal vs negative integer
		datatypes::Decimal d4{ 0 };
		datatypes::Int128 i4{ -1 };
		EXPECT_FALSE( d4 == i4 );
		EXPECT_TRUE( d4 != i4 );
		EXPECT_FALSE( d4 < i4 );
		EXPECT_FALSE( d4 <= i4 );
		EXPECT_TRUE( d4 > i4 );
		EXPECT_TRUE( d4 >= i4 );
	}

	TEST( DecimalInt128Comparison, LargeValueComparison )
	{
		// Test with large positive values (within Decimal's 28-digit precision)
		datatypes::Decimal d1{ "1234567890123456789012345678" }; // 28 digits max
		datatypes::Int128 i1{ "1234567890123456789012345678" };
		EXPECT_TRUE( d1 == i1 );
		EXPECT_FALSE( d1 != i1 );

		// Test with large negative values (within Decimal's 28-digit precision)
		datatypes::Decimal d2{ "-1234567890123456789012345678" };
		datatypes::Int128 i2{ "-1234567890123456789012345678" };
		EXPECT_TRUE( d2 == i2 );
		EXPECT_FALSE( d2 != i2 );

		// Test ordering with large values
		datatypes::Decimal d3{ "1234567890123456789012345677" };
		datatypes::Int128 i3{ "1234567890123456789012345678" };
		EXPECT_FALSE( d3 == i3 );
		EXPECT_TRUE( d3 != i3 );
		EXPECT_TRUE( d3 < i3 );
		EXPECT_TRUE( d3 <= i3 );
		EXPECT_FALSE( d3 > i3 );
		EXPECT_FALSE( d3 >= i3 );

		// Test with values approaching but within Decimal precision limits
		datatypes::Decimal d4{ "9876543210987654321098765432" }; // 28 digits, different pattern
		datatypes::Int128 i4{ "9876543210987654321098765432" };
		EXPECT_TRUE( d4 == i4 );
	}

	TEST( DecimalInt128Comparison, LargeValueWithFractionalComparison )
	{
		// Test with decimal having fractional part vs integer (should not be equal)
		datatypes::Decimal d1{ "12345678901234567890.8" }; // Has fractional part
		datatypes::Int128 i1{ "12345678901234567890" };	   // Integer part only
		EXPECT_FALSE( d1 == i1 );						   // Cannot be equal due to fractional part
		EXPECT_TRUE( d1 != i1 );
		EXPECT_FALSE( d1 < i1 ); // d1 > i1 because d1 has additional 0.8
		EXPECT_FALSE( d1 <= i1 );
		EXPECT_TRUE( d1 > i1 );
		EXPECT_TRUE( d1 >= i1 );

		// Test with negative decimal having fractional part vs integer
		datatypes::Decimal d2{ "-12345678901234567890.8" }; // More negative
		datatypes::Int128 i2{ "-12345678901234567890" };	// Less negative
		EXPECT_FALSE( d2 == i2 );
		EXPECT_TRUE( d2 != i2 );
		EXPECT_TRUE( d2 < i2 ); // d2 < i2 because d2 is more negative
		EXPECT_TRUE( d2 <= i2 );
		EXPECT_FALSE( d2 > i2 );
		EXPECT_FALSE( d2 >= i2 );

		// Test ordering with large values having small fractional differences
		datatypes::Decimal d3{ "12345678901234567890.1" };
		datatypes::Int128 i3{ "12345678901234567890" }; // Same integer part
		EXPECT_FALSE( d3 == i3 );
		EXPECT_TRUE( d3 != i3 );
		EXPECT_FALSE( d3 < i3 ); // d3 > i3 because d3 has fractional part 0.1
		EXPECT_FALSE( d3 <= i3 );
		EXPECT_TRUE( d3 > i3 );
		EXPECT_TRUE( d3 >= i3 );

		// Test with maximum precision decimal vs large integer (within Int128 limits)
		datatypes::Decimal d4{ "12345678901234567890.123456" }; // 26 total digits with fractional part
		datatypes::Int128 i4{ "12345678901234567890" };			// Same integer part
		EXPECT_FALSE( d4 == i4 );								// Cannot be equal due to fractional part
		EXPECT_TRUE( d4 != i4 );
		EXPECT_FALSE( d4 < i4 ); // d4 > i4 because of fractional part
		EXPECT_FALSE( d4 <= i4 );
		EXPECT_TRUE( d4 > i4 );
		EXPECT_TRUE( d4 >= i4 );

		// Test edge case: decimal close to but less than next integer
		datatypes::Decimal d5{ "999999999999999999.9" };
		datatypes::Int128 i5{ "1000000000000000000" }; // Next integer
		EXPECT_FALSE( d5 == i5 );
		EXPECT_TRUE( d5 != i5 );
		EXPECT_TRUE( d5 < i5 ); // d5 is still less than i5
		EXPECT_TRUE( d5 <= i5 );
		EXPECT_FALSE( d5 > i5 );
		EXPECT_FALSE( d5 >= i5 );
	}

	TEST( DecimalInt128Comparison, PrecisionEdgeCases )
	{
		// Test decimal with significant fractional part vs integer
		datatypes::Decimal d1{ "42.1" }; // Clear fractional part
		datatypes::Int128 i1{ 42 };
		EXPECT_FALSE( d1 == i1 ); // Fractional part makes them not equal
		EXPECT_TRUE( d1 != i1 );
		EXPECT_FALSE( d1 < i1 );
		EXPECT_FALSE( d1 <= i1 );
		EXPECT_TRUE( d1 > i1 );
		EXPECT_TRUE( d1 >= i1 );

		// Test decimal that normalizes to exact integer vs integer
		datatypes::Decimal d2{ "42.0000000000000000000000000000" };
		datatypes::Int128 i2{ 42 };
		// This should be exactly equal after normalization
		EXPECT_TRUE( d2 == i2 );
		EXPECT_FALSE( d2 != i2 );

		// Test very small decimal vs zero (within precision)
		datatypes::Decimal d3{ "0.0000000000000000000000000001" };
		datatypes::Int128 i3{ 0 };
		EXPECT_FALSE( d3 == i3 );
		EXPECT_TRUE( d3 != i3 );
		EXPECT_FALSE( d3 < i3 );
		EXPECT_FALSE( d3 <= i3 );
		EXPECT_TRUE( d3 > i3 );
		EXPECT_TRUE( d3 >= i3 );
	}

	TEST( DecimalInt128Comparison, ConsistencyWithReversedOperands )
	{
		// Test that Int128 vs Decimal gives consistent results
		// Note: This requires the Int128 operators to be implemented correctly

		datatypes::Decimal d1{ 100 };
		datatypes::Int128 i1{ 200 };

		// Test decimal < int128 vs int128 > decimal
		EXPECT_TRUE( d1 < i1 );
		EXPECT_TRUE( i1 > d1 );

		// Test decimal > int128 vs int128 < decimal
		datatypes::Decimal d2{ 300 };
		datatypes::Int128 i2{ 200 };
		EXPECT_TRUE( d2 > i2 );
		EXPECT_TRUE( i2 < d2 );

		// Test decimal == int128 vs int128 == decimal
		datatypes::Decimal d3{ 150 };
		datatypes::Int128 i3{ 150 };
		EXPECT_TRUE( d3 == i3 );
		EXPECT_TRUE( i3 == d3 );

		// Test with fractional decimal (should not equal integer)
		datatypes::Decimal d4{ 42.5 };
		datatypes::Int128 i4{ 42 };
		EXPECT_FALSE( d4 == i4 );
		EXPECT_FALSE( i4 == d4 );
		EXPECT_TRUE( d4 > i4 );
		EXPECT_TRUE( i4 < d4 );
	}

	//----------------------------------------------
	// Precision
	//----------------------------------------------

	TEST( DecimalPrecision, MaxPrecision )
	{
		// Test with maximum Decimal places (28)
		std::string max_precision{ "1.2345678901234567890123456789" };
		datatypes::Decimal d1;
		EXPECT_TRUE( datatypes::Decimal::tryParse( max_precision, d1 ) );
		EXPECT_FALSE( d1.isZero() );

		// Test exceeding string to be truncated
		// 29 Decimal places -> 28
		std::string too_precise{ "1.23456789012345678901234567890" };
		EXPECT_TRUE( datatypes::Decimal::tryParse( too_precise, d1 ) );
	}

	TEST( DecimalPrecision, LargeNumbers )
	{
		// Test with large integers
		datatypes::Decimal d1{ std::numeric_limits<std::int64_t>::max() };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );

		datatypes::Decimal d2{ std::numeric_limits<std::int64_t>::min() };
		EXPECT_FALSE( d2.isZero() );
		EXPECT_TRUE( d2.isNegative() );
	}

	TEST( DecimalPrecision, VerySmallNumbers )
	{
		// Test very small Decimal values
		// 28 Decimal places
		datatypes::Decimal d1{ "0.0000000000000000000000000001" };
		EXPECT_FALSE( d1.isZero() );
		EXPECT_FALSE( d1.isNegative() );
		EXPECT_EQ( d1.scale(), 28 );
	}

	//----------------------------------------------
	// Performance
	//----------------------------------------------

	TEST( DecimalPerformance, StressTestLargeOperations )
	{
		// Perform many operations to test stability
		datatypes::Decimal accumulator{ "0" };
		datatypes::Decimal increment{ "0.001" };

		for ( int i{ 0 }; i < 1000; ++i )
		{
			accumulator += increment;
		}

		// Should be close to 1.0
		EXPECT_FALSE( accumulator.isZero() );
		EXPECT_FALSE( accumulator.isNegative() );
	}

	//----------------------------------------------
	// Utilities
	//----------------------------------------------

	TEST( DecimalUtlities, DecimalPlacesCount )
	{
		// Test zero values
		datatypes::Decimal d1{ "0" };
		EXPECT_EQ( d1.decimalPlacesCount(), 0 );

		datatypes::Decimal d2{ "0.0" };
		EXPECT_EQ( d2.decimalPlacesCount(), 0 );

		datatypes::Decimal d3{ "0.000" };
		EXPECT_EQ( d3.decimalPlacesCount(), 0 );

		// Test integers (no decimal places)
		datatypes::Decimal d4{ "123" };
		EXPECT_EQ( d4.decimalPlacesCount(), 0 );

		datatypes::Decimal d5{ "-456" };
		EXPECT_EQ( d5.decimalPlacesCount(), 0 );

		// Test decimals without trailing zeros
		datatypes::Decimal d6{ "123.456" };
		EXPECT_EQ( d6.decimalPlacesCount(), 3 );

		datatypes::Decimal d7{ "-789.123" };
		EXPECT_EQ( d7.decimalPlacesCount(), 3 );

		datatypes::Decimal d8{ "0.001" };
		EXPECT_EQ( d8.decimalPlacesCount(), 3 );

		datatypes::Decimal d9{ "0.5" };
		EXPECT_EQ( d9.decimalPlacesCount(), 1 );

		// Test decimals with trailing zeros (should ignore them)
		datatypes::Decimal d10{ "123.4500" };
		EXPECT_EQ( d10.decimalPlacesCount(), 2 ); // 123.45, trailing zeros ignored

		datatypes::Decimal d11{ "123.000" };
		EXPECT_EQ( d11.decimalPlacesCount(), 0 ); // Integer value

		datatypes::Decimal d12{ "456.7800" };
		EXPECT_EQ( d12.decimalPlacesCount(), 2 ); // 456.78, trailing zeros ignored

		datatypes::Decimal d13{ "0.1000" };
		EXPECT_EQ( d13.decimalPlacesCount(), 1 ); // 0.1, trailing zeros ignored

		// Test edge cases with various trailing zero patterns
		datatypes::Decimal d14{ "123.4560" };
		EXPECT_EQ( d14.decimalPlacesCount(), 3 ); // 123.456, one trailing zero ignored

		datatypes::Decimal d15{ "123.4000" };
		EXPECT_EQ( d15.decimalPlacesCount(), 1 ); // 123.4, three trailing zeros ignored

		datatypes::Decimal d16{ "999.9990" };
		EXPECT_EQ( d16.decimalPlacesCount(), 3 ); // 999.999, one trailing zero ignored

		// Test small decimal values
		datatypes::Decimal d17{ "0.01" };
		EXPECT_EQ( d17.decimalPlacesCount(), 2 );

		datatypes::Decimal d18{ "0.001" };
		EXPECT_EQ( d18.decimalPlacesCount(), 3 );

		datatypes::Decimal d19{ "0.0001" };
		EXPECT_EQ( d19.decimalPlacesCount(), 4 );

		// Test high precision values
		datatypes::Decimal d20{ "123.123456789" };
		EXPECT_EQ( d20.decimalPlacesCount(), 9 );

		datatypes::Decimal d21{ "0.123456789012345678901234567" };
		EXPECT_EQ( d21.decimalPlacesCount(), 27 );

		// Test maximum precision with trailing zeros
		datatypes::Decimal d22{ "1.1234567890123456789012345000" };
		EXPECT_EQ( d22.decimalPlacesCount(), 25 ); // Should remove 3 trailing zeros

		// Test very small values with high precision
		datatypes::Decimal d23{ "0.0000000000000000000000000001" };
		EXPECT_EQ( d23.decimalPlacesCount(), 28 ); // Maximum precision

		// Test consistency with scale() method
		datatypes::Decimal d24{ "123.4500" };
		EXPECT_EQ( d24.scale(), 2 );			  // Internal scale includes trailing zeros
		EXPECT_EQ( d24.decimalPlacesCount(), 2 ); // Actual precision excludes trailing zeros

		datatypes::Decimal d25{ "789.12300" };
		EXPECT_EQ( d25.scale(), 3 );			  // Internal scale includes trailing zeros
		EXPECT_EQ( d25.decimalPlacesCount(), 3 ); // Actual precision excludes trailing zeros

		// Test with constructed from integer (should have scale 0 and decimalPlacesCount 0)
		datatypes::Decimal d26{ 42 };
		EXPECT_EQ( d26.scale(), 0 );
		EXPECT_EQ( d26.decimalPlacesCount(), 0 );

		// Test with constructed from double
		datatypes::Decimal d27{ 123.456 };
		EXPECT_TRUE( d27.decimalPlacesCount() >= 3 ); // At least 3, could be more due to double precision

		// Test negative values (sign shouldn't affect decimal places count)
		datatypes::Decimal d28{ "-123.456" };
		EXPECT_EQ( d28.decimalPlacesCount(), 3 );

		datatypes::Decimal d29{ "-0.001" };
		EXPECT_EQ( d29.decimalPlacesCount(), 3 );

		datatypes::Decimal d30{ "-123.4500" };
		EXPECT_EQ( d30.decimalPlacesCount(), 2 ); // Trailing zeros ignored
	}

	//----------------------------------------------
	// Maths
	//----------------------------------------------

	TEST( DecimalMaths, BinaryRepresentation )
	{
		// Test toBits() method that returns Decimal binary representation
		datatypes::Decimal d1{ 123.456 };
		auto bits{ d1.toBits() };

		// Should return 4 elements: [low, mid, high, flags]
		EXPECT_EQ( bits.size(), 4 );

		// Test zero
		datatypes::Decimal zero{ 0 };
		auto zeroBits{ zero.toBits() };
		EXPECT_EQ( zeroBits[0], 0 );
		EXPECT_EQ( zeroBits[1], 0 );
		EXPECT_EQ( zeroBits[2], 0 );
		// Flags should be zero for positive zero
		EXPECT_EQ( zeroBits[3], 0u );

		// Test negative value
		datatypes::Decimal negative{ -123.456 };
		auto negativeBits{ negative.toBits() };
		// Sign bit should be set in flags
		EXPECT_NE( static_cast<std::uint32_t>( negativeBits[3] ) & constants::decimal::SIGN_MASK, 0u );
	}

	TEST( DecimalMaths, TruncateMethod )
	{
		// Test instance method
		datatypes::Decimal d1{ "123.789" };
		auto truncated{ d1.truncate() };
		EXPECT_EQ( truncated.toString(), "123" );

		// Test negative truncation
		datatypes::Decimal d2{ "-123.789" };
		truncated = d2.truncate();
		EXPECT_EQ( truncated.toString(), "-123" );

		// Test zero
		datatypes::Decimal d3{ "0.123" };
		truncated = d3.truncate();
		EXPECT_EQ( truncated.toString(), "0" );

		// Test integer (no change)
		datatypes::Decimal d4{ "123" };
		truncated = d4.truncate();
		EXPECT_EQ( truncated.toString(), "123" );

		// Test static method
		datatypes::Decimal d5{ "456.789" };
		auto staticTruncated{ datatypes::Decimal::truncate( d5 ) };
		EXPECT_EQ( staticTruncated.toString(), "456" );
	}

	TEST( DecimalMaths, FloorMethod )
	{
		// Test positive values
		datatypes::Decimal d1{ "123.789" };
		auto floored{ d1.floor() };
		EXPECT_EQ( floored.toString(), "123" );

		// Test negative values (should round down)
		datatypes::Decimal d2{ "-123.789" };
		floored = d2.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		// Test positive value close to integer
		datatypes::Decimal d3{ "123.001" };
		floored = d3.floor();
		EXPECT_EQ( floored.toString(), "123" );

		// Test negative value close to integer
		datatypes::Decimal d4{ "-123.001" };
		floored = d4.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		// Test integer (no change)
		datatypes::Decimal d5{ "123" };
		floored = d5.floor();
		EXPECT_EQ( floored.toString(), "123" );

		// Test static method
		datatypes::Decimal d6{ "456.789" };
		auto staticFloored{ datatypes::Decimal::floor( d6 ) };
		EXPECT_EQ( staticFloored.toString(), "456" );
	}

	TEST( DecimalMaths, CeilingMethod )
	{
		// Test positive values (should round up)
		datatypes::Decimal d1{ "123.123" };
		auto ceiled{ d1.ceiling() };
		EXPECT_EQ( ceiled.toString(), "124" );

		// Test negative values
		datatypes::Decimal d2{ "-123.123" };
		ceiled = d2.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		// Test positive value close to integer
		datatypes::Decimal d3{ "123.001" };
		ceiled = d3.ceiling();
		EXPECT_EQ( ceiled.toString(), "124" );

		// Test negative value close to integer
		datatypes::Decimal d4{ "-123.001" };
		ceiled = d4.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		// Test integer (no change)
		datatypes::Decimal d5{ "123" };
		ceiled = d5.ceiling();
		EXPECT_EQ( ceiled.toString(), "123" );

		// Test static method
		datatypes::Decimal d6{ "456.123" };
		auto staticCeiled{ datatypes::Decimal ::ceiling( d6 ) };
		EXPECT_EQ( staticCeiled.toString(), "457" );
	}

	TEST( DecimalMaths, RoundMethod )
	{
		// Test basic rounding to nearest integer
		datatypes::Decimal d1{ "123.4" };
		auto rounded{ d1.round() };
		EXPECT_EQ( rounded.toString(), "123" );

		datatypes::Decimal d2{ "123.6" };
		rounded = d2.round();
		EXPECT_EQ( rounded.toString(), "124" );

		// Test exact half - should round to nearest even (banker's rounding)
		datatypes::Decimal d3{ "123.5" };
		rounded = d3.round();
		EXPECT_EQ( rounded.toString(), "124" );

		// Test negative rounding
		datatypes::Decimal d4{ "-123.4" };
		rounded = d4.round();
		EXPECT_EQ( rounded.toString(), "-123" );

		datatypes::Decimal d5{ "-123.6" };
		rounded = d5.round();
		EXPECT_EQ( rounded.toString(), "-122" );

		// Test static method
		datatypes::Decimal d6{ "456.7" };
		auto staticRounded{ datatypes::Decimal ::round( d6 ) };
		EXPECT_EQ( staticRounded.toString(), "457" );
	}

	TEST( DecimalMaths, RoundWithDecimalPlaces )
	{
		// Test rounding to specific Decimal places
		datatypes::Decimal d1{ "123.4567" };

		// Round to 2 Decimal places
		auto rounded{ d1.round( 2 ) };
		EXPECT_EQ( rounded.toString(), "123.46" );

		// Round to 1 Decimal place
		rounded = d1.round( 1 );
		EXPECT_EQ( rounded.toString(), "123.5" );

		// Round to 0 Decimal places (same as round())
		rounded = d1.round( 0 );
		EXPECT_EQ( rounded.toString(), "123" );

		// Test negative values
		datatypes::Decimal d2{ "-123.4567" };
		rounded = d2.round( 2 );
		EXPECT_EQ( rounded.toString(), "-123.44" );

		// Test static method
		datatypes::Decimal d3{ "789.1234" };
		auto staticRounded{ datatypes::Decimal ::round( d3, 3 ) };
		EXPECT_EQ( staticRounded.toString(), "789.123" );

		// Test rounding to more places than available (should return unchanged)
		datatypes::Decimal d4{ "123.45" };
		rounded = d4.round( 5 );
		EXPECT_EQ( rounded.toString(), "123.45" );
	}

	TEST( DecimalMaths, AbsMethod )
	{
		// Test positive value (should remain unchanged)
		datatypes::Decimal d1{ "123.456" };
		auto absValue{ d1.abs() };
		EXPECT_EQ( absValue.toString(), "123.456" );
		EXPECT_FALSE( absValue.isNegative() );

		// Test negative value (should become positive)
		datatypes::Decimal d2{ "-123.456" };
		absValue = d2.abs();
		EXPECT_EQ( absValue.toString(), "123.456" );
		EXPECT_FALSE( absValue.isNegative() );

		// Test zero (should remain zero)
		datatypes::Decimal d3{ "0" };
		absValue = d3.abs();
		EXPECT_EQ( absValue.toString(), "0" );
		EXPECT_TRUE( absValue.isZero() );

		// Test negative zero (should become positive zero)
		datatypes::Decimal d4{ "-0.0" };
		absValue = d4.abs();
		EXPECT_TRUE( absValue.isZero() );
		EXPECT_FALSE( absValue.isNegative() );

		// Test static method
		datatypes::Decimal d5{ "-789.123" };
		auto staticAbs{ datatypes::Decimal ::abs( d5 ) };
		EXPECT_EQ( staticAbs.toString(), "789.123" );
		EXPECT_FALSE( staticAbs.isNegative() );

		// Test very small negative value
		datatypes::Decimal d6{ "-0.000000000000000000000000001" };
		absValue = d6.abs();
		EXPECT_FALSE( absValue.isNegative() );
		EXPECT_FALSE( absValue.isZero() );
	}

	TEST( DecimalMaths, MathematicalConsistency )
	{
		// Test that mathematical operations are consistent
		datatypes::Decimal a{ "123.45" };
		datatypes::Decimal b{ "67.89" };

		// Test that (a + b) - a == b
		datatypes::Decimal sum{ a + b };
		datatypes::Decimal diff{ sum - a };
		EXPECT_TRUE( diff == b );

		// Test that a * b / a == b (within precision limits)
		datatypes::Decimal product{ a * b };
		datatypes::Decimal quotient{ product / a };
		datatypes::Decimal difference{ quotient - b };

		// Due to precision, we test that they're very close
		datatypes::Decimal tolerance{ "0.00001" };
		EXPECT_TRUE( difference.abs() < tolerance );
	}

	TEST( DecimalMaths, RoundingConsistency )
	{
		// Test that different rounding methods work consistently
		datatypes::Decimal value{ "123.456789" };

		// Truncate should always round toward zero
		datatypes::Decimal truncated{ value.truncate() };
		EXPECT_EQ( truncated.toString(), "123" );

		// Floor should always round down
		datatypes::Decimal floored{ value.floor() };
		EXPECT_EQ( floored.toString(), "123" );

		// Ceiling should always round up
		datatypes::Decimal ceiled{ value.ceiling() };
		EXPECT_EQ( ceiled.toString(), "124" );

		// Round should use banker's rounding
		datatypes::Decimal rounded{ value.round() };
		EXPECT_EQ( rounded.toString(), "123" );

		// Test with negative values
		datatypes::Decimal negValue{ "-123.456789" };

		truncated = negValue.truncate();
		EXPECT_EQ( truncated.toString(), "-123" );

		floored = negValue.floor();
		EXPECT_EQ( floored.toString(), "-124" );

		ceiled = negValue.ceiling();
		EXPECT_EQ( ceiled.toString(), "-123" );

		rounded = negValue.round();
		EXPECT_EQ( rounded.toString(), "-123" );
	}

	TEST( DecimalMaths, PrecisionPreservation )
	{
		// Test that precision is preserved in operations
		datatypes::Decimal a{ "0.1" };
		datatypes::Decimal b{ "0.2" };
		datatypes::Decimal c{ "0.3" };

		// This should be exactly 0.3, not 0.30000000000000004 like with double
		datatypes::Decimal sum{ a + b };
		EXPECT_TRUE( sum == c );

		// Test with many Decimal places
		datatypes::Decimal precise1{ "0.1234567890123456789012345678" };
		datatypes::Decimal precise2{ "0.0000000000000000000000000001" };
		datatypes::Decimal preciseSum{ precise1 + precise2 };

		// Should maintain precision
		EXPECT_FALSE( preciseSum == precise1 );
		EXPECT_TRUE( preciseSum > precise1 );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( DecimalStringParsing, StringConstruction )
	{
		// Valid strings
		datatypes::Decimal d1{ "123.456" };
		EXPECT_EQ( d1.toString(), "123.456" );

		datatypes::Decimal d2{ "-789.123" };
		EXPECT_TRUE( d2.isNegative() );

		datatypes::Decimal d3{ "0" };
		EXPECT_TRUE( d3.isZero() );

		datatypes::Decimal d4{ "0.0001" };
		EXPECT_FALSE( d4.isZero() );

		// Test with many Decimal places
		datatypes::Decimal d5{ "123.1234567890123456789" };
		EXPECT_FALSE( d5.isZero() );
	}

	TEST( DecimalStringParsing, TryParseMethod )
	{
		datatypes::Decimal result;

		// Valid positive number
		EXPECT_TRUE( datatypes::Decimal::tryParse( "123.456", result ) );
		EXPECT_EQ( result.toString(), "123.456" );
		EXPECT_FALSE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid negative number
		EXPECT_TRUE( datatypes::Decimal::tryParse( "-789.123", result ) );
		EXPECT_TRUE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid zero
		EXPECT_TRUE( datatypes::Decimal::tryParse( "0", result ) );
		EXPECT_TRUE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Valid zero with decimal
		EXPECT_TRUE( datatypes::Decimal::tryParse( "0.0", result ) );
		EXPECT_TRUE( result.isZero() );

		// Valid number with positive sign
		EXPECT_TRUE( datatypes::Decimal::tryParse( "+456.789", result ) );
		EXPECT_FALSE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid integer
		EXPECT_TRUE( datatypes::Decimal::tryParse( "12345", result ) );
		EXPECT_FALSE( result.isZero() );

		// Valid decimal with leading zero
		EXPECT_TRUE( datatypes::Decimal::tryParse( "0.123", result ) );
		EXPECT_FALSE( result.isZero() );

		// Valid high precision number (within storage limits)
		EXPECT_TRUE( datatypes::Decimal::tryParse( "1.2345678901234567890123456789", result ) );
		EXPECT_FALSE( result.isZero() );

		// Invalid: empty string
		EXPECT_FALSE( datatypes::Decimal::tryParse( "", result ) );

		// Invalid: non-numeric characters
		EXPECT_FALSE( datatypes::Decimal::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12a34", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12.34abc", result ) );

		// Invalid: multiple decimal points
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12.34.56", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "1.2.3", result ) );

		// Invalid: multiple signs
		EXPECT_FALSE( datatypes::Decimal::tryParse( "+-123", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "--123", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "++123", result ) );

		// Invalid: sign in wrong position
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12+34", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12-34", result ) );

		// Invalid: only decimal point
		EXPECT_FALSE( datatypes::Decimal::tryParse( ".", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "..", result ) );

		// Invalid: only sign
		EXPECT_FALSE( datatypes::Decimal::tryParse( "+", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "-", result ) );

		// Invalid: whitespace
		EXPECT_FALSE( datatypes::Decimal::tryParse( " 123", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "123 ", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "1 23", result ) );

		// Invalid: scientific notation
		EXPECT_FALSE( datatypes::Decimal::tryParse( "1.23e10", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "1.23E-5", result ) );
	}

	TEST( DecimalStringParsing, ParseMethod )
	{
		// Valid positive number
		auto result{ datatypes::Decimal::parse( "123.456" ) };
		EXPECT_EQ( result.toString(), "123.456" );
		EXPECT_FALSE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid negative number
		result = datatypes::Decimal::parse( "-789.123" );
		EXPECT_TRUE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid zero
		result = datatypes::Decimal::parse( "0" );
		EXPECT_TRUE( result.isZero() );
		EXPECT_FALSE( result.isNegative() );

		// Valid number with positive sign
		result = datatypes::Decimal::parse( "+456.789" );
		EXPECT_FALSE( result.isNegative() );
		EXPECT_FALSE( result.isZero() );

		// Valid high precision number (within storage limits)
		result = datatypes::Decimal::parse( "1.2345678901234567890123456789" );
		EXPECT_FALSE( result.isZero() );

		// Invalid: empty string should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "" ), std::invalid_argument );

		// Invalid: non-numeric should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "abc" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "12a34" ), std::invalid_argument );

		// Invalid: multiple decimal points should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "12.34.56" ), std::invalid_argument );

		// Invalid: multiple signs should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "+-123" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "--123" ), std::invalid_argument );

		// Invalid: sign in wrong position should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "12+34" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "12-34" ), std::invalid_argument );

		// Invalid: only decimal point should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "." ), std::invalid_argument );

		// Invalid: only sign should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "+" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "-" ), std::invalid_argument );

		// Invalid: whitespace should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( " 123" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "123 " ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "1 23" ), std::invalid_argument );

		// Invalid: scientific notation should throw
		EXPECT_THROW( (void)datatypes::Decimal::parse( "1.23e10" ), std::invalid_argument );
		EXPECT_THROW( (void)datatypes::Decimal::parse( "1.23E-5" ), std::invalid_argument );
	}

	//----------------------------------------------
	// Compatibility
	//----------------------------------------------

	TEST( DecimalCompatibility, CrossPlatformConsistency )
	{
		// Test that results are consistent across platforms
		datatypes::Decimal d1{ "123456789.123456789" };
		datatypes::Decimal d2{ "987654321.987654321" };

		// Addition should work consistently
		datatypes::Decimal sum{ d1 + d2 };
		EXPECT_FALSE( sum.isZero() );

		// Multiplication should work consistently
		datatypes::Decimal product{ datatypes::Decimal{ "123.456" } * datatypes::Decimal{ "789.123" } };
		EXPECT_FALSE( product.isZero() );

		// Division should work consistently
		datatypes::Decimal quotient{ datatypes::Decimal{ "1000" } / datatypes::Decimal{ "3" } };
		EXPECT_FALSE( quotient.isZero() );
	}

	TEST( DecimalCompatibility, StandardDecimalCompatibility )
	{
		// Test behaviors that should match standard Decimal arithmetic

		// Decimal has 28-29 significant digits
		datatypes::Decimal d1{ "1234567890123456789012345678.9" };
		EXPECT_FALSE( d1.isZero() );

		// Decimal preserves trailing zeros in scale
		datatypes::Decimal d2{ "123.4500" };
		EXPECT_EQ( d2.scale(), 2 ); // Should preserve 2 Decimal places

		// Decimal arithmetic should be exact (no floating-point errors)
		datatypes::Decimal d3{ "0.1" };
		datatypes::Decimal d4{ "0.2" };
		datatypes::Decimal sum{ d3 + d4 };

		// Should be exactly 0.3, not 0.30000000000000004 like double
		datatypes::Decimal expected{ "0.3" };
		EXPECT_TRUE( sum == expected );
	}

	TEST( DecimalCompatibility, NormalizationBehavior )
	{
		// Test that normalization removes unnecessary trailing zeros
		datatypes::Decimal d1{ "123.4500" };
		// Force normalization through conversion
		std::string normalized{ d1.toString() };

		// After normalization, trailing zeros should be removed
		// but scale should still be maintained for precision
		EXPECT_FALSE( d1.isZero() );

		// Test zero normalization
		datatypes::Decimal zero1{ "0.000" };
		datatypes::Decimal zero2{ 0 };
		EXPECT_TRUE( zero1 == zero2 );
	}

	//----------------------------------------------
	// Stream
	//----------------------------------------------

	TEST( DecimalStream, StreamOutput )
	{
		datatypes::Decimal d1{ "123.456" };
		std::ostringstream oss;
		oss << d1;
		EXPECT_EQ( oss.str(), "123.456" );

		datatypes::Decimal d2{ "-789.123" };
		oss.str( "" );
		oss << d2;
		EXPECT_EQ( oss.str(), "-789.123" );
	}

	TEST( DecimalStream, StreamInput )
	{
		datatypes::Decimal d1;
		std::istringstream iss( "456.789" );
		iss >> d1;
		EXPECT_FALSE( iss.fail() );
		EXPECT_EQ( d1.toString(), "456.789" );

		// Test invalid input
		datatypes::Decimal d2;
		std::istringstream iss2( "invalid" );
		iss2 >> d2;
		EXPECT_TRUE( iss2.fail() );
	}

	//----------------------------------------------
	// Edge case and overflow
	//----------------------------------------------

	TEST( DecimalEdgeCaseAndOverflow, DivisionByZeroHandling )
	{
		datatypes::Decimal dividend{ "123.45" };
		datatypes::Decimal zero{ "0" };

		// Division by zero should throw
		EXPECT_THROW( dividend / zero, std::overflow_error );
		EXPECT_THROW( dividend /= zero, std::overflow_error );
	}

	TEST( DecimalEdgeCaseAndOverflow, OverflowHandling )
	{
		// Test near maximum values
		try
		{
			// Create large Decimal values within limits
			datatypes::Decimal large1{ "99999999999999999999999999.99" };
			datatypes::Decimal large2{ "1.01" };

			// Operations should either succeed or throw appropriately
			datatypes::Decimal result{ large1 * large2 };
			// If we get here, the operation succeeded
			EXPECT_FALSE( result.isZero() );
		}
		catch ( const std::exception& )
		{
			// Throwing on overflow is acceptable behavior
			SUCCEED();
		}
	}

	TEST( DecimalEdgeCaseAndOverflow, InvalidInputHandling )
	{
		datatypes::Decimal result;

		// Test various invalid string formats
		EXPECT_FALSE( datatypes::Decimal::tryParse( "", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "abc", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12.34.56", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "12a34", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "+-123", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( "123..", result ) );
		EXPECT_FALSE( datatypes::Decimal::tryParse( ".123.", result ) );

		// Test strings that are too long - should be truncated
		std::string tooLong{ "1." };
		for ( int i{ 0 }; i < 50; ++i )
		{
			tooLong += "1";
		}

		EXPECT_TRUE( datatypes::Decimal::tryParse( tooLong, result ) );
	}
}
