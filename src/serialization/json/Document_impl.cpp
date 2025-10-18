/**
 * @file Document_impl.cpp
 * @brief Implementation of Document_impl Pimpl class
 * @details Provides concrete implementation for the Document facade, wrapping nlohmann::ordered_json.
 */

#include "nfx/serialization/json/Document.h"
#include "Document_impl.h"

#include <nlohmann/json.hpp>

namespace nfx::serialization::json
{
	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Document_impl::Document_impl()
	{
		// Using assignment here avoids initializer_list ambiguity with nlohmann::ordered_json.
		m_data = nlohmann::ordered_json::object();
	}

	Document_impl::Document_impl( nlohmann::ordered_json jsonData )
	{
		// Using assignment here avoids initializer_list ambiguity with nlohmann::ordered_json
		m_data = std::move( jsonData );
	}

	Document_impl::Document_impl( const Document_impl& other )
	{
		// Using assignment instead of initializer list to avoid nlohmann::json copy issues
		m_data = other.m_data;
		m_lastError = other.m_lastError;
	}

	Document_impl::Document_impl( Document_impl&& other ) noexcept
		: m_data{ std::move( other.m_data ) },
		  m_lastError{ std::move( other.m_lastError ) }
	{
	}

	Document_impl& Document_impl::operator=( const Document_impl& other )
	{
		if ( this != &other )
		{
			m_data = other.m_data;
			m_lastError = other.m_lastError;
		}
		return *this;
	}

	Document_impl& Document_impl::operator=( Document_impl&& other ) noexcept
	{
		if ( this != &other )
		{
			m_data = std::move( other.m_data );
			m_lastError = std::move( other.m_lastError );
		}
		return *this;
	}

	//----------------------------------------------
	// Navigation methods
	//----------------------------------------------

	nlohmann::ordered_json* Document_impl::navigateToPath( std::string_view path, bool createPath )
	{
		if ( path.empty() )
		{
			return &m_data;
		}

		nlohmann::ordered_json* current = &m_data;
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
					( *current )[arrayName] = nlohmann::ordered_json::array();
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
					( *current )[segment] = nlohmann::ordered_json::object();
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

	const nlohmann::ordered_json* Document_impl::navigateToPath( std::string_view path ) const
	{
		return const_cast<Document_impl*>( this )->navigateToPath( path, false );
	}

	nlohmann::ordered_json* Document_impl::navigateToJsonPointer( std::string_view pointer, bool createPath )
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

		nlohmann::ordered_json* current = &m_data;
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
						current->push_back( nlohmann::ordered_json::object() );
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
							current->push_back( nlohmann::ordered_json::object() );
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
							( *current )[token] = nlohmann::ordered_json::array();
						}
						else
						{
							( *current )[token] = nlohmann::ordered_json::object();
						}
					}
					else
					{
						// This is the final token, create as object by default
						( *current )[token] = nlohmann::ordered_json::object();
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

	const nlohmann::ordered_json* Document_impl::navigateToJsonPointer( std::string_view pointer ) const
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

	template <typename T>
	std::optional<T> Document_impl::getArrayImpl( std::string_view arrayPath, size_t index, const Document* docPtr ) const
	{
		if ( !docPtr )
		{
			return std::nullopt;
		}

		// Navigate to the array first
		const nlohmann::ordered_json* arrayNode = nullptr;
		if ( arrayPath.empty() )
		{
			arrayNode = &m_data;
		}
		else if ( arrayPath[0] == '/' )
		{
			arrayNode = navigateToJsonPointer( arrayPath );
		}
		else
		{
			arrayNode = navigateToPath( arrayPath );
		}

		if ( arrayNode && arrayNode->is_array() && index < arrayNode->size() )
		{
			const auto& element = ( *arrayNode )[index];

			// Handle primitive types
			if constexpr ( std::is_same_v<T, std::string_view> )
			{
				if ( element.is_string() )
				{
					return std::string_view( element.get_ref<const std::string&>() );
				}
			}
			else if constexpr ( std::is_same_v<T, std::string> )
			{
				if ( element.is_string() )
				{
					return element.get<std::string>();
				}
			}
			else if constexpr ( std::is_same_v<T, char> )
			{
				if ( element.is_string() )
				{
					std::string str = element.get<std::string>();
					if ( str.length() == 1 )
					{
						return str[0];
					}
				}
			}
			else if constexpr ( std::is_same_v<T, bool> )
			{
				if ( element.is_boolean() )
				{
					return element.get<bool>();
				}
			}
			else if constexpr ( std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> )
			{
				if ( element.is_number_integer() )
				{
					int64_t val = element.get<int64_t>();
					if constexpr ( std::is_same_v<T, int8_t> )
					{
						if ( val >= std::numeric_limits<int8_t>::min() && val <= std::numeric_limits<int8_t>::max() )
						{
							return static_cast<int8_t>( val );
						}
					}
					else if constexpr ( std::is_same_v<T, int16_t> )
					{
						if ( val >= std::numeric_limits<int16_t>::min() && val <= std::numeric_limits<int16_t>::max() )
						{
							return static_cast<int16_t>( val );
						}
					}
					else if constexpr ( std::is_same_v<T, int32_t> )
					{
						if ( val >= std::numeric_limits<int32_t>::min() && val <= std::numeric_limits<int32_t>::max() )
						{
							return static_cast<int32_t>( val );
						}
					}
				}
			}
			else if constexpr ( std::is_same_v<T, int64_t> )
			{
				if ( element.is_number_integer() )
				{
					return element.get<int64_t>();
				}
			}
			else if constexpr ( std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> )
			{
				if ( element.is_number_unsigned() )
				{
					uint64_t val = element.get<uint64_t>();
					if constexpr ( std::is_same_v<T, uint8_t> )
					{
						if ( val <= std::numeric_limits<uint8_t>::max() )
						{
							return static_cast<uint8_t>( val );
						}
					}
					else if constexpr ( std::is_same_v<T, uint16_t> )
					{
						if ( val <= std::numeric_limits<uint16_t>::max() )
						{
							return static_cast<uint16_t>( val );
						}
					}
					else if constexpr ( std::is_same_v<T, uint32_t> )
					{
						if ( val <= std::numeric_limits<uint32_t>::max() )
						{
							return static_cast<uint32_t>( val );
						}
					}
				}
			}
			else if constexpr ( std::is_same_v<T, uint64_t> )
			{
				if ( element.is_number_unsigned() )
				{
					return element.get<uint64_t>();
				}
			}
			else if constexpr ( std::is_same_v<T, float> )
			{
				if ( element.is_number_float() )
				{
					return static_cast<float>( element.get<double>() );
				}
			}
			else if constexpr ( std::is_same_v<T, double> )
			{
				if ( element.is_number_float() )
				{
					return element.get<double>();
				}
			}

			// Handle complex types
			else if constexpr ( std::is_same_v<T, Document> )
			{
				Document result;
				static_cast<Document_impl*>( result.m_impl )->setData( element );
				return result;
			}
			else if constexpr ( std::is_same_v<T, Document::Array> )
			{
				if ( element.is_array() )
				{
					std::string elementPath;
					if ( arrayPath.empty() )
					{
						elementPath = "/" + std::to_string( index );
					}
					else if ( arrayPath[0] == '/' )
					{
						elementPath = std::string( arrayPath ) + "/" + std::to_string( index );
					}
					else
					{
						elementPath = "/" + std::string( arrayPath ) + "/" + std::to_string( index );
					}
					return Document::Array( const_cast<Document*>( docPtr ), elementPath );
				}
			}
			else if constexpr ( std::is_same_v<T, Document::Object> )
			{
				if ( element.is_object() )
				{
					std::string elementPath;
					if ( arrayPath.empty() )
					{
						elementPath = "/" + std::to_string( index );
					}
					else if ( arrayPath[0] == '/' )
					{
						elementPath = std::string( arrayPath ) + "/" + std::to_string( index );
					}
					else
					{
						elementPath = "/" + std::string( arrayPath ) + "/" + std::to_string( index );
					}
					return Document::Object( const_cast<Document*>( docPtr ), elementPath );
				}
			}
		}

		return std::nullopt;
	}

	template std::optional<std::string_view> Document_impl::getArrayImpl<std::string_view>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<std::string> Document_impl::getArrayImpl<std::string>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<char> Document_impl::getArrayImpl<char>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<bool> Document_impl::getArrayImpl<bool>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<int8_t> Document_impl::getArrayImpl<int8_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<int16_t> Document_impl::getArrayImpl<int16_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<int32_t> Document_impl::getArrayImpl<int32_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<int64_t> Document_impl::getArrayImpl<int64_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<uint8_t> Document_impl::getArrayImpl<uint8_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<uint16_t> Document_impl::getArrayImpl<uint16_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<uint32_t> Document_impl::getArrayImpl<uint32_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<uint64_t> Document_impl::getArrayImpl<uint64_t>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<float> Document_impl::getArrayImpl<float>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<double> Document_impl::getArrayImpl<double>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<Document> Document_impl::getArrayImpl<Document>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<Document::Object> Document_impl::getArrayImpl<Document::Object>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;
	template std::optional<Document::Array> Document_impl::getArrayImpl<Document::Array>( std::string_view arrayPath, size_t index, const Document* docPtr ) const;

	template <typename T>
	void Document_impl::setArrayImpl( std::string_view arrayPath, size_t index, T&& value )
	{
		// Navigate to the array first
		nlohmann::ordered_json* arrayNode = nullptr;
		if ( arrayPath.empty() )
		{
			arrayNode = &m_data;
		}
		else if ( arrayPath[0] == '/' )
		{
			arrayNode = navigateToJsonPointer( arrayPath, true );
		}
		else
		{
			arrayNode = navigateToPath( arrayPath, true );
		}

		// Create array if it doesn't exist or is not an array
		if ( !arrayNode || !arrayNode->is_array() )
		{
			if ( arrayNode )
			{
				*arrayNode = nlohmann::ordered_json::array();
			}
			else
			{
				return; // Could not create path
			}
		}

		// Expand array if index is beyond current size
		while ( arrayNode->size() <= index )
		{
			arrayNode->push_back( nullptr );
		}

		// Set the value based on type
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			( *arrayNode )[index] = std::string( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			( *arrayNode )[index] = std::forward<T>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			( *arrayNode )[index] = std::string( 1, value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			( *arrayNode )[index] = std::forward<T>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int8_t> || std::is_same_v<std::decay_t<T>, int16_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			( *arrayNode )[index] = static_cast<int64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			( *arrayNode )[index] = std::forward<T>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint8_t> || std::is_same_v<std::decay_t<T>, uint16_t> || std::is_same_v<std::decay_t<T>, uint32_t> )
		{
			( *arrayNode )[index] = static_cast<uint64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint64_t> )
		{
			( *arrayNode )[index] = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, float> )
		{
			( *arrayNode )[index] = static_cast<double>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			( *arrayNode )[index] = std::forward<T>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			( *arrayNode )[index] = static_cast<Document_impl*>( value.m_impl )->data();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Object> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* objNode = nullptr;
				if ( value.m_path.empty() )
				{
					objNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( objNode )
				{
					( *arrayNode )[index] = *objNode;
				}
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* arrNode = nullptr;
				if ( value.m_path.empty() )
				{
					arrNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( arrNode )
				{
					( *arrayNode )[index] = *arrNode;
				}
			}
		}
	}

	template void Document_impl::setArrayImpl<std::string_view>( std::string_view arrayPath, size_t index, std::string_view&& value );
	template void Document_impl::setArrayImpl<std::string>( std::string_view arrayPath, size_t index, std::string&& value );
	template void Document_impl::setArrayImpl<const std::string&>( std::string_view arrayPath, size_t index, const std::string& value );
	template void Document_impl::setArrayImpl<char>( std::string_view arrayPath, size_t index, char&& value );
	template void Document_impl::setArrayImpl<bool>( std::string_view arrayPath, size_t index, bool&& value );
	template void Document_impl::setArrayImpl<int8_t>( std::string_view arrayPath, size_t index, int8_t&& value );
	template void Document_impl::setArrayImpl<int16_t>( std::string_view arrayPath, size_t index, int16_t&& value );
	template void Document_impl::setArrayImpl<int32_t>( std::string_view arrayPath, size_t index, int32_t&& value );
	template void Document_impl::setArrayImpl<int64_t>( std::string_view arrayPath, size_t index, int64_t&& value );
	template void Document_impl::setArrayImpl<uint8_t>( std::string_view arrayPath, size_t index, uint8_t&& value );
	template void Document_impl::setArrayImpl<uint16_t>( std::string_view arrayPath, size_t index, uint16_t&& value );
	template void Document_impl::setArrayImpl<uint32_t>( std::string_view arrayPath, size_t index, uint32_t&& value );
	template void Document_impl::setArrayImpl<uint64_t>( std::string_view arrayPath, size_t index, uint64_t&& value );
	template void Document_impl::setArrayImpl<float>( std::string_view arrayPath, size_t index, float&& value );
	template void Document_impl::setArrayImpl<double>( std::string_view arrayPath, size_t index, double&& value );
	template void Document_impl::setArrayImpl<Document>( std::string_view arrayPath, size_t index, Document&& value );
	template void Document_impl::setArrayImpl<const Document&>( std::string_view arrayPath, size_t index, const Document& value );
	template void Document_impl::setArrayImpl<Document::Object>( std::string_view arrayPath, size_t index, Document::Object&& value );
	template void Document_impl::setArrayImpl<const Document::Object&>( std::string_view arrayPath, size_t index, const Document::Object& value );
	template void Document_impl::setArrayImpl<Document::Array>( std::string_view arrayPath, size_t index, Document::Array&& value );
	template void Document_impl::setArrayImpl<const Document::Array&>( std::string_view arrayPath, size_t index, const Document::Array& value );

	template <typename T>
	void Document_impl::addArrayImpl( std::string_view arrayPath, T&& value )
	{
		// Navigate to the array first
		nlohmann::ordered_json* arrayNode = nullptr;
		if ( arrayPath.empty() )
		{
			arrayNode = &m_data;
		}
		else if ( arrayPath[0] == '/' )
		{
			arrayNode = navigateToJsonPointer( arrayPath, true );
		}
		else
		{
			arrayNode = navigateToPath( arrayPath, true );
		}

		// Create array if it doesn't exist or is not an array
		if ( !arrayNode || !arrayNode->is_array() )
		{
			if ( arrayNode )
			{
				*arrayNode = nlohmann::ordered_json::array();
			}
			else
			{
				return; // Could not create path
			}
		}

		// Add the value based on type
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			arrayNode->push_back( std::string( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			arrayNode->push_back( std::forward<T>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			arrayNode->push_back( std::string( 1, value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			arrayNode->push_back( std::forward<T>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int8_t> || std::is_same_v<std::decay_t<T>, int16_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			arrayNode->push_back( static_cast<int64_t>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			arrayNode->push_back( std::forward<T>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint8_t> || std::is_same_v<std::decay_t<T>, uint16_t> || std::is_same_v<std::decay_t<T>, uint32_t> )
		{
			arrayNode->push_back( static_cast<uint64_t>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint64_t> )
		{
			arrayNode->push_back( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, float> )
		{
			arrayNode->push_back( static_cast<double>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			arrayNode->push_back( std::forward<T>( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			arrayNode->push_back( static_cast<Document_impl*>( value.m_impl )->data() );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Object> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* objNode = nullptr;
				if ( value.m_path.empty() )
				{
					objNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( objNode )
				{
					arrayNode->push_back( *objNode );
				}
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* arrNode = nullptr;
				if ( value.m_path.empty() )
				{
					arrNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( arrNode )
				{
					arrayNode->push_back( *arrNode );
				}
			}
		}
	}

	template void Document_impl::addArrayImpl<std::string_view>( std::string_view arrayPath, std::string_view&& value );
	template void Document_impl::addArrayImpl<std::string>( std::string_view arrayPath, std::string&& value );
	template void Document_impl::addArrayImpl<const std::string&>( std::string_view arrayPath, const std::string& value );
	template void Document_impl::addArrayImpl<char>( std::string_view arrayPath, char&& value );
	template void Document_impl::addArrayImpl<bool>( std::string_view arrayPath, bool&& value );
	template void Document_impl::addArrayImpl<int8_t>( std::string_view arrayPath, int8_t&& value );
	template void Document_impl::addArrayImpl<int16_t>( std::string_view arrayPath, int16_t&& value );
	template void Document_impl::addArrayImpl<int32_t>( std::string_view arrayPath, int32_t&& value );
	template void Document_impl::addArrayImpl<int64_t>( std::string_view arrayPath, int64_t&& value );
	template void Document_impl::addArrayImpl<uint8_t>( std::string_view arrayPath, uint8_t&& value );
	template void Document_impl::addArrayImpl<uint16_t>( std::string_view arrayPath, uint16_t&& value );
	template void Document_impl::addArrayImpl<uint32_t>( std::string_view arrayPath, uint32_t&& value );
	template void Document_impl::addArrayImpl<uint64_t>( std::string_view arrayPath, uint64_t&& value );
	template void Document_impl::addArrayImpl<float>( std::string_view arrayPath, float&& value );
	template void Document_impl::addArrayImpl<double>( std::string_view arrayPath, double&& value );
	template void Document_impl::addArrayImpl<Document>( std::string_view arrayPath, Document&& value );
	template void Document_impl::addArrayImpl<const Document&>( std::string_view arrayPath, const Document& value );
	template void Document_impl::addArrayImpl<Document::Object>( std::string_view arrayPath, Document::Object&& value );
	template void Document_impl::addArrayImpl<const Document::Object&>( std::string_view arrayPath, const Document::Object& value );
	template void Document_impl::addArrayImpl<Document::Array>( std::string_view arrayPath, Document::Array&& value );
	template void Document_impl::addArrayImpl<const Document::Array&>( std::string_view arrayPath, const Document::Array& value );

	template <typename T>
	void Document_impl::insertArrayImpl( std::string_view arrayPath, size_t index, T&& value )
	{
		// Navigate to the array first
		nlohmann::ordered_json* arrayNode = nullptr;
		if ( arrayPath.empty() )
		{
			arrayNode = &m_data;
		}
		else if ( arrayPath[0] == '/' )
		{
			arrayNode = navigateToJsonPointer( arrayPath, true );
		}
		else
		{
			arrayNode = navigateToPath( arrayPath, true );
		}

		// Create array if it doesn't exist or is not an array
		if ( !arrayNode || !arrayNode->is_array() )
		{
			if ( arrayNode )
			{
				*arrayNode = nlohmann::ordered_json::array();
			}
			else
			{
				return; // Could not create path
			}
		}

		// Insert the value based on type
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( std::string( value ) );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, std::string( value ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( std::forward<T>( value ) );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, std::forward<T>( value ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			std::string str( 1, value );
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( str );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, str );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( std::forward<T>( value ) );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, std::forward<T>( value ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int8_t> || std::is_same_v<std::decay_t<T>, int16_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			int64_t val = static_cast<int64_t>( value );
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( val );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, val );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( std::forward<T>( value ) );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, std::forward<T>( value ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint8_t> || std::is_same_v<std::decay_t<T>, uint16_t> || std::is_same_v<std::decay_t<T>, uint32_t> )
		{
			uint64_t val = static_cast<uint64_t>( value );
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( val );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, val );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, uint64_t> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( value );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, value );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, float> )
		{
			double val = static_cast<double>( value );
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( val );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, val );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( std::forward<T>( value ) );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, std::forward<T>( value ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			const auto& docData = static_cast<Document_impl*>( value.m_impl )->data();
			if ( index >= arrayNode->size() )
			{
				arrayNode->push_back( docData );
			}
			else
			{
				arrayNode->insert( arrayNode->begin() + index, docData );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Object> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* objNode = nullptr;
				if ( value.m_path.empty() )
				{
					objNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					objNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( objNode )
				{
					if ( index >= arrayNode->size() )
					{
						arrayNode->push_back( *objNode );
					}
					else
					{
						arrayNode->insert( arrayNode->begin() + index, *objNode );
					}
				}
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( value.m_doc )
			{
				const nlohmann::ordered_json* arrNode = nullptr;
				if ( value.m_path.empty() )
				{
					arrNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					arrNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( arrNode )
				{
					if ( index >= arrayNode->size() )
					{
						arrayNode->push_back( *arrNode );
					}
					else
					{
						arrayNode->insert( arrayNode->begin() + index, *arrNode );
					}
				}
			}
		}
	}

	template void Document_impl::insertArrayImpl<std::string_view>( std::string_view arrayPath, size_t index, std::string_view&& value );
	template void Document_impl::insertArrayImpl<std::string>( std::string_view arrayPath, size_t index, std::string&& value );
	template void Document_impl::insertArrayImpl<const std::string&>( std::string_view arrayPath, size_t index, const std::string& value );
	template void Document_impl::insertArrayImpl<char>( std::string_view arrayPath, size_t index, char&& value );
	template void Document_impl::insertArrayImpl<bool>( std::string_view arrayPath, size_t index, bool&& value );
	template void Document_impl::insertArrayImpl<int8_t>( std::string_view arrayPath, size_t index, int8_t&& value );
	template void Document_impl::insertArrayImpl<int16_t>( std::string_view arrayPath, size_t index, int16_t&& value );
	template void Document_impl::insertArrayImpl<int32_t>( std::string_view arrayPath, size_t index, int32_t&& value );
	template void Document_impl::insertArrayImpl<int64_t>( std::string_view arrayPath, size_t index, int64_t&& value );
	template void Document_impl::insertArrayImpl<uint8_t>( std::string_view arrayPath, size_t index, uint8_t&& value );
	template void Document_impl::insertArrayImpl<uint16_t>( std::string_view arrayPath, size_t index, uint16_t&& value );
	template void Document_impl::insertArrayImpl<uint32_t>( std::string_view arrayPath, size_t index, uint32_t&& value );
	template void Document_impl::insertArrayImpl<uint64_t>( std::string_view arrayPath, size_t index, uint64_t&& value );
	template void Document_impl::insertArrayImpl<float>( std::string_view arrayPath, size_t index, float&& value );
	template void Document_impl::insertArrayImpl<double>( std::string_view arrayPath, size_t index, double&& value );
	template void Document_impl::insertArrayImpl<Document>( std::string_view arrayPath, size_t index, Document&& value );
	template void Document_impl::insertArrayImpl<const Document&>( std::string_view arrayPath, size_t index, const Document& value );
	template void Document_impl::insertArrayImpl<Document::Object>( std::string_view arrayPath, size_t index, Document::Object&& value );
	template void Document_impl::insertArrayImpl<const Document::Object&>( std::string_view arrayPath, size_t index, const Document::Object& value );
	template void Document_impl::insertArrayImpl<Document::Array>( std::string_view arrayPath, size_t index, Document::Array&& value );
	template void Document_impl::insertArrayImpl<const Document::Array&>( std::string_view arrayPath, size_t index, const Document::Array& value );
} // namespace nfx::serialization::json
