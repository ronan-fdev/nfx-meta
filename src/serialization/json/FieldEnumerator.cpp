/**
 * @file FieldEnumerator.cpp
 * @brief Implementation of FieldEnumerator class
 * @details Stateful enumerator for JSON object fields using Pimpl pattern.
 */

#include <stdexcept>

#include "nfx/serialization/json/FieldEnumerator.h"

#include "nfx/serialization/json/Document.h"
#include "FieldEnumerator_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// FieldEnumerator class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	FieldEnumerator::FieldEnumerator( const Document& document )
		: m_impl{ new FieldEnumerator_impl{ document } }
	{
	}

	FieldEnumerator::FieldEnumerator( const FieldEnumerator& other )
		: m_impl{ new FieldEnumerator_impl{ *static_cast<FieldEnumerator_impl*>( other.m_impl ) } }
	{
	}

	FieldEnumerator::FieldEnumerator( FieldEnumerator&& other ) noexcept
		: m_impl{ other.m_impl }
	{
		other.m_impl = nullptr;
	}

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	FieldEnumerator::~FieldEnumerator()
	{
		delete static_cast<FieldEnumerator_impl*>( m_impl );
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	FieldEnumerator& FieldEnumerator::operator=( const FieldEnumerator& other )
	{
		if ( this != &other )
		{
			delete static_cast<FieldEnumerator_impl*>( m_impl );
			m_impl = new FieldEnumerator_impl{ *static_cast<FieldEnumerator_impl*>( other.m_impl ) };
		}
		return *this;
	}

	FieldEnumerator& FieldEnumerator::operator=( FieldEnumerator&& other ) noexcept
	{
		if ( this != &other )
		{
			delete static_cast<FieldEnumerator_impl*>( m_impl );
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}
		return *this;
	}

	//----------------------------------------------
	// Navigation configuration
	//----------------------------------------------

	bool FieldEnumerator::setPath( std::string_view path )
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->navigateToPath( path );
	}

	bool FieldEnumerator::setPointer( std::string_view pointer )
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->navigateToPointer( pointer );
	}

	void FieldEnumerator::reset()
	{
		static_cast<FieldEnumerator_impl*>( m_impl )->reset();
	}

	//----------------------------------------------
	// Field information
	//----------------------------------------------

	bool FieldEnumerator::isValid() const
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->isValidObject();
	}

	size_t FieldEnumerator::size() const
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->objectSize();
	}

	bool FieldEnumerator::isEnd() const
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->isAtEnd();
	}

	size_t FieldEnumerator::index() const
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->currentIndex();
	}

	//----------------------------------------------
	// Field access interface
	//----------------------------------------------

	std::string FieldEnumerator::currentKey() const
	{
		return static_cast<FieldEnumerator_impl*>( m_impl )->currentKey();
	}

	Document FieldEnumerator::currentValue() const
	{
		FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );

		auto docPtr = impl->currentValueAsDocument();
		if ( !docPtr )
		{
			throw std::runtime_error( "FieldEnumerator: Failed to create Document for current field value" );
		}

		return *docPtr;
	}

	std::optional<std::string> FieldEnumerator::currentString() const
	{
		try
		{
			FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
			const nlohmann::json& value = impl->currentValue();

			if ( value.is_string() )
			{
				return value.get<std::string>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<int64_t> FieldEnumerator::currentInt() const
	{
		try
		{
			FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
			const nlohmann::json& value = impl->currentValue();

			if ( value.is_number_integer() )
			{
				return value.get<int64_t>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<double> FieldEnumerator::currentDouble() const
	{
		try
		{
			FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
			const nlohmann::json& value = impl->currentValue();

			if ( value.is_number_float() )
			{
				return value.get<double>();
			}
		}
		catch ( const std::exception& )
		{
			// Fall through to return nullopt
		}

		return std::nullopt;
	}

	std::optional<bool> FieldEnumerator::currentBool() const
	{
		try
		{
			FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
			const nlohmann::json& value = impl->currentValue();

			if ( value.is_boolean() )
			{
				return value.get<bool>();
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

	bool FieldEnumerator::next()
	{
		FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
		return impl->advance();
	}

	bool FieldEnumerator::previous()
	{
		FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
		return impl->movePrevious();
	}

	bool FieldEnumerator::moveTo( size_t index )
	{
		FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
		return impl->moveToIndex( index );
	}

	bool FieldEnumerator::moveToKey( std::string_view key )
	{
		FieldEnumerator_impl* impl = static_cast<FieldEnumerator_impl*>( m_impl );
		return impl->moveToFieldKey( key );
	}
} // namespace nfx::serialization::json
