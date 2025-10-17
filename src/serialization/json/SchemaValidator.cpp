/**
 * @file SchemaValidator.cpp
 * @brief Implementation of SchemaValidator class for JSON Schema validation
 * @details Provides concrete implementation for JSON Schema validation with detailed error reporting.
 */

#include <set>
#include <stdexcept>
#include <sstream>

#include "nfx/serialization/json/SchemaValidator.h"
#include "SchemaValidator_impl.h"

#include "nfx/serialization/json/Document.h"
#include "nfx/serialization/json/FieldEnumerator.h"
#include "nfx/string/Utils.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// ValidationError class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ValidationError::ValidationError( const ErrorEntry& entry )
		: m_error{ entry }
	{
	}

	ValidationError::ValidationError( std::string path, std::string message, std::string constraint, std::string expectedValue, std::string actualValue )
		: m_error{ std::move( path ),
			  std::move( message ),
			  std::move( constraint ),
			  std::move( expectedValue ),
			  std::move( actualValue ) }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	std::string ValidationError::toString() const
	{
		std::ostringstream oss;
		oss << "Validation error at path '" << m_error.path << "': " << m_error.message;

		if ( !m_error.constraint.empty() )
		{
			oss << " (constraint: " << m_error.constraint;
			if ( !m_error.expectedValue.empty() )
			{
				oss << ", expected: " << m_error.expectedValue;
			}
			if ( !m_error.actualValue.empty() )
			{
				oss << ", actual: " << m_error.actualValue;
			}
			oss << ")";
		}

		return oss.str();
	}

	//=====================================================================
	// ValidationResult class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	ValidationResult::ValidationResult( std::vector<ValidationError> errors )
		: m_errors{ std::move( errors ) }
	{
	}

	//----------------------------------------------
	// Error access
	//----------------------------------------------

	const ValidationError& ValidationResult::error( size_t index ) const
	{
		if ( index >= m_errors.size() )
		{
			throw std::out_of_range( "ValidationResult error index out of range" );
		}
		return m_errors[index];
	}

	std::string ValidationResult::errorSummary() const
	{
		if ( m_errors.empty() )
		{
			return "No validation errors";
		}

		std::ostringstream oss;
		oss << "Validation failed with " << m_errors.size() << " error(s):\n";

		for ( size_t i = 0; i < m_errors.size(); ++i )
		{
			oss << "  " << ( i + 1 ) << ". " << m_errors[i].toString() << "\n";
		}

		return oss.str();
	}

	//----------------------------------------------
	// Error manipulation
	//----------------------------------------------

	void ValidationResult::addError( const ValidationError::ErrorEntry& entry )
	{
		m_errors.emplace_back( entry );
	}

	void ValidationResult::addError( const ValidationError& error )
	{
		m_errors.push_back( error );
	}

	void ValidationResult::addError( std::string_view path, std::string_view message, std::string_view constraint, std::string_view expectedValue, std::string_view actualValue )
	{
		m_errors.emplace_back( std::string{ path }, std::string{ message }, std::string{ constraint }, std::string{ expectedValue }, std::string{ actualValue } );
	}

	//=====================================================================
	// SchemaValidator class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	SchemaValidator::SchemaValidator()
		: m_impl{ new SchemaValidator_impl() }
	{
	}

	SchemaValidator::SchemaValidator( const Document& schema )
		: m_impl{ new SchemaValidator_impl{ schema } }
	{
	}

	SchemaValidator::SchemaValidator( const SchemaValidator& other )
		: m_impl{ new SchemaValidator_impl{ *static_cast<SchemaValidator_impl*>( other.m_impl ) } }
	{
	}

	SchemaValidator::SchemaValidator( SchemaValidator&& other ) noexcept
		: m_impl{ other.m_impl }
	{
		other.m_impl = nullptr;
	}

	//----------------------------------------------
	// Destruction
	//----------------------------------------------

	SchemaValidator::~SchemaValidator()
	{
		if ( m_impl )
		{
			delete static_cast<SchemaValidator_impl*>( m_impl );
		}
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	SchemaValidator& SchemaValidator::operator=( const SchemaValidator& other )
	{
		if ( this != &other )
		{
			delete static_cast<SchemaValidator_impl*>( m_impl );
			m_impl = new SchemaValidator_impl{ *static_cast<SchemaValidator_impl*>( other.m_impl ) };
		}
		return *this;
	}

	SchemaValidator& SchemaValidator::operator=( SchemaValidator&& other ) noexcept
	{
		if ( this != &other )
		{
			delete static_cast<SchemaValidator_impl*>( m_impl );
			m_impl = other.m_impl;
			other.m_impl = nullptr;
		}
		return *this;
	}

	//----------------------------------------------
	// Schema management
	//----------------------------------------------

	bool SchemaValidator::loadSchema( const Document& schema )
	{
		if ( !schema.isValid() )
		{
			return false;
		}

		if ( !schema.is<Document::Object>( "" ) )
		{
			return false;
		}

		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		impl->setSchema( schema );
		impl->setSchemaLoaded( true );
		return true;
	}

	bool SchemaValidator::loadSchemaFromString( std::string_view schemaJson )
	{
		auto maybeSchema = Document::fromJsonString( schemaJson );
		if ( !maybeSchema.has_value() )
		{
			return false;
		}

		return loadSchema( maybeSchema.value() );
	}

	bool SchemaValidator::hasSchema() const
	{
		if ( !m_impl )
		{
			return false;
		}
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		return impl->isSchemaLoaded();
	}

	void SchemaValidator::clearSchema()
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		impl->setSchema( Document() );
		impl->setSchemaLoaded( false );
	}

	Document SchemaValidator::schema() const
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		return impl->getSchema();
	}

	//----------------------------------------------
	// Validation operations
	//----------------------------------------------

	ValidationResult SchemaValidator::validate( const Document& document ) const
	{
		if ( !m_impl )
		{
			throw std::runtime_error( "No schema loaded for validation" );
		}

		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		if ( !impl->isSchemaLoaded() )
		{
			throw std::runtime_error( "No schema loaded for validation" );
		}
		return impl->validate( document );
	}

	ValidationResult SchemaValidator::validateAtPath( const Document& document, std::string_view documentPath, std::string_view schemaPath ) const
	{
		if ( !m_impl )
		{
			throw std::runtime_error( "No schema loaded for validation" );
		}
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		if ( !impl->isSchemaLoaded() )
		{
			throw std::runtime_error( "No schema loaded for validation" );
		}
		return impl->validateAtPath( document, documentPath, schemaPath );
	}

	bool SchemaValidator::isValid( const Document& document ) const
	{
		try
		{
			ValidationResult result = validate( document );
			return result.isValid();
		}
		catch ( ... )
		{
			return false;
		}
	}

	//----------------------------------------------
	// Schema information
	//----------------------------------------------

	std::string SchemaValidator::schemaVersion() const
	{
		if ( !hasSchema() )
		{
			return {};
		}

		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		auto result = impl->getSchema().get<std::string>( "$schema" );
		return result.value_or( "" );
	}

	std::string SchemaValidator::schemaTitle() const
	{
		if ( !hasSchema() )
		{
			return {};
		}

		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		auto result = impl->getSchema().get<std::string>( "title" );
		return result.value_or( "" );
	}

	std::string SchemaValidator::schemaDescription() const
	{
		if ( !hasSchema() )
		{
			return {};
		}

		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		auto result = impl->getSchema().get<std::string>( "description" );
		return result.value_or( "" );
	}

	//----------------------------------------------
	// Validation configuration
	//----------------------------------------------

	void SchemaValidator::setStrictMode( bool strict )
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		impl->setStrictMode( strict );
	}

	bool SchemaValidator::isStrictMode() const
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		return impl->isStrictMode();
	}

	void SchemaValidator::setMaxDepth( size_t maxDepth )
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		impl->setMaxDepth( maxDepth );
	}

	size_t SchemaValidator::maxDepth() const
	{
		auto* impl = static_cast<SchemaValidator_impl*>( m_impl );
		return impl->getMaxDepth();
	}
} // namespace nfx::serialization::json
