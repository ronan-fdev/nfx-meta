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
		if ( schema.hasField( "$ref" ) )
		{
			std::string ref = schema.getString( "$ref" ).value_or( "" );
			Document resolvedSchema = resolveReference( ref );
			if ( resolvedSchema.isValid() )
			{
				validateNode( document, resolvedSchema, path, result );
				return;
			}
			else
			{
				result.addError( path, "Could not resolve reference: " + ref, "$ref", ref, "unresolved" );
				return;
			}
		}

		// Validate type
		validateType( document, schema, path, result );

		// Validate based on type
		std::string expectedType = schema.getString( "type" ).value_or( "" );

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
		if ( !schema.hasField( "type" ) )
		{
			return; // No type constraint
		}

		std::string expectedType = schema.getString( "type" ).value_or( "" );
		std::string type = actualType( document, path );

		bool typeMatches = false;

		if ( expectedType == "object" && document.isObject( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "array" && document.isArray( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "string" && document.isString( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "number" && ( document.isDouble( path ) || document.isInt( path ) ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "integer" && document.isInt( path ) )
		{
			typeMatches = true;
		}
		else if ( expectedType == "boolean" && document.isBool( path ) )
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
		if ( !schema.hasField( "required" ) )
		{
			return; // No required fields
		}

		size_t requiredCount = schema.getArraySize( "required" );
		for ( size_t i = 0; i < requiredCount; ++i )
		{
			auto requiredField = schema.getArrayElementString( "required", i );
			if ( requiredField.has_value() )
			{
				std::string fieldPath = path.empty() ? requiredField.value() : std::string{ path } + "." + requiredField.value();

				if ( !document.hasField( fieldPath ) )
				{
					result.addError( fieldPath, "Required field missing", "required", requiredField.value(), "undefined" );
				}
			}
		}
	}

	void SchemaValidator_impl::validateProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		if ( !schema.hasField( "properties" ) )
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
			if ( document.hasField( propertyPath ) )
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
		if ( schema.hasField( "additionalProperties" ) )
		{
			auto additionalProps = schema.getBool( "additionalProperties" );
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
		if ( !document.isArray( path ) )
		{
			return; // Not an array, type validation will catch this
		}

		size_t arraySize = document.getArraySize( path );

		// Validate minItems
		if ( schema.hasField( "minItems" ) )
		{
			auto minItems = schema.getInt( "minItems" );
			if ( minItems.has_value() && arraySize < static_cast<size_t>( minItems.value() ) )
			{
				result.addError( path, "Array has too few items", "minItems",
					std::to_string( minItems.value() ), std::to_string( arraySize ) );
			}
		}

		// Validate maxItems
		if ( schema.hasField( "maxItems" ) )
		{
			auto maxItems = schema.getInt( "maxItems" );
			if ( maxItems.has_value() && arraySize > static_cast<size_t>( maxItems.value() ) )
			{
				result.addError( path, "Array has too many items", "maxItems",
					std::to_string( maxItems.value() ), std::to_string( arraySize ) );
			}
		}

		// Validate items
		if ( schema.hasField( "items" ) )
		{
			// Extract the items schema definition
			Document itemsSchema = extractSubDocument( schema, "items" );

			// If items schema has a $ref, resolve it
			if ( itemsSchema.hasField( "$ref" ) )
			{
				std::string itemsRef = itemsSchema.getString( "$ref" ).value_or( "" );
				itemsSchema = resolveReference( itemsRef );
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
		if ( document.isInt( path ) )
		{
			auto intVal = document.getInt( path );
			if ( intVal.has_value() )
			{
				value = static_cast<double>( intVal.value() );
			}
		}
		else if ( document.isDouble( path ) )
		{
			value = document.getDouble( path );
		}

		if ( !value.has_value() )
		{
			return; // Not a number
		}

		// Validate minimum
		if ( schema.hasField( "minimum" ) )
		{
			auto minimum = schema.getDouble( "minimum" );
			if ( minimum.has_value() && value.value() < minimum.value() )
			{
				result.addError( path, "Value below minimum", "minimum",
					std::to_string( minimum.value() ), std::to_string( value.value() ) );
			}
		}

		// Validate maximum
		if ( schema.hasField( "maximum" ) )
		{
			auto maximum = schema.getDouble( "maximum" );
			if ( maximum.has_value() && value.value() > maximum.value() )
			{
				result.addError( path, "Value above maximum", "maximum",
					std::to_string( maximum.value() ), std::to_string( value.value() ) );
			}
		}
	}

	void SchemaValidator_impl::validateStringConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const
	{
		auto stringValue = document.getString( path );
		if ( !stringValue.has_value() )
		{
			return; // Not a string
		}

		const std::string& value = stringValue.value();

		// Validate minLength
		if ( schema.hasField( "minLength" ) )
		{
			auto minLength = schema.getInt( "minLength" );
			if ( minLength.has_value() && value.length() < static_cast<size_t>( minLength.value() ) )
			{
				result.addError( path, "String too short", "minLength",
					std::to_string( minLength.value() ), std::to_string( value.length() ) );
			}
		}

		// Validate maxLength
		if ( schema.hasField( "maxLength" ) )
		{
			auto maxLength = schema.getInt( "maxLength" );
			if ( maxLength.has_value() && value.length() > static_cast<size_t>( maxLength.value() ) )
			{
				result.addError( path, "String too long", "maxLength",
					std::to_string( maxLength.value() ), std::to_string( value.length() ) );
			}
		}

		// Validate format (basic implementation)
		if ( schema.hasField( "format" ) )
		{
			std::string format = schema.getString( "format" ).value_or( "" );
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
			return m_schema ? m_schema->hasField( defPath ) : false;
		}

		// Handle other JSON Pointer references like "#/properties/someProperty"
		if ( reference.starts_with( "#/" ) )
		{
			std::string_view jsonPointerPath = reference.substr( 2 ); // Remove "#/"
			// Convert JSON Pointer path to dot notation
			std::string dotPath = convertJsonPointerToDotPath( jsonPointerPath );
			return m_schema ? m_schema->hasField( dotPath ) : false;
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

			if ( m_schema && m_schema->hasField( defPath ) )
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

			if ( m_schema && m_schema->hasField( dotPath ) )
			{
				return extractSubDocument( *m_schema, dotPath );
			}
		}

		return Document{}; // Reference not found
	}

	std::string SchemaValidator_impl::actualType( const Document& document, std::string_view path ) const noexcept
	{
		if ( document.isObject( path ) )
		{
			return "object";
		}
		if ( document.isArray( path ) )
		{
			return "array";
		}
		if ( document.isString( path ) )
		{
			return "string";
		}
		if ( document.isInt( path ) )
		{
			return "integer";
		}
		if ( document.isDouble( path ) )
		{
			return "number";
		}
		if ( document.isBool( path ) )
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
		if ( !document.hasField( path ) )
		{
			return Document(); // Empty document if path not found
		}

		// Create a new document and try to extract the value at the path
		Document result = Document::createObject();

		// Try different types to copy the value
		if ( document.isObject( path ) )
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
					if ( value.isString( "" ) )
					{
						auto str = value.getString( "" );
						if ( str.has_value() )
						{
							result.setString( key, str.value() );
						}
					}
					else if ( value.isInt( "" ) )
					{
						auto intVal = value.getInt( "" );
						if ( intVal.has_value() )
						{
							result.setInt( key, intVal.value() );
						}
					}
					else if ( value.isDouble( "" ) )
					{
						auto doubleVal = value.getDouble( "" );
						if ( doubleVal.has_value() )
						{
							result.setDouble( key, doubleVal.value() );
						}
					}
					else if ( value.isBool( "" ) )
					{
						auto boolVal = value.getBool( "" );
						if ( boolVal.has_value() )
						{
							result.setBool( key, boolVal.value() );
						}
					}
					else if ( value.isObject( "" ) || value.isArray( "" ) )
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
		else if ( document.isArray( path ) )
		{
			// TODO: Consider if array document creation is the most efficient approach
			result = Document::createArray();
			size_t arraySize = document.getArraySize( path );

			for ( size_t i = 0; i < arraySize; ++i )
			{
				Document arrayElement = document.getArrayElement( path, i );
				result.addDocument( arrayElement );
			}
		}
		else
		{
			// For primitive values, we can't really return a sub-document
			// This shouldn't happen in schema resolution context
			return Document();
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
			if ( !m_schema || !m_schema->hasField( schemaPathStr ) )
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
			if ( !document.hasField( docPathStr ) )
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
