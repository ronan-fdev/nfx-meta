/**
 * @file SchemaValidator_impl.cpp
 * @brief Implementation of SchemaValidator_impl class
 */

#include <algorithm>
#include <set>
#include <stdexcept>
#include <sstream>

#include "SchemaValidator_impl.h"

#include "nfx/serialization/json/Document.h"
#include "nfx/serialization/json/FieldEnumerator.h"
#include "nfx/serialization/json/SchemaValidator.h"
#include "Document_impl.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// SchemaValidator_impl class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	SchemaValidator_impl::SchemaValidator_impl()
		: m_schema{ nullptr },
		  m_schemaLoaded{ false },
		  m_strictMode{ false },
		  m_maxDepth{ 0 }
	{
	}

	SchemaValidator_impl::SchemaValidator_impl( const Document& schema )
		: m_schema{ std::make_unique<Document>( schema ) },
		  m_schemaLoaded{ true },
		  m_strictMode{ false },
		  m_maxDepth{ 0 }
	{
	}

	SchemaValidator_impl::SchemaValidator_impl( const SchemaValidator_impl& other )
		: m_schema{ other.m_schema ? std::make_unique<Document>( *other.m_schema ) : nullptr },
		  m_schemaLoaded{ other.m_schemaLoaded },
		  m_strictMode{ other.m_strictMode },
		  m_maxDepth{ other.m_maxDepth }
	{
	}

	SchemaValidator_impl::SchemaValidator_impl( SchemaValidator_impl&& other ) noexcept
		: m_schema{ std::move( other.m_schema ) },
		  m_schemaLoaded{ other.m_schemaLoaded },
		  m_strictMode{ other.m_strictMode },
		  m_maxDepth{ other.m_maxDepth }
	{
		other.m_schemaLoaded = false;
		other.m_strictMode = false;
		other.m_maxDepth = 0;
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	SchemaValidator_impl& SchemaValidator_impl::operator=( const SchemaValidator_impl& other )
	{
		if ( this != &other )
		{
			m_schema = other.m_schema ? std::make_unique<Document>( *other.m_schema ) : nullptr;
			m_schemaLoaded = other.m_schemaLoaded;
			m_strictMode = other.m_strictMode;
			m_maxDepth = other.m_maxDepth;
		}
		return *this;
	}

	SchemaValidator_impl& SchemaValidator_impl::operator=( SchemaValidator_impl&& other ) noexcept
	{
		if ( this != &other )
		{
			m_schema = std::move( other.m_schema );
			m_schemaLoaded = other.m_schemaLoaded;
			m_strictMode = other.m_strictMode;
			m_maxDepth = other.m_maxDepth;

			other.m_schemaLoaded = false;
			other.m_strictMode = false;
			other.m_maxDepth = 0;
		}
		return *this;
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	void SchemaValidator_impl::validateNode( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		// Handle $ref references
		if ( schema.hasValue( "$ref" ) )
		{
			auto refOpt = schema.get<std::string>( "$ref" );
			if ( refOpt.has_value() )
			{
				Document resolvedSchema = resolveReference( refOpt.value() );
				if ( resolvedSchema.isValid() )
				{
					validateNode( document, resolvedSchema, path, result );
					return;
				}
				else
				{
					result.addError( path, "Could not resolve reference: " + refOpt.value(), "$ref", refOpt.value(), "unresolved" );
					return;
				}
			}
		}

		// Validate type
		validateType( document, schema, path, result );

		// Validate based on type
		auto expectedType = schema.get<std::string>( "type" );

		if ( expectedType == "object" )
		{
			validateRequired( document, schema, path, result );
			validateProperties( document, schema, path, result );
		}
		else if ( expectedType == "array" )
		{
			validateArray( document, schema, path, result );
		}
		else if ( expectedType == "string" )
		{
			validateStringConstraints( document, schema, path, result );
		}
		else if ( expectedType == "number" || expectedType == "integer" )
		{
			validateNumericConstraints( document, schema, path, result );
		}
	}

	void SchemaValidator_impl::validateType( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !schema.hasValue( "type" ) )
		{
			return; // No type constraint
		}

		auto expectedType = schema.get<std::string>( "type" ).value_or( "" );
		std::string type = actualType( document, path );

		bool typeMatches = false;

		if ( expectedType == "object" && document.is<Document::Object>( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "array" && document.is<Document::Array>( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "string" && document.is<std::string>( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "number" && ( document.is<double>( path ) || document.is<int>( path ) ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "integer" && document.is<int>( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "boolean" && document.is<bool>( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "null" && document.isNull( path ) )
		{
			typeMatches = true;
		}

		if ( !typeMatches )
		{
			result.addError( path, "Type mismatch", "type", expectedType, type );
		}
	}

	void SchemaValidator_impl::validateRequired( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !schema.hasValue( "required" ) )
		{
			return; // No required fields
		}

		auto requiredArray = schema.get<Document::Array>( "required" );
		if ( !requiredArray.has_value() )
		{
			return;
		}

		size_t requiredCount = requiredArray.value().size();
		for ( size_t i = 0; i < requiredCount; ++i )
		{
			auto arrayElementOpt = requiredArray.value().get<std::string>( i );
			if ( arrayElementOpt.has_value() )
			{
				std::string requiredField = arrayElementOpt.value();
				std::string fieldPath = path.empty() ? requiredField : std::string{ path } + "." + requiredField;

				if ( !document.hasValue( fieldPath ) )
				{
					result.addError( fieldPath, "Required field missing", "required", requiredField, "undefined" );
				}
			}
		}
	}

	void SchemaValidator_impl::validateProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !schema.hasValue( "properties" ) )
		{
			return; // No properties defined
		}

		// Use FieldEnumerator to iterate over the properties schema definition
		FieldEnumerator propertiesEnum{ schema };
		if ( !propertiesEnum.setPath( "properties" ) )
		{
			return; // Cannot access properties
		}

		// Validate each property defined in the schema
		while ( !propertiesEnum.isEnd() )
		{
			std::string propertyName = propertiesEnum.currentKey();
			Document propertySchema = propertiesEnum.currentValue();

			// Build the path for this property in the document
			std::string propertyPath = path.empty() ? propertyName : std::string{ path } + "." + propertyName;

			// Check if the property exists in the document
			if ( document.hasValue( propertyPath ) )
			{
				// Validate the property against its schema
				validateNode( document, propertySchema, propertyPath, result );
			}
			// Note: Missing properties are handled by validateRequired()

			if ( !propertiesEnum.next() )
			{
				break; // End of enumeration
			}
		}

		// Check additionalProperties constraint
		if ( schema.hasValue( "additionalProperties" ) && schema.is<bool>( "additionalProperties" ) )
		{
			auto additionalProps = schema.get<bool>( "additionalProperties" );
			if ( additionalProps.has_value() && !additionalProps.value() )
			{
				// additionalProperties: false - strict validation
				// We need to check if document has properties not in schema
				validateAdditionalProperties( document, schema, path, result );
			}
		}
	}

	void SchemaValidator_impl::validateArray( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !document.is<Document::Array>( path ) )
		{
			return; // Not an array, type validation will catch this
		}

		auto documentArray = document.get<Document::Array>( path );
		if ( !documentArray.has_value() )
		{
			return;
		}
		size_t arraySize = documentArray.value().size();

		// Validate minItems
		if ( schema.hasValue( "minItems" ) && schema.is<int>( "minItems" ) )
		{
			auto minItemsOpt = schema.get<int64_t>( "minItems" );
			if ( minItemsOpt.has_value() )
			{
				int64_t minItems = minItemsOpt.value();
				if ( minItems > 0 && arraySize < static_cast<size_t>( minItems ) )
				{
					result.addError( path, "Array has too few items", "minItems",
						std::to_string( minItems ), std::to_string( arraySize ) );
				}
			}
		}

		// Validate maxItems
		if ( schema.hasValue( "maxItems" ) && schema.is<int>( "maxItems" ) )
		{
			auto maxItemsOpt = schema.get<int64_t>( "maxItems" );
			if ( maxItemsOpt.has_value() )
			{
				int64_t maxItems = maxItemsOpt.value();
				if ( maxItems > 0 && arraySize > static_cast<size_t>( maxItems ) )
				{
					result.addError( path, "Array has too many items", "maxItems",
						std::to_string( maxItems ), std::to_string( arraySize ) );
				}
			}
		}

		// Validate items
		if ( schema.hasValue( "items" ) )
		{
			// Extract the items schema definition
			Document itemsSchema = extractSubDocument( schema, "items" );

			// If items schema has a $ref, resolve it
			if ( itemsSchema.hasValue( "$ref" ) )
			{
				auto itemsRef = itemsSchema.get<std::string>( "$ref" );
				if ( itemsRef.has_value() )
				{
					itemsSchema = resolveReference( itemsRef.value() );
				}
			}

			for ( size_t i = 0; i < arraySize; ++i )
			{
				std::string itemPath = std::string{ path } + "[" + std::to_string( i ) + "]";

				validateNode( document, itemsSchema, itemPath, result );
			}
		}
	}

	void SchemaValidator_impl::validateNumericConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		// Get the numeric value
		std::optional<double> value;
		if ( document.is<int>( path ) )
		{
			auto intVal = document.get<int64_t>( path );
			if ( intVal.has_value() )
			{
				value = static_cast<double>( intVal.value() );
			}
		}
		else if ( document.is<double>( path ) )
		{
			value = document.get<double>( path );
		}

		if ( !value.has_value() )
		{
			return; // Not a number
		}

		// Validate minimum
		if ( schema.hasValue( "minimum" ) && ( schema.is<double>( "minimum" ) || schema.is<int>( "minimum" ) ) )
		{
			double minimum;
			if ( schema.is<double>( "minimum" ) )
			{
				auto minOpt = schema.get<double>( "minimum" );
				if ( !minOpt.has_value() )
					return;
				minimum = minOpt.value();
			}
			else
			{
				auto minOpt = schema.get<int64_t>( "minimum" );
				if ( !minOpt.has_value() )
					return;
				minimum = static_cast<double>( minOpt.value() );
			}
			if ( value.value() < minimum )
			{
				result.addError( path, "Value below minimum", "minimum",
					std::to_string( minimum ), std::to_string( value.value() ) );
			}
		}

		// Validate maximum
		if ( schema.hasValue( "maximum" ) && ( schema.is<double>( "maximum" ) || schema.is<int>( "maximum" ) ) )
		{
			double maximum;
			if ( schema.is<double>( "maximum" ) )
			{
				auto maxOpt = schema.get<double>( "maximum" );
				if ( !maxOpt.has_value() )
					return;
				maximum = maxOpt.value();
			}
			else
			{
				auto maxOpt = schema.get<int64_t>( "maximum" );
				if ( !maxOpt.has_value() )
					return;
				maximum = static_cast<double>( maxOpt.value() );
			}
			if ( value.value() > maximum )
			{
				result.addError( path, "Value above maximum", "maximum",
					std::to_string( maximum ), std::to_string( value.value() ) );
			}
		}
	}

	void SchemaValidator_impl::validateStringConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !document.is<std::string>( path ) )
		{
			return; // Not a string
		}

		auto valueOpt = document.get<std::string>( path );
		if ( !valueOpt.has_value() )
		{
			return; // No value
		}
		const std::string& value = valueOpt.value();

		// Validate minLength
		if ( schema.hasValue( "minLength" ) && schema.is<int>( "minLength" ) )
		{
			auto minLengthOpt = schema.get<int64_t>( "minLength" );
			if ( minLengthOpt.has_value() )
			{
				int64_t minLength = minLengthOpt.value();
				if ( minLength > 0 && value.length() < static_cast<size_t>( minLength ) )
				{
					result.addError( path, "String too short", "minLength",
						std::to_string( minLength ), std::to_string( value.length() ) );
				}
			}
		}

		// Validate maxLength
		if ( schema.hasValue( "maxLength" ) && schema.is<int>( "maxLength" ) )
		{
			auto maxLengthOpt = schema.get<int64_t>( "maxLength" );
			if ( maxLengthOpt.has_value() )
			{
				int64_t maxLength = maxLengthOpt.value();
				if ( maxLength > 0 && value.length() > static_cast<size_t>( maxLength ) )
				{
					result.addError( path, "String too long", "maxLength",
						std::to_string( maxLength ), std::to_string( value.length() ) );
				}
			}
		}

		// Validate format (basic implementation)
		if ( schema.hasValue( "format" ) )
		{
			auto format = schema.get<std::string>( "format" );
			if ( format == "date-time" )
			{
				// Basic ISO 8601 format check - very simplified
				if ( value.length() < 19 || value.find( 'T' ) == std::string::npos )
				{
					result.addError( path, "Invalid date-time format", "format", "ISO 8601 date-time", value );
				}
			}
		}
	}

	bool SchemaValidator_impl::referenceExists( std::string_view reference ) const noexcept
	{
		if ( reference.empty() || reference[0] != '#' )
		{
			return false; // TODO: Add support for external references (HTTP/file URLs)
		}

		// Parse reference like "#/definitions/Package"
		if ( reference.starts_with( "#/definitions/" ) )
		{
			std::string_view defName = reference.substr( 14 ); // Skip "#/definitions/"
			std::string defPath = "definitions." + std::string{ defName };
			return m_schema ? m_schema->hasValue( defPath ) : false;
		}

		// Handle other JSON Pointer references like "#/properties/someProperty"
		if ( reference.starts_with( "#/" ) )
		{
			std::string_view jsonPointerPath = reference.substr( 2 ); // Remove "#/"
			// Convert JSON Pointer path to dot notation
			std::string dotPath = convertJsonPointerToDotPath( jsonPointerPath );
			return m_schema ? m_schema->hasValue( dotPath ) : false;
		}

		return false; // Reference not found
	}

	Document SchemaValidator_impl::resolveReference( std::string_view reference ) const
	{
		if ( reference.empty() || reference[0] != '#' )
		{
			return Document(); // TODO: Add support for external references (HTTP/file URLs)
		}

		// Parse reference like "#/definitions/Package"
		if ( reference.starts_with( "#/definitions/" ) )
		{
			std::string_view defName = reference.substr( 14 ); // Skip "#/definitions/"
			std::string defPath = "definitions." + std::string{ defName };

			if ( m_schema && m_schema->hasValue( defPath ) )
			{
				// Extract the sub-document from the definitions section
				return extractSubDocument( *m_schema, defPath );
			}
		}

		// Handle other JSON Pointer references like "#/properties/someProperty"
		if ( reference.starts_with( "#/" ) )
		{
			std::string_view jsonPointerPath = reference.substr( 2 ); // Remove "#/"
			// Convert JSON Pointer path to dot notation
			std::string dotPath = convertJsonPointerToDotPath( jsonPointerPath );

			if ( m_schema && m_schema->hasValue( dotPath ) )
			{
				return extractSubDocument( *m_schema, dotPath );
			}
		}

		return Document{}; // Reference not found
	}

	std::string SchemaValidator_impl::actualType( const Document& document, std::string_view path ) const noexcept
	{
		if ( document.is<Document::Object>( path ) )
		{
			return "object";
		}
		if ( document.is<Document::Array>( path ) )
		{
			return "array";
		}
		if ( document.is<std::string>( path ) )
		{
			return "string";
		}
		if ( document.is<int>( path ) )
		{
			return "integer";
		}
		if ( document.is<double>( path ) )
		{
			return "number";
		}
		if ( document.is<bool>( path ) )
		{
			return "boolean";
		}
		if ( document.isNull( path ) )
		{
			return "null";
		}
		return "unknown";
	}

	void SchemaValidator_impl::validateAdditionalProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		// Get all properties defined in the schema
		std::set<std::string> schemaProperties;

		FieldEnumerator propertiesEnum( schema );
		if ( propertiesEnum.setPath( "properties" ) )
		{
			while ( !propertiesEnum.isEnd() )
			{
				schemaProperties.insert( propertiesEnum.currentKey() );
				if ( !propertiesEnum.next() )
				{
					break;
				}
			}
		}

		// Check all properties in document against schema
		FieldEnumerator docEnum( document );
		if ( docEnum.setPath( path ) )
		{
			while ( !docEnum.isEnd() )
			{
				std::string propertyName = docEnum.currentKey();

				// If property is not in schema and additionalProperties is false, it's an error
				if ( schemaProperties.find( propertyName ) == schemaProperties.end() )
				{
					std::string propertyPath = path.empty() ? propertyName : std::string{ path } + "." + propertyName;
					result.addError( propertyPath, "Additional property not allowed", "additionalProperties", "false", propertyName );
				}

				if ( !docEnum.next() )
				{
					break;
				}
			}
		}
	}

	Document SchemaValidator_impl::extractSubDocument( const Document& document, std::string_view path ) const
	{
		// TODO: Optimize by directly extracting JSON subtree instead of manual field copying

		// Check if the path exists
		if ( !document.hasValue( path ) )
		{
			return Document(); // Empty document if path not found
		}

		// Create a new document and try to extract the value at the path
		Document result;

		// Try different types to copy the value
		if ( document.is<Document::Object>( path ) )
		{
			// For objects, we need to copy the entire sub-object
			// We'll navigate using FieldEnumerator to copy all fields
			// TODO: Implement native JSON subtree extraction for better performance

			FieldEnumerator enumerator{ document };
			if ( enumerator.setPath( path ) )
			{
				// Copy all fields from the source object to result
				while ( !enumerator.isEnd() )
				{
					std::string key = enumerator.currentKey();
					Document value = enumerator.currentValue();

					// Copy the value based on its type
					if ( value.is<std::string>( "" ) )
					{
						auto str = value.get<std::string>( "" );
						if ( str.has_value() && !str.value().empty() )
						{
							result.set<std::string>( key, str.value() );
						}
					}
					else if ( value.is<int>( "" ) )
					{
						auto intVal = value.get<int64_t>( "" );
						if ( intVal.has_value() )
						{
							result.set<int64_t>( key, intVal.value() );
						}
					}
					else if ( value.is<double>( "" ) )
					{
						auto doubleVal = value.get<double>( "" );
						if ( doubleVal.has_value() )
						{
							result.set<double>( key, doubleVal.value() );
						}
					}
					else if ( value.is<bool>( "" ) )
					{
						auto boolVal = value.get<bool>( "" );
						if ( boolVal.has_value() )
						{
							result.set<bool>( key, boolVal.value() );
						}
					}
					else if ( value.is<Document::Object>( "" ) || value.is<Document::Array>( "" ) )
					{
						// For complex objects/arrays, copy as sub-document
						result.update( key, value );
					}

					if ( !enumerator.next() )
					{
						break;
					}
				}
			}
		}
		else if ( document.is<Document::Array>( path ) )
		{
			auto documentArray = document.get<Document::Array>( path );
			if ( documentArray.has_value() )
			{
				size_t arraySize = documentArray.value().size();
				auto resultArray = result.get<Document::Array>( "" );
				if ( resultArray.has_value() )
				{
					for ( size_t i = 0; i < arraySize; ++i )
					{
						auto arrayElementOpt = documentArray.value().get<Document>( i );
						if ( arrayElementOpt.has_value() )
						{
							resultArray.value().add<Document>( arrayElementOpt.value() );
						}
					}
				}
			}
		}
		else
		{
			// For primitive values, we can't really return a sub-document
			// This shouldn't happen in schema resolution context
			return Document{};
		}

		return result;
	}

	std::string SchemaValidator_impl::convertJsonPointerToDotPath( std::string_view jsonPointerPath ) const noexcept
	{
		std::string result{ jsonPointerPath };

		// Replace all '/' with '.'
		for ( size_t pos = 0; pos < result.length(); ++pos )
		{
			if ( result[pos] == '/' )
			{
				result[pos] = '.';
			}
		}

		return result;
	}

	//----------------------------------------------
	// Main validation interface
	//----------------------------------------------

	ValidationResult SchemaValidator_impl::validate( const Document& document ) const
	{
		if ( !m_schemaLoaded )
		{
			ValidationResult result;
			result.addError( "", "No schema loaded for validation", "schema", "", "" );
			return result;
		}

		ValidationResult result;
		validateNode( document, *m_schema, "", result );
		return result;
	}

	ValidationResult SchemaValidator_impl::validateAtPath( const Document& document, std::string_view documentPath, std::string_view schemaPath ) const
	{
		if ( !m_schemaLoaded )
		{
			ValidationResult result;
			result.addError( "", "No schema loaded for validation", "schema", "", "" );
			return result;
		}

		ValidationResult result;
		Document targetSchema;

		// Handle different schema path formats
		if ( schemaPath.empty() )
		{
			// Use root schema
			targetSchema = *m_schema;
		}
		else if ( schemaPath.starts_with( "#/" ) )
		{
			// JSON Pointer reference
			if ( !referenceExists( schemaPath ) )
			{
				result.addError( std::string{ documentPath },
					"Schema path not found: " + std::string{ schemaPath },
					"$ref",
					std::string{ schemaPath },
					"" );
				return result;
			}
			targetSchema = resolveReference( schemaPath );
		}
		else if ( schemaPath.starts_with( "#" ) )
		{
			// Anchor reference
			if ( !referenceExists( schemaPath ) )
			{
				result.addError( std::string{ documentPath },
					"Schema path not found: " + std::string{ schemaPath },
					"$anchor",
					std::string{ schemaPath },
					"" );
				return result;
			}
			targetSchema = resolveReference( schemaPath );
		}
		else
		{
			// Direct property path
			std::string schemaPathStr{ schemaPath };
			if ( !m_schema || !m_schema->hasValue( schemaPathStr ) )
			{
				result.addError( std::string{ documentPath },
					"Schema path not found: " + schemaPathStr,
					"path",
					schemaPathStr,
					"" );
				return result;
			}
			targetSchema = extractSubDocument( *m_schema, schemaPathStr );
		}

		// Check if document path exists (if not empty)
		if ( !documentPath.empty() )
		{
			std::string docPathStr{ documentPath };
			if ( !document.hasValue( docPathStr ) )
			{
				result.addError( std::string{ documentPath },
					"Document path not found: " + docPathStr,
					"path",
					docPathStr,
					"" );
				return result;
			}
		}

		validateNode( document, targetSchema, documentPath, result );
		return result;
	}

	//----------------------------------------------
	// Accessor methods
	//----------------------------------------------

	void SchemaValidator_impl::setSchema( const Document& schema )
	{
		m_schema = std::make_unique<Document>( schema );
	}
} // namespace nfx::serialization::json
