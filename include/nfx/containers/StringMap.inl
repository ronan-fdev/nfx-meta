/**
 * @file StringMap.inl
 * @brief Template implementations for StringMap heterogeneous lookup container
 * @details Contains the template function implementations for zero-copy string operations
 *          and heterogeneous key support in StringMap
 */

namespace nfx::containers
{
	//=====================================================================
	// StringMap class
	//=====================================================================

	//----------------------------------------------
	// Heterogeneous operator[] overloads
	//----------------------------------------------

	template <typename T>
	inline T& StringMap<T>::operator[]( const char* key )
	{
		return this->try_emplace( std::string{ key }, T{} ).first->second;
	}

	template <typename T>
	inline T& StringMap<T>::operator[]( std::string_view key )
	{
		return this->try_emplace( std::string{ key }, T{} ).first->second;
	}

	//----------------------------------------------
	// Heterogeneous try_emplace overloads
	//----------------------------------------------

	template <typename T>
	template <typename... Args>
	inline std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::try_emplace( const char* key, Args&&... args )
	{
		return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
	}

	template <typename T>
	template <typename... Args>
	inline std::pair<typename StringMap<T>::Base::iterator, bool> StringMap<T>::try_emplace( std::string_view key, Args&&... args )
	{
		return Base::try_emplace( std::string{ key }, std::forward<Args>( args )... );
	}
}
