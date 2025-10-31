/**
 * @file Serializer.inl
 * @brief Templated JSON serializer implementation file
 * @details Contains the template method implementations for the Serializer class.
 *          This file provides the actual implementation of serialization and
 *          deserialization methods for all supported types.
 */

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "nfx/config.h"

// nfx container support
#include "nfx/containers/HashMap.h"
#include "nfx/containers/StringMap.h"
#include "nfx/containers/StringSet.h"

#include "nfx/serialization/json/Document.h"

namespace nfx::serialization::json
{
	//=====================================================================
	// Internal implementation type traits
	//=====================================================================

	namespace detail
	{
		/**
		 * @brief Type trait to detect if a type has a serialize method (void version)
		 * @tparam T The type to check
		 * @details Uses SFINAE to detect if type T has a serialize method that accepts
		 *          a Serializer<T>& parameter and Document& parameter. Used for compile-time
		 *          dispatch to custom serialization methods.
		 */
		template <typename T>
		struct has_serialize_method
		{
		private:
			template <typename U>
			static auto test( int ) -> decltype( std::declval<const U&>().serialize(
													 std::declval<const Serializer<U>&>(),
													 std::declval<Document&>() ),
				std::true_type{} );
			template <typename>
			static std::false_type test( ... );

		public:
			/** @brief True if type T has a serialize method, false otherwise */
			static constexpr bool value = decltype( test<T>( 0 ) )::value;
		};

		/**
		 * @brief Type trait to detect if a type has a serialize method (Document return version)
		 * @tparam T The type to check
		 * @details Uses SFINAE to detect if type T has a serialize method that returns
		 *          Document and accepts a Serializer<T>& parameter. Used for compile-time
		 *          dispatch to custom serialization methods.
		 */
		template <typename T>
		struct has_serialize_method_returning_document
		{
		private:
			template <typename U>
			static auto test( int ) -> decltype( std::declval<const U&>().serialize(
													 std::declval<const Serializer<U>&>() ),
				std::true_type{} );
			template <typename>
			static std::false_type test( ... );

		public:
			/** @brief True if type T has a serialize method returning Document, false otherwise */
			static constexpr bool value = decltype( test<T>( 0 ) )::value;
		};

		/**
		 * @brief Type trait to detect if a type has a serialize method (no parameters version)
		 * @tparam T The type to check
		 * @details Uses SFINAE to detect if type T has a serialize method that returns
		 *          Document and takes no parameters. Used for compile-time dispatch to
		 *          custom serialization methods.
		 */
		template <typename T>
		struct has_serialize_method_no_params
		{
		private:
			template <typename U>
			static auto test( int ) -> decltype( std::declval<const U&>().serialize(),
				std::true_type{} );
			template <typename>
			static std::false_type test( ... );

		public:
			/** @brief True if type T has a serialize method taking no parameters, false otherwise */
			static constexpr bool value = decltype( test<T>( 0 ) )::value;
		};

		/**
		 * @brief Type trait to detect if a type has a deserialize method
		 * @tparam T The type to check
		 * @details Uses SFINAE to detect if type T has a deserialize method that accepts
		 *          a const Serializer<T>& parameter. Used for compile-time dispatch to custom
		 *          deserialization methods.
		 */
		template <typename T>
		struct has_deserialize_method
		{
		private:
			template <typename U>
			static auto test( int ) -> decltype( std::declval<U>().deserialize( std::declval<const Serializer<U>&>(), std::declval<const Document&>() ), std::true_type{} );
			template <typename>
			static std::false_type test( ... );

		public:
			/** @brief True if type T has a deserialize method, false otherwise */
			static constexpr bool value = decltype( test<T>( 0 ) )::value;
		};

		/**
		 * @brief Type trait to detect if a type is a container
		 * @tparam T The type to check
		 * @details Base template that evaluates to false. Specialized for supported
		 *          container types including vector, array, map, and unordered_map.
		 */
		template <typename T>
		struct is_container : std::false_type
		{
		};

		/** @brief Specialization for std::vector */
		template <typename T>
		struct is_container<std::vector<T>> : std::true_type
		{
		};

		/** @brief Specialization for std::array */
		template <typename T, std::size_t N>
		struct is_container<std::array<T, N>> : std::true_type
		{
		};

		/** @brief Specialization for std::map */
		template <typename K, typename V>
		struct is_container<std::map<K, V>> : std::true_type
		{
		};

		/** @brief Specialization for std::unordered_map */
		template <typename K, typename V>
		struct is_container<std::unordered_map<K, V>> : std::true_type
		{
		};

		/** @brief Specialization for std::set */
		template <typename T>
		struct is_container<std::set<T>> : std::true_type
		{
		};

		/** @brief Specialization for std::unordered_set */
		template <typename T>
		struct is_container<std::unordered_set<T>> : std::true_type
		{
		};

		/** @brief Specialization for std::list */
		template <typename T>
		struct is_container<std::list<T>> : std::true_type
		{
		};

		/** @brief Specialization for std::deque */
		template <typename T>
		struct is_container<std::deque<T>> : std::true_type
		{
		};

		/**
		 * @brief Type trait to detect nfx container types
		 * @tparam T The type to check
		 * @details Base template that evaluates to false. Specialized for nfx container types.
		 */
		template <typename T>
		struct is_nfx_container : std::false_type
		{
		};

		/** @brief Specialization for nfx::containers::HashMap */
		template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
		struct is_nfx_container<nfx::containers::HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>> : std::true_type
		{
		};

		/** @brief Specialization for nfx::containers::StringMap (STL-compatible) */
		template <typename T>
		struct is_nfx_container<nfx::containers::StringMap<T>> : std::true_type
		{
		};

		/** @brief Specialization for nfx::containers::StringSet (STL-compatible) */
		template <>
		struct is_nfx_container<nfx::containers::StringSet> : std::true_type
		{
		};

		/**
		 * @brief Type trait to detect if a type is std::optional
		 * @tparam T The type to check
		 * @details Base template that evaluates to false. Specialized for std::optional<T>.
		 */
		template <typename T>
		struct is_optional : std::false_type
		{
		};

		/** @brief Specialization for std::optional */
		template <typename T>
		struct is_optional<std::optional<T>> : std::true_type
		{
		};

		/**
		 * @brief Type trait to detect if a type is a smart pointer
		 * @tparam T The type to check
		 * @details Base template that evaluates to false. Specialized for std::unique_ptr
		 *          and std::shared_ptr types.
		 */
		template <typename T>
		struct is_smart_pointer : std::false_type
		{
		};

		/** @brief Specialization for std::unique_ptr */
		template <typename T>
		struct is_smart_pointer<std::unique_ptr<T>> : std::true_type
		{
		};

		/** @brief Specialization for std::shared_ptr */
		template <typename T>
		struct is_smart_pointer<std::shared_ptr<T>> : std::true_type
		{
		};
	} // namespace detail

	//=====================================================================
	// Serializer class
	//=====================================================================

	//----------------------------------------------
	// Serialization options and context
	//----------------------------------------------

	template <typename T>
	template <typename U>
	NFX_META_INLINE void Serializer<T>::Options::copyFrom( const typename Serializer<U>::Options& other )
	{
		includeNullFields = other.includeNullFields;
		prettyPrint = other.prettyPrint;
		validateOnDeserialize = other.validateOnDeserialize;
	}

	template <typename T>
	template <typename U>
	NFX_META_INLINE typename Serializer<T>::Options Serializer<T>::Options::createFrom( const typename Serializer<U>::Options& other )
	{
		Options result;
		result.includeNullFields = other.includeNullFields;
		result.prettyPrint = other.prettyPrint;
		result.validateOnDeserialize = other.validateOnDeserialize;
		return result;
	}

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename T>
	NFX_META_INLINE Serializer<T>::Serializer( const Options& options ) noexcept
		: m_options{ options }
	{
	}

	//----------------------------------------------
	// Options management
	//----------------------------------------------

	template <typename T>
	NFX_META_INLINE const Serializer<T>::Options& Serializer<T>::options() const noexcept
	{
		return m_options;
	}

	template <typename T>
	NFX_META_INLINE void Serializer<T>::setOptions( const Options& options ) noexcept
	{
		m_options = options;
	}

	//----------------------------------------------
	// Static convenience serialization methods
	//----------------------------------------------

	template <typename T>
	inline std::string Serializer<T>::toJson( const T& obj, const Serializer<T>::Options& options )
	{
		Serializer<T> serializer( options );
		return serializer.serializeToString( obj );
	}

	template <typename T>
	inline T Serializer<T>::fromJson( std::string_view jsonStr, const Serializer<T>::Options& options )
	{
		Serializer<T> serializer( options );
		return serializer.deserializeFromString( jsonStr );
	}

	//----------------------------------------------
	// Instance serialization methods
	//----------------------------------------------

	template <typename T>
	inline Document Serializer<T>::serialize( const T& obj ) const
	{
		Document doc;
		// Look for serialize method with no parameters
		if constexpr ( detail::has_serialize_method_no_params<T>::value )
		{
			doc = obj.serialize();
		}
		// Look for serialize method returning Document with serializer parameter
		else if constexpr ( detail::has_serialize_method_returning_document<T>::value )
		{
			doc = obj.serialize( *this );
		}
		// Look for traditional serialize method with serializer and document parameters
		else if constexpr ( detail::has_serialize_method<T>::value )
		{
			// Use custom serialize method if available
			obj.serialize( *this, doc );
		}
		else
		{
			// Use unified templated serialization
			serializeValue( obj, doc );
		}
		return doc;
	}

	template <typename T>
	inline std::string Serializer<T>::serializeToString( const T& obj ) const
	{
		Document doc = serialize( obj );
		return doc.toJsonString( m_options.prettyPrint ? 2 : 0 );
	}

	template <typename T>
	inline T Serializer<T>::deserialize( const Document& doc ) const
	{
		T obj{};
		if constexpr ( detail::has_deserialize_method<T>::value )
		{
			// Use custom deserialize method if available
			obj.deserialize( *this, doc );
		}
		else
		{
			// Use unified templated deserialization
			deserializeValue( doc, obj );
		}
		return obj;
	}

	template <typename T>
	inline T Serializer<T>::deserializeFromString( std::string_view jsonStr ) const
	{
		auto optDoc = Document::fromJsonString( jsonStr );
		if ( !optDoc )
		{
			throw std::runtime_error( "Failed to parse JSON string" );
		}
		return deserialize( *optDoc );
	}

	//----------------------------------------------
	// Private methods
	//----------------------------------------------

	template <typename T>
	template <typename U>
	inline void Serializer<T>::serializeValue( const U& obj, Document& doc ) const
	{
		// Handle built-in types with library-defined logic
		if constexpr ( std::is_same_v<U, bool> )
		{
			// Handle bool separately (before is_integral check)
			doc.set<bool>( "", obj );
		}
		else if constexpr ( std::is_integral_v<U> )
		{
			// Handle integral types (int, long, etc.)
			doc.set<int64_t>( "", static_cast<int64_t>( obj ) );
		}
		else if constexpr ( std::is_floating_point_v<U> )
		{
			// Handle floating point types
			doc.set<double>( "", static_cast<double>( obj ) );
		}
		else if constexpr ( std::is_same_v<U, std::string> )
		{
			// Handle std::string
			doc.set<std::string>( "", obj );
		}
		else if constexpr ( detail::is_optional<U>::value )
		{
			if ( obj.has_value() )
			{
				serializeValue( obj.value(), doc );
			}
			else
			{
				doc.setNull( "" );
			}
		}
		else if constexpr ( detail::is_smart_pointer<U>::value )
		{
			if ( obj )
			{
				serializeValue( *obj, doc );
			}
			else
			{
				doc.setNull( "" );
			}
		}

		else if constexpr ( detail::is_container<U>::value )
		{
			if constexpr ( requires { typename U::mapped_type; } )
			{ // Map-like containers (std::map, std::unordered_map) - serialize as JSON object
				for ( const auto& pair : obj )
				{
					std::string key;
					if constexpr ( std::is_convertible_v<decltype( pair.first ), std::string> )
					{
						key = std::string( pair.first );
					}
					else
					{
						key = std::to_string( pair.first );
					}

					Document valueDoc;
					serializeValue( pair.second, valueDoc );

					// Set field in object using JSON Pointer syntax
					std::string fieldPath = "/" + key;
					if ( valueDoc.is<std::string>( "" ) )
					{
						auto str = valueDoc.get<std::string>( "" );
						doc.set<std::string>( fieldPath, *str );
					}
					else if ( valueDoc.is<int>( "" ) )
					{
						auto val = valueDoc.get<int64_t>( "" );
						doc.set<int64_t>( fieldPath, *val );
					}
					else if ( valueDoc.is<double>( "" ) )
					{
						auto val = valueDoc.get<double>( "" );
						doc.set<double>( fieldPath, *val );
					}
					else if ( valueDoc.is<bool>( "" ) )
					{
						auto val = valueDoc.get<bool>( "" );
						doc.set<bool>( fieldPath, *val );
					}
					else if ( valueDoc.isNull( "" ) )
					{
						doc.setNull( fieldPath );
					}
					else if ( valueDoc.is<Document::Array>( "" ) || valueDoc.is<Document::Object>( "" ) )
					{
						// Handle nested arrays and objects (e.g., std::vector<int>, std::map<string, int>)
						doc.set<Document>( fieldPath, valueDoc );
					}
				}
			}
			else
			{
				doc.set<Document::Array>( "" );

				size_t index = 0;
				for ( const auto& item : obj )
				{
					Document itemDoc;
					serializeValue( item, itemDoc );

					// Buffer sized for "/%zu" format: 1 (slash) + 20 (max digits for size_t) + 1 (null) = 22 bytes minimum
					// Using 32 bytes for comfortable margin and power-of-2 alignment
					char arrayPath[32];
					std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );
					if ( itemDoc.is<std::string>( "" ) )
					{
						auto str = itemDoc.get<std::string>( "" );
						doc.set<std::string>( arrayPath, *str );
					}
					else if ( itemDoc.is<int>( "" ) )
					{
						auto val = itemDoc.get<int64_t>( "" );
						doc.set<int64_t>( arrayPath, *val );
					}
					else if ( itemDoc.is<double>( "" ) )
					{
						auto val = itemDoc.get<double>( "" );
						doc.set<double>( arrayPath, *val );
					}
					else if ( itemDoc.is<bool>( "" ) )
					{
						auto val = itemDoc.get<bool>( "" );
						doc.set<bool>( arrayPath, *val );
					}
					else if ( itemDoc.isNull( "" ) )
					{
						doc.setNull( arrayPath );
					}
					else if ( itemDoc.is<Document::Array>( "" ) || itemDoc.is<Document::Object>( "" ) )
					{
						// Handle nested arrays and objects in array elements
						doc.set<Document>( arrayPath, itemDoc );
					}
					++index;
				}
			}
		}
		else if constexpr ( detail::is_nfx_container<U>::value )
		{
			// Handle nfx containers
			if constexpr ( requires { typename U::mapped_type; } )
			{
				// Map-like nfx containers (HashMap, StringMap)
				for ( const auto& pair : obj )
				{
					std::string key;
					if constexpr ( std::is_convertible_v<decltype( pair.first ), std::string> )
					{
						key = std::string( pair.first );
					}
					else
					{
						key = std::to_string( pair.first );
					}

					Document valueDoc;
					serializeValue( pair.second, valueDoc );

					// Set field in object using JSON Pointer syntax
					std::string fieldPath = "/" + key;
					if ( valueDoc.is<std::string>( "" ) )
					{
						auto str = valueDoc.get<std::string>( "" );
						doc.set<std::string>( fieldPath, *str );
					}
					else if ( valueDoc.is<int>( "" ) )
					{
						auto val = valueDoc.get<int64_t>( "" );
						doc.set<int64_t>( fieldPath, *val );
					}
					else if ( valueDoc.is<double>( "" ) )
					{
						auto val = valueDoc.get<double>( "" );
						doc.set<double>( fieldPath, *val );
					}
					else if ( valueDoc.is<bool>( "" ) )
					{
						auto val = valueDoc.get<bool>( "" );
						doc.set<bool>( fieldPath, *val );
					}
					else if ( valueDoc.isNull( "" ) )
					{
						doc.setNull( fieldPath );
					}
					else if ( valueDoc.is<Document::Array>( "" ) || valueDoc.is<Document::Object>( "" ) )
					{
						// Handle nested arrays and objects for nfx map containers
						doc.set<Document>( fieldPath, valueDoc );
					}
				}
			}
			else
			{
				// Set-like nfx containers
				doc.set<Document::Array>( "" );

				size_t index = 0;
				for ( const auto& item : obj )
				{
					Document itemDoc;
					serializeValue( item, itemDoc );

					// Buffer sized for "/%zu" format: 1 (slash) + 20 (max digits for size_t) + 1 (null) = 22 bytes minimum
					// Using 32 bytes for comfortable margin and power-of-2 alignment
					char arrayPath[32];
					std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );
					if ( itemDoc.is<std::string>( "" ) )
					{
						auto str = itemDoc.get<std::string>( "" );
						doc.set<std::string>( arrayPath, *str );
					}
					else if ( itemDoc.is<int>( "" ) )
					{
						auto val = itemDoc.get<int64_t>( "" );
						doc.set<int64_t>( arrayPath, *val );
					}
					else if ( itemDoc.is<double>( "" ) )
					{
						auto val = itemDoc.get<double>( "" );
						doc.set<double>( arrayPath, *val );
					}
					else if ( itemDoc.is<bool>( "" ) )
					{
						auto val = itemDoc.get<bool>( "" );
						doc.set<bool>( arrayPath, *val );
					}
					else if ( itemDoc.isNull( "" ) )
					{
						doc.setNull( arrayPath );
					}
					else if ( itemDoc.is<Document::Array>( "" ) || itemDoc.is<Document::Object>( "" ) )
					{
						// Handle nested arrays and objects for nfx set containers
						doc.set<Document>( arrayPath, itemDoc );
					}
					++index;
				}
			}
		}
		else
		{
			// Fall back to SerializationTraits (handles member methods or specializations)
			SerializationTraits<U>::serialize( obj, doc );
		}
	}

	template <typename T>
	template <typename U>
	inline void Serializer<T>::deserializeValue( const Document& doc, U& obj ) const
	{
		// Handle built-in types with library-defined logic
		if constexpr ( std::is_same_v<U, bool> )
		{
			// Handle bool
			auto val = doc.get<bool>( "" );
			if ( val )
				obj = *val;
		}
		else if constexpr ( std::is_integral_v<U> )
		{
			// Handle integral types
			auto val = doc.get<int64_t>( "" );
			if ( val )
				obj = static_cast<U>( *val );
		}
		else if constexpr ( std::is_floating_point_v<U> )
		{
			// Handle floating point types
			auto val = doc.get<double>( "" );
			if ( val )
				obj = static_cast<U>( *val );
		}
		else if constexpr ( std::is_same_v<U, std::string> )
		{
			// Handle std::string
			auto val = doc.get<std::string>( "" );
			if ( val )
				obj = *val;
		}
		else if constexpr ( detail::is_optional<U>::value )
		{
			// Handle std::optional types
			if ( doc.isNull( "" ) )
			{
				obj = std::nullopt;
			}
			else
			{
				typename U::value_type value{};
				deserializeValue( doc, value );
				obj = std::move( value );
			}
		}
		else if constexpr ( detail::is_smart_pointer<U>::value )
		{
			if ( doc.isNull( "" ) )
			{
				obj = nullptr;
			}
			else
			{
				auto value = std::make_unique<typename U::element_type>();
				deserializeValue( doc, *value );
				if constexpr ( std::is_same_v<U, std::unique_ptr<typename U::element_type>> )
				{
					obj = std::move( value );
				}
				else
				{
					obj = std::shared_ptr<typename U::element_type>( value.release() );
				}
			}
		}

		else if constexpr ( detail::is_container<U>::value )
		{
			// Handle STL containers with flexible JSON input types
			if constexpr ( requires { obj.clear(); } )
			{
				obj.clear();
			}

			if constexpr ( requires { typename U::mapped_type; } )
			{
				// Map-like containers: only accept JSON objects
				if ( doc.is<Document::Object>( "" ) )
				{
					// Object → map: iterate over object fields using FieldEnumerator
					FieldEnumerator enumerator( doc );
					if ( enumerator.setPointer( "" ) && enumerator.isValid() )
					{
						enumerator.reset();

						while ( !enumerator.isEnd() )
						{
							std::string key = enumerator.currentKey();
							Document valueDoc = enumerator.currentValue();

							typename U::mapped_type value{};
							deserializeValue( valueDoc, value );

							obj[key] = std::move( value );

							if ( !enumerator.next() )
							{
								break;
							}
						}
					}
				}
				else if ( doc.isNull( "" ) )
				{
					// Handle null → empty map (obj.clear() already called above)
				}
				else
				{
					throw std::runtime_error( "Cannot deserialize non-object JSON value into map container" );
				}
			}
			else
			{
				// Non-map containers: accept arrays and single values
				if ( doc.is<Document::Array>( "" ) )
				{
					// Standard case: JSON array → container using ArrayEnumerator
					ArrayEnumerator enumerator( doc );
					if ( enumerator.setPointer( "" ) && enumerator.isValid() )
					{
						if constexpr ( std::is_same_v<U, std::vector<typename U::value_type>> )
						{
							obj.reserve( enumerator.size() );
						}

						enumerator.reset();
						size_t arrayIndex = 0;

						while ( !enumerator.isEnd() )
						{
							Document elementDoc = enumerator.currentElement();
							typename U::value_type item{};

							deserializeValue( elementDoc, item );

							if constexpr ( requires { obj.push_back( std::move( item ) ); } )
							{
								obj.push_back( std::move( item ) );
							}
							else if constexpr ( requires { obj.insert( std::move( item ) ); } )
							{
								obj.insert( std::move( item ) );
							}
							else if constexpr ( requires { obj.insert( obj.end(), std::move( item ) ); } )
							{
								obj.insert( obj.end(), std::move( item ) );
							}
							else if constexpr ( requires { obj[arrayIndex] = std::move( item ); } )
							{
								if ( arrayIndex < obj.size() )
								{
									obj[arrayIndex] = std::move( item );
								}
							}
							else
							{
								static_assert( std::is_void_v<U>, "Container doesn't support push_back, insert, or indexed assignment" );
							}

							++arrayIndex;

							if ( !enumerator.next() )
							{
								break;
							}
						}
					}
				}
				else if ( doc.isNull( "" ) )
				{
					// Handle null → empty container (obj.clear() already called above)
				}
				else
				{
					// Single value → container
					typename U::value_type item{};
					deserializeValue( doc, item );

					if constexpr ( std::is_same_v<U, std::vector<typename U::value_type>> )
					{
						obj.push_back( std::move( item ) );
					}
					else if constexpr ( requires { obj.insert( std::move( item ) ); } )
					{
						obj.insert( std::move( item ) );
					}
					else if constexpr ( requires { obj.insert( obj.end(), std::move( item ) ); } )
					{
						obj.insert( obj.end(), std::move( item ) );
					}
					else
					{
						// Fixed-size containers (like std::array) don't support insertion - skip
					}
				}
			}
		}
		else if constexpr ( detail::is_nfx_container<U>::value )
		{
			// Handle nfx containers
			if constexpr ( requires { obj.clear(); } )
			{
				obj.clear();
			}

			if constexpr ( requires { typename U::mapped_type; } )
			{
				// Map-like nfx containers: only accept JSON objects
				if ( doc.is<Document::Object>( "" ) )
				{
					// Object → nfx map: iterate over object fields using FieldEnumerator
					FieldEnumerator enumerator( doc );
					if ( enumerator.setPointer( "" ) && enumerator.isValid() )
					{
						enumerator.reset();

						while ( !enumerator.isEnd() )
						{
							std::string key = enumerator.currentKey();
							Document valueDoc = enumerator.currentValue();

							typename U::mapped_type value{};
							deserializeValue( valueDoc, value );

							// Try to insert into nfx map using different methods
							if constexpr ( requires { obj[key] = std::move( value ); } )
							{
								obj[key] = std::move( value );
							}
							else if constexpr ( requires { obj.insertOrAssign( key, std::move( value ) ); } )
							{
								obj.insertOrAssign( key, std::move( value ) );
							}
							else
							{
								throw std::runtime_error( "nfx map container doesn't support standard assignment methods (operator[] or insertOrAssign)" );
							}

							if ( !enumerator.next() )
							{
								break;
							}
						}
					}
				}
				else if ( doc.isNull( "" ) )
				{
					// Handle null → empty nfx map (obj.clear() already called above if supported)
				}
				else
				{
					throw std::runtime_error( "Cannot deserialize non-object JSON value into nfx map container" );
				}
			}
			else
			{
				// Non-map nfx containers: accept arrays and single values
				if ( doc.is<Document::Array>( "" ) )
				{
					// Standard case: JSON array → nfx container using ArrayEnumerator
					ArrayEnumerator enumerator( doc );
					if ( enumerator.setPointer( "" ) && enumerator.isValid() )
					{
						enumerator.reset();
						size_t arrayIndex = 0;

						while ( !enumerator.isEnd() )
						{
							Document elementDoc = enumerator.currentElement();
							typename U::value_type item{};

							deserializeValue( elementDoc, item );

							// Try different insertion methods based on nfx container API
							if constexpr ( requires { obj.push_back( std::move( item ) ); } )
							{
								obj.push_back( std::move( item ) );
							}
							else if constexpr ( requires { obj.insert( std::move( item ) ); } )
							{
								obj.insert( std::move( item ) );
							}
							else if constexpr ( requires { obj.insert( obj.end(), std::move( item ) ); } )
							{
								obj.insert( obj.end(), std::move( item ) );
							}
							else if constexpr ( requires { obj[arrayIndex] = std::move( item ); } )
							{
								if ( arrayIndex < obj.size() )
								{
									obj[arrayIndex] = std::move( item );
								}
							}
							else
							{
								throw std::runtime_error( "nfx container doesn't support standard insertion methods (push_back, insert, or indexed assignment)" );
							}

							++arrayIndex;

							if ( !enumerator.next() )
							{
								break;
							}
						}
					}
				}
				else if ( doc.isNull( "" ) )
				{
					// Handle null → empty nfx container (obj.clear() already called above if supported)
				}
				else
				{
					// Single value → nfx container
					typename U::value_type item{};
					deserializeValue( doc, item );

					if constexpr ( requires { obj.push_back( std::move( item ) ); } )
					{
						obj.push_back( std::move( item ) );
					}
					else if constexpr ( requires { obj.insert( std::move( item ) ); } )
					{
						obj.insert( std::move( item ) );
					}
					else if constexpr ( requires { obj.insert( obj.end(), std::move( item ) ); } )
					{
						obj.insert( obj.end(), std::move( item ) );
					}
					else
					{
						throw std::runtime_error( "nfx container doesn't support standard insertion methods (push_back, insert, or indexed assignment)" );
					}
				}
			}
		}
		else
		{
			// Fall back to SerializationTraits (handles member methods or specializations)
			SerializationTraits<U>::deserialize( obj, doc );
		}
	}
} // namespace nfx::serialization::json
