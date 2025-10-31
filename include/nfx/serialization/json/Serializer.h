/**
 * @file Serializer.h
 * @brief Templated JSON serializer with reflection capabilities
 * @details Provides comprehensive object serialization to/from JSON with automatic
 *          type detection, field mapping, and customizable serialization strategies.
 *          Supports POD types, containers, custom objects, and nested structures.
 */

#pragma once

#include "nfx/config.h"

#include "Document.h"
#include "ArrayEnumerator.h"
#include "FieldEnumerator.h"
#include "SerializationTraits.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// Serializer class
	//=====================================================================

	/**
	 * @brief Templated JSON serializer with reflection capabilities
	 * @tparam T The type to serialize/deserialize
	 * @details Provides automatic serialization and deserialization of C++ objects
	 *          to/from JSON using compile-time reflection and type traits.
	 */
	template <typename T>
	class Serializer final
	{
	public:
		//----------------------------------------------
		// Serialization options and context
		//----------------------------------------------

		/**
		 * @brief Serialization options and context
		 */
		struct Options
		{
			bool includeNullFields = false;	   ///< Include fields with null values
			bool prettyPrint = false;		   ///< Format output with indentation
			bool validateOnDeserialize = true; ///< Validate data during deserialization

			/**
			 * @brief Default constructor
			 */
			Options() = default;

			/**
			 * @brief Copy values from another serializer's options
			 * @tparam U The source serializer type
			 * @param other Options from another serializer type
			 */
			template <typename U>
			NFX_META_INLINE void copyFrom( const typename Serializer<U>::Options& other );

			/**
			 * @brief Create Options with values copied from another serializer's options
			 * @tparam U The source serializer type
			 * @param other Options from another serializer type
			 * @return New Options instance with copied values
			 */
			template <typename U>
			NFX_META_INLINE static Options createFrom( const typename Serializer<U>::Options& other );
		};

		//----------------------------------------------
		// Type aliases
		//----------------------------------------------

		/** @brief The type being serialized/deserialized */
		using value_type = T;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor
		 */
		Serializer() = default;

		/**
		 * @brief Constructor with options
		 * @param options Serialization options to use
		 */
		NFX_META_INLINE explicit Serializer( const Options& options ) noexcept;

		//----------------------------------------------
		// Options management
		//----------------------------------------------

		/**
		 * @brief Get current serialization options
		 * @return Current options
		 */
		NFX_META_INLINE const Options& options() const noexcept;

		/**
		 * @brief Set serialization options
		 * @param options New options to use
		 */
		NFX_META_INLINE void setOptions( const Options& options ) noexcept;

		//----------------------------------------------
		// Static convenience serialization methods
		//----------------------------------------------

		/**
		 * @brief Serialize object to JSON string
		 * @tparam T Type of object to serialize
		 * @param obj Object to serialize
		 * @param options Serialization options (optional, uses defaults if not provided)
		 * @return JSON string representation
		 */
		inline static std::string toJson( const T& obj, const Options& options = {} );

		/**
		 * @brief Deserialize object from JSON string
		 * @tparam T Type of object to deserialize
		 * @param jsonStr JSON string to deserialize from
		 * @param options Serialization options (optional, uses defaults if not provided)
		 * @return Deserialized object
		 */
		inline static T fromJson( std::string_view jsonStr, const Options& options = {} );

		//----------------------------------------------
		// Instance serialization methods
		//----------------------------------------------

		/**
		 * @brief Serialize object to JSON document
		 * @param obj The object to serialize
		 * @return Document containing the serialized JSON
		 * @throws std::runtime_error if serialization fails
		 */
		inline Document serialize( const T& obj ) const;

		/**
		 * @brief Serialize object to JSON string
		 * @param obj The object to serialize
		 * @return JSON string representation
		 * @throws std::runtime_error if serialization fails
		 */
		inline std::string serializeToString( const T& obj ) const;

		/**
		 * @brief Deserialize object from JSON document
		 * @param doc The document to deserialize from
		 * @return Deserialized object
		 * @throws std::runtime_error if deserialization fails
		 */
		inline T deserialize( const Document& doc ) const;

		/**
		 * @brief Deserialize object from JSON string
		 * @param jsonStr The JSON string to deserialize from
		 * @return Deserialized object
		 * @throws std::runtime_error if deserialization fails
		 */
		inline T deserializeFromString( std::string_view jsonStr ) const;

	private:
		//----------------------------------------------
		// Private methods
		//----------------------------------------------

		/**
		 * @brief Unified templated serialization method
		 * @tparam U The type to serialize (deduced from parameter)
		 * @param obj Object to serialize
		 * @param doc Document to serialize into
		 */
		template <typename U>
		inline void serializeValue( const U& obj, Document& doc ) const;

		/**
		 * @brief Unified templated deserialization method
		 * @tparam U The type to deserialize (deduced from parameter)
		 * @param doc Document to deserialize from
		 * @param obj Object to deserialize into
		 */
		template <typename U>
		inline void deserializeValue( const Document& doc, U& obj ) const;

		//----------------------------------------------
		// Member variables
		//----------------------------------------------

		Options m_options{}; ///< Serialization options
	};
} // namespace nfx::serialization::json

#include "nfx/detail/serialization/json/Serializer.inl"
