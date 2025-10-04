/**
 * @file StringFunctors.inl
 * @brief Implementation of heterogeneous lookup functors for string containers
 * @details Contains the inline implementations of StringViewHash and StringViewEqual functors
 */

namespace nfx::containers
{
	//=====================================================================
	// Heterogeneous lookup functors for zero-copy string operations
	//=====================================================================

	//----------------------------------------------
	// StringViewHash struct
	//----------------------------------------------

	NFX_CORE_INLINE size_t StringViewHash::operator()( const char* s ) const noexcept
	{
		return std::hash<std::string_view>{}( std::string_view{ s } );
	}

	NFX_CORE_INLINE size_t StringViewHash::operator()( const std::string& s ) const noexcept
	{
		return std::hash<std::string_view>{}( std::string_view{ s.data(), s.size() } );
	}

	NFX_CORE_INLINE size_t StringViewHash::operator()( std::string_view sv ) const noexcept
	{
		return std::hash<std::string_view>{}( sv );
	}

	//----------------------------------------------
	// StringViewEqual struct
	//----------------------------------------------

	NFX_CORE_INLINE constexpr bool StringViewEqual::operator()( const char* lhs, const char* rhs ) const noexcept
	{
		std::string_view lhs_view{ lhs };
		std::string_view rhs_view{ rhs };
		return lhs_view.size() == rhs_view.size() && lhs_view == rhs_view;
	}

	inline NFX_CORE_CONDITIONAL_CONSTEXPR bool StringViewEqual::operator()( const std::string& lhs, const std::string& rhs ) const noexcept
	{
		return lhs.size() == rhs.size() && lhs == rhs;
	}

	constexpr inline bool StringViewEqual::operator()( std::string_view lhs, std::string_view rhs ) const noexcept
	{
		return lhs.size() == rhs.size() && lhs == rhs;
	}

	inline NFX_CORE_CONDITIONAL_CONSTEXPR bool StringViewEqual::operator()( const char* lhs, const std::string& rhs ) const noexcept
	{
		std::string_view lhs_view{ lhs };
		return lhs_view.size() == rhs.size() && lhs_view == rhs;
	}

	NFX_CORE_INLINE constexpr bool StringViewEqual::operator()( const char* lhs, std::string_view rhs ) const noexcept
	{
		std::string_view lhs_view{ lhs };
		return lhs_view.size() == rhs.size() && lhs_view == rhs;
	}

	inline NFX_CORE_CONDITIONAL_CONSTEXPR bool StringViewEqual::operator()( const std::string& lhs, const char* rhs ) const noexcept
	{
		std::string_view rhs_view{ rhs };
		return lhs.size() == rhs_view.size() && lhs == rhs_view;
	}

	inline NFX_CORE_CONDITIONAL_CONSTEXPR bool StringViewEqual::operator()( const std::string& lhs, std::string_view rhs ) const noexcept
	{
		return lhs.size() == rhs.size() && lhs == rhs;
	}

	NFX_CORE_INLINE constexpr bool StringViewEqual::operator()( std::string_view lhs, const char* rhs ) const noexcept
	{
		std::string_view rhs_view{ rhs };
		return lhs.size() == rhs_view.size() && lhs == rhs_view;
	}

	inline NFX_CORE_CONDITIONAL_CONSTEXPR bool StringViewEqual::operator()( std::string_view lhs, const std::string& rhs ) const noexcept
	{
		return lhs.size() == rhs.size() && lhs == rhs;
	}
} // namespace nfx::containers
