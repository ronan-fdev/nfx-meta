/**
 * @file Int128.h
 * @brief Cross-platform 128-bit integer arithmetic type
 * @details Provides portable 128-bit signed integer operations with
 *          native __int128 on GCC/Clang and manual implementation on MSVC
 *
 *          Performance characteristics:
 *          - GCC/Clang: Direct hardware 128-bit operations where available
 *            * Native __int128 provides single-instruction arithmetic on modern CPUs
 *            * Compiler generates optimal assembly for 128-bit operations
 *            * Hardware carry/borrow propagation for addition/subtraction
 *          - MSVC: Optimized multi-precision arithmetic using 64-bit operations
 *            * Efficient carry propagation using compiler intrinsics
 *            * Optimized multiplication using _umul128 and _mul128 where available
 *            * Hand-tuned algorithms for division and modulo operations
 *
 *          Integration with Decimal:
 *          - Designed specifically for high-performance decimal arithmetic
 *          - Optimized for mantissa storage and manipulation in Decimal class
 *          - Efficient conversion between 96-bit decimal mantissa and 128-bit integer
 *
 *          Memory Layout and Internal Representation:
 *          =========================================
 *
 *          The Int128 class uses platform-specific storage to maximize performance:
 *
 *          1. GCC/Clang with native __int128 support (NFX_CORE_HAS_INT128=1):
 *          @code
 *          ┌─────────────────────────────────────────────────────────────────┐
 *          │                     Native __int128                             │
 *          │                        (16 bytes)                               │
 *          └─────────────────────────────────────────────────────────────────┘
 *          Bit 127                                                       Bit 0
 *          @endcode
 *
 *          2. MSVC and other compilers (NFX_CORE_HAS_INT128=0):
 *          @code
 *          ┌─────────────────────────────────┬─────────────────────────────────┐
 *          │          upper64bits            │          lower64bits            │
 *          │       (most significant)        │       (least significant)       │
 *          │           8 bytes               │           8 bytes               │
 *          └─────────────────────────────────┴─────────────────────────────────┘
 *          Bit 127                     Bit 64 Bit 63                       Bit 0
 *          @endcode
 *
 *          Bit Layout and Sign Representation:
 *          ===================================
 *
 *          The 128-bit signed integer uses two's complement representation:
 *          @code
 *          ┌─┬───────────────────────────────────────────────────────────────────┐
 *          │S│                    Value Bits (127 bits)                          │
 *          └─┴───────────────────────────────────────────────────────────────────┘
 *          Bit 127                                                           Bit 0
 *          @endcode
 *
 *          Where:
 *          - S (bit 127): Sign bit (0 = positive, 1 = negative)
 *          - Bits 126-0: Magnitude in two's complement form
 *
 *          Value Range:
 *          - Minimum: -2^127 = -170,141,183,460,469,231,731,687,303,715,884,105,728
 *          - Maximum:  2^127-1 = 170,141,183,460,469,231,731,687,303,715,884,105,727
 *
 *          Examples with Memory Layout:
 *          ============================
 *
 *          Example 1: Small positive number (42)
 *          - Decimal: 42
 *          - Hex: 0x0000000000000000000000000000002A
 *          - Memory layout (little-endian on x86-64):
 *            * upper64bits: 0x0000000000000000
 *            * lower64bits: 0x000000000000002A
 *          - Bit pattern: 0000...00101010 (127 zeros followed by 101010)
 *
 *          Example 2: Large positive number (12,345,678,901,234,567,890)
 *          - Decimal: 12,345,678,901,234,567,890
 *          - Hex: 0x00000000000000000AB54A98CEB1F0D2
 *          - Memory layout:
 *            * upper64bits: 0x0000000000000000
 *            * lower64bits: 0x0AB54A98CEB1F0D2
 *          - Bit breakdown:
 *            * Bits 127-64: All zeros (positive number, high bits unused)
 *            * Bits 63-0: 0x0AB54A98CEB1F0D2 = 12,345,678,901,234,567,890
 *
 *          Example 3: Very large number requiring full 128 bits
 *          - Decimal: 123,456,789,012,345,678,901,234,567,890,123,456,789
 *          - Hex: 0x0173DC35270122E8EBC2CE4F3C95D6F5
 *          - Memory layout:
 *            * upper64bits: 0x0173DC35270122E8
 *            * lower64bits: 0xEBC2CE4F3C95D6F5
 *          - Bit breakdown:
 *            * Bits 127-64: 0x0173DC35270122E8 = 1,662,554,368,463,341,288
 *            * Bits 63-0:   0xEBC2CE4F3C95D6F5 = 17,034,473,836,310,554,357
 *            * Full value: (1,662,554,368,463,341,288 << 64) + 17,034,473,836,310,554,357
 *
 *          Example 4: Negative number (-42)
 *          - Decimal: -42
 *          - Two's complement hex: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD6
 *          - Memory layout:
 *            * upper64bits: 0xFFFFFFFFFFFFFFFF
 *            * lower64bits: 0xFFFFFFFFFFFFFFD6
 *          - Bit pattern: 1111...11010110 (sign extension with two's complement)
 *
 *          Platform-Specific Performance Characteristics:
 *          =============================================
 *
 *          Native __int128 (GCC/Clang):
 *          - Single memory load/store for entire 128-bit value
 *          - Hardware-accelerated arithmetic on modern x86-64 and AArch64
 *          - Compiler generates optimal assembly using native instructions
 *          - Zero overhead for most arithmetic operations
 *
 *          Manual implementation (MSVC):
 *          - Two 64-bit operations per 128-bit operation
 *          - Optimized carry propagation using compiler intrinsics
 *          - Efficient multiplication using _umul128/_mul128 where available
 *          - Hand-tuned algorithms for division and complex operations
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include "nfx/config.h"

namespace nfx::datatypes
{
	class Decimal;

	//=====================================================================
	// Int128 class
	//=====================================================================

	/**
	 * @brief Cross-platform 128-bit signed integer type
	 * @details Implements portable 128-bit signed integer arithmetic with:
	 *          - Native __int128 support on GCC/Clang for maximum performance
	 *          - Manual implementation on MSVC using two 64-bit words
	 *          - Full arithmetic and comparison operator support
	 *          - Optimized for decimal arithmetic operations
	 *          - Cross-platform value consistency and API compatibility
	 */
	class Int128 final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor
		 */
		constexpr Int128() noexcept;

		/**
		 * @brief Construct from two 64-bit words
		 * @param low Lower 64 bits
		 * @param high Upper 64 bits
		 */
		constexpr Int128( std::uint64_t low, std::uint64_t high ) noexcept;

		/**
		 * @brief Construct from unsigned 64-bit integer
		 * @param val 64-bit unsigned value
		 */
		explicit constexpr Int128( std::uint64_t val ) noexcept;

		/**
		 * @brief Construct from signed 64-bit integer
		 * @param val 64-bit signed value
		 */
		explicit constexpr Int128( std::int64_t val ) noexcept;

		/**
		 * @brief Construct from unsigned 32-bit integer
		 * @param val 32-bit unsigned value
		 */
		explicit constexpr Int128( std::uint32_t val ) noexcept;

		/**
		 * @brief Construct from signed 32-bit integer
		 * @param val 32-bit signed value
		 */
		explicit constexpr Int128( int val ) noexcept;

#if NFX_CORE_HAS_INT128
		/**
		 * @brief Construct from native __int128 (GCC/Clang only)
		 * @param val Native 128-bit value
		 */
		explicit constexpr Int128( NFX_CORE_INT128 val ) noexcept;
#endif

		/**
		 * @brief Construct from string (exact parsing)
		 * @param str String representation (e.g., "123", "-456789")
		 * @throws std::invalid_argument if string is not a valid integer
		 */
		explicit Int128( std::string_view str );

		/**
		 * @brief Construct from single-precision floating-point value
		 * @param val Float value to convert
		 * @details Follows C++ standard truncation behavior (like static_cast<int>(float)):
		 *          - Fractional parts are truncated toward zero (42.9f → 42, -42.9f → -42)
		 *          - NaN and infinity values are converted to zero
		 *          - Values exceeding int64 range are clamped to int64 limits
		 * @note This matches the behavior of static_cast<int>(float) for consistency
		 */
		explicit Int128( float val );

		/**
		 * @brief Construct from double-precision floating-point value
		 * @param val Double value to convert
		 * @details Follows C++ standard truncation behavior (like static_cast<int>(double)):
		 *          - Fractional parts are truncated toward zero (42.7 → 42, -42.7 → -42)
		 *          - NaN and infinity values are converted to zero
		 *          - Very large values exceeding Int128 range are clamped to Int128 limits
		 *          - Uses string conversion for high precision when needed
		 * @note This matches the behavior of static_cast<int>(double) for consistency
		 */
		explicit Int128( double val );

		/**
		 * @brief Construct from Decimal value
		 * @param decimal Decimal value to convert
		 * @details Follows C++ standard truncation behavior (like static_cast<int>(double)):
		 *          - Fractional parts are truncated toward zero (42.7 → 42, -42.7 → -42)
		 *          - Integer part is extracted by dividing mantissa by 10^scale
		 *          - Zero values are handled efficiently
		 *          - Large values within Int128 range are supported
		 * @note This matches the behavior of static_cast<int>(double) for consistency,
		 *       rather than throwing exceptions for fractional parts
		 */
		explicit Int128( const Decimal& decimal );

		/**
		 * @brief Copy constructor
		 * @param other The Int128 object to copy from
		 */
		constexpr Int128( const Int128& other ) noexcept = default;

		/**
		 * @brief Move constructor
		 * @param other The Int128 object to move from
		 */
		constexpr Int128( Int128&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Int128() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @param other The Int128 object to copy from
		 * @return Reference to this Int128 object after assignment
		 */
		constexpr Int128& operator=( const Int128& other ) noexcept = default;

		/**
		 * @brief Move assignment operator
		 * @param other The Int128 object to move from
		 * @return Reference to this Int128 object after assignment
		 */
		constexpr Int128& operator=( Int128&& other ) noexcept = default;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Addition operator
		 * @param other Right operand
		 * @return Sum of this and other
		 */
		Int128 operator+( const Int128& other ) const noexcept;

		/**
		 * @brief Subtraction operator
		 * @param other Right operand
		 * @return Difference of this and other
		 */
		Int128 operator-( const Int128& other ) const noexcept;

		/**
		 * @brief Multiplication operator
		 * @param other Right operand
		 * @return Product of this and other
		 */
		Int128 operator*( const Int128& other ) const noexcept;

		/**
		 * @brief Division operator
		 * @param other Divisor
		 * @return Result of division
		 * @throws std::overflow_error if divisor is zero
		 */
		Int128 operator/( const Int128& other ) const;

		/**
		 * @brief Modulo operator
		 * @param other Divisor
		 * @return Remainder of division
		 * @throws std::overflow_error if divisor is zero
		 */
		Int128 operator%( const Int128& other ) const;

		/**
		 * @brief Unary minus operator
		 * @return Negated value
		 */
		Int128 operator-() const noexcept;

		//----------------------------------------------
		// Todo: Arithmetic operators with Decimal
		//----------------------------------------------

		//----------------------------------------------
		// Todo: Arithmetic operators with standard types
		//----------------------------------------------

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality operator
		 * @param other Right operand
		 * @return true if values are equal
		 */
		bool operator==( const Int128& other ) const noexcept;

		/**
		 * @brief Inequality operator
		 * @param other Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( const Int128& other ) const noexcept;

		/**
		 * @brief Less than operator
		 * @param other Right operand
		 * @return true if this is less than other
		 */
		bool operator<( const Int128& other ) const noexcept;

		/**
		 * @brief Less than or equal operator
		 * @param other Right operand
		 * @return true if this is less than or equal to other
		 */
		bool operator<=( const Int128& other ) const noexcept;

		/**
		 * @brief Greater than operator
		 * @param other Right operand
		 * @return true if this is greater than other
		 */
		bool operator>( const Int128& other ) const noexcept;

		/**
		 * @brief Greater than or equal operator
		 * @param other Right operand
		 * @return true if this is greater than or equal to other
		 */
		bool operator>=( const Int128& other ) const noexcept;

		//----------------------------------------------
		// Comparison with built-in integer types
		//----------------------------------------------

		/**
		 * @brief Equality comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if values are equal
		 */
		bool operator==( std::int64_t val ) const noexcept;

		/**
		 * @brief Inequality comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( std::int64_t val ) const noexcept;

		/**
		 * @brief Less than comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( std::int64_t val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( std::int64_t val ) const noexcept;

		/**
		 * @brief Greater than comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( std::int64_t val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with signed 64-bit integer
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( std::int64_t val ) const noexcept;

		/**
		 * @brief Equality comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if values are equal
		 */
		bool operator==( std::uint64_t val ) const noexcept;

		/**
		 * @brief Inequality comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( std::uint64_t val ) const noexcept;

		/**
		 * @brief Less than comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( std::uint64_t val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( std::uint64_t val ) const noexcept;

		/**
		 * @brief Greater than comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( std::uint64_t val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with unsigned 64-bit integer
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( std::uint64_t val ) const noexcept;

		/**
		 * @brief Equality comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if values are equal
		 */
		bool operator==( int val ) const noexcept;

		/**
		 * @brief Inequality comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( int val ) const noexcept;

		/**
		 * @brief Less than comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( int val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( int val ) const noexcept;

		/**
		 * @brief Greater than comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( int val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with signed 32-bit integer
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( int val ) const noexcept;

		//----------------------------------------------
		// Comparison with built-in floating point types
		//----------------------------------------------

		/**
		 * @brief Equality comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if values are equal (within floating-point precision)
		 */
		bool operator==( double val ) const noexcept;

		/**
		 * @brief Inequality comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( double val ) const noexcept;

		/**
		 * @brief Less than comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( double val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( double val ) const noexcept;

		/**
		 * @brief Greater than comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( double val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with double-precision floating-point
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( double val ) const noexcept;

		/**
		 * @brief Equality comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if values are equal (within floating-point precision)
		 */
		bool operator==( float val ) const noexcept;

		/**
		 * @brief Inequality comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( float val ) const noexcept;

		/**
		 * @brief Less than comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( float val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( float val ) const noexcept;

		/**
		 * @brief Greater than comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( float val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with single-precision floating-point
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( float val ) const noexcept;

		//----------------------------------------------
		// Comparison with nfx Decimal
		//----------------------------------------------

		/**
		 * @brief Equality comparison with Decimal
		 * @param val Right operand
		 * @return true if values are equal
		 * @note For equality, the Decimal must have no fractional part and represent the same integer value
		 */
		bool operator==( const Decimal& val ) const noexcept;

		/**
		 * @brief Inequality comparison with Decimal
		 * @param val Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( const Decimal& val ) const noexcept;

		/**
		 * @brief Less than comparison with Decimal
		 * @param val Right operand
		 * @return true if this is less than val
		 */
		bool operator<( const Decimal& val ) const noexcept;

		/**
		 * @brief Less than or equal comparison with Decimal
		 * @param val Right operand
		 * @return true if this is less than or equal to val
		 */
		bool operator<=( const Decimal& val ) const noexcept;

		/**
		 * @brief Greater than comparison with Decimal
		 * @param val Right operand
		 * @return true if this is greater than val
		 */
		bool operator>( const Decimal& val ) const noexcept;

		/**
		 * @brief Greater than or equal comparison with Decimal
		 * @param val Right operand
		 * @return true if this is greater than or equal to val
		 */
		bool operator>=( const Decimal& val ) const noexcept;

		//----------------------------------------------
		// String parsing and conversion
		//----------------------------------------------

		/**
		 * @brief Parse 128-bit integer from string
		 * @param str String representation of integer (e.g., "12345", "-9876543210")
		 * @return Parsed Int128 value
		 * @throws std::invalid_argument if string format is invalid or represents value outside Int128 range
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static Int128 parse( std::string_view str );

		/**
		 * @brief Try to parse 128-bit integer from string without throwing
		 * @param str String representation of integer (e.g., "12345", "-9876543210")
		 * @param result Output parameter to store parsed value on success
		 * @return true if parsing succeeded, false if string format is invalid or value is outside Int128 range
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, Int128& result ) noexcept;

		//----------------------------------------------
		// Type conversion
		//----------------------------------------------

		/**
		 * @brief Convert to string with exact precision
		 * @return String representation
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get internal 32-bit representation
		 * @return Array of 4 32-bit integers representing the 128-bit integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::array<std::int32_t, 4> toBits() const noexcept;

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
		// Mathematical operations
		//----------------------------------------------

		/**
		 * @brief Get absolute value
		 * @return Absolute value of the integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] Int128 abs() const noexcept;

		//----------------------------------------------
		// Access operations
		//----------------------------------------------

		/**
		 * @brief Get lower 64 bits
		 * @return Lower 64 bits as unsigned integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::uint64_t toLow() const noexcept;

		/**
		 * @brief Get upper 64 bits
		 * @return Upper 64 bits as unsigned integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::uint64_t toHigh() const noexcept;

#if NFX_CORE_HAS_INT128
		/**
		 * @brief Get native 128-bit integer value (GCC/Clang only)
		 * @return Native __int128 value for direct use with compiler intrinsics
		 * @details This method provides access to the underlying native 128-bit integer
		 *          when compiled with GCC or Clang. Useful for interfacing with APIs
		 *          that expect native __int128 types or for maximum performance operations.
		 *
		 *          Performance benefits:
		 *          - Zero-cost conversion to native type
		 *          - Direct hardware instruction utilization
		 *          - Seamless integration with compiler intrinsics
		 *          - Optimal performance for bulk arithmetic operations
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		constexpr NFX_CORE_INT128 toNative() const noexcept;
#endif

	private:
		//----------------------------------------------
		// Platform-specific storage
		//----------------------------------------------

#if NFX_CORE_HAS_INT128
		/** @brief Native 128-bit integer storage (GCC/Clang) */
		NFX_CORE_INT128 m_value;
#else

		struct Layout
		{
			/** @brief Manual 128-bit storage using two 64-bit words (MSVC) */
			std::uint64_t lower64bits;
			std::uint64_t upper64bits;
		} m_layout;
#endif
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator
	 * @param os Output stream
	 * @param value Int128 value to output
	 * @return Reference to output stream
	 */
	std::ostream& operator<<( std::ostream& os, const Int128& value );

	/**
	 * @brief Input stream operator
	 * @param is Input stream
	 * @param value Int128 value to input
	 * @return Reference to input stream
	 */
	std::istream& operator>>( std::istream& is, Int128& value );
} // namespace nfx::datatypes

#include "nfx/detail/datatypes/Int128.inl"
