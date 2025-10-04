/**
 * @file SerializationTraits.h
 * @brief Serialization traits and type specializations for JSON serialization
 * @details Contains SerializationTraits template specializations for nfx-core library types.
 *          This file provides the extensible serialization framework that allows users to
 *          customize serialization behavior for their own types.
 */

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "nfx/config.h"
#include "nfx/string/Splitter.h"

// nfx container support
#include "nfx/containers/ChdHashMap.h"

// nfx datatype support
#include "nfx/datatypes/Int128.h"
#include "nfx/datatypes/Decimal.h"

// nfx time support
#include "nfx/time/DateTime.h"

#include "nfx/serialization/json/Document.h"
#include "nfx/serialization/json/FieldEnumerator.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	template <typename T>
	class Serializer;

	namespace detail
	{
		template <typename T>
		struct has_serialize_method;

		template <typename T>
		struct has_serialize_method_returning_document;

		template <typename T>
		struct has_serialize_method_no_params;

		template <typename T>
		struct has_deserialize_method;
	} // namespace detail

	//=====================================================================
	// Serialization Traits (extensible by users)
	//=====================================================================

	/**
	 * @brief Default serialization traits - users can specialize this
	 * @tparam T The type to serialize/deserialize
	 * @details This is the extension point for users to define custom serialization.
	 *          Users can specialize this template for their types or even override
	 *          library types with custom serialization logic.
	 */
	template <typename T>
	struct SerializationTraits
	{
		/**
		 * @brief Default serialize implementation - delegates to member method
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		static void serialize( const T& obj, Document& doc )
		{
			// Look for serialize method with no parameters
			if constexpr ( detail::has_serialize_method_no_params<T>::value )
			{
				doc = obj.serialize();
			}
			// Look for serialize method returning Document with serializer parameter
			else if constexpr ( detail::has_serialize_method_returning_document<T>::value )
			{
				Serializer<T> serializer;
				doc = obj.serialize( serializer );
			}
			// Look for traditional serialize method with serializer and document parameters
			else if constexpr ( detail::has_serialize_method<T>::value )
			{
				Serializer<T> serializer;
				obj.serialize( serializer, doc );
			}
			else
			{
				static_assert( detail::has_serialize_method<T>::value ||
								   detail::has_serialize_method_returning_document<T>::value ||
								   detail::has_serialize_method_no_params<T>::value,
					"Type must either specialize SerializationTraits or have a serialize() member method" );
			}
		}

		/**
		 * @brief Default deserialize implementation - delegates to member method
		 * @param obj Object to deserialize into
		 * @param doc Document to deserialize from
		 */
		static void deserialize( T& obj, const Document& doc )
		{
			// Look for member deserialize method
			if constexpr ( detail::has_deserialize_method<T>::value )
			{
				Serializer<T> serializer;
				obj.deserialize( serializer, doc );
			}
			else
			{
				static_assert( detail::has_deserialize_method<T>::value,
					"Type must either specialize SerializationTraits or have a deserialize() member method" );
			}
		}
	};

	//=====================================================================
	// nfx-core library types SerializationTraits specializations
	//=====================================================================

	/**
	 * @brief Specialization for nfx::datatypes::Int128
	 */
	template <>
	struct SerializationTraits<nfx::datatypes::Int128>
	{
		/**
		 * @brief Serialize Int128 to JSON document using platform-independent string representation
		 * @param obj The Int128 object to serialize
		 * @param doc The document to serialize into
		 * @details Uses toString() method to ensure cross-platform compatibility.
		 *          String representation works consistently across GCC/Clang (native __int128)
		 *          and MSVC (manual implementation) platforms.
		 */
		static void serialize( const nfx::datatypes::Int128& obj, Document& doc )
		{
			std::string value = obj.toString();
			doc.setStringByPointer( "", value );
		}

		/**
		 * @brief Deserialize Int128 from JSON document using platform-independent string parsing
		 * @param obj The Int128 object to deserialize into
		 * @param doc The document to deserialize from
		 * @details Uses tryParse() method to ensure cross-platform compatibility.
		 *          Can deserialize values created on any platform (GCC/Clang/MSVC).
		 */
		static void deserialize( nfx::datatypes::Int128& obj, const Document& doc )
		{
			auto val = doc.getStringByPointer( "" );
			if ( val )
			{
				if ( !nfx::datatypes::Int128::tryParse( *val, obj ) )
				{
					throw std::runtime_error( "Invalid Int128 format: unable to parse string representation" );
				}
			}
		}
	};

	/**
	 * @brief Specialization for nfx::datatypes::Decimal
	 */
	template <>
	struct SerializationTraits<nfx::datatypes::Decimal>
	{
		/**
		 * @brief Serialize Decimal to JSON document using platform-independent string representation
		 * @param obj The Decimal object to serialize
		 * @param doc The document to serialize into
		 * @details Uses toString() method to ensure cross-platform compatibility.
		 *          String representation is platform-independent and preserves full precision.
		 */
		static void serialize( const nfx::datatypes::Decimal& obj, Document& doc )
		{
			std::string value = obj.toString();
			doc.setStringByPointer( "", value );
		}

		/**
		 * @brief Deserialize Decimal from JSON document using platform-independent string parsing
		 * @param obj The Decimal object to deserialize into
		 * @param doc The document to deserialize from
		 * @details Uses tryParse() method to ensure cross-platform compatibility.
		 *          Can deserialize values created on any platform (GCC/Clang/MSVC).
		 */
		static void deserialize( nfx::datatypes::Decimal& obj, const Document& doc )
		{
			auto val = doc.getStringByPointer( "" );
			if ( val )
			{
				if ( !nfx::datatypes::Decimal::tryParse( *val, obj ) )
				{
					throw std::runtime_error( "Invalid Decimal format: unable to parse string representation" );
				}
			}
		}
	};

	/**
	 * @brief Specialization for nfx::time::TimeSpan
	 */
	template <>
	struct SerializationTraits<nfx::time::TimeSpan>
	{
		/**
		 * @brief Serialize TimeSpan to JSON document using platform-optimized methods
		 * @param obj The TimeSpan object to serialize
		 * @param doc The document to serialize into
		 */
		static void serialize( const nfx::time::TimeSpan& obj, Document& doc )
		{
			doc.setIntByPointer( "", obj.ticks() );
		}

		/**
		 * @brief Deserialize TimeSpan from JSON document using platform-optimized methods
		 * @param obj The TimeSpan object to deserialize into
		 * @param doc The document to deserialize from
		 */
		static void deserialize( nfx::time::TimeSpan& obj, const Document& doc )
		{
			auto ticksVal = doc.getIntByPointer( "" );
			if ( ticksVal )
			{
				obj = nfx::time::TimeSpan( *ticksVal );
			}
			else
			{
				throw std::runtime_error( "Invalid TimeSpan format: expected integer ticks" );
			}
		}
	};

	/**
	 * @brief Specialization for nfx::time::DateTime
	 */
	template <>
	struct SerializationTraits<nfx::time::DateTime>
	{
		/**
		 * @brief Serialize DateTime to JSON document using platform-optimized methods
		 * @param obj The DateTime object to serialize
		 * @param doc The document to serialize into
		 */
		static void serialize( const nfx::time::DateTime& obj, Document& doc )
		{
			std::string value = obj.toIso8601Extended();
			doc.setStringByPointer( "", value );
		}

		/**
		 * @brief Deserialize DateTime from JSON document using platform-optimized methods
		 * @param obj The DateTime object to deserialize into
		 * @param doc The document to deserialize from
		 */
		static void deserialize( nfx::time::DateTime& obj, const Document& doc )
		{
			auto val = doc.getStringByPointer( "" );
			if ( val )
			{
				if ( !nfx::time::DateTime::tryParse( *val, obj ) )
				{
					throw std::runtime_error( "Invalid DateTime format: expected ISO 8601 string" );
				}
			}
		}
	};

	/**
	 * @brief Specialization for nfx::time::DateTimeOffset
	 */
	template <>
	struct SerializationTraits<nfx::time::DateTimeOffset>
	{
		/**
		 * @brief Serialize DateTimeOffset to JSON document using platform-optimized methods
		 * @param obj The DateTimeOffset object to serialize
		 * @param doc The document to serialize into
		 */
		static void serialize( const nfx::time::DateTimeOffset& obj, Document& doc )
		{
			std::string value = obj.toIso8601Extended();
			doc.setStringByPointer( "", value );
		}

		/**
		 * @brief Deserialize DateTimeOffset from JSON document using platform-optimized methods
		 * @param obj The DateTimeOffset object to deserialize into
		 * @param doc The document to deserialize from
		 */
		static void deserialize( nfx::time::DateTimeOffset& obj, const Document& doc )
		{
			auto val = doc.getStringByPointer( "" );
			if ( val )
			{
				if ( !nfx::time::DateTimeOffset::tryParse( *val, obj ) )
				{
					throw std::runtime_error( "Invalid DateTimeOffset format: expected ISO 8601 string with offset" );
				}
			}
		}
	};

	/**
	 * @brief Specialization for nfx::containers::ChdHashMap
	 */
	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	struct SerializationTraits<nfx::containers::ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>>
	{
		/**
		 * @brief Serialize ChdHashMap to JSON document as an object
		 * @param obj The ChdHashMap object to serialize
		 * @param doc The document to serialize into
		 */
		static void serialize( const nfx::containers::ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>& obj, Document& doc )
		{
			doc = Document::createObject();

			// Store CHD construction parameters for proper deserialization
			doc.setIntByPointer( "/maxSeedSearchMultiplier", static_cast<int64_t>( obj.maxSeedSearchMultiplier() ) );

			// Create nested object for the key-value data
			Document dataDoc = Document::createObject();

			// Use ChdHashMap's custom iterator to traverse all key-value pairs
			for ( auto it = obj.begin(); it != obj.end(); ++it )
			{
				const auto& pair = *it;
				const std::string& key = pair.first;
				const TValue& value = pair.second;

				// Serialize the value using a temporary serializer
				Document valueDoc;
				Serializer<TValue> valueSerializer;
				valueDoc = valueSerializer.serialize( value );

				// Set field in data object using JSON Pointer syntax
				std::string fieldPath = "/" + key;
				if ( valueDoc.hasStringByPointer( "" ) )
				{
					auto str = valueDoc.getStringByPointer( "" );
					dataDoc.setStringByPointer( fieldPath, *str );
				}
				else if ( valueDoc.hasIntByPointer( "" ) )
				{
					auto val = valueDoc.getIntByPointer( "" );
					dataDoc.setIntByPointer( fieldPath, *val );
				}
				else if ( valueDoc.hasDoubleByPointer( "" ) )
				{
					auto val = valueDoc.getDoubleByPointer( "" );
					dataDoc.setDoubleByPointer( fieldPath, *val );
				}
				else if ( valueDoc.hasBoolByPointer( "" ) )
				{
					auto val = valueDoc.getBoolByPointer( "" );
					dataDoc.setBoolByPointer( fieldPath, *val );
				}
				else if ( valueDoc.hasNullByPointer( "" ) )
				{
					dataDoc.setNullByPointer( fieldPath );
				}
				else if ( valueDoc.hasArrayByPointer( "" ) || valueDoc.hasObjectByPointer( "" ) )
				{
					// Handle nested arrays and objects
					dataDoc.setDocumentByPointer( fieldPath, valueDoc );
				}
			}

			// Store the data object in the main document
			doc.setDocumentByPointer( "/data", dataDoc );
		}

		/**
		 * @brief Deserialize ChdHashMap from JSON document
		 * @param obj The ChdHashMap object to deserialize into
		 * @param doc The document to deserialize from
		 */
		static void deserialize( nfx::containers::ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>& obj, const Document& doc )
		{
			if ( !doc.isObject( "" ) )
			{
				throw std::runtime_error( "Cannot deserialize non-object JSON value into ChdHashMap" );
			}

			uint32_t maxSeedSearchMultiplier = 100;
			if ( doc.hasIntByPointer( "/maxSeedSearchMultiplier" ) )
			{
				if ( auto multiplierOpt = doc.getIntByPointer( "/maxSeedSearchMultiplier" ) )
				{
					maxSeedSearchMultiplier = static_cast<uint32_t>( *multiplierOpt );
				}
			}

			// Collect key-value pairs for ChdHashMap construction
			std::vector<std::pair<std::string, TValue>> items;

			// Get the data object from the structured format
			Document dataDoc;
			if ( auto dataDocOpt = doc.getDocumentByPointer( "/data" ) )
			{
				dataDoc = *dataDocOpt;
			}
			else
			{
				throw std::runtime_error( "Failed to retrieve data field from ChdHashMap JSON" );
			}

			// Iterate over data object fields using FieldEnumerator
			FieldEnumerator enumerator( dataDoc );
			if ( enumerator.setPointer( "" ) && enumerator.isValid() )
			{
				enumerator.reset();

				while ( !enumerator.isEnd() )
				{
					std::string key = enumerator.currentKey();
					Document valueDoc = enumerator.currentValue();

					// Deserialize the value using a temporary serializer
					TValue value{};
					Serializer<TValue> valueSerializer;
					value = valueSerializer.deserialize( valueDoc );

					items.emplace_back( std::move( key ), std::move( value ) );

					if ( !enumerator.next() )
					{
						break;
					}
				}
			}

			obj = nfx::containers::ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>( std::move( items ), maxSeedSearchMultiplier );
		}
	};
} // namespace nfx::serialization::json
