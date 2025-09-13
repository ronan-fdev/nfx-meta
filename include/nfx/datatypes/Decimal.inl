/**
 * @file Decimal.inl
 * @brief Inline implementations for cross-platform Decimal class
 */

#include "constants/DecimalConstants.h"
#include "nfx/datatypes/Int128.h"

namespace nfx::datatypes
{
	//=====================================================================
	// Decimal class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr Decimal::Decimal() noexcept
		: m_layout{ 0, { { 0, 0, 0 } } }
	{
	}

	//----------------------------------------------
	// Decimal constants
	//----------------------------------------------

	inline constexpr Decimal Decimal::zero() noexcept
	{
		return Decimal{};
	}

	inline constexpr Decimal Decimal::one() noexcept
	{
		Decimal result{};
		result.m_layout.mantissa[0] = 1;

		return result;
	}

	inline constexpr Decimal Decimal::minValue() noexcept
	{
		Decimal result{};
		result.m_layout.mantissa[0] = 1;
		result.m_layout.flags = ( constants::decimal::MAXIMUM_PLACES << constants::decimal::SCALE_SHIFT );

		return result;
	}

	inline constexpr Decimal Decimal::maxValue() noexcept
	{
		Decimal result{};
		result.m_layout.mantissa[0] = 0xFFFFFFFF;
		result.m_layout.mantissa[1] = 0xFFFFFFFF;
		result.m_layout.mantissa[2] = 0xFFFFFFFF;

		return result;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline std::uint8_t Decimal::scale() const noexcept
	{
		return static_cast<std::uint8_t>( ( m_layout.flags & constants::decimal::SCALE_MASK ) >> constants::decimal::SCALE_SHIFT );
	}

	inline const std::uint32_t& Decimal::flags() const noexcept
	{
		return m_layout.flags;
	}

	inline std::uint32_t& Decimal::flags() noexcept
	{
		return m_layout.flags;
	}

	inline const std::array<std::uint32_t, 3>& Decimal::mantissa() const noexcept
	{
		return m_layout.mantissa;
	}

	inline std::array<std::uint32_t, 3>& Decimal::mantissa() noexcept
	{
		return m_layout.mantissa;
	}

	//----------------------------------------------
	// State checking
	//----------------------------------------------

	inline bool Decimal::isZero() const noexcept
	{
		return m_layout.mantissa[0] == 0 && m_layout.mantissa[1] == 0 && m_layout.mantissa[2] == 0;
	}

	inline bool Decimal::isNegative() const noexcept
	{
		return ( m_layout.flags & constants::decimal::SIGN_MASK ) != 0;
	}

	//----------------------------------------------
	// Utilities
	//----------------------------------------------

	inline std::uint8_t Decimal::decimalPlacesCount() const noexcept
	{
		// If the value is zero, it has 0 decimal places
		if ( isZero() )
		{
			return 0;
		}

		// Get the current scale
		std::uint8_t currentScale = scale();

		// If scale is 0, it's an integer - no decimal places
		if ( currentScale == 0 )
		{
			return 0;
		}

		// Convert mantissa to Int128 for proper arithmetic
		const auto& mantissaArray = mantissa();
#if NFX_CORE_HAS_INT128
		NFX_CORE_INT128 mantissaValue{ static_cast<NFX_CORE_INT128>( mantissaArray[2] ) << 64 |
									   static_cast<NFX_CORE_INT128>( mantissaArray[1] ) << 32 |
									   static_cast<NFX_CORE_INT128>( mantissaArray[0] ) };
		Int128 mantissa128{ mantissaValue };
#else
		std::uint64_t low{ static_cast<std::uint64_t>( mantissaArray[1] ) << 32 | mantissaArray[0] };
		std::uint64_t high{ mantissaArray[2] };
		Int128 mantissa128{ low, high };
#endif

		std::uint8_t trailingZeros = 0;
		Int128 ten{ 10 };

		// Count trailing zeros by testing divisibility by 10 iteratively
		while ( trailingZeros < currentScale )
		{
			// If there's a remainder, we can't divide evenly by 10
			if ( mantissa128 % ten != Int128{ 0 } )
			{
				break;
			}

			// Continue testing with the next power of 10
			mantissa128 = mantissa128 / ten;
			trailingZeros++;
		}

		return currentScale - trailingZeros;
	}
}
