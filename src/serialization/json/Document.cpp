/**
 * @file Document.cpp
 * @brief Implementation of the Document class for JSON serialization.
 * @details Provides the concrete implementation for the Document facade, wrapping nlohmann::json.
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
		: m_impl{ other.m_impl }
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
	// Factory
	//----------------------------------------------

	std::optional<Document> Document::fromJsonString( std::string_view jsonStr )
	{
		// Empty string is not valid JSON
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
			auto jsonData = nlohmann::json::parse( jsonStr );
			Document doc;
			delete static_cast<Document_impl*>( doc.m_impl );
			doc.m_impl = new Document_impl{ std::move( jsonData ) };
			return doc;
		}
		catch ( const nlohmann::json::exception& )
		{
			return std::nullopt;
		}
	}

	Document Document::createObject()
	{
		Document doc;
		static_cast<Document_impl*>( doc.m_impl )->setData( nlohmann::json::object() );
		return doc;
	}

	Document Document::createArray()
	{
		Document doc;
		static_cast<Document_impl*>( doc.m_impl )->setData( nlohmann::json::array() );
		return doc;
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
		catch ( const nlohmann::json::exception& e )
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
	// Value access
	//----------------------------------------------

	bool Document::hasField( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node != nullptr;
	}

	std::optional<std::string> Document::getString( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_string() )
		{
			return node->get<std::string>();
		}
		return std::nullopt;
	}

	std::optional<int64_t> Document::getInt( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_number_integer() )
		{
			return node->get<std::int64_t>();
		}
		return std::nullopt;
	}

	std::optional<double> Document::getDouble( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_number() )
		{
			return node->get<double>();
		}
		return std::nullopt;
	}

	std::optional<bool> Document::getBool( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_boolean() )
		{
			return node->get<bool>();
		}
		return std::nullopt;
	}

	//----------------------------------------------
	// JSON Pointer access (RFC 6901)
	//----------------------------------------------

	bool Document::hasFieldByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( !node )
			return false;

		if ( pointer.empty() || pointer == "/" )
		{
			return node->is_object();
		}

		size_t lastSlash = pointer.find_last_of( '/' );
		if ( lastSlash == 0 )
		{
			const nlohmann::json& root = static_cast<Document_impl*>( m_impl )->data();
			return root.is_object();
		}

		std::string parentPointer{ pointer.substr( 0, lastSlash ) };
		const nlohmann::json* parent = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( parentPointer );
		return parent && parent->is_object();
	}

	bool Document::hasValueByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node != nullptr;
	}

	bool Document::hasArrayByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_array();
	}

	bool Document::hasObjectByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_object();
	}

	bool Document::hasStringByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_string();
	}

	bool Document::hasIntByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_number_integer();
	}

	bool Document::hasDoubleByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_number_float();
	}

	bool Document::hasBoolByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_boolean();
	}

	bool Document::hasNullByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		return node && node->is_null();
	}

	std::optional<Document> Document::getDocumentByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node )
		{
			Document result;
			static_cast<Document_impl*>( result.m_impl )->setData( *node );
			return result;
		}
		return std::nullopt;
	}

	std::optional<Document> Document::getArrayByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_array() )
		{
			Document result;
			static_cast<Document_impl*>( result.m_impl )->setData( *node );
			return result;
		}
		return std::nullopt;
	}

	std::optional<Document> Document::getObjectByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_object() )
		{
			Document result;
			static_cast<Document_impl*>( result.m_impl )->setData( *node );
			return result;
		}
		return std::nullopt;
	}

	std::optional<std::string> Document::getStringByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_string() )
		{
			return node->get<std::string>();
		}
		return std::nullopt;
	}

	std::optional<int64_t> Document::getIntByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_number_integer() )
		{
			return node->get<std::int64_t>();
		}
		return std::nullopt;
	}

	std::optional<double> Document::getDoubleByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_number() )
		{
			return node->get<double>();
		}
		return std::nullopt;
	}

	std::optional<bool> Document::getBoolByPointer( std::string_view pointer ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer );
		if ( node && node->is_boolean() )
		{
			return node->get<bool>();
		}
		return std::nullopt;
	}

	//----------------------------------------------
	// Value setting
	//----------------------------------------------

	void Document::setString( std::string_view path, std::string_view value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setInt( std::string_view path, int64_t value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setDouble( std::string_view path, double value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setBool( std::string_view path, bool value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setNull( std::string_view path )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = nullptr;
		}
	}

	//----------------------------------------------
	// JSON Pointer value setting (RFC 6901)
	//----------------------------------------------

	void Document::setDocumentByPointer( std::string_view pointer, const Document& document )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = static_cast<Document_impl*>( document.m_impl )->data();
		}
	}

	void Document::setArrayByPointer( std::string_view pointer, const Document& arrayDocument )
	{
		// Convenience wrapper around the generic method
		setDocumentByPointer( pointer, arrayDocument );
	}

	void Document::setObjectByPointer( std::string_view pointer, const Document& objectDocument )
	{
		// Convenience wrapper around the generic method
		setDocumentByPointer( pointer, objectDocument );
	}

	void Document::setStringByPointer( std::string_view pointer, std::string_view value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setIntByPointer( std::string_view pointer, int64_t value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setDoubleByPointer( std::string_view pointer, double value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setBoolByPointer( std::string_view pointer, bool value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = value;
		}
	}

	void Document::setNullByPointer( std::string_view pointer )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToJsonPointer( pointer, true );
		if ( node )
		{
			*node = nullptr;
		}
	}

	//----------------------------------------------
	// Array operations
	//----------------------------------------------

	size_t Document::getArraySize( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() )
		{
			return node->size();
		}
		return 0;
	}

	void Document::addToArray( std::string_view path, std::string_view value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			if ( !node->is_array() )
			{
				*node = nlohmann::json::array();
			}
			node->push_back( value );
		}
	}

	void Document::addToArray( std::string_view path, const char* value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			if ( !node->is_array() )
			{
				*node = nlohmann::json::array();
			}
			node->push_back( std::string( value ) );
		}
	}

	void Document::addToArray( std::string_view path, int64_t value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			if ( !node->is_array() )
			{
				*node = nlohmann::json::array();
			}
			node->push_back( value );
		}
	}

	void Document::addToArray( std::string_view path, double value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			if ( !node->is_array() )
			{
				*node = nlohmann::json::array();
			}
			node->push_back( value );
		}
	}

	void Document::addToArray( std::string_view path, bool value )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			if ( !node->is_array() )
			{
				*node = nlohmann::json::array();
			}
			node->push_back( value );
		}
	}

	void Document::clearArray( std::string_view path )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, false );
		if ( node && node->is_array() )
		{
			node->clear();
		}
	}

	std::optional<std::string> Document::getArrayElementString( std::string_view path, size_t index ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() && index < node->size() )
		{
			const auto& element = ( *node )[index];
			if ( element.is_string() )
			{
				return element.get<std::string>();
			}
		}
		return std::nullopt;
	}

	std::optional<int64_t> Document::getArrayElementInt( std::string_view path, size_t index ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() && index < node->size() )
		{
			const auto& element = ( *node )[index];
			if ( element.is_number_integer() )
			{
				return element.get<int64_t>();
			}
		}
		return std::nullopt;
	}

	std::optional<double> Document::getArrayElementDouble( std::string_view path, size_t index ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() && index < node->size() )
		{
			const auto& element = ( *node )[index];
			if ( element.is_number() )
			{
				return element.get<double>();
			}
		}
		return std::nullopt;
	}

	std::optional<bool> Document::getArrayElementBool( std::string_view path, size_t index ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() && index < node->size() )
		{
			const auto& element = ( *node )[index];
			if ( element.is_boolean() )
			{
				return element.get<bool>();
			}
		}
		return std::nullopt;
	}

	//----------------------------------------------
	// Advanced array and document operations
	//----------------------------------------------

	bool Document::isArray( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_array();
	}

	Document Document::getArrayElement( std::string_view path, size_t index ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		if ( node && node->is_array() && index < node->size() )
		{
			Document result;
			static_cast<Document_impl*>( result.m_impl )->setData( ( *node )[index] );
			return result;
		}
		return Document{}; // Return empty document if not found
	}

	void Document::setArray( std::string_view path, const Document& arrayDocument )
	{
		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = static_cast<Document_impl*>( arrayDocument.m_impl )->data();
		}
	}

	void Document::addDocument( const Document& document )
	{
		// This method is for array documents - add a document to this array
		if ( !static_cast<Document_impl*>( m_impl )->data().is_array() )
		{
			static_cast<Document_impl*>( m_impl )->setData( nlohmann::json::array() );
		}
		static_cast<Document_impl*>( m_impl )->data().push_back(
			static_cast<Document_impl*>( document.m_impl )->data() );
	}

	size_t Document::size() const
	{
		// For array documents, return the size
		if ( static_cast<Document_impl*>( m_impl )->data().is_array() )
		{
			return static_cast<Document_impl*>( m_impl )->data().size();
		}
		return 0;
	}

	//----------------------------------------------
	// Field operations
	//----------------------------------------------

	bool Document::removeField( std::string_view path )
	{
		// Handle root path removal
		if ( path.empty() )
		{
			return false; // Cannot remove root
		}

		// Find the last dot to separate parent path from field name
		size_t lastDot = path.find_last_of( '.' );
		if ( lastDot == std::string::npos )
		{
			// Direct field in root object
			nlohmann::json* root = &static_cast<Document_impl*>( m_impl )->data();
			if ( root->is_object() && root->contains( path ) )
			{
				root->erase( path );
				return true;
			}
			return false;
		}

		// Navigate to parent and remove field
		std::string parentPath{ path.substr( 0, lastDot ) };
		std::string fieldName{ path.substr( lastDot + 1 ) };

		nlohmann::json* parentNode = static_cast<Document_impl*>( m_impl )->navigateToPath( parentPath, false );
		if ( parentNode && parentNode->is_object() && parentNode->contains( fieldName ) )
		{
			parentNode->erase( fieldName );
			return true;
		}

		return false;
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

		nlohmann::json& thisData = static_cast<Document_impl*>( m_impl )->data();
		const nlohmann::json& otherData = static_cast<Document_impl*>( other.m_impl )->data();

		// Recursive merge function
		std::function<void( nlohmann::json&, const nlohmann::json& )> mergeRecursive =
			[&]( nlohmann::json& target, const nlohmann::json& source ) -> void {
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

		nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path, true );
		if ( node )
		{
			*node = static_cast<Document_impl*>( value.m_impl )->data();
		}
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	bool Document::isString( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_string();
	}

	bool Document::isInt( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_number_integer();
	}

	bool Document::isDouble( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_number_float();
	}

	bool Document::isBool( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_boolean();
	}

	bool Document::isNull( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_null();
	}

	bool Document::isObject( std::string_view path ) const
	{
		const nlohmann::json* node = static_cast<Document_impl*>( m_impl )->navigateToPath( path );
		return node && node->is_object();
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

			// Check if it's not discarded (nlohmann::json's invalid state)
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
} // namespace nfx::serialization::json
