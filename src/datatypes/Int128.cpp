/**
 * @file Int128.cpp
 * @brief Implementation of cross-platform 128-bit integer arithmetic
 * @details Provides exact 128-bit integer arithmetic with portable operations
 */

#include <istream>
#include <ostream>

#include "nfx/datatypes/constants/Int128Constants.h"
#include "nfx/datatypes/Int128.h"

namespace nfx::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

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
}
