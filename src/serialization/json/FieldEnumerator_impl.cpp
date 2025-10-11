/**
 * @file FieldEnumerator_impl.cpp
 * @brief Implementation of FieldEnumerator_impl class
 * @details Provides JSON object field iteration functionality using nlohmann::ordered_json backend.
 */

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "FieldEnumerator_impl.h"

#include "nfx/serialization/json/Document.h"
#include "Document_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// FieldEnumerator_impl class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	FieldEnumerator_impl::FieldEnumerator_impl( const Document& document )
		: m_document{ document },
		  m_currentPath{},
		  m_currentObject{ nullptr },
		  m_fieldKeys{},
		  m_currentIndex{ 0 },
		  m_currentValueDoc{ nullptr }
	{
	}

	FieldEnumerator_impl::FieldEnumerator_impl( const FieldEnumerator_impl& other )
		: m_document{ other.m_document },
		  m_currentPath{ other.m_currentPath },
		  m_currentObject{ other.m_currentObject },
		  m_fieldKeys{ other.m_fieldKeys },
		  m_currentIndex{ other.m_currentIndex },
		  m_currentValueDoc{ nullptr }
	{
	}

	FieldEnumerator_impl::FieldEnumerator_impl( FieldEnumerator_impl&& other ) noexcept
		: m_document{ other.m_document },
		  m_currentPath{ std::move( other.m_currentPath ) },
		  m_currentObject{ other.m_currentObject },
		  m_fieldKeys{ std::move( other.m_fieldKeys ) },
		  m_currentIndex{ other.m_currentIndex },
		  m_currentValueDoc{ std::move( other.m_currentValueDoc ) }
	{
		other.m_currentObject = nullptr;
		other.m_currentIndex = 0;
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	FieldEnumerator_impl& FieldEnumerator_impl::operator=( const FieldEnumerator_impl& other )
	{
		if ( this != &other )
		{
			m_currentPath = other.m_currentPath;
			m_currentObject = other.m_currentObject;
			m_fieldKeys = other.m_fieldKeys;
			m_currentIndex = other.m_currentIndex;
			m_currentValueDoc.reset();
		}
		return *this;
	}

	FieldEnumerator_impl& FieldEnumerator_impl::operator=( FieldEnumerator_impl&& other ) noexcept
	{
		if ( this != &other )
		{
			m_currentPath = std::move( other.m_currentPath );
			m_currentObject = other.m_currentObject;
			m_fieldKeys = std::move( other.m_fieldKeys );
			m_currentIndex = other.m_currentIndex;
			m_currentValueDoc = std::move( other.m_currentValueDoc );

			other.m_currentObject = nullptr;
			other.m_currentIndex = 0;
		}
		return *this;
	}

	//----------------------------------------------
	// Navigation methods
	//----------------------------------------------

	bool FieldEnumerator_impl::navigateToPath( std::string_view path )
	{
		try
		{
			// Clear previous state
			m_currentObject = nullptr;
			m_currentIndex = 0;
			m_fieldKeys.clear();
			m_currentValueDoc.reset();
			m_currentPath = std::string( path );

			// Get Document implementation
			auto docImpl = static_cast<Document_impl*>( m_document.m_impl );
			if ( !docImpl )
			{
				return false;
			}

			// Navigate to the specified path
			auto targetNode = docImpl->navigateToPath( path );
			if ( !targetNode || !targetNode->is_object() )
			{
				return false;
			}

			m_currentObject = targetNode;
			buildFieldList();
			return true;
		}
		catch ( const std::exception& )
		{
			m_currentObject = nullptr;
			return false;
		}
	}

	bool FieldEnumerator_impl::navigateToPointer( std::string_view pointer )
	{
		try
		{
			// Clear previous state
			m_currentObject = nullptr;
			m_currentIndex = 0;
			m_fieldKeys.clear();
			m_currentValueDoc.reset();
			m_currentPath = std::string( pointer );

			// Get document implementation
			Document_impl* docImpl = static_cast<Document_impl*>( m_document.m_impl );
			if ( !docImpl )
			{
				return false;
			}

			// Navigate using JSON Pointer
			auto targetNode = docImpl->navigateToJsonPointer( pointer );
			if ( !targetNode || !targetNode->is_object() )
			{
				return false;
			}

			m_currentObject = targetNode;
			buildFieldList();
			return true;
		}
		catch ( const std::exception& )
		{
			m_currentObject = nullptr;
			return false;
		}
	}

	void FieldEnumerator_impl::reset() noexcept
	{
		m_currentIndex = 0;
		m_currentValueDoc.reset();
	}

	//----------------------------------------------
	// Object access methods
	//----------------------------------------------

	bool FieldEnumerator_impl::isValidObject() const noexcept
	{
		return m_currentObject != nullptr && m_currentObject->is_object();
	}

	size_t FieldEnumerator_impl::objectSize() const noexcept
	{
		if ( !isValidObject() )
		{
			return 0;
		}
		return m_fieldKeys.size();
	}

	bool FieldEnumerator_impl::isAtEnd() const noexcept
	{
		if ( !isValidObject() )
		{
			return true;
		}
		return m_currentIndex >= m_fieldKeys.size();
	}

	size_t FieldEnumerator_impl::currentIndex() const noexcept
	{
		return m_currentIndex;
	}

	bool FieldEnumerator_impl::advance() noexcept
	{
		if ( !isValidObject() || isAtEnd() )
		{
			return false;
		}

		++m_currentIndex;
		m_currentValueDoc.reset(); // Clear cached value
		return true;
	}

	std::string FieldEnumerator_impl::currentKey() const
	{
		if ( !isValidObject() )
		{
			throw std::runtime_error( "FieldEnumerator: Not positioned at valid object" );
		}

		if ( isAtEnd() )
		{
			throw std::runtime_error( "FieldEnumerator: At end position, cannot access field" );
		}

		return m_fieldKeys[m_currentIndex];
	}

	const nlohmann::ordered_json& FieldEnumerator_impl::currentValue() const
	{
		if ( !isValidObject() )
		{
			throw std::runtime_error( "FieldEnumerator: Not positioned at valid object" );
		}

		if ( isAtEnd() )
		{
			throw std::runtime_error( "FieldEnumerator: At end position, cannot access field" );
		}

		const std::string& key = m_fieldKeys[m_currentIndex];
		return ( *m_currentObject )[key];
	}

	std::unique_ptr<Document> FieldEnumerator_impl::currentValueAsDocument() const
	{
		// Get current field value JSON
		const nlohmann::ordered_json& value = currentValue();

		// Create new Document from this value using factory method
		auto docOpt = Document::fromJsonString( value.dump() );
		if ( !docOpt.has_value() )
		{
			throw std::runtime_error( "FieldEnumerator: Failed to create Document from JSON field value" );
		}

		return std::make_unique<Document>( std::move( docOpt.value() ) );
	}

	//----------------------------------------------
	// Movement methods
	//----------------------------------------------

	bool FieldEnumerator_impl::movePrevious() noexcept
	{
		if ( !isValidObject() || m_currentIndex == 0 )
		{
			return false;
		}

		--m_currentIndex;
		m_currentValueDoc.reset();
		return true;
	}

	bool FieldEnumerator_impl::moveToIndex( size_t index ) noexcept
	{
		if ( !isValidObject() || index >= objectSize() )
		{
			return false;
		}

		m_currentIndex = index;
		m_currentValueDoc.reset();
		return true;
	}

	bool FieldEnumerator_impl::moveToFieldKey( std::string_view key ) noexcept
	{
		if ( !isValidObject() )
		{
			return false;
		}

		// Find the key in our field list
		auto it = std::find( m_fieldKeys.begin(), m_fieldKeys.end(), key );
		if ( it == m_fieldKeys.end() )
		{
			return false;
		}

		m_currentIndex = std::distance( m_fieldKeys.begin(), it );
		m_currentValueDoc.reset(); // Clear cached value
		return true;
	}

	//----------------------------------------------
	// Helper methods
	//----------------------------------------------

	void FieldEnumerator_impl::buildFieldList()
	{
		m_fieldKeys.clear();
		if ( !isValidObject() )
		{
			return;
		}

		// Build sorted list of field keys for consistent iteration order
		for ( auto it = m_currentObject->begin(); it != m_currentObject->end(); ++it )
		{
			m_fieldKeys.push_back( it.key() );
		}

		// Sort keys for deterministic iteration order
		std::sort( m_fieldKeys.begin(), m_fieldKeys.end() );
	}
} // namespace nfx::serialization::json
