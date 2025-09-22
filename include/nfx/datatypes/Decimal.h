/**
 * @file Decimal.h
 * @brief Cross-platform high-precision decimal arithmetic type
 * @details Implements the Decimal data type with exact decimal arithmetic and no floating-point rounding errors.
 *
 *          Range and Precision:
 *          - Values from -79,228,162,514,264,337,593,543,950,335 to +79,228,162,514,264,337,593,543,950,335
 *          - 28 significant decimal digits maximum
 *          - Finite set of values of the form m / 10^e where:
 *            * m is an integer such that -2^96 < m < 2^96
 *            * e is an integer between 0 and 28 inclusive
 *
 *          Key Benefits:
 *          - Exact representation of decimal fractions (e.g., 0.1 is represented exactly)
 *          - No round-off errors in financial calculations
 *          - Ideally suited for monetary and high-precision decimal arithmetic
 *
 *          Storage Format:
 *          - 96-bit mantissa + 32-bit scale/sign = 128-bit total storage
 *
 *          Memory Layout of Decimal (128 bits / 16 bytes):
 *          ==============================================
 *
 *          1. Flags (32 bits):
 *          ┌───────────┬─────────────────────────────────────┬───────────────────────────────────────────────────┐
 *          │    Bits   │             Description             │                       Notes                       │
 *          ├───────────┼─────────────────────────────────────┼───────────────────────────────────────────────────┤
 *          │   0 - 15  │  Unused (must be zero)              │  Reserved - Required to be zero for valid format  │
 *          │  16 - 23  │  Scale (0-28)                       │  Number of decimal digits after decimal point     │
 *          │  24 - 30  │  Unused (must be zero)              │  Reserved - Required to be zero for valid format  │
 *          │  31       │  Sign (0 = positive, 1 = negative)  │  Sign bit                                         │
 *          └───────────┴─────────────────────────────────────┴───────────────────────────────────────────────────┘
 *
 *          2. Mantissa (96 bits total):
 *          ┌───────────────┬───────────┬─────────────────────────────────┐
 *          │ Mantissa Part │   Bits    │           Description           │
 *          ├───────────────┼───────────┼─────────────────────────────────┤
 *          │  mantissa[0]  │   0 - 31  │  Lower 32 bits of the mantissa  │
 *          │  mantissa[1]  │  32 - 63  │  Middle 32 bits of the mantissa │
 *          │  mantissa[2]  │  64 - 95  │  Upper 32 bits of the mantissa  │
 *          └───────────────┴───────────┴─────────────────────────────────┘
 *
 *          Complete Memory Layout (128 bits / 16 bytes):
 *          =============================================
 *
 *          ┌─────────────────────────────────┬─────────────────────────────────┬─────────────────────────────────┬─────────────────────────────────┐
 *          │         mantissa[2]             │         mantissa[1]             │          mantissa[0]            │            flags                │
 *          │       (upper 32 bits)           │      (middle 32 bits)           │       (lower 32 bits)           │        (scale + sign)           │
 *          │          32 bits                │          32 bits                │           32 bits               │            32 bit               │
 *          └─────────────────────────────────┴─────────────────────────────────┴─────────────────────────────────┴─────────────────────────────────┘
 *          Bit 127                     Bit 96 Bit 95                     Bit 64 Bit 63                     Bit 32 Bit 31                       Bit 0
 *
 *          Where the 96-bit mantissa represents an unsigned integer from 0 to 2^96-1
 *          and the sign is stored separately in bit 31 of the flags word.
 *
 *          Summary:
 *          =======
 *
 *          - Total storage: 128 bits (16 bytes)
 *          - Value formula: decimal_value = mantissa / 10^scale × (sign ? -1 : 1)
 *
 *          Examples with Memory Layout:
 *          ============================
 *
 *          Example 1 - Value 123.45:
 *          - mantissa: 12345 (stored across mantissa[0-2])
 *          - scale   : 2 (bits 16-23 of flags, 2 decimal places)
 *          - sign    : 0 (bit 31 of flags, positive)
 *          - result  : 12345 / 10² = 123.45
 *
 *          Example 2 - Value -12,345,678,901,234,567,890.123456789:
 *          - mantissa: 12345678901234567890123456789 (96-bit value across mantissa[0-2])
 *                      mantissa[0] = 0x15CD5B07  - 365,072,135   (lower 32 bits)
 *                      mantissa[1] = 0x9CE5A30A  - 2,632,713,994 (middle 32 bits)
 *                      mantissa[2] = 0x27B95E997 - 669,260,439   (upper 32 bits)
 *          - scale   : 9 (bits 16-23 of flags, 9 decimal places)
 *          - sign    : 1 (bit 31 of flags, negative)
 *          - result  : 12345678901234567890123456789 / 10⁹ × (-1) = -12,345,678,901,234,567,890.123456789
 *
 *          - Original = (mantissa[2] × 2^64) + (mantissa[1] × 2^32) + mantissa[0]
 *          - Original = (669,260,439 × 18,446,744,073,709,551,616) + (2,632,713,994 × 4,294,967,296) + 365,072,135
 *          - Original = 12345678901234567890123456789
 *
 *          IEEE 754-2008 binary64 Input Compatibility:
 *          - Construction from double uses IEEE 754-2008 std::isnan/std::isinf functions
 *          - Preserves IEEE 754 binary64 precision limits (~15-17 digits)
 *          - NaN and Infinity from double are converted to zero
 *          - String construction provides exact decimal precision (up to 28 digits)
 *
 * @note This is NOT IEEE 754 decimal128 arithmetic - it implements
 *       exact fixed-point decimal arithmetic without floating-point rounding errors.
 *
 * @note Design inspired by .NET System.Decimal semantics (96-bit mantissa + scale, 28–29 digits, banker's rounding).
 */

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "Int128.h"

namespace nfx::datatypes
{
	//=====================================================================
	// Decimal class
	//=====================================================================

	/**
	 * @brief Cross-platform high-precision decimal type
	 * @details Implements exact decimal arithmetic with:
	 *          - Range: ±79,228,162,514,264,337,593,543,950,335
	 *          - 28 significant decimal digits maximum
	 *          - 96-bit mantissa + 32-bit scale/sign = 128-bit total storage
	 *          - Exact decimal arithmetic (no floating-point rounding errors)
	 *
	 *          Value Representation:
	 *          - All values are of the form: mantissa / 10^scale
	 *          - Mantissa: 96-bit unsigned integer (0 ≤ m < 2^96)
	 *          - Sign: Stored separately in bit 31 of flags word
	 *          - Scale: 0 to 28 inclusive (number of decimal places)
	 *
	 *          Financial Calculation Benefits:
	 *          - Decimal fractions like 0.1 are represented exactly (no 0.099999... issues)
	 *          - No round-off errors in monetary calculations
	 *          - Predictable precision for financial applications
	 *
	 *          IEEE 754-2008 binary64 Input Compatibility:
	 *          - Double constructor uses IEEE 754-2008 std::isnan/std::isinf functions
	 *          - Respects IEEE 754 binary64 precision limits (~15-17 digits)
	 *          - NaN and Infinity from double input are converted to zero
	 *          - No internal NaN/Infinity states (division by zero throws std::overflow_error)
	 *          - Use string constructor for values requiring exact decimal precision
	 *
	 * @note Note: This implements exact fixed-point decimal arithmetic, NOT IEEE 754 decimal128.
	 */
	class Decimal final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor (zero value)
		 */
		constexpr Decimal() noexcept;

		/**
		 * @brief Construct from double with IEEE 754-2008 input compatibility
		 * @param value Double value to convert
		 * @note Limited to IEEE 754 binary64 precision (~15-17 significant digits).
		 *       Uses IEEE 754-2008 std::isnan/std::isinf for special value detection.
		 *       NaN and Infinity values are converted to zero.
		 *       For exact precision, use string constructor instead.
		 */
		explicit Decimal( double value ) noexcept;

		/**
		 * @brief Construct from 32-bit integer
		 * @param value Integer value to convert
		 */
		explicit Decimal( std::int32_t value ) noexcept;

		/**
		 * @brief Construct from 64-bit integer
		 * @param value Integer value to convert
		 */
		explicit Decimal( std::int64_t value ) noexcept;

		/**
		 * @brief Construct from 32-bit unsigned integer
		 * @param value Unsigned integer value to convert
		 */
		explicit Decimal( std::uint32_t value ) noexcept;

		/**
		 * @brief Construct from 64-bit unsigned integer
		 * @param value Unsigned integer value to convert
		 */
		explicit Decimal( std::uint64_t value ) noexcept;

		/**
		 * @brief Construct from string (exact parsing)
		 * @param str String representation (e.g., "123.456")
		 * @throws std::invalid_argument if string is not a valid decimal
		 * @see parse() for static parsing with error handling
		 * @see tryParse() for non-throwing parsing
		 */
		explicit Decimal( std::string_view str );

		/**
		 * @brief Copy constructor
		 * @param other The Decimal object to copy from
		 */
		Decimal( const Decimal& other ) noexcept = default;

		/**
		 * @brief Move constructor
		 * @param other The Decimal object to move from
		 */
		Decimal( Decimal&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Decimal() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Decimal object to copy from
		 * @return Reference to this Decimal object after assignment
		 */
		Decimal& operator=( const Decimal& other ) noexcept = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Decimal object to move from
		 * @return Reference to this Decimal object after assignment
		 */
		Decimal& operator=( Decimal&& other ) noexcept = default;

		//----------------------------------------------
		// Decimal constants
		//----------------------------------------------

		/**
		 * @brief Zero value constant
		 * @return Decimal representing zero
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr Decimal zero() noexcept;

		/**
		 * @brief One value constant
		 * @return Decimal representing one
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr Decimal one() noexcept;

		/**
		 * @brief Minimum finite value constant
		 * @return Smallest representable positive decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr Decimal minValue() noexcept;

		/**
		 * @brief Maximum finite value constant
		 * @return Largest representable decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr Decimal maxValue() noexcept;

		//----------------------------------------------
		// Static mathematical operations
		//----------------------------------------------

		/**
		 * @brief Remove fractional part
		 * @param value Decimal to truncate
		 * @return Decimal with fractional part removed
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal truncate( const Decimal& value ) noexcept;

		/**
		 * @brief Round down to nearest integer
		 * @param value Decimal to floor
		 * @return Largest integer less than or equal to value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal floor( Decimal& value ) noexcept;

		/**
		 * @brief Round up to nearest integer
		 * @param value Decimal to ceiling
		 * @return Smallest integer greater than or equal to value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal ceiling( Decimal& value ) noexcept;

		/**
		 * @brief Round to nearest integer
		 * @param value Decimal to round
		 * @return Rounded decimal value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal round( Decimal& value ) noexcept;

		/**
		 * @brief Round to specified decimal places
		 * @param value Decimal to round
		 * @param decimalsPlacesCount Number of decimal places to round to
		 * @return Rounded decimal value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal round( Decimal& value, std::int32_t decimalsPlacesCount ) noexcept;

		/**
		 * @brief Get absolute value
		 * @param value Decimal to get absolute value of
		 * @return Absolute value of the decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal abs( const Decimal& value ) noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Addition operator
		 * @param other The Decimal value to add
		 * @return Result of addition
		 */
		Decimal operator+( const Decimal& other );

		/**
		 * @brief Subtraction operator
		 * @param other The Decimal value to subtract
		 * @return Result of subtraction
		 */
		Decimal operator-( const Decimal& other );

		/**
		 * @brief Multiplication operator
		 * @param other The Decimal value to multiply by
		 * @return Result of multiplication
		 */
		Decimal operator*( const Decimal& other ) const;

		/**
		 * @brief Division operator
		 * @param other Divisor
		 * @return Result of division
		 * @throws std::overflow_error if divisor is zero (no NaN/Infinity representation)
		 */
		Decimal operator/( const Decimal& other ) const;

		/**
		 * @brief Addition assignment operator
		 * @param other The Decimal value to add
		 * @return Reference to this Decimal after addition
		 */
		Decimal& operator+=( const Decimal& other );

		/**
		 * @brief Subtraction assignment operator
		 * @param other The Decimal value to subtract
		 * @return Reference to this Decimal after subtraction
		 */
		Decimal& operator-=( const Decimal& other );

		/**
		 * @brief Multiplication assignment operator
		 * @param other The Decimal value to multiply by
		 * @return Reference to this Decimal after multiplication
		 */
		Decimal& operator*=( const Decimal& other );

		/**
		 * @brief Division assignment operator
		 * @param other Divisor
		 * @return Reference to this after division
		 * @throws std::overflow_error if divisor is zero (no NaN/Infinity representation)
		 */
		Decimal& operator/=( const Decimal& other );

		/**
		 * @brief Unary minus operator (negation)
		 * @return Negated decimal value
		 */
		Decimal operator-() const noexcept;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if values are equal, false otherwise
		 */
		bool operator==( const Decimal& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if values are not equal, false otherwise
		 */
		bool operator!=( const Decimal& other ) noexcept;

		/**
		 * @brief Less than comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if this value is less than other, false otherwise
		 */
		bool operator<( const Decimal& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if this value is less than or equal to other, false otherwise
		 */
		bool operator<=( const Decimal& other ) noexcept;

		/**
		 * @brief Greater than comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if this value is greater than other, false otherwise
		 */
		bool operator>( const Decimal& other ) noexcept;

		/**
		 * @brief Greater than or equal comparison operator
		 * @param other The Decimal value to compare with
		 * @return true if this value is greater than or equal to other, false otherwise
		 */
		bool operator>=( const Decimal& other ) noexcept;

		//----------------------------------------------
		// String parsing and conversion
		//----------------------------------------------

		/**
		 * @brief Parse high-precision decimal from string
		 * @param str String representation of decimal number (e.g., "123.456", "-0.001", "999999999999999999999999999.999")
		 * @return Parsed Decimal value with exact precision
		 * @throws std::invalid_argument if string format is invalid or represents value outside Decimal range
		 * @note Provides exact decimal parsing up to 28-29 significant digits without floating-point rounding errors
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Decimal parse( std::string_view str );

		/**
		 * @brief Parse string to decimal with error handling
		 * @param str String to parse
		 * @param result Output decimal value
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, Decimal& result ) noexcept;

		//----------------------------------------------
		// Type conversion
		//----------------------------------------------

		/**
		 * @brief Convert to double (may lose precision)
		 * @return Double representation
		 * @note Conversion may lose precision beyond ~15-17 significant digits
		 * @note For exact precision, use toString() method instead
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double toDouble() const noexcept;

		/**
		 * @brief Convert to string with exact precision
		 * @return String representation
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get internal 32-bit representation
		 * @return Array of 4 32-bit integers representing the decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::array<std::int32_t, 4> toBits() const noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get decimal scale (number of decimal places)
		 * @return Scale value (0-28)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::uint8_t scale() const noexcept;

		/**
		 * @brief Get flags value
		 * @return Reference to flags
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::uint32_t& flags() const noexcept;

		/**
		 * @brief Get mutable flags value
		 * @return Mutable reference to flags
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::uint32_t& flags() noexcept;

		/**
		 * @brief Get mantissa array
		 * @return Reference to mantissa array
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] const std::array<std::uint32_t, 3>& mantissa() const noexcept;

		/**
		 * @brief Get mutable mantissa array
		 * @return Mutable reference to mantissa array
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::array<std::uint32_t, 3>& mantissa() noexcept;

		//----------------------------------------------
		// State checking
		//----------------------------------------------

		/**
		 * @brief Check if value is zero
		 * @return true if zero
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isZero() const noexcept;

		/**
		 * @brief Check if value is negative
		 * @return true if negative
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isNegative() const noexcept;

		//----------------------------------------------
		// Utilities
		//----------------------------------------------

		/**
		 * @brief Count actual decimal places (excluding trailing zeros)
		 * @return Number of significant decimal places (0-28)
		 * @details Examples:
		 *          - Decimal("123.4500") returns 2 (trailing zeros ignored)
		 *          - Decimal("123.000") returns 0 (integer value)
		 *          - Decimal("0.001") returns 3
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::uint8_t decimalPlacesCount() const noexcept;

		//----------------------------------------------
		// Mathematical operations
		//----------------------------------------------

		/**
		 * @brief Remove fractional part
		 * @return Decimal with fractional part removed
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal truncate() const noexcept;

		/**
		 * @brief Round down to nearest integer
		 * @return Largest integer less than or equal to value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal floor() noexcept;

		/**
		 * @brief Round up to nearest integer
		 * @return Smallest integer greater than or equal to value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal ceiling() noexcept;

		/**
		 * @brief Round to nearest integer
		 * @return Rounded decimal value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal round() noexcept;

		/**
		 * @brief Round to specified decimal places
		 * @param decimalsPlacesCount Number of decimal places to round to
		 * @return Rounded decimal value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal round( std::int32_t decimalsPlacesCount ) noexcept;

		/**
		 * @brief Get absolute value
		 * @return Absolute value of the decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Decimal abs() const noexcept;

	private:
		//----------------------------------------------
		// Internal representation
		//----------------------------------------------

		/** @brief Internal storage layout for 128-bit decimal representation */
		struct Layout
		{
			/** @brief Scale (bits 16-23) + Sign (bit 31) */
			std::uint32_t flags;

			/** @brief 96-bit mantissa (3 x 32-bit) */
			std::array<std::uint32_t, 3> mantissa;
		} m_layout;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator
	 * @param os Output stream
	 * @param decimal Decimal value to output
	 * @return Reference to output stream
	 */
	std::ostream& operator<<( std::ostream& os, const Decimal& decimal );

	/**
	 * @brief Input stream operator
	 * @param is Input stream
	 * @param decimal Decimal value to input
	 * @return Reference to input stream
	 */
	std::istream& operator>>( std::istream& is, Decimal& decimal );
}

#include "Decimal.inl"
