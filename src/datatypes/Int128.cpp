/**
 * @file Int128.cpp
 * @brief Implementation of cross-platform 128-bit integer arithmetic
 * @details Provides exact 128-bit integer arithmetic with portable operations
 */

#include <istream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

#include "nfx/datatypes/constants/Int128Constants.h"
#include "nfx/datatypes/Decimal.h"
#include "nfx/datatypes/Int128.h"

namespace nfx::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Int128::Int128( float val )
	{
		// Convert float to Int128, truncating fractional part (like static_cast<int>(float))
		if ( std::isnan( val ) || std::isinf( val ) )
		{
			// NaN or infinity - set to zero (matches some C++ implementations)
#if NFX_CORE_HAS_INT128
			m_value = 0;
#else
			m_layout.lower64bits = 0;
			m_layout.upper64bits = 0;
#endif
			return;
		}

		// Truncate toward zero (standard C++ behavior)
		double truncated = std::trunc( static_cast<double>( val ) );

		// Check for overflow
		if ( truncated > static_cast<double>( std::numeric_limits<std::int64_t>::max() ) ||
			 truncated < static_cast<double>( std::numeric_limits<std::int64_t>::min() ) )
		{
			// For values outside int64 range, we need more complex handling
			// For now, clamp to int64 range (could also throw overflow_error)
			if ( truncated > 0 )
			{
				*this = Int128{ std::numeric_limits<std::int64_t>::max() };
			}
			else
			{
				*this = Int128{ std::numeric_limits<std::int64_t>::min() };
			}
			return;
		}

		// Safe to convert via int64
		*this = Int128{ static_cast<std::int64_t>( truncated ) };
	}

	Int128::Int128( double val )
	{
		// Convert double to Int128, truncating fractional part (like static_cast<int>(double))
		if ( std::isnan( val ) || std::isinf( val ) )
		{
			// NaN or infinity - set to zero (matches some C++ implementations)
#if NFX_CORE_HAS_INT128
			m_value = 0;
#else
			m_layout.lower64bits = 0;
			m_layout.upper64bits = 0;
#endif
			return;
		}

		// Truncate toward zero (standard C++ behavior)
		double truncated = std::trunc( val );

		// Check for overflow - double can represent much larger values than Int128
		// Int128 max is approximately 1.7e38, double can go up to ~1.8e308
		const double int128_max_approx = 1.7014118346046923e38;	 // 2^127 - 1
		const double int128_min_approx = -1.7014118346046924e38; // -2^127

		if ( truncated > int128_max_approx || truncated < int128_min_approx )
		{
			// Overflow - clamp to Int128 range
			if ( truncated > 0 )
			{
				// Set to maximum positive Int128
#if NFX_CORE_HAS_INT128
				m_value = std::numeric_limits<NFX_CORE_INT128>::max();
#else
				m_layout.lower64bits = 0xFFFFFFFFFFFFFFFFULL;
				m_layout.upper64bits = 0x7FFFFFFFFFFFFFFFULL;
#endif
			}
			else
			{
				// Set to minimum negative Int128
#if NFX_CORE_HAS_INT128
				m_value = std::numeric_limits<NFX_CORE_INT128>::min();
#else
				m_layout.lower64bits = 0;
				m_layout.upper64bits = 0x8000000000000000ULL;
#endif
			}
			return;
		}

		// For values within reasonable range, convert via string parsing for accuracy
		// This avoids floating-point precision issues
		std::ostringstream oss;
		oss << std::fixed << std::setprecision( 0 ) << truncated;
		*this = Int128::parse( oss.str() );
	}

	Int128::Int128( const Decimal& decimal )
	{
		// Note: Following C++ standard behavior - truncate fractional parts
		// (similar to static_cast<int>(double) which truncates toward zero)

		// Handle zero case
		if ( decimal.isZero() )
		{
#if NFX_CORE_HAS_INT128
			m_value = 0;
#else
			m_layout.lower64bits = 0;
			m_layout.upper64bits = 0;
#endif
			return;
		}

		// Extract integer part from Decimal (truncate fractional part)
		// If Decimal has scale, we need to divide by 10^scale to get integer part
		Int128 integerPart;

		// Get the mantissa first
		const auto& mantissaArray = decimal.mantissa();
		std::uint64_t low = static_cast<std::uint64_t>( mantissaArray[1] ) << 32 | mantissaArray[0];
		std::uint64_t high = mantissaArray[2];

		// Build the full mantissa value
#if NFX_CORE_HAS_INT128
		NFX_CORE_INT128 mantissaValue = static_cast<NFX_CORE_INT128>( high ) << 64 | low;
		integerPart = Int128{ mantissaValue };
#else
		integerPart = Int128{ low, high };
#endif

		// If there's a scale, divide by 10^scale to get integer part (truncating)
		if ( decimal.scale() > 0 )
		{
			Int128 divisor{ 1 };
			for ( std::uint8_t i = 0; i < decimal.scale(); ++i )
			{
				divisor = divisor * Int128{ 10 };
			}
			integerPart = integerPart / divisor; // Integer division truncates
		}

		bool isNegative = decimal.isNegative();

		// Apply sign to the integer part
		if ( isNegative )
		{
			integerPart = -integerPart;
		}

		// Copy the result to this object
		*this = integerPart;
	}

	//----------------------------------------------
	// Comparison with nfx Decimal
	//----------------------------------------------

	bool Int128::operator==( const Decimal& val ) const noexcept
	{
		// If Decimal has fractional part, it can't equal an integer
		if ( val.scale() > 0 )
		{
			return false;
		}

		// Handle signs
		if ( isNegative() != val.isNegative() )
		{
			// Different signs can only be equal if both are zero
			return isZero() && val.isZero();
		}

		// Convert Decimal's mantissa to Int128 for comparison
		Int128 decimalMantissa;

		// Extract mantissa from Decimal's internal representation
		const auto& mantissaArray = val.mantissa();
		std::uint64_t low = static_cast<std::uint64_t>( mantissaArray[1] ) << 32 | mantissaArray[0];
		std::uint64_t high = mantissaArray[2];

#if NFX_CORE_HAS_INT128
		NFX_CORE_INT128 mantissaValue = static_cast<NFX_CORE_INT128>( high ) << 64 | low;
		decimalMantissa = Int128{ mantissaValue };
#else
		decimalMantissa = Int128{ low, high };
#endif

		// Compare absolute values
		Int128 thisAbs = abs();
		return thisAbs == decimalMantissa;
	}

	bool Int128::operator!=( const Decimal& val ) const noexcept
	{
		return !( *this == val );
	}

	bool Int128::operator<( const Decimal& val ) const noexcept
	{
		// Handle different signs
		if ( isNegative() && !val.isNegative() )
		{
			return true; // Negative < Non-negative
		}
		if ( !isNegative() && val.isNegative() )
		{
			return false; // Non-negative > Negative
		}

		// Same signs - need to handle Decimal's scale
		Int128 decimalMantissa;
		const auto& mantissaArray = val.mantissa();
		std::uint64_t low = static_cast<std::uint64_t>( mantissaArray[1] ) << 32 | mantissaArray[0];
		std::uint64_t high = mantissaArray[2];

#if NFX_CORE_HAS_INT128
		NFX_CORE_INT128 mantissaValue = static_cast<NFX_CORE_INT128>( high ) << 64 | low;
		decimalMantissa = Int128{ mantissaValue };
#else
		decimalMantissa = Int128{ low, high };
#endif

		if ( val.scale() > 0 )
		{
			// Decimal has fractional part - scale up this integer for comparison
			Int128 scaledThis = *this;

			// Apply power of 10 scaling
			for ( std::uint8_t i = 0; i < val.scale(); ++i )
			{
				scaledThis = scaledThis * Int128{ 10 };
			}

			if ( isNegative() )
			{
				// Both negative - compare absolute values with flipped result
				return scaledThis.abs() > decimalMantissa;
			}
			else
			{
				return scaledThis < decimalMantissa;
			}
		}
		else
		{
			// No fractional part - direct comparison of absolute values
			if ( isNegative() )
			{
				// Both negative - flip comparison
				return abs() > decimalMantissa;
			}
			else
			{
				return abs() < decimalMantissa;
			}
		}
	}

	bool Int128::operator<=( const Decimal& val ) const noexcept
	{
		return *this < val || *this == val;
	}

	bool Int128::operator>( const Decimal& val ) const noexcept
	{
		return !( *this <= val );
	}

	bool Int128::operator>=( const Decimal& val ) const noexcept
	{
		return !( *this < val );
	}

	//----------------------------------------------
	// String parsing and conversion
	//----------------------------------------------

	Int128 Int128::parse( std::string_view str )
	{
		Int128 result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument{ "Invalid Int128 string format" };
		}

		return result;
	}

	bool Int128::tryParse( std::string_view str, Int128& result ) noexcept
	{
		try
		{
			if ( str.empty() )
			{
				return false;
			}

			// Handle sign
			bool isNegative = false;
			size_t pos = 0;

			if ( str[0] == '-' )
			{
				isNegative = true;
				pos = 1;
			}
			else if ( str[0] == '+' )
			{
				pos = 1;
			}

			// Check if we have digits after sign
			if ( pos >= str.length() )
			{
				return false;
			}

			// Parse digits and build the number
			result = Int128{ 0 };

			// Quick overflow check: if string is too long, it's definitely overflow
			size_t digitCount = str.length() - pos;
			if ( digitCount > 39 ) // 128-bit signed max has 39 digits
			{
				return false;
			}

			// For exactly 39 digits, we need to check against max values
			if ( digitCount == 39 )
			{
				std::string_view digits = str.substr( pos );

				if ( !isNegative )
				{
					// Check against max positive
					if ( digits > constants::int128::MAX_POSITIVE_STRING )
					{
						return false;
					}
				}
				else
				{
					// Check against max negative (absolute value)
					if ( digits > constants::int128::MAX_NEGATIVE_STRING )
					{
						return false;
					}
				}
			}

			for ( size_t i = pos; i < str.length(); ++i )
			{
				char c{ str[i] };
				if ( c < '0' || c > '9' )
				{
					return false; // Invalid character
				}

				int digit = c - '0';
				result = result * Int128{ constants::int128::BASE } + Int128{ digit };
			}

			// Apply sign
			if ( isNegative )
			{
				result = -result;
			}

			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//----------------------------------------------
	// Type conversion
	//----------------------------------------------

	std::string Int128::toString() const
	{
		if ( isZero() )
		{
			return "0";
		}

		// Handle the special case of minimum value (-2^127)
		// This value cannot be represented positively in 128-bit signed integer
#if NFX_CORE_HAS_INT128
		if ( toNative() == static_cast<NFX_CORE_INT128>( constants::int128::MIN_NEGATIVE_HIGH ) << 64 )
#else
		if ( toHigh() == constants::int128::MIN_NEGATIVE_HIGH && toLow() == constants::int128::MIN_NEGATIVE_LOW )
#endif
		{
			return "-" + std::string{ constants::int128::MAX_NEGATIVE_STRING };
		}

		std::string result;
		result.reserve( 40 ); // Reserve space for efficiency (128-bit max ~39 digits)

		Int128 temp = abs();

		// Extract digits by repeated division by 10
		while ( !temp.isZero() )
		{
			Int128 quotient = temp / Int128{ constants::int128::BASE };
			Int128 remainder = temp % Int128{ constants::int128::BASE };

			// remainder should be 0-9, extract as single digit
			char digit{ static_cast<char>( '0' + remainder.toLow() ) };
			result = digit + result;

			temp = quotient;
		}

		if ( isNegative() )
		{
			result = '-' + result;
		}

		return result;
	}

	std::array<std::int32_t, 4> Int128::toBits() const noexcept
	{
		std::array<std::int32_t, 4> bits{};

#if NFX_CORE_HAS_INT128
		// For native __int128, extract 32-bit chunks
		auto nativeValue = toNative();
		bits[0] = static_cast<std::int32_t>( nativeValue );
		bits[1] = static_cast<std::int32_t>( nativeValue >> 32 );
		bits[2] = static_cast<std::int32_t>( nativeValue >> 64 );
		bits[3] = static_cast<std::int32_t>( nativeValue >> 96 );
#else
		// For manual implementation, extract from low/high 64-bit words
		std::uint64_t low = toLow();
		std::uint64_t high = toHigh();

		bits[0] = static_cast<std::int32_t>( low );
		bits[1] = static_cast<std::int32_t>( low >> 32 );
		bits[2] = static_cast<std::int32_t>( high );
		bits[3] = static_cast<std::int32_t>( high >> 32 );
#endif

		return bits;
	}

	//----------------------------------------------
	// Stream operators
	//----------------------------------------------

	std::ostream& operator<<( std::ostream& os, const Int128& value )
	{
		using namespace constants::int128;

		// Simple string conversion for display purposes
		if ( value.isZero() )
		{
			return os << '0';
		}

		Int128 temp = value.abs();
		std::string result;

		// Extract digits by repeated division by 10
		while ( !temp.isZero() )
		{
			Int128 quotient = temp / Int128{ BASE };
			Int128 remainder = temp % Int128{ BASE };

			// remainder should be 0-9, extract as single digit
			char digit{ static_cast<char>( '0' + remainder.toLow() ) };
			result = digit + result;

			temp = quotient;
		}

		if ( value.isNegative() )
		{
			result = '-' + result;
		}

		return os << result;
	}

	std::istream& operator>>( std::istream& is, Int128& value )
	{
		std::string str;
		is >> str;

		if ( !Int128::tryParse( str, value ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
} // namespace nfx::datatypes
