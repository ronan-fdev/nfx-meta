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
	// Internal helper methods
	//----------------------------------------------
}
