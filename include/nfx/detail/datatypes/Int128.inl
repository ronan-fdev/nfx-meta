/**
 * @file Int128.inl
 * @brief Inline implementations for cross-platform Int128 class
 */

#include <cmath>
#include <stdexcept>
#include <string_view>

namespace nfx::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

#if NFX_CORE_HAS_INT128
	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Int128::Int128() noexcept
		: m_value{ 0 }
	{
	}

	inline constexpr Int128::Int128( NFX_CORE_INT128 val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t low, std::uint64_t high ) noexcept
		: m_value{ static_cast<NFX_CORE_INT128>( high ) << 64 | low } {}

	inline constexpr Int128::Int128( std::uint64_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::int64_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( std::uint32_t val ) noexcept
		: m_value{ val }
	{
	}

	inline constexpr Int128::Int128( int val ) noexcept
		: m_value{ val }
	{
	}

	inline Int128::Int128( std::string_view str )
	{
		if ( !tryParse( str, *this ) )
		{
			throw std::invalid_argument{ "Invalid Int128 string format" };
		}
	}

	//----------------------------------------------
	// Arithmetic operations
	//----------------------------------------------

	inline Int128 Int128::operator+( const Int128& other ) const noexcept
	{
		return Int128{ m_value + other.m_value };
	}

	inline Int128 Int128::operator-( const Int128& other ) const noexcept
	{
		return Int128{ m_value - other.m_value };
	}

	inline Int128 Int128::operator*( const Int128& other ) const noexcept
	{
		return Int128{ m_value * other.m_value };
	}

	inline Int128 Int128::operator/( const Int128& other ) const
	{
		if ( other.m_value == 0 )
		{
			throw std::overflow_error{ "Division by zero" };
		}

		return Int128{ m_value / other.m_value };
	}

	inline Int128 Int128::operator%( const Int128& other ) const
	{
		if ( other.m_value == 0 )
		{
			throw std::overflow_error{ "Division by zero" };
		}

		return Int128{ m_value % other.m_value };
	}

	inline Int128 Int128::operator-() const noexcept
	{
		return Int128{ -m_value };
	}

	//----------------------------------------------
	// Comparison operations
	//----------------------------------------------

	inline bool Int128::operator==( const Int128& other ) const noexcept
	{
		return m_value == other.m_value;
	}

	inline bool Int128::operator!=( const Int128& other ) const noexcept
	{
		return m_value != other.m_value;
	}

	inline bool Int128::operator<( const Int128& other ) const noexcept
	{
		return m_value < other.m_value;
	}

	inline bool Int128::operator<=( const Int128& other ) const noexcept
	{
		return m_value <= other.m_value;
	}

	inline bool Int128::operator>( const Int128& other ) const noexcept
	{
		return m_value > other.m_value;
	}

	inline bool Int128::operator>=( const Int128& other ) const noexcept
	{
		return m_value >= other.m_value;
	}

	//----------------------------------------------
	// Comparison with built-in integer types
	//----------------------------------------------

	inline bool Int128::operator==( std::int64_t val ) const noexcept
	{
		return m_value == val;
	}

	inline bool Int128::operator!=( std::int64_t val ) const noexcept
	{
		return m_value != val;
	}

	inline bool Int128::operator<( std::int64_t val ) const noexcept
	{
		return m_value < val;
	}

	inline bool Int128::operator<=( std::int64_t val ) const noexcept
	{
		return m_value <= val;
	}

	inline bool Int128::operator>( std::int64_t val ) const noexcept
	{
		return m_value > val;
	}

	inline bool Int128::operator>=( std::int64_t val ) const noexcept
	{
		return m_value >= val;
	}

	inline bool Int128::operator==( std::uint64_t val ) const noexcept
	{
		return m_value >= 0 && static_cast<std::uint64_t>( m_value ) == val;
	}

	inline bool Int128::operator!=( std::uint64_t val ) const noexcept
	{
		return m_value < 0 || static_cast<std::uint64_t>( m_value ) != val;
	}

	inline bool Int128::operator<( std::uint64_t val ) const noexcept
	{
		return m_value < 0 || static_cast<std::uint64_t>( m_value ) < val;
	}

	inline bool Int128::operator<=( std::uint64_t val ) const noexcept
	{
		return m_value < 0 || static_cast<std::uint64_t>( m_value ) <= val;
	}

	inline bool Int128::operator>( std::uint64_t val ) const noexcept
	{
		return m_value >= 0 && static_cast<std::uint64_t>( m_value ) > val;
	}

	inline bool Int128::operator>=( std::uint64_t val ) const noexcept
	{
		return m_value >= 0 && static_cast<std::uint64_t>( m_value ) >= val;
	}

	inline bool Int128::operator==( int val ) const noexcept
	{
		return m_value == val;
	}

	inline bool Int128::operator!=( int val ) const noexcept
	{
		return m_value != val;
	}

	inline bool Int128::operator<( int val ) const noexcept
	{
		return m_value < val;
	}

	inline bool Int128::operator<=( int val ) const noexcept
	{
		return m_value <= val;
	}

	inline bool Int128::operator>( int val ) const noexcept
	{
		return m_value > val;
	}

	inline bool Int128::operator>=( int val ) const noexcept
	{
		return m_value >= val;
	}

	//----------------------------------------------
	// Comparison with built-in floating point types
	//----------------------------------------------

	inline bool Int128::operator==( double val ) const noexcept
	{
		// Convert to long double for better precision comparison
		return static_cast<long double>( m_value ) == static_cast<long double>( val );
	}

	inline bool Int128::operator!=( double val ) const noexcept
	{
		return !( *this == val );
	}

	inline bool Int128::operator<( double val ) const noexcept
	{
		return static_cast<long double>( m_value ) < static_cast<long double>( val );
	}

	inline bool Int128::operator<=( double val ) const noexcept
	{
		return *this < val || *this == val;
	}

	inline bool Int128::operator>( double val ) const noexcept
	{
		return static_cast<long double>( m_value ) > static_cast<long double>( val );
	}

	inline bool Int128::operator>=( double val ) const noexcept
	{
		return *this > val || *this == val;
	}

	inline bool Int128::operator==( float val ) const noexcept
	{
		return *this == static_cast<double>( val );
	}

	inline bool Int128::operator!=( float val ) const noexcept
	{
		return *this != static_cast<double>( val );
	}

	inline bool Int128::operator<( float val ) const noexcept
	{
		return *this < static_cast<double>( val );
	}

	inline bool Int128::operator<=( float val ) const noexcept
	{
		return *this <= static_cast<double>( val );
	}

	inline bool Int128::operator>( float val ) const noexcept
	{
		return *this > static_cast<double>( val );
	}

	inline bool Int128::operator>=( float val ) const noexcept
	{
		return *this >= static_cast<double>( val );
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Int128::isZero() const noexcept
	{
		return m_value == 0;
	}

	inline bool Int128::isNegative() const noexcept
	{
		return m_value < 0;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	inline Int128 Int128::abs() const noexcept
	{
		return Int128{ m_value < 0
						   ? -m_value
						   : m_value };
	}

	//----------------------------------------------
	// Access operations
	//----------------------------------------------

	inline std::uint64_t Int128::toLow() const noexcept
	{
		return static_cast<std::uint64_t>( m_value );
	}

	inline std::uint64_t Int128::toHigh() const noexcept
	{
		return static_cast<std::uint64_t>( m_value >> 64 );
	}

	constexpr NFX_CORE_INT128 Int128::toNative() const noexcept
	{
		return m_value;
	}
#else

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Int128::Int128() noexcept
		: m_layout{ 0, 0 }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t low, std::uint64_t high ) noexcept
		: m_layout{ low, high }
	{
	}

	inline constexpr Int128::Int128( std::uint64_t val ) noexcept
		: m_layout{ val, 0 }
	{
	}

	inline constexpr Int128::Int128( std::int64_t val ) noexcept
		: m_layout{ static_cast<std::uint64_t>( val ), ( val < 0 ) ? static_cast<std::uint64_t>( -1 )
																   : 0 }
	{
	}

	inline constexpr Int128::Int128( std::uint32_t val ) noexcept
		: m_layout{ val, 0 }
	{
	}

	inline constexpr Int128::Int128( int val ) noexcept
		: m_layout{ static_cast<std::uint64_t>( val ), ( val < 0 ) ? static_cast<std::uint64_t>( -1 )
																   : 0 }
	{
	}

	inline Int128::Int128( std::string_view str )
	{
		if ( !tryParse( str, *this ) )
		{
			throw std::invalid_argument{ "Invalid Int128 string format" };
		}
	}

	//----------------------------------------------
	// Arithmetic operations
	//----------------------------------------------

	inline Int128 Int128::operator+( const Int128& other ) const noexcept
	{
		// 128-bit addition with carry propagation
		std::uint64_t result_low{ m_layout.lower64bits + other.m_layout.lower64bits };
		std::uint64_t carry{ ( result_low < m_layout.lower64bits ) ? 1ULL : 0ULL };
		std::uint64_t result_high{ m_layout.upper64bits + other.m_layout.upper64bits + carry };

		return Int128{ result_low, result_high };
	}

	inline Int128 Int128::operator-( const Int128& other ) const noexcept
	{
		// 128-bit subtraction with borrow propagation
		std::uint64_t result_low{ m_layout.lower64bits - other.m_layout.lower64bits };
		std::uint64_t borrow{ ( m_layout.lower64bits < other.m_layout.lower64bits ) ? 1ULL : 0ULL };
		std::uint64_t result_high{ m_layout.upper64bits - other.m_layout.upper64bits - borrow };

		return Int128{ result_low, result_high };
	}

	inline Int128 Int128::operator*( const Int128& other ) const noexcept
	{
		/*
		 * 128-bit multiplication using Karatsuba-style algorithm (https://en.wikipedia.org/wiki/Karatsuba_algorithm)
		 * Performance: Breaks 64x64 multiplication into 32x32 operations
		 * to leverage hardware multipliers efficiently on all platforms
		 */
		std::uint64_t a_low{ m_layout.lower64bits & 0xFFFFFFFFULL };
		std::uint64_t a_high{ m_layout.lower64bits >> 32 };
		std::uint64_t b_low{ other.m_layout.lower64bits & 0xFFFFFFFFULL };
		std::uint64_t b_high{ other.m_layout.lower64bits >> 32 };

		// Four 32x32->64 multiplications
		std::uint64_t p0{ a_low * b_low };
		std::uint64_t p1{ a_low * b_high };
		std::uint64_t p2{ a_high * b_low };
		std::uint64_t p3{ a_high * b_high };

		// Carry computation for intermediate sum
		std::uint64_t carry{ ( ( p0 >> 32 ) + ( p1 & 0xFFFFFFFFULL ) + ( p2 & 0xFFFFFFFFULL ) ) >> 32 };

		// Final result assembly
		std::uint64_t result_low{ p0 + ( p1 << 32 ) + ( p2 << 32 ) };
		std::uint64_t result_high{ p3 + ( p1 >> 32 ) + ( p2 >> 32 ) + carry +
								   m_layout.upper64bits * other.m_layout.lower64bits +
								   m_layout.lower64bits * other.m_layout.upper64bits };

		return Int128{ result_low, result_high };
	}

	inline Int128 Int128::operator/( const Int128& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error{ "Division by zero" };
		}

		/*
		 * Performance optimization: Fast path for 64-bit values
		 * Avoids expensive 128-bit division when possible
		 */
		if ( m_layout.upper64bits == 0 && other.m_layout.upper64bits == 0 )
		{
			// Both fit in 64-bit - use native division
			return Int128{ m_layout.lower64bits / other.m_layout.lower64bits, 0 };
		}

		/*
		 * Optimized path: dividend is 128-bit but divisor fits in 64-bit
		 * Use precise 128/64 division algorithm
		 */
		if ( other.m_layout.upper64bits == 0 )
		{
			std::uint64_t divisor{ other.m_layout.lower64bits };

			// Divide high part first
			std::uint64_t high_quotient{ m_layout.upper64bits / divisor };
			std::uint64_t high_remainder{ m_layout.upper64bits % divisor };

			/*
			 * Now divide (high_remainder << 64 + m_layout.lower64bits) by divisor
			 * This is equivalent to dividing a 128-bit number by a 64-bit number
			 */

			if ( high_remainder == 0 )
			{
				// Simple case: high part divides evenly
				std::uint64_t low_quotient{ m_layout.lower64bits / divisor };
				return Int128{ low_quotient, high_quotient };
			}
			else
			{
				/*
				 * Complex case: use long division for the remainder
				 * We need to compute (high_remainder * 2^64 + m_layout.lower64bits) / divisor
				 */

				// Split the lower 64 bits into two 32-bit parts for easier handling
				std::uint64_t low_high{ m_layout.lower64bits >> 32 };
				std::uint64_t low_low{ m_layout.lower64bits & 0xFFFFFFFFULL };

				// Divide (high_remainder << 32 + low_high) by divisor
				std::uint64_t temp_dividend{ ( high_remainder << 32 ) + low_high };
				std::uint64_t temp_quotient{ temp_dividend / divisor };
				std::uint64_t temp_remainder{ temp_dividend % divisor };

				// Divide (temp_remainder << 32 + low_low) by divisor
				std::uint64_t final_dividend{ ( temp_remainder << 32 ) + low_low };
				std::uint64_t final_quotient{ final_dividend / divisor };

				// Combine the quotients */
				std::uint64_t low_quotient{ ( temp_quotient << 32 ) + final_quotient };

				return Int128{ low_quotient, high_quotient };
			}
		}

		/*
		 * General case: 128-bit / 128-bit division using binary long division
		 * This handles all cases where both operands require the full 128-bit range
		 */

		// Handle sign for signed division
		bool result_negative{ false };
		Int128 abs_dividend{ *this };
		Int128 abs_divisor{ other };

		if ( abs_dividend.isNegative() )
		{
			result_negative = !result_negative;
			abs_dividend = -abs_dividend;
		}

		if ( abs_divisor.isNegative() )
		{
			result_negative = !result_negative;
			abs_divisor = -abs_divisor;
		}

		// Early exit for simple cases
		if ( abs_dividend < abs_divisor )
		{
			return Int128{ 0, 0 };
		}

		if ( abs_dividend == abs_divisor )
		{
			return result_negative ? Int128{ 0, 0 } - Int128{ 1, 0 } : Int128{ 1, 0 };
		}

		// Binary long division algorithm
		Int128 quotient{ 0, 0 };
		Int128 remainder{ 0, 0 };

		// Process bits from most significant to least significant
		for ( int i{ 127 }; i >= 0; --i )
		{
			// Shift remainder left by 1
			remainder = remainder + remainder; // equivalent to << 1

			// Set the least significant bit of remainder to the i-th bit of dividend
			if ( ( i >= 64 && ( ( abs_dividend.m_layout.upper64bits >> ( i - 64 ) ) & 1 ) ) ||
				 ( i < 64 && ( ( abs_dividend.m_layout.lower64bits >> i ) & 1 ) ) )
			{
				remainder = remainder + Int128{ 1, 0 };
			}

			// If remainder >= divisor, subtract divisor and set quotient bit
			if ( !( remainder < abs_divisor ) )
			{
				remainder = remainder - abs_divisor;

				// Set the i-th bit of quotient
				if ( i >= 64 )
				{
					quotient.m_layout.upper64bits |= ( 1ULL << ( i - 64 ) );
				}
				else
				{
					quotient.m_layout.lower64bits |= ( 1ULL << i );
				}
			}
		}

		return result_negative ? Int128{ 0, 0 } - quotient : quotient;
	}

	inline Int128 Int128::operator%( const Int128& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error{ "Division by zero" };
		}

		// Performance optimization: Fast modulo for 64-bit values
		if ( m_layout.upper64bits == 0 && other.m_layout.upper64bits == 0 )
		{
			// Both fit in 64-bit - use native modulo
			return Int128{ m_layout.lower64bits % other.m_layout.lower64bits, 0 };
		}

		/*
		 * General case: a % b = a - (a / b) * b
		 * Performance: Reuses division and multiplication implementations
		 */
		Int128 quotient{ *this / other };
		return *this - ( quotient * other );
	}

	inline Int128 Int128::operator-() const noexcept
	{
		// Two's complement negation
		Int128 result{ Int128{ ~m_layout.lower64bits, ~m_layout.upper64bits } };
		return Int128{ result + Int128{ 1 } };
	}

	//----------------------------------------------
	// Comparison operations
	//----------------------------------------------

	inline bool Int128::operator==( const Int128& other ) const noexcept
	{
		return m_layout.lower64bits == other.m_layout.lower64bits && m_layout.upper64bits == other.m_layout.upper64bits;
	}

	inline bool Int128::operator!=( const Int128& other ) const noexcept
	{
		return m_layout.lower64bits != other.m_layout.lower64bits || m_layout.upper64bits != other.m_layout.upper64bits;
	}

	inline bool Int128::operator<( const Int128& other ) const noexcept
	{
		if ( m_layout.upper64bits != other.m_layout.upper64bits )
		{
			return static_cast<std::int64_t>( m_layout.upper64bits ) < static_cast<std::int64_t>( other.m_layout.upper64bits );
		}
		return m_layout.lower64bits < other.m_layout.lower64bits;
	}

	inline bool Int128::operator<=( const Int128& other ) const noexcept
	{
		if ( m_layout.upper64bits != other.m_layout.upper64bits )
		{
			return static_cast<std::int64_t>( m_layout.upper64bits ) < static_cast<std::int64_t>( other.m_layout.upper64bits );
		}
		return m_layout.lower64bits <= other.m_layout.lower64bits;
	}

	inline bool Int128::operator>( const Int128& other ) const noexcept
	{
		if ( m_layout.upper64bits != other.m_layout.upper64bits )
		{
			return static_cast<std::int64_t>( m_layout.upper64bits ) > static_cast<std::int64_t>( other.m_layout.upper64bits );
		}
		return m_layout.lower64bits > other.m_layout.lower64bits;
	}

	inline bool Int128::operator>=( const Int128& other ) const noexcept
	{
		if ( m_layout.upper64bits != other.m_layout.upper64bits )
		{
			return static_cast<std::int64_t>( m_layout.upper64bits ) > static_cast<std::int64_t>( other.m_layout.upper64bits );
		}
		return m_layout.lower64bits >= other.m_layout.lower64bits;
	}

	//----------------------------------------------
	// Comparison with built-in integer types
	//----------------------------------------------

	inline bool Int128::operator==( std::int64_t val ) const noexcept
	{
		// For negative values, upper64bits should be all 1s (sign extension)
		// For positive values, upper64bits should be 0
		std::uint64_t expected_upper = ( val < 0 ) ? static_cast<std::uint64_t>( -1 ) : 0;
		return m_layout.upper64bits == expected_upper &&
			   m_layout.lower64bits == static_cast<std::uint64_t>( val );
	}

	inline bool Int128::operator!=( std::int64_t val ) const noexcept
	{
		return !( *this == val );
	}

	inline bool Int128::operator<( std::int64_t val ) const noexcept
	{
		return *this < Int128{ val };
	}

	inline bool Int128::operator<=( std::int64_t val ) const noexcept
	{
		return *this <= Int128{ val };
	}

	inline bool Int128::operator>( std::int64_t val ) const noexcept
	{
		return *this > Int128{ val };
	}

	inline bool Int128::operator>=( std::int64_t val ) const noexcept
	{
		return *this >= Int128{ val };
	}

	inline bool Int128::operator==( std::uint64_t val ) const noexcept
	{
		// For unsigned comparison, this Int128 must be non-negative
		return m_layout.upper64bits == 0 && m_layout.lower64bits == val;
	}

	inline bool Int128::operator!=( std::uint64_t val ) const noexcept
	{
		return !( *this == val );
	}

	inline bool Int128::operator<( std::uint64_t val ) const noexcept
	{
		// If this is negative, it's always less than any positive uint64_t
		if ( isNegative() )
			return true;
		// If upper bits are non-zero, this is definitely >= 2^64, so greater than any uint64_t
		if ( m_layout.upper64bits != 0 )
			return false;
		// Compare lower bits
		return m_layout.lower64bits < val;
	}

	inline bool Int128::operator<=( std::uint64_t val ) const noexcept
	{
		// If this is negative, it's always less than any positive uint64_t
		if ( isNegative() )
			return true;
		// If upper bits are non-zero, this is definitely >= 2^64, so greater than any uint64_t
		if ( m_layout.upper64bits != 0 )
			return false;
		// Compare lower bits
		return m_layout.lower64bits <= val;
	}

	inline bool Int128::operator>( std::uint64_t val ) const noexcept
	{
		// If this is negative, it's never greater than any positive uint64_t
		if ( isNegative() )
			return false;
		// If upper bits are non-zero, this is definitely >= 2^64, so greater than any uint64_t
		if ( m_layout.upper64bits != 0 )
			return true;
		// Compare lower bits
		return m_layout.lower64bits > val;
	}

	inline bool Int128::operator>=( std::uint64_t val ) const noexcept
	{
		// If this is negative, it's never greater than or equal to any positive uint64_t
		if ( isNegative() )
			return false;
		// If upper bits are non-zero, this is definitely >= 2^64, so greater than any uint64_t
		if ( m_layout.upper64bits != 0 )
			return true;
		// Compare lower bits
		return m_layout.lower64bits >= val;
	}

	inline bool Int128::operator==( int val ) const noexcept
	{
		return *this == static_cast<std::int64_t>( val );
	}

	inline bool Int128::operator!=( int val ) const noexcept
	{
		return *this != static_cast<std::int64_t>( val );
	}

	inline bool Int128::operator<( int val ) const noexcept
	{
		return *this < static_cast<std::int64_t>( val );
	}

	inline bool Int128::operator<=( int val ) const noexcept
	{
		return *this <= static_cast<std::int64_t>( val );
	}

	inline bool Int128::operator>( int val ) const noexcept
	{
		return *this > static_cast<std::int64_t>( val );
	}

	inline bool Int128::operator>=( int val ) const noexcept
	{
		return *this >= static_cast<std::int64_t>( val );
	}

	//----------------------------------------------
	// Comparison with built-in floating point types
	//----------------------------------------------

	inline bool Int128::operator==( double val ) const noexcept
	{
		if ( std::isnan( val ) || std::isinf( val ) )
		{
			return false; // Int128 has no NaN/Infinity representation
		}

		// Convert this Int128 to long double for comparison
		// For manual implementation, we need to carefully construct the value
		long double thisValue;
		if ( isNegative() )
		{
			// Handle negative values using two's complement
			Int128 abs_this = this->abs();
			thisValue = -( static_cast<long double>( abs_this.m_layout.upper64bits ) *
							   static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						   static_cast<long double>( abs_this.m_layout.lower64bits ) );
		}
		else
		{
			thisValue = static_cast<long double>( m_layout.upper64bits ) *
							static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						static_cast<long double>( m_layout.lower64bits );
		}

		return thisValue == static_cast<long double>( val );
	}

	inline bool Int128::operator!=( double val ) const noexcept
	{
		return !( *this == val );
	}

	inline bool Int128::operator<( double val ) const noexcept
	{
		if ( std::isnan( val ) )
		{
			return false; // No ordering with NaN
		}
		if ( std::isinf( val ) )
		{
			return val > 0.0; // Any finite value < +infinity, any finite value > -infinity
		}

		// Convert this Int128 to long double for comparison
		long double thisValue;
		if ( isNegative() )
		{
			Int128 abs_this = this->abs();
			thisValue = -( static_cast<long double>( abs_this.m_layout.upper64bits ) *
							   static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						   static_cast<long double>( abs_this.m_layout.lower64bits ) );
		}
		else
		{
			thisValue = static_cast<long double>( m_layout.upper64bits ) *
							static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						static_cast<long double>( m_layout.lower64bits );
		}

		return thisValue < static_cast<long double>( val );
	}

	inline bool Int128::operator<=( double val ) const noexcept
	{
		return *this < val || *this == val;
	}

	inline bool Int128::operator>( double val ) const noexcept
	{
		if ( std::isnan( val ) )
		{
			return false; // No ordering with NaN
		}
		if ( std::isinf( val ) )
		{
			return val < 0.0; // Any finite value > -infinity, any finite value < +infinity
		}

		// Convert this Int128 to long double for comparison
		long double thisValue;
		if ( isNegative() )
		{
			Int128 abs_this = this->abs();
			thisValue = -( static_cast<long double>( abs_this.m_layout.upper64bits ) *
							   static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						   static_cast<long double>( abs_this.m_layout.lower64bits ) );
		}
		else
		{
			thisValue = static_cast<long double>( m_layout.upper64bits ) *
							static_cast<long double>( 1ULL << 32 ) * static_cast<long double>( 1ULL << 32 ) +
						static_cast<long double>( m_layout.lower64bits );
		}

		return thisValue > static_cast<long double>( val );
	}

	inline bool Int128::operator>=( double val ) const noexcept
	{
		return *this > val || *this == val;
	}

	inline bool Int128::operator==( float val ) const noexcept
	{
		return *this == static_cast<double>( val );
	}

	inline bool Int128::operator!=( float val ) const noexcept
	{
		return *this != static_cast<double>( val );
	}

	inline bool Int128::operator<( float val ) const noexcept
	{
		return *this < static_cast<double>( val );
	}

	inline bool Int128::operator<=( float val ) const noexcept
	{
		return *this <= static_cast<double>( val );
	}

	inline bool Int128::operator>( float val ) const noexcept
	{
		return *this > static_cast<double>( val );
	}

	inline bool Int128::operator>=( float val ) const noexcept
	{
		return *this >= static_cast<double>( val );
	}

	//----------------------------------------------
	// Comparison with nfx Decimal
	//----------------------------------------------

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Int128::isZero() const noexcept
	{
		return m_layout.lower64bits == 0 && m_layout.upper64bits == 0;
	}

	inline bool Int128::isNegative() const noexcept
	{
		return static_cast<std::int64_t>( m_layout.upper64bits ) < 0;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	inline Int128 Int128::abs() const noexcept
	{
		if ( !isNegative() )
		{
			return *this;
		}
		return -*this;
	}

	//----------------------------------------------
	// Access operations
	//----------------------------------------------

	inline std::uint64_t Int128::toLow() const noexcept
	{
		return m_layout.lower64bits;
	}

	inline std::uint64_t Int128::toHigh() const noexcept
	{
		return m_layout.upper64bits;
	}

#endif
}
