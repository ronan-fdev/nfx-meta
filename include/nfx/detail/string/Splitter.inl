/**
 * @file Splitter.inl
 * @brief Implementation of zero-allocation string splitting utilities
 * @details Inline implementations for high-performance string_view-based splitting
 */

#include <string_view>

#include "nfx/config.h"

namespace nfx::string
{
	//=====================================================================
	// Splitter class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename String>
	NFX_CORE_INLINE Splitter::Splitter( String&& str, char delimiter ) noexcept
		: m_str{ std::string_view{ std::forward<String>( str ) } },
		  m_delimiter{ delimiter }
	{
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	NFX_CORE_INLINE Splitter::Iterator Splitter::begin() const noexcept
	{
		return Iterator{ *this };
	}

	NFX_CORE_INLINE Splitter::Iterator Splitter::end() const noexcept
	{
		return Iterator{ *this, true };
	}

	//----------------------------------------------
	// Splitter::Iterator class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	NFX_CORE_INLINE Splitter::Iterator::Iterator( const Splitter& splitter, bool at_end ) noexcept
		: m_splitter{ splitter },
		  m_start{ 0 },
		  m_end{ 0 },
		  m_isAtEnd{ at_end || splitter.m_str.empty() }
	{
		if ( !m_isAtEnd )
		{
			m_end = m_splitter.m_str.find( m_splitter.m_delimiter, 0 );
			if ( m_end == std::string_view::npos )
			{
				m_end = m_splitter.m_str.length();
			}
		}
	}

	//-----------------------------
	// Iterator operators
	//-----------------------------

	NFX_CORE_INLINE std::string_view Splitter::Iterator::operator*() const noexcept
	{
		const size_t length = m_end - m_start;
		return m_splitter.m_str.substr( m_start, length );
	}

	NFX_CORE_INLINE Splitter::Iterator& Splitter::Iterator::operator++() noexcept
	{
		m_start = m_end + 1;

		const size_t str_len = m_splitter.m_str.length();
		if ( m_start > str_len )
		{
			m_isAtEnd = true;
			return *this;
		}

		m_end = m_splitter.m_str.find( m_splitter.m_delimiter, m_start );
		if ( m_end == std::string_view::npos ) [[unlikely]]
		{
			m_end = str_len;
		}

		return *this;
	}

	//-----------------------------
	// Comparison operators
	//-----------------------------

	NFX_CORE_INLINE bool Splitter::Iterator::operator==( const Iterator& other ) const noexcept
	{
		return m_isAtEnd == other.m_isAtEnd;
	}

	//=====================================================================
	// String splitting factory functions
	//=====================================================================

	template <typename String>
	NFX_CORE_INLINE Splitter splitView( String&& str, char delimiter ) noexcept
	{
		return Splitter{ std::string_view{ std::forward<String>( str ) }, delimiter };
	}
}
