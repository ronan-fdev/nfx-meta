/**
 * @file Document_impl.cpp
 * @brief Implementation of Document_impl Pimpl class
 * @details Provides concrete implementation for the Document facade, wrapping nlohmann::json.
 */

#include "Document_impl.h"

#include <nlohmann/json.hpp>

namespace nfx::serialization::json
{
	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Document_impl::Document_impl()
	{
		// Using assignment here avoids initializer_list ambiguity with nlohmann::json.
		m_data = nlohmann::json::object();
	}

	Document_impl::Document_impl( nlohmann::json jsonData )
	{
		// Using assignment here avoids initializer_list ambiguity with nlohmann::json
		m_data = std::move( jsonData );
	}

	//----------------------------------------------
	// Navigation methods
	//----------------------------------------------

	nlohmann::json* Document_impl::navigateToPath( std::string_view path, bool createPath )
	{
		if ( path.empty() )
		{
			return &m_data;
		}

		nlohmann::json* current = &m_data;
		size_t start = 0;
		size_t pos = 0;

		while ( pos != std::string_view::npos )
		{
			pos = path.find( '.', start );
			std::string_view segmentView = path.substr( start, pos == std::string_view::npos ? pos : pos - start );

			if ( segmentView.empty() )
			{
				start = pos + 1;
				continue;
			}

			// Handle array access like "data[0]"
			size_t bracketPos = segmentView.find( '[' );
			if ( bracketPos != std::string_view::npos )
			{
				std::string_view arrayNameView = segmentView.substr( 0, bracketPos );
				std::string_view indexView = segmentView.substr( bracketPos + 1 );
				indexView.remove_suffix( 1 ); // Remove ']'

				std::string arrayName( arrayNameView );

				if ( createPath && !current->contains( arrayName ) )
				{
					( *current )[arrayName] = nlohmann::json::array();
				}

				if ( !current->contains( arrayName ) || !( *current )[arrayName].is_array() )
				{
					return nullptr;
				}

				try
				{
					std::string indexStr( indexView );
					size_t index = std::stoull( indexStr );
					current = &( *current )[arrayName][index];
				}
				catch ( ... )
				{
					return nullptr;
				}
			}
			else
			{
				// Regular object field access - convert to string only when needed
				std::string segment( segmentView );

				if ( createPath && !current->contains( segment ) )
				{
					( *current )[segment] = nlohmann::json::object();
				}

				if ( !current->contains( segment ) )
				{
					return nullptr;
				}

				current = &( *current )[segment];
			}

			start = pos + 1;
		}

		return current;
	}

	const nlohmann::json* Document_impl::navigateToPath( std::string_view path ) const
	{
		return const_cast<Document_impl*>( this )->navigateToPath( path, false );
	}

	nlohmann::json* Document_impl::navigateToJsonPointer( std::string_view pointer, bool createPath )
	{
		// RFC 6901: Empty string means root document
		if ( pointer.empty() )
		{
			return &m_data;
		}

		// RFC 6901: JSON Pointer must start with "/"
		if ( pointer[0] != '/' )
		{
			return nullptr;
		}

		nlohmann::json* current = &m_data;
		size_t start = 1; // Skip initial "/"

		while ( start < pointer.length() )
		{
			// Find next "/" or end of string
			size_t pos = pointer.find( '/', start );
			if ( pos == std::string_view::npos )
			{
				pos = pointer.length();
			}

			std::string_view tokenView = pointer.substr( start, pos - start );

			if ( tokenView.empty() )
			{
				// Empty token - invalid pointer
				return nullptr;
			}

			// Unescape the token according to RFC 6901
			std::string token = unescapeJsonPointerToken( tokenView );

			// Handle array indexing
			if ( current->is_array() )
			{
				// Special case: "-" means append to array (only valid for creation)
				if ( token == "-" )
				{
					if ( createPath && pos == pointer.length() )
					{
						// Append new element to array
						current->push_back( nlohmann::json::object() );
						return &current->back();
					}
					else
					{
						// "-" not valid for navigation without creation
						return nullptr;
					}
				}

				// Validate array index
				if ( !isValidArrayIndex( token ) )
				{
					return nullptr;
				}

				try
				{
					size_t index = std::stoull( token );

					if ( createPath )
					{
						// Extend array if needed
						while ( current->size() <= index )
						{
							current->push_back( nlohmann::json::object() );
						}
					}
					else if ( index >= current->size() )
					{
						return nullptr;
					}

					current = &( *current )[index];
				}
				catch ( ... )
				{
					return nullptr;
				}
			}
			// Handle object property access
			else if ( current->is_object() )
			{
				if ( createPath && !current->contains( token ) )
				{
					// Determine if next level should be array or object
					if ( pos < pointer.length() )
					{
						// Look ahead to next token to determine type
						size_t nextStart = pos + 1;
						size_t nextEnd = pointer.find( '/', nextStart );
						if ( nextEnd == std::string_view::npos )
						{
							nextEnd = pointer.length();
						}

						std::string_view nextTokenView = pointer.substr( nextStart, nextEnd - nextStart );
						std::string nextToken = unescapeJsonPointerToken( nextTokenView );

						if ( isValidArrayIndex( nextToken ) || nextToken == "-" )
						{
							( *current )[token] = nlohmann::json::array();
						}
						else
						{
							( *current )[token] = nlohmann::json::object();
						}
					}
					else
					{
						// This is the final token, create as object by default
						( *current )[token] = nlohmann::json::object();
					}
				}

				if ( !current->contains( token ) )
				{
					return nullptr;
				}

				current = &( *current )[token];
			}
			else
			{
				// Current node is neither array nor object
				return nullptr;
			}

			start = pos + 1;
		}

		return current;
	}

	const nlohmann::json* Document_impl::navigateToJsonPointer( std::string_view pointer ) const
	{
		return const_cast<Document_impl*>( this )->navigateToJsonPointer( pointer, false );
	}

	std::string Document_impl::unescapeJsonPointerToken( std::string_view token ) noexcept
	{
		std::string result;
		result.reserve( token.length() );

		for ( size_t i = 0; i < token.length(); ++i )
		{
			if ( token[i] == '~' && i + 1 < token.length() )
			{
				if ( token[i + 1] == '1' )
				{
					result += '/';
					++i; // Skip the '1'
				}
				else if ( token[i + 1] == '0' )
				{
					result += '~';
					++i; // Skip the '0'
				}
				else
				{
					// Invalid escape sequence, include as-is
					result += token[i];
				}
			}
			else
			{
				result += token[i];
			}
		}

		return result;
	}

	bool Document_impl::isValidArrayIndex( std::string_view token ) noexcept
	{
		if ( token.empty() )
		{
			return false;
		}

		// RFC 6901: Array index must be either "0" or not start with "0"
		if ( token.length() > 1 && token[0] == '0' )
		{
			return false;
		}

		// Check if all characters are digits
		for ( char c : token )
		{
			if ( c < '0' || c > '9' )
			{
				return false;
			}
		}

		return true;
	}
} // namespace nfx::serialization::json
