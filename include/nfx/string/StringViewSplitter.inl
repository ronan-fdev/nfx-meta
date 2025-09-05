/**
 * @file StringViewSplitter.inl
 * @brief Implementation of zero-allocation string splitting utilities
 * @details Inline implementations for high-performance string_view-based splitting
 */

#include <string_view>

#include "nfx/config.h"

namespace nfx::string
{
	//=====================================================================
	// StringViewSplitter class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	NFX_CORE_INLINE StringViewSplitter::StringViewSplitter( std::string_view str, char delimiter ) noexcept
		: m_str{ str },
		  m_delimiter{ delimiter }
	{
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	NFX_CORE_INLINE StringViewSplitter::Iterator StringViewSplitter::begin() const noexcept
	{
		return Iterator{ this };
	}

	NFX_CORE_INLINE StringViewSplitter::Iterator StringViewSplitter::end() const noexcept
	{
		return Iterator{ this, true };
	}

	//----------------------------------------------
	// StringViewSplitter::Iterator class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	NFX_CORE_INLINE StringViewSplitter::Iterator::Iterator( const StringViewSplitter* splitter, bool at_end ) noexcept
		: m_splitter{ splitter },
		  m_currentPos{ 0 },
		  m_isAtEnd{ at_end }
	{
		if ( !m_isAtEnd )
		{
			advance();
		}
	}

	//-----------------------------
	// Iterator operators
	//-----------------------------

	NFX_CORE_INLINE std::string_view StringViewSplitter::Iterator::operator*() const noexcept
	{
		return m_currentSegment;
	}

	NFX_CORE_INLINE StringViewSplitter::Iterator& StringViewSplitter::Iterator::operator++() noexcept
	{
		advance();
		return *this;
	}

	//-----------------------------
	// Comparison operators
	//-----------------------------

	NFX_CORE_INLINE bool StringViewSplitter::Iterator::operator==( const Iterator& other ) const noexcept
	{
		return m_isAtEnd == other.m_isAtEnd;
	}

	NFX_CORE_INLINE bool StringViewSplitter::Iterator::operator!=( const Iterator& other ) const noexcept
	{
		return m_isAtEnd != other.m_isAtEnd;
	}

	//-----------------------------
	// Private methods
	//-----------------------------

	NFX_CORE_INLINE void StringViewSplitter::Iterator::advance() noexcept
	{
		if ( m_splitter->m_str.empty() )
		{
			m_isAtEnd = true;
			return;
		}

		if ( m_currentPos > m_splitter->m_str.length() )
		{
			m_isAtEnd = true;
			return;
		}

		const size_t start = m_currentPos;
		const size_t delimiterPos = m_splitter->m_str.find( m_splitter->m_delimiter, start );

		if ( delimiterPos == std::string_view::npos )
		{
			if ( start <= m_splitter->m_str.length() )
			{
				m_currentSegment = m_splitter->m_str.substr( start );
				m_currentPos = m_splitter->m_str.length() + 1;
			}
			else
			{
				m_isAtEnd = true;
			}
		}
		else
		{
			m_currentSegment = m_splitter->m_str.substr( start, delimiterPos - start );
			m_currentPos = delimiterPos + 1;
		}
	}

	//=====================================================================
	// String splitting factory functions
	//=====================================================================

	NFX_CORE_INLINE StringViewSplitter splitView( std::string_view str, char delimiter ) noexcept
	{
		return StringViewSplitter{ str, delimiter };
	}
}
