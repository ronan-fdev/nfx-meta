/**
 * @file Utils.inl
 * @brief Implementation file for high-performance string utilities
 * @details Inline implementations of string validation, parsing, and comparison functions
 *          with optimized algorithms for maximum runtime performance and minimal overhead.
 */

#include <cctype>
#include <charconv>

namespace nfx::string
{
	//=====================================================================
	// String utilities
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	NFX_CORE_INLINE constexpr bool hasExactLength( std::string_view str, std::size_t expectedLength ) noexcept
	{
		return str.size() == expectedLength;
	}

	NFX_CORE_INLINE constexpr bool isEmpty( std::string_view str ) noexcept
	{
		return str.empty();
	}

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	inline bool tryParseBool( std::string_view str, bool& result ) noexcept
	{
		if ( str.empty() )
		{
			result = false;
			return false;
		}

		// Handle single character cases first
		if ( str.size() == 1 )
		{
			const char c{ toLower( str[0] ) };
			if ( c == '1' || c == 't' || c == 'y' )
			{
				result = true;
				return true;
			}
			else if ( c == '0' || c == 'f' || c == 'n' )
			{
				result = false;
				return true;
			}
			return false;
		}

		// Handle multi-character cases
		if ( str.size() == 2 )
		{
			if ( ( toLower( str[0] ) == 'o' && toLower( str[1] ) == 'n' ) )
			{
				result = true;
				return true;
			}
			else if ( ( toLower( str[0] ) == 'n' && toLower( str[1] ) == 'o' ) )
			{
				result = false;
				return true;
			}
		}
		else if ( str.size() == 3 )
		{
			if ( toLower( str[0] ) == 'y' && toLower( str[1] ) == 'e' && toLower( str[2] ) == 's' )
			{
				result = true;
				return true;
			}
			else if ( toLower( str[0] ) == 'o' && toLower( str[1] ) == 'f' && toLower( str[2] ) == 'f' )
			{
				result = false;
				return true;
			}
		}
		else if ( str.size() == 4 )
		{
			if ( toLower( str[0] ) == 't' && toLower( str[1] ) == 'r' &&
				 toLower( str[2] ) == 'u' && toLower( str[3] ) == 'e' )
			{
				result = true;
				return true;
			}
		}
		else if ( str.size() == 5 )
		{
			if ( toLower( str[0] ) == 'f' && toLower( str[1] ) == 'a' &&
				 toLower( str[2] ) == 'l' && toLower( str[3] ) == 's' && toLower( str[4] ) == 'e' )
			{
				result = false;
				return true;
			}
		}

		result = false;
		return false;
	}

	NFX_CORE_INLINE bool tryParseInt( std::string_view str, int& result ) noexcept
	{
		if ( str.empty() )
		{
			result = 0;
			return false;
		}

		const char* const begin = str.data();
		const char* const end = std::next( begin, static_cast<std::ptrdiff_t>( str.length() ) );
		const auto parseResult{ std::from_chars( begin, end, result ) };
		return parseResult.ec == std::errc{} && parseResult.ptr == end;
	}

	NFX_CORE_INLINE bool tryParseUInt( std::string_view str, std::uint32_t& result ) noexcept
	{
		if ( str.empty() )
		{
			result = 0u;
			return false;
		}

		const char* const begin = str.data();
		const char* const end = std::next( begin, static_cast<std::ptrdiff_t>( str.size() ) );
		const auto parseResult{ std::from_chars( begin, end, result ) };
		return parseResult.ec == std::errc{} && parseResult.ptr == end;
	}

	NFX_CORE_INLINE bool tryParseLong( std::string_view str, std::int64_t& result ) noexcept
	{
		if ( str.empty() )
		{
			result = 0LL;
			return false;
		}

		const char* const begin = str.data();
		const char* const end = std::next( begin, static_cast<std::ptrdiff_t>( str.size() ) );
		const auto parseResult{ std::from_chars( begin, end, result ) };
		return parseResult.ec == std::errc{} && parseResult.ptr == end;
	}

	NFX_CORE_INLINE bool tryParseDouble( std::string_view str, double& result ) noexcept
	{
		if ( str.empty() )
		{
			result = 0.0;
			return false;
		}

		const char* const end = std::next( str.data(), static_cast<std::ptrdiff_t>( str.size() ) );
		const auto parseResult{ std::from_chars( str.data(), end, result ) };
		return parseResult.ec == std::errc{} && parseResult.ptr == end;
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	NFX_CORE_INLINE constexpr bool endsWith( std::string_view str, std::string_view suffix ) noexcept
	{
		return str.size() >= suffix.size() && str.compare( str.size() - suffix.size(), suffix.size(), suffix ) == 0;
	}

	NFX_CORE_INLINE constexpr bool startsWith( std::string_view str, std::string_view prefix ) noexcept
	{
		return str.size() >= prefix.size() && str.compare( 0, prefix.size(), prefix ) == 0;
	}

	NFX_CORE_INLINE constexpr bool contains( std::string_view str, std::string_view substr ) noexcept
	{
		return str.find( substr ) != std::string_view::npos;
	}

	NFX_CORE_INLINE constexpr bool equals( std::string_view lhs, std::string_view rhs ) noexcept
	{
		return lhs == rhs;
	}

	NFX_CORE_INLINE bool iequals( std::string_view lhs, std::string_view rhs ) noexcept
	{
		if ( lhs.size() != rhs.size() )
		{
			return false;
		}

		return std::equal( lhs.begin(), lhs.end(), rhs.begin(),
			[]( char a, char b ) noexcept { return toLower( a ) == toLower( b ); } );
	}

	//----------------------------------------------
	// Case Conversion
	//----------------------------------------------

	NFX_CORE_INLINE constexpr char toLower( char c ) noexcept
	{
		return ( c >= 'A' && c <= 'Z' ) ? static_cast<char>( c + ( 'a' - 'A' ) ) : c;
	}

	NFX_CORE_INLINE constexpr char toUpper( char c ) noexcept
	{
		return ( c >= 'a' && c <= 'z' ) ? static_cast<char>( c - ( 'a' - 'A' ) ) : c;
	}

	inline std::string toLower( std::string_view str )
	{
		std::string result;
		result.reserve( str.size() );

		for ( char c : str )
		{
			result.push_back( toLower( c ) );
		}

		return result;
	}

	inline std::string toUpper( std::string_view str )
	{
		std::string result;
		result.reserve( str.size() );

		for ( char c : str )
		{
			result.push_back( toUpper( c ) );
		}

		return result;
	}
}
