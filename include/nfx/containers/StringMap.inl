/**
 * @file StringMap.inl
 * @brief Template implementations for StringMap heterogeneous lookup container
 * @details Contains the template function implementations for zero-copy string operations
 *          and heterogeneous key support in StringMap
 */

#include <stdexcept>

namespace nfx::containers
{
	//=====================================================================
	// StringMap class
	//=====================================================================

	//----------------------------------------------
	// Heterogeneous operator[] overloads
	//----------------------------------------------

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::operator[]( const char* key ) noexcept
	{
		return ( *this )[std::string_view{ key }];
	}

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::operator[]( char* key ) noexcept
	{
		return ( *this )[std::string_view{ key }];
	}

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::operator[]( std::string_view key ) noexcept
	{
		auto it = this->find( key );
		if ( it != this->end() )
		{
			return it->second;
		}

		return this->emplace( std::string{ key }, T{} ).first->second;
	}

	//----------------------------------------------
	// Heterogeneous at() overloads
	//----------------------------------------------

	template <typename T>
	NFX_CORE_INLINE const T& StringMap<T>::at( const char* key ) const
	{
		return this->at( std::string_view{ key } );
	}

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::at( const char* key )
	{
		return this->at( std::string_view{ key } );
	}

	template <typename T>
	NFX_CORE_INLINE const T& StringMap<T>::at( char* key ) const
	{
		return this->at( std::string_view{ key } );
	}

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::at( char* key )
	{
		return this->at( std::string_view{ key } );
	}

	template <typename T>
	NFX_CORE_INLINE const T& StringMap<T>::at( std::string_view key ) const
	{
		auto it = this->find( key );
		if ( it == this->end() )
		{
			throw std::out_of_range( "StringMap::at: key not found" );
		}
		return it->second;
	}

	template <typename T>
	NFX_CORE_INLINE T& StringMap<T>::at( std::string_view key )
	{
		auto it = this->find( key );
		if ( it == this->end() )
		{
			throw std::out_of_range( "StringMap::at: key not found" );
		}
		return it->second;
	}

	//----------------------------------------------
	// Heterogeneous try_emplace overloads
	//----------------------------------------------

	template <typename T>
	template <typename... Args>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::try_emplace( const char* key, Args&&... args ) noexcept(
		std::is_nothrow_constructible_v<T, Args...> )
	{
		return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
	}

	template <typename T>
	template <typename... Args>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::try_emplace( char* key, Args&&... args ) noexcept(
		std::is_nothrow_constructible_v<T, Args...> )
	{
		return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
	}

	template <typename T>
	template <typename... Args>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::try_emplace( std::string_view key, Args&&... args ) noexcept(
		std::is_nothrow_constructible_v<T, Args...> )
	{
		return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
	}

	//----------------------------------------------
	// Heterogeneous insert_or_assign overloads
	//----------------------------------------------

	template <typename T>
	template <typename M>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::insert_or_assign( const char* key, M&& obj ) noexcept(
		std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> )
	{
		return Base::insert_or_assign( std::string{ key }, std::forward<M>( obj ) );
	}

	template <typename T>
	template <typename M>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::insert_or_assign( char* key, M&& obj ) noexcept(
		std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> )
	{
		return Base::insert_or_assign( std::string{ key }, std::forward<M>( obj ) );
	}

	template <typename T>
	template <typename M>
	NFX_CORE_INLINE std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::insert_or_assign( std::string_view key, M&& obj ) noexcept(
		std::is_nothrow_assignable_v<T&, M> && std::is_nothrow_constructible_v<T, M> )
	{
		return Base::insert_or_assign( std::string{ key }, std::forward<M>( obj ) );
	}
}
