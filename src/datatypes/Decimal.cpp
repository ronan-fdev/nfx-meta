/**
 * @file Decimal.cpp
 * @brief Implementation of cross-platform 128-bit decimal arithmetic
 * @details Provides exact decimal arithmetic with portable 128-bit operations
 */

#include <cmath>
#include <istream>
#include <ostream>

#include "nfx/config.h"

#include "nfx/datatypes/Decimal.h"
#include "nfx/datatypes/constants/DecimalConstants.h"

namespace nfx::datatypes
{
	namespace internal
	{
		//=====================================================================
		// Internal helper functions
		//=====================================================================

		/**
		 * @brief Get power of 10 as Int128 for any scale 0-28
		 * @param power The power (0-28)
		 * @return Int128 representing 10^power
		 */
		inline Int128 getPowerOf10( std::uint8_t power ) noexcept
		{
			if ( power < constants::decimal::POWER_TABLE_SIZE && constants::decimal::POWERS_OF_10[power] != 0 )
			{
				// Use 64-bit lookup table for powers 0-19
				return Int128{ constants::decimal::POWERS_OF_10[power] };
			}
			else if ( power >= 20 && power <= 28 )
			{
				// Use pre-computed 128-bit values for powers 20-28
				const auto& extended{ constants::decimal::EXTENDED_POWERS_OF_10[power - 20] };
				return Int128{ extended.first, extended.second };
			}
			else
			{
				// Fallback to iterative computation for invalid powers (shouldn't happen)
				Int128 result{ 1 };
				for ( std::uint8_t i{ 0 }; i < power; ++i )
				{
					result = result * Int128{ constants::decimal::BASE };
				}
				return result;
			}
		}

		/**
		 * @brief Extract 128-bit mantissa value from Decimal
		 * @param decimal The decimal value to extract mantissa from
		 * @return Int128 representation of the mantissa
		 */
		inline Int128 mantissaAsInt128( const Decimal& decimal ) noexcept
		{
#if NFX_CORE_HAS_INT128
			const auto& mantissaArray{ decimal.mantissa() };
			NFX_CORE_INT128 value{ static_cast<NFX_CORE_INT128>( mantissaArray[2] ) << 64 |
								   static_cast<NFX_CORE_INT128>( mantissaArray[1] ) << 32 |
								   static_cast<NFX_CORE_INT128>( mantissaArray[0] ) };

			return Int128{ value };
#else
			const auto& mantissaArray{ decimal.mantissa() };
			std::uint64_t low{ static_cast<std::uint64_t>( mantissaArray[1] ) << 32 | mantissaArray[0] };
			std::uint64_t high{ mantissaArray[2] };

			return Int128{ low, high };
#endif
		}

		/**
		 * @brief Align scales of two decimals for arithmetic operations
		 * @param decimal First decimal value
		 * @param other Second decimal value
		 * @return Pair of Int128 mantissas with aligned scales
		 */
		inline std::pair<Int128, Int128> alignScale( const Decimal& decimal, const Decimal& other )
		{
			Int128 left{ mantissaAsInt128( decimal ) };
			Int128 right{ mantissaAsInt128( other ) };

			std::uint8_t leftScale{ decimal.scale() };
			std::uint8_t rightScale{ other.scale() };

			// Optimized scaling using enhanced power-of-10 lookup with 128-bit support
			if ( leftScale < rightScale )
			{
				std::uint8_t scaleDiff{ static_cast<std::uint8_t>( rightScale - leftScale ) };
				left = left * getPowerOf10( scaleDiff );
			}
			else if ( rightScale < leftScale )
			{
				std::uint8_t scaleDiff{ static_cast<std::uint8_t>( leftScale - rightScale ) };
				right = right * getPowerOf10( scaleDiff );
			}

			return { std::move( left ), std::move( right ) };
		}

		/**
		 * @brief Set mantissa value in Decimal from Int128
		 * @param decimal The decimal to modify
		 * @param value The Int128 mantissa value to set
		 */
		inline void setMantissa( Decimal& decimal, const Int128& value ) noexcept
		{
#if NFX_CORE_HAS_INT128
			auto nativeValue{ value.toNative() };
			auto& mantissa{ decimal.mantissa() };
			mantissa[0] = static_cast<std::uint32_t>( nativeValue );
			mantissa[1] = static_cast<std::uint32_t>( nativeValue >> 32 );
			mantissa[2] = static_cast<std::uint32_t>( nativeValue >> 64 );
#else
			auto& mantissa{ decimal.mantissa() };
			std::uint64_t low{ value.toLow() };
			std::uint64_t high{ value.toHigh() };

			mantissa[0] = static_cast<std::uint32_t>( low );
			mantissa[1] = static_cast<std::uint32_t>( low >> 32 );
			mantissa[2] = static_cast<std::uint32_t>( high );
#endif
		}

		/**
		 * @brief Divide decimal mantissa by power of 10
		 * @param decimal The decimal to modify
		 * @param power The power of 10 to divide by (0-28)
		 */
		inline void divideByPowerOf10( Decimal& decimal, std::uint8_t power )
		{
			Int128 mantissa{ mantissaAsInt128( decimal ) };

			// Use enhanced power-of-10 lookup with full 128-bit support
			mantissa = mantissa / getPowerOf10( power );

			setMantissa( decimal, mantissa );
		}

		/**
		 * @brief Normalize decimal by removing trailing zeros and reducing scale
		 * @param decimal The decimal to normalize
		 */
		inline void normalize( Decimal& decimal ) noexcept
		{
			// Remove trailing zeros and reduce scale
			while ( decimal.scale() > 0 && ( mantissaAsInt128( decimal ) % Int128{ constants::decimal::BASE } ) == Int128{ 0 } )
			{
				divideByPowerOf10( decimal, 1U );
				std::uint8_t currentScale{ decimal.scale() };
				decimal.flags() = ( decimal.flags() & ~constants::decimal::SCALE_MASK ) |
								  ( static_cast<std::uint32_t>( currentScale - 1U )
									  << constants::decimal::SCALE_SHIFT );
			}
		}
	}

	//=====================================================================
	// Decimal class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Decimal::Decimal( double value ) noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		if ( std::isnan( value ) || std::isinf( value ) )
		{
			// Invalid values become zero
			return;
		}

		if ( value == 0.0 )
		{
			return;
		}

		// Extract sign
		bool negative{ value < 0.0 };
		if ( negative )
		{
			value = -value;
			m_layout.flags |= constants::decimal::SIGN_MASK;
		}

		/*
		 * Convert double to decimal with limited precision to match IEEE 754 behavior
		 * Find the appropriate scale but limit to what double can accurately represent
		 */
		std::uint8_t currentScale{ 0 };
		double scaledValue{ value };

		// Scale up while we have fractional parts and maintain precision
		while ( currentScale < 15 && currentScale < constants::decimal::MAXIMUM_PLACES && scaledValue != std::floor( scaledValue ) )
		{
			scaledValue *= 10.0;
			++currentScale;
		}

		// Convert to integer mantissa
		std::uint64_t mantissaValue{ static_cast<std::uint64_t>( std::round( scaledValue ) ) };

		// Set scale
		m_layout.flags |= ( static_cast<std::uint32_t>( currentScale ) << constants::decimal::SCALE_SHIFT );

		// Store mantissa
		m_layout.mantissa[0] = static_cast<std::uint32_t>( mantissaValue );
		m_layout.mantissa[1] = static_cast<std::uint32_t>( mantissaValue >> 32 );
		m_layout.mantissa[2] = 0;

		internal::normalize( *this );
	}

	Decimal::Decimal( std::int32_t value ) noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		if ( value < 0 )
		{
			m_layout.flags |= constants::decimal::SIGN_MASK;
			value = -value;
		}

		m_layout.mantissa[0] = static_cast<std::uint32_t>( value );
	}

	Decimal::Decimal( std::int64_t value ) noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		if ( value < 0 )
		{
			m_layout.flags |= constants::decimal::SIGN_MASK;
			value = -value;
		}

		m_layout.mantissa[0] = static_cast<std::uint32_t>( value );
		m_layout.mantissa[1] = static_cast<std::uint32_t>( value >> 32 );
	}

	Decimal::Decimal( std::uint32_t value ) noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		m_layout.mantissa[0] = value;
	}

	Decimal::Decimal( std::uint64_t value ) noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		m_layout.mantissa[0] = static_cast<std::uint32_t>( value );
		m_layout.mantissa[1] = static_cast<std::uint32_t>( value >> 32 );
	}

	Decimal::Decimal( std::string_view str )
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
		if ( !tryParse( str, *this ) )
		{
			throw std::invalid_argument{ "Invalid decimal string format" };
		}
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	Decimal Decimal::operator+( const Decimal& other )
	{
		if ( isZero() )
		{
			return other;
		}
		if ( other.isZero() )
		{
			return *this;
		}

		Decimal result;
		auto [left, right]{ internal::alignScale( *this, other ) };

		internal::setMantissa( result, left + right );
		result.m_layout.flags = ( m_layout.flags & ~constants::decimal::SCALE_MASK ) |
								( std::max( scale(), other.scale() ) << constants::decimal::SCALE_SHIFT );

		// Handle sign
		if ( isNegative() == other.isNegative() )
		{
			if ( isNegative() )
			{
				result.m_layout.flags |= constants::decimal::SIGN_MASK;
			}
		}
		else
		{
			// Different signs - need subtraction logic
			if ( left > right )
			{
				internal::setMantissa( result, left - right );
				if ( isNegative() )
				{
					result.m_layout.flags |= constants::decimal::SIGN_MASK;
				}
			}
			else
			{
				internal::setMantissa( result, right - left );
				if ( other.isNegative() )
				{
					result.m_layout.flags |= constants::decimal::SIGN_MASK;
				}
			}
		}

		internal::normalize( result );

		return result;
	}

	Decimal Decimal::operator-( const Decimal& other )
	{
		Decimal negatedOther{ other };

		negatedOther.m_layout.flags ^= constants::decimal::SIGN_MASK;

		return *this + negatedOther;
	}

	Decimal Decimal::operator*( const Decimal& other ) const
	{
		if ( isZero() || other.isZero() )
		{
			return Decimal{};
		}

		Decimal result;
		Int128 left{ internal::mantissaAsInt128( *this ) };
		Int128 right{ internal::mantissaAsInt128( other ) };

		// Calculate the product mantissa without storing it yet
		Int128 productMantissa{ left * right };

		// Combine scales
		std::uint8_t newScale{ static_cast<std::uint8_t>( scale() + other.scale() ) };

		// Check if the mantissa fits in 96 bits (max value: 2^96 - 1)
		const Int128 max96bit{ Int128::parse( "79228162514264337593543950335" ) }; // 2^96 - 1

		// If mantissa exceeds 96 bits OR scale exceeds maximum, we need to truncate precision
		while ( ( productMantissa > max96bit ) || ( newScale > constants::decimal::MAXIMUM_PLACES ) )
		{
			// Divide mantissa by 10 to reduce precision
			productMantissa = productMantissa / Int128{ 10 };
			newScale--;

			// Safety check to prevent infinite loop
			if ( newScale == 0 && productMantissa > max96bit )
			{
				/*
				 * If we still can't fit in 96 bits even with scale 0,
				 * the number is too large for Decimal representation
				 */
				break;
			}
		}

		// Now store the properly scaled mantissa
		internal::setMantissa( result, productMantissa );

		result.m_layout.flags = ( static_cast<std::uint32_t>( newScale ) << constants::decimal::SCALE_SHIFT );

		// Combine signs
		if ( isNegative() != other.isNegative() )
		{
			result.m_layout.flags |= constants::decimal::SIGN_MASK;
		}

		internal::normalize( result );

		return result;
	}

	Decimal Decimal::operator/( const Decimal& other ) const
	{
		if ( other.isZero() )
		{
			throw std::overflow_error{ "Division by zero" };
		}

		if ( isZero() )
		{
			return Decimal{};
		}

		Decimal result;
		Int128 dividend{ internal::mantissaAsInt128( *this ) };
		Int128 divisor{ internal::mantissaAsInt128( other ) };

		/**
		 * Scale adjustment for division:
		 * If dividend has scale d and divisor has scale s,
		 * result should have scale (d - s)
		 * To maintain precision, we ALWAYS scale up the dividend
		 */

		std::int32_t targetScale{ static_cast<std::int32_t>( scale() ) - static_cast<std::int32_t>( other.scale() ) };

		// Scale up dividend to maintain precision
		std::uint8_t extraPrecision{ 18 }; // Add extra precision digits
		for ( std::uint8_t i{ 0U }; i < extraPrecision; ++i )
		{
			// Check if scaling would cause overflow
			if ( dividend.toHigh() > ( 0x1999999999999999ULL ) ) // Check for overflow when multiplying by 10
			{
				break; // Stop scaling to prevent overflow
			}
			dividend = dividend * Int128{ 10 };
			targetScale++;
		}

		// If target scale would still be negative, scale up more
		if ( targetScale < 0 )
		{
			std::uint8_t scaleUp{ static_cast<std::uint8_t>( -targetScale ) };
			for ( std::uint8_t i{ 0U }; i < scaleUp && i < constants::decimal::MAXIMUM_PLACES; ++i )
			{
				if ( dividend.toHigh() > ( 0x1999999999999999ULL ) ) // Check for overflow when multiplying by 10
				{
					break; // Stop scaling to prevent overflow
				}
				dividend = dividend * Int128{ 10 };
				targetScale++;
			}
		}

		internal::setMantissa( result, dividend / divisor );
		result.m_layout.flags = ( static_cast<std::uint32_t>( targetScale ) << constants::decimal::SCALE_SHIFT );

		// Combine signs
		if ( isNegative() != other.isNegative() )
		{
			result.m_layout.flags |= constants::decimal::SIGN_MASK;
		}

		internal::normalize( result );

		return result;
	}

	Decimal& Decimal::operator+=( const Decimal& other )
	{
		*this = *this + other;
		return *this;
	}

	Decimal& Decimal::operator-=( const Decimal& other )
	{
		*this = *this - other;
		return *this;
	}

	Decimal& Decimal::operator*=( const Decimal& other )
	{
		*this = *this * other;
		return *this;
	}

	Decimal& Decimal::operator/=( const Decimal& other )
	{
		*this = *this / other;
		return *this;
	}

	Decimal Decimal::operator-() const noexcept
	{
		Decimal result{ *this };

		result.m_layout.flags ^= constants::decimal::SIGN_MASK;
		return result;
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	bool Decimal::operator==( const Decimal& other ) const noexcept
	{
		if ( isZero() && other.isZero() )
		{
			return true;
		}

		if ( isNegative() != other.isNegative() )
		{
			return false;
		}

		auto [left, right] = internal::alignScale( *this, other );

		return left == right;
	}

	bool Decimal::operator!=( const Decimal& other ) noexcept
	{
		return !( *this == other );
	}

	bool Decimal::operator<( const Decimal& other ) const noexcept
	{
		if ( isNegative() != other.isNegative() )
		{
			return isNegative();
		}

		auto [left, right] = internal::alignScale( *this, other );

		if ( isNegative() )
		{
			return left > right;
		}
		else
		{
			return left < right;
		}
	}

	bool Decimal::operator<=( const Decimal& other ) noexcept
	{
		return *this < other || *this == other;
	}

	bool Decimal::operator>( const Decimal& other ) noexcept
	{
		return !( *this <= other );
	}

	bool Decimal::operator>=( const Decimal& other ) noexcept
	{
		return !( *this < other );
	}

	//----------------------------------------------
	// String parsing and conversion
	//----------------------------------------------

	Decimal Decimal::parse( std::string_view str )
	{
		Decimal result;
		if ( !tryParse( str, result ) )
		{
			throw std::invalid_argument{ "Invalid decimal string format" };
		}
		return result;
	}

	bool Decimal::tryParse( std::string_view str, Decimal& result ) noexcept
	{
		try
		{
			result = Decimal{};

			if ( str.empty() )
			{
				return false;
			}

			// Handle sign
			bool negative{ false };
			size_t pos{ 0 };
			if ( str[0] == '-' )
			{
				negative = true;
				pos = 1;
			}
			else if ( str[0] == '+' )
			{
				pos = 1;
			}

			// Check if we have at least one character after sign
			if ( pos >= str.length() )
			{
				return false;
			}

			// Find decimal point and validate there's only one
			size_t decimalPos{ std::string_view::npos };
			std::uint8_t currentScale{ 0 };
			size_t decimalCount{ 0 };

			for ( size_t i{ pos }; i < str.length(); ++i )
			{
				if ( str[i] == '.' )
				{
					decimalCount++;
					if ( decimalCount > 1 )
					{
						return false;
					}

					decimalPos = i;
				}
			}

			if ( decimalPos != std::string_view::npos )
			{
				currentScale = static_cast<std::uint8_t>( str.length() - decimalPos - 1 );
				if ( currentScale > constants::decimal::MAXIMUM_PLACES )
				{
					currentScale = constants::decimal::MAXIMUM_PLACES;
				}
			}

			// Optimized digit accumulation
			Int128 mantissaValue;
			const Int128 ten{ 10 };
			bool hasDigits{ false };
			std::uint8_t significantDigits{ 0 };
			std::uint8_t decimalDigitsProcessed{ 0 };

			for ( size_t i{ pos }; i < str.length(); ++i )
			{
				if ( str[i] == '.' )
				{
					continue;
				}

				if ( str[i] < '0' || str[i] > '9' )
				{
					// Invalid character
					return false;
				}

				hasDigits = true;
				std::uint64_t digit{ static_cast<std::uint64_t>( str[i] - '0' ) };

				// .NET Decimal specification: maximum 28 significant digits
				if ( significantDigits >= constants::decimal::MAXIMUM_PLACES )
				{
					// Truncate excess digits - adjust scale based on actual decimal digits processed
					if ( decimalPos != std::string_view::npos )
					{
						currentScale = decimalDigitsProcessed;
					}
					break;
				}

				// Count significant digits (skip leading zeros only before decimal point)
				if ( digit != 0 || mantissaValue != Int128{ 0 } || ( decimalPos != std::string_view::npos && i > decimalPos ) )
				{
					significantDigits++;
				}

				// Count decimal digits processed
				if ( decimalPos != std::string_view::npos && i > decimalPos )
				{
					decimalDigitsProcessed++;
				}

				// Safe to accumulate this digit
				mantissaValue = mantissaValue * ten + Int128{ digit };
			}

			// Ensure we have at least one digit (prevents parsing ".", "+", "-", etc.)
			if ( !hasDigits )
			{
				return false;
			}

			// Check if mantissa fits in our 96-bit storage
			if ( mantissaValue.toHigh() > 0xFFFFFFFF )
			{
				// Value too large - truncate excess precision to fit
				while ( mantissaValue.toHigh() > 0xFFFFFFFF && currentScale > 0 )
				{
					mantissaValue = mantissaValue / Int128{ 10 };
					--currentScale;
				}

				// If still too large after removing all decimal places, truncate the integer part to fit
				while ( mantissaValue.toHigh() > 0xFFFFFFFF )
				{
					mantissaValue = mantissaValue / Int128{ 10 };
				}
			}

			// Set result
			if ( negative )
			{
				result.m_layout.flags |= constants::decimal::SIGN_MASK;
			}

			result.m_layout.flags |= ( static_cast<std::uint32_t>( currentScale ) << constants::decimal::SCALE_SHIFT );

			// Store the 96-bit mantissa
			std::uint64_t low{ mantissaValue.toLow() };
			std::uint64_t high{ mantissaValue.toHigh() };

			result.m_layout.mantissa[0] = static_cast<std::uint32_t>( low );
			result.m_layout.mantissa[1] = static_cast<std::uint32_t>( low >> 32 );
			result.m_layout.mantissa[2] = static_cast<std::uint32_t>( high );

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

	double Decimal::toDouble() const noexcept
	{
		Int128 mantissa{ internal::mantissaAsInt128( *this ) };

		double result;
#if NFX_CORE_HAS_INT128
		result = static_cast<double>( mantissa.toNative() );
#else
		// Convert 128-bit to double (approximate)
		result = static_cast<double>( mantissa.toHigh() ) * ( 1ULL << 32 ) * ( 1ULL << 32 ) +
				 static_cast<double>( mantissa.toLow() );
#endif
		// Apply scale
		std::uint8_t currentScale = scale();
		for ( std::uint8_t i = 0; i < currentScale; ++i )
		{
			result /= 10.0;
		}

		// Apply sign
		if ( isNegative() )
		{
			result = -result;
		}

		return result;
	}

	std::string Decimal::toString() const
	{
		if ( isZero() )
		{
			return "0";
		}

		std::string result;
		result.reserve( 64 ); // Reserve space for efficiency

		Int128 mantissa{ internal::mantissaAsInt128( *this ).abs() };
		std::uint8_t currentScale{ scale() };

		// Optimized digit extraction with fast division
		std::array<char, constants::decimal::MAX_STRING_LENGTH> digits;
		size_t digitCount{ 0 };
#if NFX_CORE_HAS_INT128

		// Fast path for values that fit in 64-bit
		if ( mantissa.toNative() <= UINT64_MAX )
		{
			std::uint64_t value{ static_cast<std::uint64_t>( mantissa.toNative() ) };
			while ( value > 0 && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( '0' + ( value % 10 ) );
				value /= 10;
			}
		}
		else
		{
			// Full 128-bit extraction
			while ( !mantissa.isZero() && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( '0' +
														  ( mantissa.toNative() % constants::decimal::BASE ) );
				mantissa = Int128{ mantissa.toNative() / constants::decimal::BASE };
			}
		}
#else
		if ( mantissa.toHigh() == 0 )
		{
			// Fast 64-bit path
			std::uint64_t value{ mantissa.toLow() };
			while ( value > 0 && digitCount < digits.size() )
			{
				digits[digitCount++] = static_cast<char>( '0' + ( value % 10 ) );
				value /= 10;
			}
		}
		else
		{
			// Manual 128-bit extraction
			while ( !mantissa.isZero() && digitCount < digits.size() )
			{
				if ( mantissa.toHigh() == 0 )
				{
					// Switched to 64-bit range
					std::uint64_t value{ mantissa.toLow() };
					while ( value > 0 && digitCount < digits.size() )
					{
						digits[digitCount++] = static_cast<char>( '0' + ( value % 10 ) );
						value /= 10;
					}
					break;
				}
				digits[digitCount++] = static_cast<char>( '0' +
														  ( mantissa.toLow() % constants::decimal::BASE ) );

				mantissa = mantissa / Int128{ constants::decimal::BASE };
			}
		}
#endif

		if ( digitCount == 0 )
		{
			digitCount = 1;
			digits[0] = '0';
		}

		// Handle sign
		if ( isNegative() )
		{
			result.push_back( '-' );
		}

		// Apply decimal point formatting
		if ( currentScale > 0 )
		{
			if ( currentScale >= digitCount )
			{
				// Need leading zeros: "0.00123"
				result.push_back( '0' );
				result.push_back( '.' );

				// Add leading zeros
				for ( size_t i = 0; i < currentScale - digitCount; ++i )
				{
					result.push_back( '0' );
				}

				// Add digits in reverse order
				for ( size_t i = digitCount; i > 0; --i )
				{
					result.push_back( digits[i - 1] );
				}
			}
			else
			{
				// Add integer part (reverse order)
				for ( size_t i = digitCount; i > currentScale; --i )
				{
					result.push_back( digits[i - 1] );
				}

				result.push_back( '.' );

				// Add fractional part (reverse order)
				for ( size_t i = currentScale; i > 0; --i )
				{
					result.push_back( digits[i - 1] );
				}
			}
		}
		else
		{
			// No decimal point, just add digits in reverse order
			for ( size_t i = digitCount; i > 0; --i )
			{
				result.push_back( digits[i - 1] );
			}
		}

		return result;
	}

	std::array<std::int32_t, 4> Decimal::toBits() const noexcept
	{
		std::array<std::int32_t, 4> bits{};

		// First three elements are the 96-bit mantissa
		bits[0] = static_cast<std::int32_t>( m_layout.mantissa[0] );
		bits[1] = static_cast<std::int32_t>( m_layout.mantissa[1] );
		bits[2] = static_cast<std::int32_t>( m_layout.mantissa[2] );

		// Fourth element contains scale and sign information
		bits[3] = static_cast<std::int32_t>( m_layout.flags );

		return bits;
	}

	//----------------------------------------------
	// Mathematical operations
	//----------------------------------------------

	Decimal Decimal::truncate() const noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal result = *this;
		std::uint8_t currentScale = scale();

		// Remove all fractional digits
		for ( std::uint8_t i = 0; i < currentScale; ++i )
		{
			internal::divideByPowerOf10( result, 1U );
		}

		// Clear the scale
		result.m_layout.flags &= ~constants::decimal::SCALE_MASK;

		return result;
	}

	Decimal Decimal::floor() noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal truncated = truncate();

		// If negative and had fractional part, subtract 1
		if ( isNegative() && !( *this == truncated ) )
		{
			truncated = truncated - Decimal::one();
		}

		return truncated;
	}

	Decimal Decimal::ceiling() noexcept
	{
		if ( scale() == 0 || isZero() )
		{
			return *this;
		}

		Decimal truncated = truncate();

		// If positive and had fractional part, add 1
		if ( !isNegative() && !( *this == truncated ) )
		{
			truncated = truncated + Decimal::one();
		}

		return truncated;
	}

	Decimal Decimal::round() noexcept
	{
		return round( 0 );
	}

	Decimal Decimal::round( std::int32_t decimalsPlacesCount ) noexcept
	{
		if ( decimalsPlacesCount < 0 )
		{
			decimalsPlacesCount = 0;
		}

		if ( decimalsPlacesCount >= static_cast<std::int32_t>( scale() ) || isZero() )
		{
			return *this;
		}

		Decimal result{ *this };
		std::uint8_t currentScale{ scale() };
		std::uint8_t targetScale{ static_cast<std::uint8_t>( decimalsPlacesCount ) };
		std::uint8_t digitsToRemove{ static_cast<std::uint8_t>( currentScale - targetScale ) };

		// Get the digit that determines rounding direction
		Int128 mantissa{ internal::mantissaAsInt128( *this ) };
		Int128 divisor{ 1 };
		if ( digitsToRemove > 1U )
		{
			std::uint8_t divisorPowers = static_cast<std::uint8_t>( digitsToRemove - 1U );
			for ( std::uint8_t i{ 0 }; i < divisorPowers; ++i )
			{
				divisor = divisor * Int128{ 10 };
			}
		}

		Int128 roundingDigit{ ( mantissa / divisor ) % Int128{ 10 } };

		// Perform truncation to target scale
		for ( std::uint8_t i = 0; i < digitsToRemove; ++i )
		{
			internal::divideByPowerOf10( result, 1U );
		}

		result.m_layout.flags =
			( result.m_layout.flags & ~constants::decimal::SCALE_MASK ) |
			( static_cast<std::uint32_t>( targetScale ) << constants::decimal::SCALE_SHIFT );

		// Round up if digit >= 5 //TODO!!
		if ( roundingDigit.toLow() >= 5 )
		{
			Int128 resultMantissa{ internal::mantissaAsInt128( result ) };
			if ( isNegative() )
			{
				resultMantissa = resultMantissa - Int128{ 1 };
			}
			else
			{
				resultMantissa = resultMantissa + Int128{ 1 };
			}
			internal::setMantissa( result, resultMantissa );
		}

		return result;
	}

	Decimal Decimal::abs() const noexcept
	{
		if ( isNegative() )
		{
			return -*this;
		}

		return *this;
	}

	//----------------------------------------------
	// Static mathematical operations
	//----------------------------------------------

	Decimal Decimal::truncate( const Decimal& value ) noexcept
	{
		return value.truncate();
	}

	Decimal Decimal::floor( Decimal& value ) noexcept
	{
		return value.floor();
	}

	Decimal Decimal::ceiling( Decimal& value ) noexcept
	{
		return value.ceiling();
	}

	Decimal Decimal::round( Decimal& value ) noexcept
	{
		return value.round();
	}

	Decimal Decimal::round( Decimal& value, std::int32_t decimalsPlacesCount ) noexcept
	{
		return value.round( decimalsPlacesCount );
	}

	Decimal Decimal::abs( const Decimal& value ) noexcept
	{
		return value.abs();
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const Decimal& decimal )
	{
		return os << decimal.toString();
	}

	std::istream& operator>>( std::istream& is, Decimal& decimal )
	{
		std::string str;
		is >> str;

		if ( !Decimal::tryParse( str, decimal ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
}
