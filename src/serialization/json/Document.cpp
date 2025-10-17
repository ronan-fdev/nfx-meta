/**
 * @file Document.cpp
 * @brief Implementation of the Document class for JSON serialization.
 * @details Provides the concrete implementation for the Document facade, wrapping nlohmann::ordered_json.
 */

#include <cctype>
#include <functional>
#include <sstream>
#include <vector>

#include "nfx/serialization/json/Document.h"
#include "Document_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// Document class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Document::Document()
		: m_impl{ new Document_impl() }
	{
	}

	Document::Document( const Document& other )
		: m_impl{ other.m_impl
					  ? new Document_impl{ *static_cast<Document_impl*>( other.m_impl ) }
					  : nullptr }
	{
	}

	Document::Document( Document&& other ) noexcept
		: m_impl{ std::move( other.m_impl ) }
	{
		other.m_impl = nullptr;
	}

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	Document::~Document()
	{
		if ( m_impl )
		{
			delete static_cast<Document_impl*>( m_impl );
			m_impl = nullptr;
		}
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	Document& Document::operator=( const Document& other )
	{
		if ( this != &other )
		{
			if ( other.m_impl )
			{
				if ( m_impl )
				{
					*static_cast<Document_impl*>( m_impl ) = *static_cast<Document_impl*>( other.m_impl );
				}
				else
				{
					m_impl = new Document_impl{ *static_cast<Document_impl*>( other.m_impl ) };
				}
			}
			else
			{
				delete static_cast<Document_impl*>( m_impl );
				m_impl = nullptr;
			}
		}
		return *this;
	}

	Document& Document::operator=( Document&& other ) noexcept
	{
		if ( this != &other )
		{
			delete static_cast<Document_impl*>( m_impl );
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}

		return *this;
	}

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	bool Document::operator==( const Document& other ) const
	{
		if ( !m_impl || !other.m_impl )
		{
			return m_impl == other.m_impl;
		}

		return static_cast<Document_impl*>( m_impl )->data() ==
			   static_cast<Document_impl*>( other.m_impl )->data();
	}

	bool Document::operator!=( const Document& other ) const
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// Factory
	//----------------------------------------------

	std::optional<Document> Document::fromJsonString( std::string_view jsonStr )
	{
		if ( jsonStr.empty() )
		{
			return std::nullopt;
		}

		// Trim whitespace to check for effectively empty strings
		size_t start = 0;
		size_t end = jsonStr.length();

		// Find first non-whitespace character
		while ( start < end && std::isspace( static_cast<unsigned char>( jsonStr[start] ) ) )
		{
			++start;
		}

		// Find last non-whitespace character
		while ( end > start && std::isspace( static_cast<unsigned char>( jsonStr[end - 1] ) ) )
		{
			--end;
		}

		// If only whitespace, it's not valid JSON
		if ( start >= end )
		{
			return std::nullopt;
		}

		try
		{
			auto jsonData = nlohmann::ordered_json::parse( jsonStr );
			Document doc;
			delete static_cast<Document_impl*>( doc.m_impl );
			doc.m_impl = new Document_impl{ std::move( jsonData ) };
			return doc;
		}
		catch ( const nlohmann::ordered_json::exception& )
		{
			return std::nullopt;
		}
	}

	std::optional<Document> Document::fromJsonBytes( std::vector<uint8_t>& bytes )
	{
		if ( bytes.empty() )
		{
			return std::nullopt;
		}

		try
		{
			auto jsonData = nlohmann::ordered_json::parse( bytes );
			Document doc;
			delete static_cast<Document_impl*>( doc.m_impl );
			doc.m_impl = new Document_impl{ std::move( jsonData ) };
			return doc;
		}
		catch ( const nlohmann::ordered_json::exception& )
		{
			return std::nullopt;
		}
	}

	//----------------------------------------------
	// Output
	//----------------------------------------------

	std::string Document::toJsonString( int indent ) const
	{
		try
		{
			return static_cast<Document_impl*>( m_impl )->data().dump( indent );
		}
		catch ( const nlohmann::ordered_json::exception& e )
		{
			static_cast<Document_impl*>( m_impl )->setLastError( e.what() );
			return "{}";
		}
	}

	std::vector<uint8_t> Document::toJsonBytes() const
	{
		std::string jsonStr = toJsonString( 0 );
		return std::vector<uint8_t>( jsonStr.begin(), jsonStr.end() );
	}

	//----------------------------------------------
	// Value inspection
	//----------------------------------------------

	bool Document::hasValue( std::string_view path ) const
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		const nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		}

		// Return true if any JSON value exists at the path
		return node != nullptr;
	}

	//----------------------------------------------
	// Merge / update operations
	//----------------------------------------------

	void Document::merge( const Document& other, bool overwriteArrays )
	{
		if ( !other.m_impl )
		{
			return;
		}

		auto& thisData = static_cast<Document_impl*>( m_impl )->data();
		auto& otherData = static_cast<Document_impl*>( other.m_impl )->data();

		// Recursive merge function
		std::function<void( nlohmann::ordered_json&, const nlohmann::ordered_json& )> mergeRecursive =
			[&]( nlohmann::ordered_json& target, const nlohmann::ordered_json& source ) -> void {
			if ( source.is_object() && target.is_object() )
			{
				for ( auto it = source.begin(); it != source.end(); ++it )
				{
					if ( target.contains( it.key() ) && !overwriteArrays &&
						 it.value().is_array() && target[it.key()].is_array() )
					{
						// Merge arrays by appending
						for ( const auto& item : it.value() )
						{
							target[it.key()].push_back( item );
						}
					}
					else if ( target.contains( it.key() ) && it.value().is_object() && target[it.key()].is_object() )
					{
						// Recursively merge objects
						mergeRecursive( target[it.key()], it.value() );
					}
					else
					{
						// Overwrite or set new value
						target[it.key()] = it.value();
					}
				}
			}
			else
			{
				// Replace target with source
				target = source;
			}
		};

		mergeRecursive( thisData, otherData );
	}

	void Document::update( std::string_view path, const Document& value )
	{
		if ( !value.m_impl )
		{
			return;
		}

		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path, true );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		}

		if ( node )
		{
			*node = static_cast<Document_impl*>( value.m_impl )->data();
		}
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	std::optional<T> Document::get( std::string_view path ) const
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		const nlohmann::ordered_json* node = nullptr;
		if ( path.empty() )
		{
			// Empty path handling - get root document
			if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
			{
				Document result;
				static_cast<Document_impl*>( result.m_impl )->setData( static_cast<Document_impl*>( m_impl )->data() );
				return result;
			}
			else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
			{
				if ( static_cast<Document_impl*>( m_impl )->data().is_object() )
				{
					return Object( const_cast<Document*>( this ), "" );
				}
				return std::nullopt;
			}
			else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
			{
				if ( static_cast<Document_impl*>( m_impl )->data().is_array() )
				{
					return Array( const_cast<Document*>( this ), "" );
				}
				return std::nullopt;
			}
			else
			{
				node = &static_cast<Document_impl*>( m_impl )->data();
			}
		}
		else if ( path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		}

		if ( !node )
		{
			return std::nullopt;
		}

		// Type-specific extraction using if constexpr
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			if ( node->is_string() )
			{
				return node->get<std::string>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			if ( node->is_string() )
			{
				return std::string_view( node->get<std::string>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			if ( node->is_number_integer() )
			{
				return node->get<int64_t>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> )
		{
			if ( node->is_number_integer() )
			{
				return static_cast<int32_t>( node->get<int64_t>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			if ( node->is_number_float() )
			{
				return node->get<double>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			if ( node->is_boolean() )
			{
				return node->get<bool>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			if ( node->is_string() && node->get<std::string>().length() == 1 )
			{
				return node->get<std::string>()[0];
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			Document result;
			static_cast<Document_impl*>( result.m_impl )->setData( *node );
			return result;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( node->is_object() )
			{
				return Object( const_cast<Document*>( this ), std::string( path ) );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
		{
			if ( node->is_array() )
			{
				return Array( const_cast<Document*>( this ), std::string( path ) );
			}
		}

		return std::nullopt;
	}

	template std::optional<std::string_view> Document::get<std::string_view>( std::string_view path ) const;
	template std::optional<std::string> Document::get<std::string>( std::string_view path ) const;
	template std::optional<char> Document::get<char>( std::string_view path ) const;
	template std::optional<bool> Document::get<bool>( std::string_view path ) const;
	template std::optional<int32_t> Document::get<int32_t>( std::string_view path ) const;
	template std::optional<int64_t> Document::get<int64_t>( std::string_view path ) const;
	template std::optional<double> Document::get<double>( std::string_view path ) const;
	template std::optional<Document> Document::get<Document>( std::string_view path ) const;
	template std::optional<Document::Array> Document::get<Document::Array>( std::string_view path ) const;
	template std::optional<Document::Object> Document::get<Document::Object>( std::string_view path ) const;

	//----------------------------------------------
	// Value modification
	//----------------------------------------------

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::set( std::string_view path, const T& value )
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path, true );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		}

		if ( !node )
		{
			return;
		}

		// Type-specific assignment using if constexpr (copy semantics)
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*node = std::string{ value };
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*node = std::string( 1, value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*node = static_cast<int64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			*node = static_cast<Document_impl*>( value.m_impl )->data();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			*node = static_cast<Document_impl*>( value.m_doc->m_impl )->data();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Object> )
		{
			*node = static_cast<Document_impl*>( value.m_doc->m_impl )->data();
		}
	}

	template void Document::set<std::string_view>( std::string_view path, const std::string_view& );
	template void Document::set<std::string>( std::string_view path, const std::string& );
	template void Document::set<char>( std::string_view path, const char& );
	template void Document::set<bool>( std::string_view path, const bool& );
	template void Document::set<int32_t>( std::string_view path, const int32_t& );
	template void Document::set<int64_t>( std::string_view path, const int64_t& );
	template void Document::set<double>( std::string_view path, const double& );
	template void Document::set<Document>( std::string_view path, const Document& );
	template void Document::set<Document::Array>( std::string_view path, const Array& );
	template void Document::set<Document::Object>( std::string_view path, const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::set( std::string_view path, T&& value )
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path, true );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		}

		if ( !node )
		{
			return;
		}

		// Type-specific assignment using if constexpr (move semantics)
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*node = std::move( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*node = std::string( 1, value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*node = static_cast<int64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*node = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			*node = std::move( static_cast<Document_impl*>( value.m_impl )->data() );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			*node = std::move( static_cast<Document_impl*>( value.m_doc->m_impl )->data() );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Object> )
		{
			*node = std::move( static_cast<Document_impl*>( value.m_doc->m_impl )->data() );
		}
	}

	template void Document::set<std::string_view>( std::string_view path, std::string_view&& );
	template void Document::set<std::string>( std::string_view path, std::string&& );
	template void Document::set<char>( std::string_view path, char&& );
	template void Document::set<bool>( std::string_view path, bool&& );
	template void Document::set<int32_t>( std::string_view path, int32_t&& );
	template void Document::set<int64_t>( std::string_view path, int64_t&& );
	template void Document::set<double>( std::string_view path, double&& );
	template void Document::set<Document>( std::string_view path, Document&& );
	template void Document::set<Document::Array>( std::string_view path, Array&& );
	template void Document::set<Document::Object>( std::string_view path, Document::Object&& );

	//-----------------------------
	// Type-only creation
	//-----------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::set( std::string_view path )
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path, true );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		}

		if ( node )
		{
			if constexpr ( std::is_same_v<T, Document> )
			{
				*node = nlohmann::ordered_json::object();
			}
			else if constexpr ( std::is_same_v<T, Document::Object> )
			{
				*node = nlohmann::ordered_json::object();
			}
			else if constexpr ( std::is_same_v<T, Document::Array> )
			{
				*node = nlohmann::ordered_json::array();
			}
		}
	}

	template void Document::set<Document>( std::string_view path );
	template void Document::set<Document::Object>( std::string_view path );
	template void Document::set<Document::Array>( std::string_view path );

	//-----------------------------
	// Null operations
	//-----------------------------

	void Document::setNull( std::string_view path )
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path, true );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		}

		if ( node )
		{
			*node = nullptr;
		}
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	bool Document::is( std::string_view path ) const
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		const nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		}

		if ( !node )
		{
			return false;
		}

		// Type-specific checking using if constexpr
		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			return node->is_string();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> || std::is_same_v<std::decay_t<T>, int64_t> )
		{
			return node->is_number_integer();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			return node->is_number_float();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			return node->is_boolean();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			return node->is_string() && node->get<std::string>().length() == 1;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			return node->is_object();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
		{
			return node->is_array();
		}

		return false;
	}

	template bool Document::is<std::string_view>( std::string_view path ) const;
	template bool Document::is<std::string>( std::string_view path ) const;
	template bool Document::is<char>( std::string_view path ) const;
	template bool Document::is<bool>( std::string_view path ) const;
	template bool Document::is<int32_t>( std::string_view path ) const;
	template bool Document::is<int64_t>( std::string_view path ) const;
	template bool Document::is<double>( std::string_view path ) const;
	template bool Document::is<Document::Array>( std::string_view path ) const;
	template bool Document::is<Document::Object>( std::string_view path ) const;

	bool Document::isNull( std::string_view path ) const
	{
		// Auto-detect path syntax: paths starting with "/" are JSON Pointer, others are dot notation
		nlohmann::ordered_json* node = nullptr;
		if ( !path.empty() && path[0] == '/' )
		{
			// JSON Pointer (RFC 6901)
			node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( path );
		}
		else
		{
			// Dot notation
			node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		}
		return node && node->is_null();
	}

	//----------------------------------------------
	// Validation and error handling
	//----------------------------------------------

	bool Document::isValid() const
	{
		if ( !m_impl )
		{
			return false;
		}

		try
		{
			const auto& data = static_cast<Document_impl*>( m_impl )->data();

			// Check if it's not discarded (nlohmann::ordered_json's invalid state)
			if ( data.is_discarded() )
			{
				return false;
			}

			// Any well-formed JSON structure is considered a valid document
			// Schema-specific validation is handled by the SchemaValidator class
			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	std::string Document::lastError() const
	{
		return static_cast<Document_impl*>( m_impl )->lastError();
	}

	//----------------------------------------------
	// Document::Object class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	Document::Object::Object( Document* doc, std::string_view path )
		: m_doc{ doc },
		  m_path{ path }
	{
	}

	Document::Object::Object()
		: m_doc{ nullptr },
		  m_path{}
	{
	}

	Document::Object::Object( const Object& other )
		: m_doc{ other.m_doc },
		  m_path{ other.m_path }
	{
	}

	Document::Object::Object( Object&& other ) noexcept
		: m_doc{ other.m_doc },
		  m_path{ std::move( other.m_path ) }
	{
		other.m_doc = nullptr;
		other.m_path.clear();
	}

	//-----------------------------
	// Assignment
	//-----------------------------

	Document::Object& Document::Object::operator=( const Document::Object& other )
	{
		if ( this != &other )
		{
			m_doc = other.m_doc;
			m_path = other.m_path;
		}
		return *this;
	}

	Document::Object& Document::Object::operator=( Document::Object&& other ) noexcept
	{
		if ( this != &other )
		{
			m_doc = other.m_doc;
			m_path = std::move( other.m_path );
			other.m_doc = nullptr;
			other.m_path.clear();
		}
		return *this;
	}

	//-----------------------------
	// Comparison
	//-----------------------------

	bool Document::Object::operator==( const Document::Object& other ) const
	{
		if ( !m_doc || !other.m_doc )
		{
			return false;
		}

		if ( m_doc == other.m_doc && m_path == other.m_path )
		{
			return true;
		}

		const nlohmann::ordered_json* thisNode = nullptr;
		const nlohmann::ordered_json* otherNode = nullptr;

		if ( m_path.empty() )
		{
			thisNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			thisNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			thisNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( other.m_path.empty() )
		{
			otherNode = &static_cast<Document_impl*>( other.m_doc->m_impl )->data();
		}
		else if ( other.m_path[0] == '/' )
		{
			otherNode = static_cast<Document_impl*>( other.m_doc->m_impl )->navigateToJsonPointer( other.m_path );
		}
		else
		{
			otherNode = static_cast<Document_impl*>( other.m_doc->m_impl )->navigateToPath( other.m_path );
		}

		if ( !thisNode || !otherNode || !thisNode->is_object() || !otherNode->is_object() )
		{
			return false;
		}

		return *thisNode == *otherNode;
	}

	bool Document::Object::operator!=( const Document::Object& other ) const
	{
		return !( *this == other );
	}

	//-----------------------------
	// Output
	//-----------------------------

	std::string Document::Object::toJsonString( int indent ) const
	{
		if ( !m_doc )
		{
			return "{}";
		}

		Document objectDoc = m_doc->get<Document>( m_path ).value_or( Document{} );
		return objectDoc.toJsonString( indent );
	}

	std::vector<uint8_t> Document::Object::toJsonBytes() const
	{
		std::string jsonStr = toJsonString( 0 );
		return std::vector<uint8_t>( jsonStr.begin(), jsonStr.end() );
	}

	//-----------------------------
	// Field inspection
	//-----------------------------

	bool Document::Object::hasField( std::string_view fieldName ) const
	{
		if ( !m_doc )
		{
			return false;
		}

		if ( fieldName.empty() )
		{
			return false;
		}

		if ( fieldName[0] == '/' )
		{
			std::string_view pathView = fieldName.substr( 1 );
			if ( pathView.find( '/' ) != std::string_view::npos )
			{
				return false;
			}
			fieldName = pathView;
		}

		const nlohmann::ordered_json* objectNode = nullptr;
		if ( m_path.empty() )
		{
			objectNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			objectNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			objectNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( objectNode && objectNode->is_object() )
		{
			return objectNode->contains( std::string( fieldName ) );
		}

		return false;
	}

	//-----------------------------
	// Size
	//-----------------------------

	std::size_t Document::Object::size() const
	{
		if ( !m_doc )
		{
			return 0;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( node && node->is_object() )
		{
			return node->size();
		}

		return 0;
	}

	//-----------------------------
	// Clearing
	//-----------------------------

	void Document::Object::clear()
	{
		if ( !m_doc )
		{
			return;
		}

		nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path ) );
		}
		else
		{
			node = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path ) );
		}

		if ( node && node->is_object() )
		{
			node->clear();
		}
	}

	//-----------------------------
	// Field removal
	//-----------------------------

	bool Document::Object::removeField( std::string_view key )
	{
		if ( !m_doc || key.empty() )
		{
			return false;
		}

		// Handle JSON Pointer syntax
		std::string fieldName( key );
		if ( key[0] == '/' )
		{
			std::string_view pathView = key.substr( 1 );
			if ( pathView.find( '/' ) != std::string_view::npos )
			{
				return false; // Not a direct field
			}
			fieldName = std::string( pathView );
		}

		nlohmann::ordered_json* objectNode = nullptr;
		if ( m_path.empty() )
		{
			objectNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			objectNode = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path ) );
		}
		else
		{
			objectNode = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path ) );
		}

		if ( objectNode && objectNode->is_object() )
		{
			auto it = objectNode->find( fieldName );
			if ( it != objectNode->end() )
			{
				objectNode->erase( it );
				return true;
			}
		}

		return false;
	}

	//-----------------------------
	// Field access
	//-----------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	std::optional<T> Document::Object::get( std::string_view path ) const
	{
		if ( !m_doc || path.empty() )
		{
			return std::nullopt;
		}

		std::string fullPath;

		// Handle different path construction scenarios
		if ( m_path.empty() )
		{
			// Object is at root level
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path }; // Already a JSON pointer
			}
			else
			{
				fullPath = "/" + std::string{ path }; // Convert to JSON pointer
			}
		}
		else
		{
			// Object is nested within the document
			if ( path[0] == '/' )
			{
				// Path is already a JSON pointer, append to our path
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				// Convert regular path to JSON pointer format
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		// Always use JSON pointer navigation since we've normalized the path
		const nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath );

		if ( !targetNode )
		{
			return std::nullopt;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			if ( targetNode->is_string() )
			{
				return targetNode->get<std::string>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			if ( targetNode->is_string() )
			{
				return std::string_view( targetNode->get<std::string>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			if ( targetNode->is_number_integer() )
			{
				return targetNode->get<int64_t>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> )
		{
			if ( targetNode->is_number_integer() )
			{
				return static_cast<int32_t>( targetNode->get<int64_t>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			if ( targetNode->is_number_float() )
			{
				return targetNode->get<double>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			if ( targetNode->is_boolean() )
			{
				return targetNode->get<bool>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			if ( targetNode->is_string() && targetNode->get<std::string>().length() == 1 )
			{
				return targetNode->get<std::string>()[0];
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			Document newDoc;
			static_cast<Document_impl*>( newDoc.m_impl )->setData( *targetNode );
			return newDoc;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( targetNode->is_array() )
			{
				Document arrayDoc;
				static_cast<Document_impl*>( arrayDoc.m_impl )->setData( *targetNode );
				return Document::Array( &arrayDoc, "" );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( targetNode->is_object() )
			{
				Document objDoc;
				static_cast<Document_impl*>( objDoc.m_impl )->setData( *targetNode );
				return Object( &objDoc, "" );
			}
		}

		return std::nullopt;
	}

	template std::optional<std::string_view> Document::Object::get<std::string_view>( std::string_view path ) const;
	template std::optional<std::string> Document::Object::get<std::string>( std::string_view path ) const;
	template std::optional<char> Document::Object::get<char>( std::string_view path ) const;
	template std::optional<bool> Document::Object::get<bool>( std::string_view path ) const;
	template std::optional<int32_t> Document::Object::get<int32_t>( std::string_view path ) const;
	template std::optional<int64_t> Document::Object::get<int64_t>( std::string_view path ) const;
	template std::optional<double> Document::Object::get<double>( std::string_view path ) const;
	template std::optional<Document> Document::Object::get<Document>( std::string_view path ) const;
	template std::optional<Document::Array> Document::Object::get<Document::Array>( std::string_view path ) const;
	template std::optional<Document::Object> Document::Object::get<Document::Object>( std::string_view path ) const;

	//-----------------------------
	// Field modification
	//-----------------------------

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Object::set( std::string_view path, const T& value )
	{
		if ( !m_doc || path.empty() )
		{
			return;
		}

		std::string fullPath;

		// Handle different path construction scenarios
		if ( m_path.empty() )
		{
			// Object is at root level
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path }; // Already a JSON pointer
			}
			else
			{
				fullPath = "/" + std::string{ path }; // Convert to JSON pointer
			}
		}
		else
		{
			// Object is nested within the document
			if ( path[0] == '/' )
			{
				// Path is already a JSON pointer, append to our path
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				// Convert regular path to JSON pointer format
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		// Always use JSON pointer navigation since we've normalized the path
		nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath, true );

		if ( !targetNode )
		{
			return;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*targetNode = std::string{ value };
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*targetNode = static_cast<int64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*targetNode = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*targetNode = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*targetNode = std::string( 1, value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			if ( value.m_impl )
			{
				*targetNode = static_cast<Document_impl*>( value.m_impl )->data();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( value.m_doc && value.m_doc->m_impl )
			{
				const nlohmann::ordered_json* arrayNode = nullptr;
				if ( value.m_path.empty() )
				{
					arrayNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					arrayNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					arrayNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( arrayNode )
				{
					*targetNode = *arrayNode;
				}
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( value.m_doc && value.m_doc->m_impl )
			{
				const nlohmann::ordered_json* objectNode = nullptr;
				if ( value.m_path.empty() )
				{
					objectNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					objectNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					objectNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( objectNode )
				{
					*targetNode = *objectNode;
				}
			}
		}
	}

	template void Document::Object::set<std::string_view>( std::string_view path, const std::string_view& );
	template void Document::Object::set<std::string>( std::string_view path, const std::string& );
	template void Document::Object::set<char>( std::string_view path, const char& );
	template void Document::Object::set<bool>( std::string_view path, const bool& );
	template void Document::Object::set<int32_t>( std::string_view path, const int32_t& );
	template void Document::Object::set<int64_t>( std::string_view path, const int64_t& );
	template void Document::Object::set<double>( std::string_view path, const double& );
	template void Document::Object::set<Document>( std::string_view path, const Document& );
	template void Document::Object::set<Document::Array>( std::string_view path, const Document::Array& );
	template void Document::Object::set<Document::Object>( std::string_view path, const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Object::set( std::string_view path, T&& value )
	{
		if ( !m_doc || path.empty() )
		{
			return;
		}

		std::string fullPath;

		// Handle different path construction scenarios
		if ( m_path.empty() )
		{
			// Object is at root level
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path }; // Already a JSON pointer
			}
			else
			{
				fullPath = "/" + std::string{ path }; // Convert to JSON pointer
			}
		}
		else
		{
			// Object is nested
			if ( path[0] == '/' )
			{
				// Path is already a JSON pointer, append to our path
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				// Convert regular path to JSON pointer format
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		// Always use JSON pointer navigation since we've normalized the path
		nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath, true );

		if ( !targetNode )
		{
			return;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*targetNode = std::string{ std::move( value ) };
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*targetNode = static_cast<int64_t>( std::move( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*targetNode = std::move( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*targetNode = std::move( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*targetNode = std::string( 1, std::move( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			if ( value.m_impl )
			{
				*targetNode = std::move( static_cast<Document_impl*>( value.m_impl )->data() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document::Array> )
		{
			if ( value.m_doc && value.m_doc->m_impl )
			{
				const nlohmann::ordered_json* arrayNode = nullptr;
				if ( value.m_path.empty() )
				{
					arrayNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					arrayNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					arrayNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( arrayNode )
				{
					*targetNode = *arrayNode; // Note: nlohmann::json doesn't have move semantics for this operation ?
				}
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( value.m_doc && value.m_doc->m_impl )
			{
				const nlohmann::ordered_json* objectNode = nullptr;
				if ( value.m_path.empty() )
				{
					objectNode = &static_cast<Document_impl*>( value.m_doc->m_impl )->data();
				}
				else if ( value.m_path[0] == '/' )
				{
					objectNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToJsonPointer( value.m_path );
				}
				else
				{
					objectNode = static_cast<Document_impl*>( value.m_doc->m_impl )->navigateToPath( value.m_path );
				}
				if ( objectNode )
				{
					*targetNode = *objectNode; // Note: nlohmann::json doesn't have move semantics for this operation ?
				}
			}
		}
	}

	template void Document::Object::set<std::string_view>( std::string_view path, std::string_view&& );
	template void Document::Object::set<std::string>( std::string_view path, std::string&& );
	template void Document::Object::set<char>( std::string_view path, char&& );
	template void Document::Object::set<bool>( std::string_view path, bool&& );
	template void Document::Object::set<int32_t>( std::string_view path, int32_t&& );
	template void Document::Object::set<int64_t>( std::string_view path, int64_t&& );
	template void Document::Object::set<double>( std::string_view path, double&& );
	template void Document::Object::set<Document>( std::string_view path, Document&& );
	template void Document::Object::set<Document::Array>( std::string_view path, Document::Array&& );
	template void Document::Object::set<Document::Object>( std::string_view path, Document::Object&& );

	//-----------------------------
	// Validation and error handling
	//-----------------------------

	bool Document::Object::isValid() const
	{
		if ( !m_doc )
		{
			return false;
		}

		if ( !m_doc->isValid() )
		{
			return false;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		return node && node->is_object();
	}

	std::string Document::Object::lastError() const
	{
		if ( !m_doc )
		{
			return "Object has no document reference";
		}

		std::string docError = m_doc->lastError();
		if ( !docError.empty() )
		{
			return docError;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( !node )
		{
			return "Object path '" + m_path + "' does not exist in document";
		}

		if ( !node->is_object() )
		{
			return "Path '" + m_path + "' does not point to an object";
		}

		return "";
	}

	//----------------------------------------------
	// Document::Array class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	Document::Array::Array( Document* doc, std::string_view path )
		: m_doc{ doc },
		  m_path{ path }
	{
	}

	Document::Array::Array()
		: m_doc{ nullptr },
		  m_path{}
	{
	}

	Document::Array::Array( const Document::Array& other )
		: m_doc{ other.m_doc },
		  m_path{ other.m_path }
	{
	}

	Document::Array::Array( Document::Array&& other ) noexcept
		: m_doc{ other.m_doc },
		  m_path{ std::move( other.m_path ) }
	{
		other.m_doc = nullptr;
		other.m_path.clear();
	}

	//-----------------------------
	// Assignment
	//-----------------------------

	Document::Array& Document::Array::operator=( const Document::Array& other )
	{
		if ( this != &other )
		{
			m_doc = other.m_doc;
			m_path = other.m_path;
		}
		return *this;
	}

	Document::Array& Document::Array::operator=( Document::Array&& other ) noexcept
	{
		if ( this != &other )
		{
			m_doc = other.m_doc;
			m_path = std::move( other.m_path );
			other.m_doc = nullptr;
			other.m_path.clear();
		}
		return *this;
	}

	//-----------------------------
	// Comparison
	//-----------------------------

	bool Document::Array::operator==( const Document::Array& other ) const
	{
		if ( !m_doc || !other.m_doc )
		{
			return false;
		}

		if ( m_doc == other.m_doc && m_path == other.m_path )
		{
			return true;
		}

		const nlohmann::ordered_json* thisNode = nullptr;
		const nlohmann::ordered_json* otherNode = nullptr;

		if ( m_path.empty() )
		{
			thisNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			thisNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			thisNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( other.m_path.empty() )
		{
			otherNode = &static_cast<Document_impl*>( other.m_doc->m_impl )->data();
		}
		else if ( other.m_path[0] == '/' )
		{
			otherNode = static_cast<Document_impl*>( other.m_doc->m_impl )->navigateToJsonPointer( other.m_path );
		}
		else
		{
			otherNode = static_cast<Document_impl*>( other.m_doc->m_impl )->navigateToPath( other.m_path );
		}

		if ( !thisNode || !otherNode || !thisNode->is_array() || !otherNode->is_array() )
		{
			return false;
		}

		return *thisNode == *otherNode;
	}

	bool Document::Array::operator!=( const Document::Array& other ) const
	{
		return !( *this == other );
	}

	//-----------------------------
	// Output
	//-----------------------------

	std::string Document::Array::toJsonString( int indent ) const
	{
		if ( !m_doc )
		{
			return "[]";
		}

		Document arrayDoc = m_doc->get<Document>( m_path ).value_or( Document{} );
		return arrayDoc.toJsonString( indent );
	}

	std::vector<uint8_t> Document::Array::toJsonBytes() const
	{
		std::string jsonStr = toJsonString( 0 );
		return std::vector<uint8_t>( jsonStr.begin(), jsonStr.end() );
	}

	//-----------------------------
	// Element inspection
	//-----------------------------

	bool Document::Array::hasElement( std::string_view indexStr ) const
	{
		if ( !m_doc )
		{
			return false;
		}

		if ( indexStr.empty() )
		{
			return false;
		}

		try
		{
			std::string indexString( indexStr );
			if ( !indexString.empty() && indexString[0] == '/' )
			{
				indexString = indexString.substr( 1 );
			}
			size_t index = std::stoull( indexString );

			const nlohmann::ordered_json* arrayNode = nullptr;
			if ( m_path.empty() )
			{
				arrayNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
			}
			else if ( m_path[0] == '/' )
			{
				arrayNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
			}
			else
			{
				arrayNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
			}

			if ( arrayNode && arrayNode->is_array() )
			{
				return index < arrayNode->size();
			}

			return false;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//-----------------------------
	// Size
	//-----------------------------

	std::size_t Document::Array::size() const
	{
		if ( !m_doc )
		{
			return 0;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( node && node->is_array() )
		{
			return node->size();
		}

		return 0;
	}

	//-----------------------------
	// Clearing
	//-----------------------------

	void Document::Array::clear()
	{
		if ( !m_doc )
		{
			return;
		}

		nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path ) );
		}
		else
		{
			node = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path ) );
		}

		if ( node && node->is_array() )
		{
			node->clear();
		}
	}

	//-----------------------------
	// Element removal
	//-----------------------------

	bool Document::Array::remove( size_t index )
	{
		if ( !m_doc )
		{
			return false;
		}

		nlohmann::ordered_json* arrayNode = nullptr;
		if ( m_path.empty() )
		{
			arrayNode = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			arrayNode = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path ) );
		}
		else
		{
			arrayNode = const_cast<nlohmann::ordered_json*>( static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path ) );
		}

		if ( arrayNode && arrayNode->is_array() && index < arrayNode->size() )
		{
			arrayNode->erase( arrayNode->begin() + index );
			return true;
		}

		return false;
	}

	//-----------------------------
	// Element access
	//-----------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	std::optional<T> Document::Array::get( size_t index ) const
	{
		if ( !m_doc )
		{
			return std::nullopt;
		}

		return static_cast<Document_impl*>( m_doc->m_impl )->getArrayImpl<T>( m_path, index, m_doc );
	}

	template std::optional<std::string_view> Document::Array::get<std::string_view>( size_t index ) const;
	template std::optional<std::string> Document::Array::get<std::string>( size_t index ) const;
	template std::optional<char> Document::Array::get<char>( size_t index ) const;
	template std::optional<bool> Document::Array::get<bool>( size_t index ) const;
	template std::optional<int32_t> Document::Array::get<int32_t>( size_t index ) const;
	template std::optional<int64_t> Document::Array::get<int64_t>( size_t index ) const;
	template std::optional<double> Document::Array::get<double>( size_t index ) const;
	template std::optional<Document> Document::Array::get<Document>( size_t index ) const;
	template std::optional<Document::Array> Document::Array::get<Document::Array>( size_t index ) const;
	template std::optional<Document::Object> Document::Array::get<Document::Object>( size_t index ) const;

	//-----------------------------
	// Nested element access
	//-----------------------------

	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	std::optional<T> Document::Array::get( std::string_view path ) const
	{
		if ( !m_doc || path.empty() )
		{
			return std::nullopt;
		}

		std::string fullPath;

		if ( m_path.empty() )
		{
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path };
			}
			else
			{
				fullPath = "/" + std::string{ path };
			}
		}
		else
		{
			if ( path[0] == '/' )
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		const nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath );

		if ( !targetNode )
		{
			return std::nullopt;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> )
		{
			if ( targetNode->is_string() )
			{
				return targetNode->get<std::string>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			if ( targetNode->is_string() )
			{
				return std::string_view( targetNode->get<std::string>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> )
		{
			if ( targetNode->is_number_integer() )
			{
				return targetNode->get<int64_t>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int32_t> )
		{
			if ( targetNode->is_number_integer() )
			{
				return static_cast<int32_t>( targetNode->get<int64_t>() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			if ( targetNode->is_number_float() )
			{
				return targetNode->get<double>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			if ( targetNode->is_boolean() )
			{
				return targetNode->get<bool>();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			if ( targetNode->is_string() && targetNode->get<std::string>().length() == 1 )
			{
				return targetNode->get<std::string>()[0];
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			Document newDoc;
			static_cast<Document_impl*>( newDoc.m_impl )->setData( *targetNode );
			return newDoc;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
		{
			if ( targetNode->is_array() )
			{
				Document arrayDoc;
				static_cast<Document_impl*>( arrayDoc.m_impl )->setData( *targetNode );
				return Array( &arrayDoc, "" );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( targetNode->is_object() )
			{
				Document objDoc;
				static_cast<Document_impl*>( objDoc.m_impl )->setData( *targetNode );
				return Object( &objDoc, "" );
			}
		}

		return std::nullopt;
	}

	template std::optional<std::string_view> Document::Array::get<std::string_view>( std::string_view path ) const;
	template std::optional<std::string> Document::Array::get<std::string>( std::string_view path ) const;
	template std::optional<char> Document::Array::get<char>( std::string_view path ) const;
	template std::optional<bool> Document::Array::get<bool>( std::string_view path ) const;
	template std::optional<int32_t> Document::Array::get<int32_t>( std::string_view path ) const;
	template std::optional<int64_t> Document::Array::get<int64_t>( std::string_view path ) const;
	template std::optional<double> Document::Array::get<double>( std::string_view path ) const;
	template std::optional<Document> Document::Array::get<Document>( std::string_view path ) const;
	template std::optional<Document::Array> Document::Array::get<Document::Array>( std::string_view path ) const;
	template std::optional<Document::Object> Document::Array::get<Document::Object>( std::string_view path ) const;

	//-----------------------------
	// Element modification
	//-----------------------------

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::set( size_t index, const T& value )
	{
		if ( m_doc )
		{
			static_cast<Document_impl*>( m_doc->m_impl )->setArrayImpl<T>( m_path, index, T{ value } );
		}
	}

	template void Document::Array::set<std::string_view>( size_t index, const std::string_view& );
	template void Document::Array::set<std::string>( size_t index, const std::string& );
	template void Document::Array::set<char>( size_t index, const char& );
	template void Document::Array::set<bool>( size_t index, const bool& );
	template void Document::Array::set<int32_t>( size_t index, const int32_t& );
	template void Document::Array::set<int64_t>( size_t index, const int64_t& );
	template void Document::Array::set<double>( size_t index, const double& );
	template void Document::Array::set<Document>( size_t index, const Document& );
	template void Document::Array::set<Document::Array>( size_t index, const Array& );
	template void Document::Array::set<Document::Object>( size_t index, const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::set( size_t index, T&& value )
	{
		if ( m_doc )
		{
			static_cast<Document_impl*>( m_doc->m_impl )->setArrayImpl<T>( m_path, index, std::move( value ) );
		}
	}

	template void Document::Array::set<std::string_view>( size_t index, std::string_view&& );
	template void Document::Array::set<std::string>( size_t index, std::string&& );
	template void Document::Array::set<char>( size_t index, char&& );
	template void Document::Array::set<bool>( size_t index, bool&& );
	template void Document::Array::set<int32_t>( size_t index, int32_t&& );
	template void Document::Array::set<int64_t>( size_t index, int64_t&& );
	template void Document::Array::set<double>( size_t index, double&& );
	template void Document::Array::set<Document>( size_t index, Document&& );
	template void Document::Array::set<Document::Array>( size_t index, Array&& );
	template void Document::Array::set<Document::Object>( size_t index, Document::Object&& );

	//-----------------------------
	// Nested element modification
	//-----------------------------

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::set( std::string_view path, const T& value )
	{
		if ( !m_doc || path.empty() )
		{
			return;
		}

		std::string fullPath;

		if ( m_path.empty() )
		{
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path };
			}
			else
			{
				fullPath = "/" + std::string{ path };
			}
		}
		else
		{
			if ( path[0] == '/' )
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath, true );

		if ( !targetNode )
		{
			return;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*targetNode = std::string{ value };
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*targetNode = static_cast<int64_t>( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*targetNode = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*targetNode = value;
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*targetNode = std::string( 1, value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			*targetNode = static_cast<Document_impl*>( value.m_impl )->data();
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
		{
			if ( value.m_doc )
			{
				Document arrDoc = value.m_doc->template get<Document>( value.m_path ).value_or( Document{} );
				*targetNode = static_cast<Document_impl*>( arrDoc.m_impl )->data();
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( value.m_doc )
			{
				Document objDoc = value.m_doc->template get<Document>( value.m_path ).value_or( Document{} );
				*targetNode = static_cast<Document_impl*>( objDoc.m_impl )->data();
			}
		}
	}

	template void Document::Array::set<std::string_view>( std::string_view path, const std::string_view& );
	template void Document::Array::set<std::string>( std::string_view path, const std::string& );
	template void Document::Array::set<char>( std::string_view path, const char& );
	template void Document::Array::set<bool>( std::string_view path, const bool& );
	template void Document::Array::set<int32_t>( std::string_view path, const int32_t& );
	template void Document::Array::set<int64_t>( std::string_view path, const int64_t& );
	template void Document::Array::set<double>( std::string_view path, const double& );
	template void Document::Array::set<Document>( std::string_view path, const Document& );
	template void Document::Array::set<Document::Array>( std::string_view path, const Array& );
	template void Document::Array::set<Document::Object>( std::string_view path, const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::set( std::string_view path, T&& value )
	{
		if ( !m_doc || path.empty() )
		{
			return;
		}

		std::string fullPath;

		if ( m_path.empty() )
		{
			if ( path[0] == '/' )
			{
				fullPath = std::string{ path };
			}
			else
			{
				fullPath = "/" + std::string{ path };
			}
		}
		else
		{
			if ( path[0] == '/' )
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + std::string{ path };
				}
			}
			else
			{
				if ( m_path[0] == '/' )
				{
					fullPath = m_path + "/" + std::string{ path };
				}
				else
				{
					fullPath = "/" + m_path + "/" + std::string{ path };
				}
			}
		}

		nlohmann::ordered_json* targetNode = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( fullPath, true );

		if ( !targetNode )
		{
			return;
		}

		if constexpr ( std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view> )
		{
			*targetNode = std::string{ std::move( value ) };
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, int64_t> || std::is_same_v<std::decay_t<T>, int32_t> )
		{
			*targetNode = static_cast<int64_t>( std::move( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, double> )
		{
			*targetNode = std::move( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, bool> )
		{
			*targetNode = std::move( value );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, char> )
		{
			*targetNode = std::string( 1, std::move( value ) );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Document> )
		{
			*targetNode = std::move( static_cast<Document_impl*>( value.m_impl )->data() );
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Array> )
		{
			if ( value.m_doc )
			{
				Document arrDoc = value.m_doc->template get<Document>( value.m_path ).value_or( Document{} );
				*targetNode = std::move( static_cast<Document_impl*>( arrDoc.m_impl )->data() );
			}
		}
		else if constexpr ( std::is_same_v<std::decay_t<T>, Object> )
		{
			if ( value.m_doc )
			{
				Document objDoc = value.m_doc->template get<Document>( value.m_path ).value_or( Document{} );
				*targetNode = std::move( static_cast<Document_impl*>( objDoc.m_impl )->data() );
			}
		}
	}

	template void Document::Array::set<std::string_view>( std::string_view path, std::string_view&& );
	template void Document::Array::set<std::string>( std::string_view path, std::string&& );
	template void Document::Array::set<char>( std::string_view path, char&& );
	template void Document::Array::set<bool>( std::string_view path, bool&& );
	template void Document::Array::set<int32_t>( std::string_view path, int32_t&& );
	template void Document::Array::set<int64_t>( std::string_view path, int64_t&& );
	template void Document::Array::set<double>( std::string_view path, double&& );
	template void Document::Array::set<Document>( std::string_view path, Document&& );
	template void Document::Array::set<Document::Array>( std::string_view path, Array&& );
	template void Document::Array::set<Document::Object>( std::string_view path, Document::Object&& );

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::add( const T& value )
	{
		if ( m_doc )
		{
			static_cast<Document_impl*>( m_doc->m_impl )->addArrayImpl<T>( m_path, T{ value } );
		}
	}

	template void Document::Array::add<std::string_view>( const std::string_view& );
	template void Document::Array::add<std::string>( const std::string& );
	template void Document::Array::add<char>( const char& );
	template void Document::Array::add<bool>( const bool& );
	template void Document::Array::add<int32_t>( const int32_t& );
	template void Document::Array::add<int64_t>( const int64_t& );
	template void Document::Array::add<double>( const double& );
	template void Document::Array::add<Document>( const Document& );
	template void Document::Array::add<Document::Array>( const Document::Array& );
	template void Document::Array::add<Document::Object>( const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::add( T&& value )
	{
		if ( m_doc )
		{
			static_cast<Document_impl*>( m_doc->m_impl )->addArrayImpl<T>( m_path, std::move( value ) );
		}
	}

	template void Document::Array::add<std::string_view>( std::string_view&& );
	template void Document::Array::add<std::string>( std::string&& );
	template void Document::Array::add<char>( char&& );
	template void Document::Array::add<bool>( bool&& );
	template void Document::Array::add<int32_t>( int32_t&& );
	template void Document::Array::add<int64_t>( int64_t&& );
	template void Document::Array::add<double>( double&& );
	template void Document::Array::add<Document>( Document&& );
	template void Document::Array::add<Document::Array>( Document::Array&& );
	template void Document::Array::add<Document::Object>( Document::Object&& );

	// Reference versions (for non-const lvalue matching) - delegate to copy version
	void Document::Array::add( Document& value )
	{
		add<Document>( static_cast<const Document&>( value ) );
	}

	void Document::Array::add( Document::Array& value )
	{
		add<Document::Array>( static_cast<const Document::Array&>( value ) );
	}

	void Document::Array::add( Document::Object& value )
	{
		add<Document::Object>( static_cast<const Document::Object&>( value ) );
	}

	// Copy version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::insert( size_t index, const T& value )
	{
		static_cast<Document_impl*>( m_doc->m_impl )->insertArrayImpl<T>( m_path, index, T{ value } );
	}

	template void Document::Array::insert<std::string_view>( size_t index, const std::string_view& );
	template void Document::Array::insert<std::string>( size_t index, const std::string& );
	template void Document::Array::insert<char>( size_t index, const char& );
	template void Document::Array::insert<bool>( size_t index, const bool& );
	template void Document::Array::insert<int32_t>( size_t index, const int32_t& );
	template void Document::Array::insert<int64_t>( size_t index, const int64_t& );
	template void Document::Array::insert<double>( size_t index, const double& );
	template void Document::Array::insert<Document>( size_t index, const Document& );
	template void Document::Array::insert<Document::Array>( size_t index, const Array& );
	template void Document::Array::insert<Document::Object>( size_t index, const Document::Object& );

	// Move version
	template <typename T>
		requires(
			std::is_same_v<std::decay_t<T>, std::string_view> ||
			std::is_same_v<std::decay_t<T>, std::string> ||
			std::is_same_v<std::decay_t<T>, char> ||
			std::is_same_v<std::decay_t<T>, bool> ||
			std::is_same_v<std::decay_t<T>, int32_t> ||
			std::is_same_v<std::decay_t<T>, int64_t> ||
			std::is_same_v<std::decay_t<T>, double> ||
			std::is_same_v<std::decay_t<T>, Document> ||
			std::is_same_v<std::decay_t<T>, Document::Object> ||
			std::is_same_v<std::decay_t<T>, Document::Array> )
	void Document::Array::insert( size_t index, T&& value )
	{
		static_cast<Document_impl*>( m_doc->m_impl )->insertArrayImpl<T>( m_path, index, std::move( value ) );
	}

	template void Document::Array::insert<std::string_view>( size_t index, std::string_view&& );
	template void Document::Array::insert<std::string>( size_t index, std::string&& );
	template void Document::Array::insert<char>( size_t index, char&& );
	template void Document::Array::insert<bool>( size_t index, bool&& );
	template void Document::Array::insert<int32_t>( size_t index, int32_t&& );
	template void Document::Array::insert<int64_t>( size_t index, int64_t&& );
	template void Document::Array::insert<double>( size_t index, double&& );
	template void Document::Array::insert<Document>( size_t index, Document&& );
	template void Document::Array::insert<Document::Array>( size_t index, Array&& );
	template void Document::Array::insert<Document::Object>( size_t index, Document::Object&& );

	// Reference versions (for non-const lvalue matching) - delegate to copy version
	void Document::Array::insert( size_t index, Document& value )
	{
		insert<Document>( index, static_cast<const Document&>( value ) );
	}

	void Document::Array::insert( size_t index, Document::Array& value )
	{
		insert<Document::Array>( index, static_cast<const Document::Array&>( value ) );
	}

	void Document::Array::insert( size_t index, Document::Object& value )
	{
		insert<Document::Object>( index, static_cast<const Document::Object&>( value ) );
	}

	//-----------------------------
	// Validation and error handling
	//-----------------------------

	bool Document::Array::isValid() const
	{
		if ( !m_doc )
		{
			return false;
		}

		if ( !m_doc->isValid() )
		{
			return false;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		return node && node->is_array();
	}

	std::string Document::Array::lastError() const
	{
		if ( !m_doc )
		{
			return "Array has no document reference";
		}

		std::string docError = m_doc->lastError();
		if ( !docError.empty() )
		{
			return docError;
		}

		const nlohmann::ordered_json* node = nullptr;
		if ( m_path.empty() )
		{
			node = &static_cast<Document_impl*>( m_doc->m_impl )->data();
		}
		else if ( m_path[0] == '/' )
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToJsonPointer( m_path );
		}
		else
		{
			node = static_cast<Document_impl*>( m_doc->m_impl )->navigateToPath( m_path );
		}

		if ( !node )
		{
			return "Array path '" + m_path + "' does not exist in document";
		}

		if ( !node->is_array() )
		{
			return "Path '" + m_path + "' does not point to an array";
		}

		return "";
	}
} // namespace nfx::serialization::json
