/**
 * @file ArrayEnumerator.cpp
 * @brief Implementation of ArrayEnumerator class
 * @details Stateful enumerator for JSON array elements using Pimpl pattern.
 */

#include <stdexcept>

#include "nfx/serialization/json/ArrayEnumerator.h"

#include "nfx/serialization/json/Document.h"
#include "ArrayEnumerator_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// ArrayEnumerator class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ArrayEnumerator::ArrayEnumerator( const Document& document )
		: m_impl{ new ArrayEnumerator_impl( document ) }
	{
	}

	ArrayEnumerator::ArrayEnumerator( const ArrayEnumerator& other )
		: m_impl{ new ArrayEnumerator_impl{ *static_cast<ArrayEnumerator_impl*>( other.m_impl ) } }
	{
	}

	ArrayEnumerator::ArrayEnumerator( ArrayEnumerator&& other ) noexcept
		: m_impl{ other.m_impl }
	{
		other.m_impl = nullptr;
	}

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	ArrayEnumerator::~ArrayEnumerator()
	{
		delete static_cast<ArrayEnumerator_impl*>( m_impl );
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	ArrayEnumerator& ArrayEnumerator::operator=( const ArrayEnumerator& other )
	{
		if ( this != &other )
		{
			delete static_cast<ArrayEnumerator_impl*>( m_impl );
			m_impl = new ArrayEnumerator_impl{ *static_cast<ArrayEnumerator_impl*>( other.m_impl ) };
		}
		return *this;
	}

	ArrayEnumerator& ArrayEnumerator::operator=( ArrayEnumerator&& other ) noexcept
	{
		if ( this != &other )
		{
			delete static_cast<ArrayEnumerator_impl*>( m_impl );
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}
		return *this;
	}

	//----------------------------------------------
	// Navigation configuration
	//----------------------------------------------

	bool ArrayEnumerator::setPath( std::string_view path )
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->navigateToPath( path );
	}

	bool ArrayEnumerator::setPointer( std::string_view pointer )
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->navigateToPointer( pointer );
	}

	void ArrayEnumerator::reset()
	{
		static_cast<ArrayEnumerator_impl*>( m_impl )->reset();
	}

	//----------------------------------------------
	// Array information
	//----------------------------------------------

	bool ArrayEnumerator::isValid() const
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->isValidArray();
	}

	size_t ArrayEnumerator::size() const
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->getArraySize();
	}

	bool ArrayEnumerator::isEnd() const
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->isAtEnd();
	}

	size_t ArrayEnumerator::index() const
	{
		return static_cast<ArrayEnumerator_impl*>( m_impl )->currentIndex();
	}

	//----------------------------------------------
	// Element access interface
	//----------------------------------------------

	Document ArrayEnumerator::currentElement() const
	{
		ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );

		auto docPtr = impl->currentElementAsDocument();
		if ( !docPtr )
		{
			throw std::runtime_error( "ArrayEnumerator: Failed to create Document for current element" );
		}

		return *docPtr;
	}

	std::optional<std::string> ArrayEnumerator::currentString() const
	{
		try
		{
			ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
			const nlohmann::json& element = impl->currentElement();

			if ( element.is_string() )
			{
				return element.get<std::string>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<int64_t> ArrayEnumerator::currentInt() const
	{
		try
		{
			ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
			const nlohmann::json& element = impl->currentElement();

			if ( element.is_number_integer() )
			{
				return element.get<int64_t>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<double> ArrayEnumerator::currentDouble() const
	{
		try
		{
			ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
			const nlohmann::json& element = impl->currentElement();

			if ( element.is_number_float() )
			{
				return element.get<double>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<bool> ArrayEnumerator::currentBool() const
	{
		try
		{
			ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
			const nlohmann::json& element = impl->currentElement();

			if ( element.is_boolean() )
			{
				return element.get<bool>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Enumerator navigation interface
	//----------------------------------------------

	bool ArrayEnumerator::next()
	{
		ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
		return impl->advance();
	}

	bool ArrayEnumerator::previous()
	{
		ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
		return impl->movePrevious();
	}

	bool ArrayEnumerator::moveTo( size_t index )
	{
		ArrayEnumerator_impl* impl = static_cast<ArrayEnumerator_impl*>( m_impl );
		return impl->moveToIndex( index );
	}
} // namespace nfx::serialization::json
