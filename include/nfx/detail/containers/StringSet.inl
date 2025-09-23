/**
 * @file StringSet.inl
 * @brief Template implementations for StringSet heterogeneous lookup container
 * @details Contains the inline function implementations for zero-copy string operations
 *          and heterogeneous key support in StringSet
 */

namespace nfx::containers
{
	//=====================================================================
	// StringSet class implementations
	//=====================================================================

	//----------------------------------------------
	// Heterogeneous insert overloads
	//----------------------------------------------

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::insert( const char* key )
	{
		return this->insert( std::string_view{ key } );
	}

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::insert( char* key )
	{
		return this->insert( std::string_view{ key } );
	}

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::insert( std::string_view key )
	{
		return Base::insert( std::string{ key } );
	}

	//----------------------------------------------
	// Heterogeneous emplace overloads
	//----------------------------------------------

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::emplace( const char* key )
	{
		return this->emplace( std::string_view{ key } );
	}

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::emplace( char* key )
	{
		return this->emplace( std::string_view{ key } );
	}

	NFX_CORE_INLINE std::pair<typename StringSet::Base::iterator, bool> StringSet::emplace( std::string_view key )
	{
		return Base::emplace( std::string{ key } );
	}

	//----------------------------------------------
	// C++20-style contains() method
	//----------------------------------------------

	NFX_CORE_INLINE bool StringSet::contains( const char* key ) const noexcept
	{
		return this->contains( std::string_view{ key } );
	}

	NFX_CORE_INLINE bool StringSet::contains( char* key ) const noexcept
	{
		return this->contains( std::string_view{ key } );
	}

	NFX_CORE_INLINE bool StringSet::contains( const std::string& key ) const noexcept
	{
		return this->contains( std::string_view{ key } );
	}

	NFX_CORE_INLINE bool StringSet::contains( std::string_view key ) const noexcept
	{
		return this->find( key ) != this->end();
	}
}
