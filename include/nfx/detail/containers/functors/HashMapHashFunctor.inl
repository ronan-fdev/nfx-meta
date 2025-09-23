/**
 * @file HashMapHashFunctor.inl
 * @brief Implementation of high-performance hash functor for HashMap
 * @details Contains hashing algorithms optimized for Robin Hood performance
 */

#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>

#include "nfx/core/hashing/Hash.h"

#if defined( _MSC_VER )
#	include <intrin.h>
#elif defined( __SSE4_2__ )
#	include <nmmintrin.h>
#endif

namespace nfx::containers
{
	//=====================================================================
	// HashMapHash implementation
	//=====================================================================

	//----------------------------------------------
	// String hashing
	//----------------------------------------------

	NFX_CORE_INLINE size_t HashMapHash::operator()( const char* s ) const noexcept
	{
		return static_cast<size_t>( core::hashing::hashStringView( std::string_view{ s } ) );
	}

	NFX_CORE_INLINE size_t HashMapHash::operator()( const std::string& s ) const noexcept
	{
		return static_cast<size_t>( core::hashing::hashStringView( std::string_view{ s.data(), s.size() } ) );
	}

	NFX_CORE_INLINE size_t HashMapHash::operator()( std::string_view sv ) const noexcept
	{
		return static_cast<size_t>( core::hashing::hashStringView( sv ) );
	}

	//----------------------------------------------
	// Integer hashing (proper mixing)
	//----------------------------------------------

	template <typename T>
	NFX_CORE_INLINE std::enable_if_t<std::is_integral_v<T>, size_t>
	HashMapHash::operator()( T value ) const noexcept
	{
		return core::hashing::hashInteger( value );
	}

	//----------------------------------------------
	// Fallback to std::hash for other types
	//----------------------------------------------

	template <typename T>
	NFX_CORE_INLINE std::enable_if_t<
		!std::is_integral_v<T> &&
			!std::is_same_v<T, std::string> &&
			!std::is_same_v<T, std::string_view> &&
			!std::is_same_v<T, const char*>,
		size_t>
	HashMapHash::operator()( const T& value ) const noexcept
	{
		// Delegate to standard library for types we don't optimize
		return std::hash<T>{}( value );
	}
}
