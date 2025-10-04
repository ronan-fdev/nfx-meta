/**
 * @file ArrayEnumerator_impl.cpp
 * @brief Implementation of ArrayEnumerator_impl class
 * @details Provides JSON array iteration functionality using nlohmann::json backend.
 */

#include <stdexcept>
#include <sstream>

#include "ArrayEnumerator_impl.h"

#include "nfx/serialization/json/Document.h"
#include "Document_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// ArrayEnumerator_impl class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ArrayEnumerator_impl::ArrayEnumerator_impl( const Document& document )
		: m_document{ document },
		  m_currentPath{},
		  m_currentArray{ nullptr },
		  m_currentIndex{ 0 },
		  m_currentElementDoc{ nullptr }
	{
	}

	ArrayEnumerator_impl::ArrayEnumerator_impl( const ArrayEnumerator_impl& other )
		: m_document{ other.m_document },
		  m_currentPath{ other.m_currentPath },
		  m_currentArray{ other.m_currentArray },
		  m_currentIndex{ other.m_currentIndex },
		  m_currentElementDoc{ nullptr }
	{
	}

	ArrayEnumerator_impl::ArrayEnumerator_impl( ArrayEnumerator_impl&& other ) noexcept
		: m_document{ other.m_document },
		  m_currentPath{ std::move( other.m_currentPath ) },
		  m_currentArray{ other.m_currentArray },
		  m_currentIndex{ other.m_currentIndex },
		  m_currentElementDoc{ std::move( other.m_currentElementDoc ) }
	{
		other.m_currentArray = nullptr;
		other.m_currentIndex = 0;
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	ArrayEnumerator_impl& ArrayEnumerator_impl::operator=( const ArrayEnumerator_impl& other )
	{
		if ( this != &other )
		{
			m_currentPath = other.m_currentPath;
			m_currentArray = other.m_currentArray;
			m_currentIndex = other.m_currentIndex;
			m_currentElementDoc.reset();
		}
		return *this;
	}

	ArrayEnumerator_impl& ArrayEnumerator_impl::operator=( ArrayEnumerator_impl&& other ) noexcept
	{
		if ( this != &other )
		{
			m_currentPath = std::move( other.m_currentPath );
			m_currentArray = other.m_currentArray;
			m_currentIndex = other.m_currentIndex;
			m_currentElementDoc = std::move( other.m_currentElementDoc );

			other.m_currentArray = nullptr;
			other.m_currentIndex = 0;
		}
		return *this;
	}

	//----------------------------------------------
	// Navigation methods
	//----------------------------------------------

	bool ArrayEnumerator_impl::navigateToPath( std::string_view path )
	{
		try
		{
			// Clear previous state
			m_currentArray = nullptr;
			m_currentIndex = 0;
			m_currentElementDoc.reset();
			m_currentPath = std::string( path );

			// Get document implementation
			Document_impl* docImpl = static_cast<Document_impl*>( m_document.m_impl );
			if ( !docImpl )
			{
				return false;
			}

			// Navigate to the specified path
			const nlohmann::json* targetNode = docImpl->navigateToPath( path );
			if ( !targetNode || !targetNode->is_array() )
			{
				return false;
			}

			m_currentArray = targetNode;
			return true;
		}
		catch ( const std::exception& )
		{
			m_currentArray = nullptr;
			return false;
		}
	}

	bool ArrayEnumerator_impl::navigateToPointer( std::string_view pointer )
	{
		try
		{
			// Clear previous state
			m_currentArray = nullptr;
			m_currentIndex = 0;
			m_currentElementDoc.reset();
			m_currentPath = std::string( pointer );

			// Get document implementation
			Document_impl* docImpl = static_cast<Document_impl*>( m_document.m_impl );
			if ( !docImpl )
			{
				return false;
			}

			// Navigate using JSON Pointer
			const nlohmann::json* targetNode = docImpl->navigateToJsonPointer( pointer );
			if ( !targetNode || !targetNode->is_array() )
			{
				return false;
			}

			m_currentArray = targetNode;
			return true;
		}
		catch ( const std::exception& )
		{
			m_currentArray = nullptr;
			return false;
		}
	}

	void ArrayEnumerator_impl::reset() noexcept
	{
		m_currentIndex = 0;
		m_currentElementDoc.reset();
	}

	//----------------------------------------------
	// Array access methods
	//----------------------------------------------

	bool ArrayEnumerator_impl::isValidArray() const noexcept
	{
		return m_currentArray != nullptr && m_currentArray->is_array();
	}

	size_t ArrayEnumerator_impl::getArraySize() const noexcept
	{
		if ( !isValidArray() )
		{
			return 0;
		}
		return m_currentArray->size();
	}

	bool ArrayEnumerator_impl::isAtEnd() const noexcept
	{
		if ( !isValidArray() )
		{
			return true;
		}
		return m_currentIndex >= m_currentArray->size();
	}

	size_t ArrayEnumerator_impl::currentIndex() const noexcept
	{
		return m_currentIndex;
	}

	bool ArrayEnumerator_impl::advance() noexcept
	{
		if ( !isValidArray() || isAtEnd() )
		{
			return false;
		}

		++m_currentIndex;
		m_currentElementDoc.reset(); // Clear cached element
		return true;
	}

	const nlohmann::json& ArrayEnumerator_impl::currentElement() const
	{
		if ( !isValidArray() )
		{
			throw std::runtime_error( "ArrayEnumerator: Not positioned at valid array" );
		}

		if ( isAtEnd() )
		{
			throw std::runtime_error( "ArrayEnumerator: At end position, cannot access element" );
		}

		return ( *m_currentArray )[m_currentIndex];
	}

	std::unique_ptr<Document> ArrayEnumerator_impl::currentElementAsDocument() const
	{
		// Get current element JSON
		const nlohmann::json& element = currentElement();

		// Create new Document from this element using factory method
		auto docOpt = Document::fromJsonString( element.dump() );
		if ( !docOpt.has_value() )
		{
			throw std::runtime_error( "ArrayEnumerator: Failed to create Document from JSON element" );
		}

		return std::make_unique<Document>( std::move( docOpt.value() ) );
	}

	//----------------------------------------------
	// Comparison methods
	//----------------------------------------------

	bool ArrayEnumerator_impl::movePrevious() noexcept
	{
		if ( !isValidArray() || m_currentIndex == 0 )
		{
			return false;
		}

		--m_currentIndex;
		m_currentElementDoc.reset(); // Clear cached element
		return true;
	}

	bool ArrayEnumerator_impl::moveToIndex( size_t index ) noexcept
	{
		if ( !isValidArray() || index >= getArraySize() )
		{
			return false;
		}

		m_currentIndex = index;
		m_currentElementDoc.reset(); // Clear cached element
		return true;
	}
} // namespace nfx::serialization::json
