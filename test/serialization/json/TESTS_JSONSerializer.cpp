/**
 * @file TESTS_JSONSerializer.cpp
 * @brief Comprehensive tests for JSON Serializer functionality
 * @details Tests covering all type specializations including primitive types,
 *          containers, smart pointers, optional types, and nfx datatypes.
 *          Validates serialization roundtrip integrity (serialize → deserialize == original).
 */

#include <gtest/gtest.h>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nfx/core/Hashing.h>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// JSON Serializer tests
	//=====================================================================

	//----------------------------------------------
	// JSON Serializer test fixture
	//----------------------------------------------

	class JSONSerializerTest : public ::testing::Test
	{
	protected:
		template <typename T>
		void testRoundTrip( const T& original, const typename Serializer<T>::Options& options = {} )
		{
			// Serialize to JSON string
			Serializer<T> serializer( options );
			std::string jsonStr = serializer.serializeToString( original );

			// Verify JSON is not empty
			EXPECT_FALSE( jsonStr.empty() ) << "Serialized JSON should not be empty";

			// Deserialize back from JSON string
			T deserialized = serializer.deserializeFromString( jsonStr );

			// Verify roundtrip integrity - SERIALIZATION == DESERIALIZATION!
			EXPECT_EQ( original, deserialized ) << "Roundtrip must preserve original value";
		}
	};

	//----------------------------------------------
	// Basic primitive type
	//----------------------------------------------

	TEST_F( JSONSerializerTest, BooleanTypes )
	{
		testRoundTrip( true );
		testRoundTrip( false );
	}

	TEST_F( JSONSerializerTest, IntegerTypes )
	{
		testRoundTrip( int{ 42 } );
		testRoundTrip( int{ -42 } );
		testRoundTrip( int{ 0 } );
		testRoundTrip( std::int64_t{ 1234567890123LL } );
		testRoundTrip( std::int32_t{ -2147483648 } );
	}

	TEST_F( JSONSerializerTest, FloatingPointTypes )
	{
		testRoundTrip( double{ 3.14159 } );
		testRoundTrip( float{ -2.71828f } );
		testRoundTrip( double{ 0.0 } );
	}

	TEST_F( JSONSerializerTest, StringTypes )
	{
		testRoundTrip( std::string{ "Hello, World!" } );
		testRoundTrip( std::string{ "" } );
		testRoundTrip( std::string{ "Unicode: 你好 🌍" } );
	}

	//----------------------------------------------
	// nfx datatypes
	//----------------------------------------------

	TEST_F( JSONSerializerTest, Int128Types )
	{
		using nfx::datatypes::Int128;

		// Test basic Int128 values
		testRoundTrip( Int128{ 0 } );	// Zero
		testRoundTrip( Int128{ 42 } );	// Small positive
		testRoundTrip( Int128{ -42 } ); // Small negative

		// Test construction from high/low parts
		testRoundTrip( Int128{ 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL } );
	}

	TEST_F( JSONSerializerTest, DecimalTypes )
	{
		using nfx::datatypes::Decimal;

		// Test basic Decimal values
		testRoundTrip( Decimal::zero() );
		testRoundTrip( Decimal::one() );
	}

	TEST_F( JSONSerializerTest, TimeSpanTypes )
	{
		using nfx::time::TimeSpan;

		// Test basic TimeSpan values
		testRoundTrip( TimeSpan{ 0 } );					  // Zero
		testRoundTrip( TimeSpan{ 123456789 } );			  // Positive ticks
		testRoundTrip( TimeSpan{ -123456789 } );		  // Negative ticks
		testRoundTrip( TimeSpan::fromDays( 1.5 ) );		  // 1.5 days
		testRoundTrip( TimeSpan::fromHours( 25.5 ) );	  // 25.5 hours
		testRoundTrip( TimeSpan::fromMinutes( 90.5 ) );	  // 90.5 minutes
		testRoundTrip( TimeSpan::fromSeconds( 3661.5 ) ); // 3661.5 seconds
	}

	TEST_F( JSONSerializerTest, DateTimeTypes )
	{
		using nfx::time::DateTime;

		// Test basic DateTime values
		testRoundTrip( DateTime::epoch() );							// Unix epoch
		testRoundTrip( DateTime::minValue() );						// Minimum DateTime
		testRoundTrip( DateTime::maxValue() );						// Maximum DateTime
		testRoundTrip( DateTime{ 2024, 1, 1 } );					// New Year 2024
		testRoundTrip( DateTime{ 2024, 12, 31, 23, 59, 59, 999 } ); // End of 2024
		testRoundTrip( DateTime::sinceEpochSeconds( 1704067200 ) ); // 2024-01-01 00:00:00 UTC
	}

	TEST_F( JSONSerializerTest, DateTimeOffsetTypes )
	{
		using nfx::time::DateTime;
		using nfx::time::DateTimeOffset;
		using nfx::time::TimeSpan;

		// Test basic DateTimeOffset values
		testRoundTrip( DateTimeOffset::unixEpoch() );													   // Unix epoch UTC
		testRoundTrip( DateTimeOffset{ DateTime{ 2024, 1, 1 }, TimeSpan::fromHours( 0 ) } );			   // UTC
		testRoundTrip( DateTimeOffset{ DateTime{ 2024, 1, 1, 12, 0, 0 }, TimeSpan::fromHours( 5 ) } );	   // +05:00
		testRoundTrip( DateTimeOffset{ DateTime{ 2024, 6, 15, 18, 30, 45 }, TimeSpan::fromHours( -8 ) } ); // -08:00
		testRoundTrip( DateTimeOffset::fromUnixTimeSeconds( 1704067200 ) );								   // From unix timestamp
	}

	//----------------------------------------------
	// STL containers
	//----------------------------------------------

	TEST_F( JSONSerializerTest, VectorTypes )
	{
		testRoundTrip( std::vector<int>{ 1, 2, 3, 4, 5 } );
		testRoundTrip( std::vector<std::string>{ "a", "b", "c" } );
		testRoundTrip( std::vector<int>{} ); // Empty vector
		testRoundTrip( std::vector<bool>{ true, false, true } );
	}

	TEST_F( JSONSerializerTest, ArrayTypes )
	{
		testRoundTrip( std::array<int, 3>{ { 1, 2, 3 } } );
		testRoundTrip( std::array<std::string, 2>{ { "hello", "world" } } );
	}

	TEST_F( JSONSerializerTest, MapTypes )
	{
		testRoundTrip( std::map<std::string, int>{ { "one", 1 }, { "two", 2 }, { "three", 3 } } );
		testRoundTrip( std::map<std::string, std::string>{ { "key1", "value1" }, { "key2", "value2" } } );
		testRoundTrip( std::map<std::string, int>{} ); // Empty map
	}

	TEST_F( JSONSerializerTest, UnorderedMapTypes )
	{
		testRoundTrip( std::unordered_map<std::string, int>{ { "alpha", 1 }, { "beta", 2 } } );
		testRoundTrip( std::unordered_map<std::string, double>{ { "pi", 3.14 }, { "e", 2.71 } } );
	}

	TEST_F( JSONSerializerTest, SetTypes )
	{
		testRoundTrip( std::set<int>{ 1, 2, 3, 4, 5 } );
		testRoundTrip( std::set<std::string>{ "apple", "banana", "cherry" } );
		testRoundTrip( std::set<int>{} ); // Empty set
	}

	TEST_F( JSONSerializerTest, ListTypes )
	{
		testRoundTrip( std::list<int>{ 10, 20, 30 } );
		testRoundTrip( std::list<std::string>{ "first", "second", "third" } );
	}

	TEST_F( JSONSerializerTest, DequeTypes )
	{
		testRoundTrip( std::deque<int>{ 100, 200, 300 } );
		testRoundTrip( std::deque<double>{ 1.1, 2.2, 3.3 } );
	}

	TEST_F( JSONSerializerTest, UnorderedSetTypes )
	{
		testRoundTrip( std::unordered_set<int>{ 1, 2, 3, 4, 5 } );
		testRoundTrip( std::unordered_set<std::string>{ "apple", "banana", "cherry" } );
		testRoundTrip( std::unordered_set<int>{} ); // Empty unordered_set
	}

	//----------------------------------------------
	// nfx containers
	//----------------------------------------------

	TEST_F( JSONSerializerTest, NfxChdHashMapTypes )
	{
		using nfx::containers::ChdHashMap;

		// Test ChdHashMap<int> with string keys
		{
			std::vector<std::pair<std::string, int>> items{
				{ "one", 1 },
				{ "two", 2 },
				{ "three", 3 },
				{ "four", 4 },
				{ "five", 5 } };
			ChdHashMap<int> chdMap( std::move( items ) );
			testRoundTrip( chdMap );
		}

		// Test ChdHashMap<string> with string values
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "greeting", "hello" },
				{ "farewell", "goodbye" },
				{ "question", "how are you?" },
				{ "answer", "fine, thanks" } };
			ChdHashMap<std::string> chdMapString( std::move( items ) );
			testRoundTrip( chdMapString );
		}

		// Test ChdHashMap<double> with numeric values
		{
			std::vector<std::pair<std::string, double>> items{
				{ "pi", 3.14159265359 },
				{ "e", 2.71828182846 },
				{ "phi", 1.61803398875 },
				{ "sqrt2", 1.41421356237 } };
			ChdHashMap<double> chdMapDouble( std::move( items ) );
			testRoundTrip( chdMapDouble );
		}

		// Test ChdHashMap<bool> with boolean values
		{
			std::vector<std::pair<std::string, bool>> items{
				{ "enabled", true },
				{ "debug", false },
				{ "production", true },
				{ "testing", false } };
			ChdHashMap<bool> chdMapBool( std::move( items ) );
			testRoundTrip( chdMapBool );
		}

		// Test empty ChdHashMap
		{
			std::vector<std::pair<std::string, int>> emptyItems;
			ChdHashMap<int> emptyChdMap( std::move( emptyItems ) );
			testRoundTrip( emptyChdMap );
		}

		// Test ChdHashMap with complex values (vectors)
		{
			std::vector<std::pair<std::string, std::vector<int>>> items{
				{ "primes", { 2, 3, 5, 7, 11 } },
				{ "fibonacci", { 1, 1, 2, 3, 5, 8 } },
				{ "squares", { 1, 4, 9, 16, 25 } },
				{ "empty", {} } };
			ChdHashMap<std::vector<int>> chdMapVector( std::move( items ) );
			testRoundTrip( chdMapVector );
		}

		// Test ChdHashMap with nfx datatype values
		{
			using nfx::datatypes::Int128;
			std::vector<std::pair<std::string, Int128>> items{
				{ "small", Int128{ 42 } },
				{ "large", Int128{ 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL } },
				{ "negative", Int128{ -999999999 } },
				{ "zero", Int128{ 0 } } };
			ChdHashMap<Int128> chdMapInt128( std::move( items ) );
			testRoundTrip( chdMapInt128 );
		}

		// Test ChdHashMap with custom FNV parameters
		{
			using CustomChdHashMap = ChdHashMap<int, 0x12345678, 0x87654321>;
			std::vector<std::pair<std::string, int>> items{
				{ "custom1", 100 },
				{ "custom2", 200 },
				{ "custom3", 300 } };
			CustomChdHashMap customChdMap( std::move( items ) );
			testRoundTrip( customChdMap );
		}

		// Test using convenience functions with ChdHashMap
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "language", "C++" },
				{ "library", "nfx-core" },
				{ "feature", "JSON serialization" } };
			ChdHashMap<std::string> original( std::move( items ) );

			std::string jsonStr = Serializer<ChdHashMap<std::string>>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "JSON should not be empty for ChdHashMap";

			ChdHashMap<std::string> deserialized = Serializer<ChdHashMap<std::string>>::fromJson( jsonStr );

			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve all key-value pairs";
		}
	}

	TEST_F( JSONSerializerTest, NfxHashMapTypes )
	{
		using nfx::containers::HashMap;

		// Test HashMap<string, int>
		HashMap<std::string, int> hashMapInt;
		hashMapInt.insertOrAssign( "one", 1 );
		hashMapInt.insertOrAssign( "two", 2 );
		hashMapInt.insertOrAssign( "three", 3 );
		testRoundTrip( hashMapInt );

		// Test HashMap<string, string>
		HashMap<std::string, std::string> hashMapString;
		hashMapString.insertOrAssign( "hello", "world" );
		hashMapString.insertOrAssign( "foo", "bar" );
		testRoundTrip( hashMapString );

		// Test empty HashMap
		testRoundTrip( HashMap<std::string, int>() );
	}

	TEST_F( JSONSerializerTest, NfxStringMapTypes )
	{
		using nfx::containers::StringMap;

		// Test StringMap<int>
		StringMap<int> stringMapInt{
			{ "alpha", 10 },
			{ "beta", 20 },
			{ "gamma", 30 } };
		testRoundTrip( stringMapInt );

		// Test StringMap<double>
		StringMap<double> stringMapDouble{
			{ "pi", 3.14159 },
			{ "e", 2.71828 } };
		testRoundTrip( stringMapDouble );

		// Test empty StringMap
		testRoundTrip( StringMap<int>() );
	}

	TEST_F( JSONSerializerTest, NfxStringSetTypes )
	{
		using nfx::containers::StringSet;

		// Test StringSet with values
		StringSet stringSet{ "apple", "banana", "cherry", "date" };
		testRoundTrip( stringSet );

		// Test empty StringSet
		testRoundTrip( StringSet() );
	}

	//----------------------------------------------
	// Optional types
	//----------------------------------------------

	TEST_F( JSONSerializerTest, OptionalTypes )
	{
		// Optional with value
		testRoundTrip( std::optional<int>{ 42 } );
		testRoundTrip( std::optional<std::string>{ "Hello" } );
		testRoundTrip( std::optional<double>{ 3.14 } );

		// Optional without value
		testRoundTrip( std::optional<int>{} );
		testRoundTrip( std::optional<std::string>{} );
		testRoundTrip( std::optional<bool>{} );
	}

	//----------------------------------------------
	// Smart pointer types
	//----------------------------------------------

	TEST_F( JSONSerializerTest, SmartPointerTypes )
	{
		// Test unique pointers with values
		{
			auto original = std::make_unique<int>( 123 );
			Serializer<std::unique_ptr<int>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_TRUE( original && deserialized ) << "Both pointers should be non-null";
			EXPECT_EQ( *original, *deserialized ) << "Content should be equal";
		}

		{
			auto original = std::make_unique<std::string>( "Smart pointer test" );
			Serializer<std::unique_ptr<std::string>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_TRUE( original && deserialized ) << "Both pointers should be non-null";
			EXPECT_EQ( *original, *deserialized ) << "Content should be equal";
		}

		// Test shared pointers with values
		{
			auto original = std::make_shared<int>( 456 );
			Serializer<std::shared_ptr<int>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_TRUE( original && deserialized ) << "Both pointers should be non-null";
			EXPECT_EQ( *original, *deserialized ) << "Content should be equal";
		}

		{
			auto original = std::make_shared<double>( 2.718 );
			Serializer<std::shared_ptr<double>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_TRUE( original && deserialized ) << "Both pointers should be non-null";
			EXPECT_EQ( *original, *deserialized ) << "Content should be equal";
		}

		// Test null pointers - check they remain null
		{
			std::unique_ptr<int> original = nullptr;
			Serializer<std::unique_ptr<int>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_FALSE( original ) << "Original should be null";
			EXPECT_FALSE( deserialized ) << "Deserialized should be null";
		}

		{
			std::shared_ptr<std::string> original = nullptr;
			Serializer<std::shared_ptr<std::string>> serializer;
			std::string jsonStr = serializer.serializeToString( original );
			auto deserialized = serializer.deserializeFromString( jsonStr );

			EXPECT_FALSE( original ) << "Original should be null";
			EXPECT_FALSE( deserialized ) << "Deserialized should be null";
		}
	}

	//----------------------------------------------
	// Convenience functions
	//----------------------------------------------

	TEST_F( JSONSerializerTest, ConvenienceFunctions )
	{
		// Test with basic types
		{
			int original{ 42 };
			std::string jsonStr = Serializer<int>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			int deserialized = Serializer<int>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		{
			std::string original{ "Hello, World!" };
			std::string jsonStr = Serializer<std::string>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			std::string deserialized = Serializer<std::string>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		// Test with containers
		{
			std::vector<int> original{ 1, 2, 3, 4, 5 };
			std::string jsonStr = Serializer<std::vector<int>>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			std::vector<int> deserialized = Serializer<std::vector<int>>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		{
			std::map<std::string, int> original{ { "one", 1 }, { "two", 2 }, { "three", 3 } };
			std::string jsonStr = Serializer<std::map<std::string, int>>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			std::map<std::string, int> deserialized = Serializer<std::map<std::string, int>>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		// Test with nfx datatypes
		{
			using nfx::datatypes::Int128;
			Int128 original( 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL );
			std::string jsonStr = Serializer<Int128>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			Int128 deserialized = Serializer<Int128>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		{
			using nfx::datatypes::Decimal;
			Decimal original = Decimal::one();
			std::string jsonStr = Serializer<Decimal>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			Decimal deserialized = Serializer<Decimal>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		// Test with optional types
		{
			std::optional<int> original{ 123 };
			std::string jsonStr = Serializer<std::optional<int>>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			std::optional<int> deserialized = Serializer<std::optional<int>>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		{
			std::optional<std::string> original{}; // null optional
			std::string jsonStr = Serializer<std::optional<std::string>>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			std::optional<std::string> deserialized = Serializer<std::optional<std::string>>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Convenience functions must preserve value";
		}

		// Test with serialization options
		{
			std::map<std::string, std::optional<int>> original{
				{ "hasValue", 42 },
				{ "noValue", {} } };

			Serializer<std::map<std::string, std::optional<int>>>::Options options;
			options.includeNullFields = true;

			std::string jsonStr = Serializer<std::map<std::string, std::optional<int>>>::toJson( original, options );
			EXPECT_FALSE( jsonStr.empty() ) << "toJson should produce non-empty JSON";

			auto deserialized = Serializer<std::map<std::string, std::optional<int>>>::fromJson( jsonStr, options );
			EXPECT_EQ( original, deserialized ) << "Convenience functions with options must preserve value";
		}

		// Test with nfx datatypes
		{
			using nfx::datatypes::Int128;
			using nfx::datatypes::Decimal;

			// Test Int128 convenience functions
			Int128 originalInt128{ 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL };
			std::string jsonStrInt128 = Serializer<Int128>::toJson( originalInt128 );
			EXPECT_FALSE( jsonStrInt128.empty() ) << "toJson should produce non-empty JSON for Int128";

			Int128 deserializedInt128 = Serializer<Int128>::fromJson( jsonStrInt128 );
			EXPECT_EQ( originalInt128, deserializedInt128 ) << "Convenience functions must preserve Int128 value";

			// Test Decimal convenience functions
			Decimal originalDecimal = Decimal::one();
			std::string jsonStrDecimal = Serializer<Decimal>::toJson( originalDecimal );
			EXPECT_FALSE( jsonStrDecimal.empty() ) << "toJson should produce non-empty JSON for Decimal";

			Decimal deserializedDecimal = Serializer<Decimal>::fromJson( jsonStrDecimal );
			EXPECT_EQ( originalDecimal, deserializedDecimal ) << "Convenience functions must preserve Decimal value";
		}

		// Test with nfx time types
		{
			using nfx::time::TimeSpan;
			using nfx::time::DateTime;
			using nfx::time::DateTimeOffset;

			// Test TimeSpan convenience functions
			TimeSpan originalTimeSpan = TimeSpan::fromHours( 25.5 );
			std::string jsonStrTimeSpan = Serializer<TimeSpan>::toJson( originalTimeSpan );
			EXPECT_FALSE( jsonStrTimeSpan.empty() ) << "toJson should produce non-empty JSON for TimeSpan";

			TimeSpan deserializedTimeSpan = Serializer<TimeSpan>::fromJson( jsonStrTimeSpan );
			EXPECT_EQ( originalTimeSpan, deserializedTimeSpan ) << "Convenience functions must preserve TimeSpan value";

			// Test DateTime convenience functions
			DateTime originalDateTime = DateTime{ 2024, 12, 31, 23, 59, 59, 999 };
			std::string jsonStrDateTime = Serializer<DateTime>::toJson( originalDateTime );
			EXPECT_FALSE( jsonStrDateTime.empty() ) << "toJson should produce non-empty JSON for DateTime";

			DateTime deserializedDateTime = Serializer<DateTime>::fromJson( jsonStrDateTime );
			EXPECT_EQ( originalDateTime, deserializedDateTime ) << "Convenience functions must preserve DateTime value";

			// Test DateTimeOffset convenience functions
			DateTimeOffset originalDateTimeOffset = DateTimeOffset{ DateTime{ 2024, 6, 15, 18, 30, 45 }, TimeSpan::fromHours( -8 ) };
			std::string jsonStrDateTimeOffset = Serializer<DateTimeOffset>::toJson( originalDateTimeOffset );
			EXPECT_FALSE( jsonStrDateTimeOffset.empty() ) << "toJson should produce non-empty JSON for DateTimeOffset";

			DateTimeOffset deserializedDateTimeOffset = Serializer<DateTimeOffset>::fromJson( jsonStrDateTimeOffset );
			EXPECT_EQ( originalDateTimeOffset, deserializedDateTimeOffset ) << "Convenience functions must preserve DateTimeOffset value";
		}
	}

	//----------------------------------------------
	// Simple custom data structure for testing serialization
	//----------------------------------------------

	struct SimpleStruct
	{
		std::string name;
		int value;
		bool enabled;

		// Equality operator for testing
		bool operator==( const SimpleStruct& other ) const
		{
			return name == other.name && value == other.value && enabled == other.enabled;
		}

		// Custom serialization method
		void serialize( const Serializer<SimpleStruct>& serializer, Document& doc ) const
		{
			doc.set<std::string>( "/name", name );
			doc.set<int64_t>( "/value", value );
			doc.set<bool>( "/enabled", enabled );

			// Example of using serializer options - could add metadata if includeNullFields is set
			if ( serializer.options().includeNullFields && name.empty() )
			{
				// Could add some metadata indicating this was an empty string vs missing field
				doc.set<std::string>( "/metadata", "empty_name_field" );
			}
		} // Custom deserialization method

		void deserialize( const Serializer<SimpleStruct>& serializer, const Document& doc )
		{
			if ( auto nameVal = doc.get<std::string>( "/name" ) )
			{
				name = *nameVal;
			}
			if ( auto valueVal = doc.get<int64_t>( "/value" ) )
			{
				value = static_cast<int>( *valueVal );
			}
			if ( auto enabledVal = doc.get<bool>( "/enabled" ) )
			{
				enabled = *enabledVal;
			}

			// Use serializer options for validation if needed
			if ( serializer.options().validateOnDeserialize )
			{
				// Example validation: ensure value is within reasonable bounds
				if ( value < -1000000 || value > 1000000 )
				{
					throw std::runtime_error( "SimpleStruct value out of acceptable range" );
				}

				// Example: handle metadata that was added during serialization
				if ( auto metadataVal = doc.get<std::string>( "/metadata" ) )
				{
					// Could log or handle special metadata cases
					if ( *metadataVal == "empty_name_field" && !name.empty() )
					{
						// Inconsistency detected - metadata says empty but name is not empty
						throw std::runtime_error( "Metadata inconsistency detected ! " );
					}
				}
			}
		}
	};

	//----------------------------------------------
	// Nested container support tests
	//----------------------------------------------

	TEST_F( JSONSerializerTest, NestedContainerSupport )
	{
		// Test std::map<std::string, std::vector<int>> - the original failing case
		{
			std::map<std::string, std::vector<int>> nestedStructure{
				{ "numbers", { 1, 2, 3 } },
				{ "primes", { 2, 3, 5, 7 } },
				{ "empty", {} } };

			testRoundTrip( nestedStructure );
		}

		// Test std::vector<std::map<std::string, int>>
		{
			std::vector<std::map<std::string, int>> vectorOfMaps{
				{ { "a", 1 }, { "b", 2 } },
				{ { "x", 10 }, { "y", 20 }, { "z", 30 } },
				{} // empty map
			};

			testRoundTrip( vectorOfMaps );
		}

		// Test std::map<std::string, std::map<std::string, int>>
		{
			std::map<std::string, std::map<std::string, int>> mapOfMaps{
				{ "group1", { { "item1", 1 }, { "item2", 2 } } },
				{ "group2", { { "itemA", 10 }, { "itemB", 20 } } },
				{ "empty_group", {} } };

			testRoundTrip( mapOfMaps );
		}

		// Test std::vector<std::vector<int>>
		{
			std::vector<std::vector<int>> vectorOfVectors{
				{ 1, 2, 3 },
				{ 4, 5 },
				{}, // empty vector
				{ 6, 7, 8, 9 } };

			testRoundTrip( vectorOfVectors );
		}

		// Test mixed nested containers with strings
		{
			std::map<std::string, std::vector<std::string>> stringVectorMap{
				{ "fruits", { "apple", "banana", "cherry" } },
				{ "colors", { "red", "green", "blue" } },
				{ "empty", {} } };

			testRoundTrip( stringVectorMap );
		}

		// Test deeply nested structures
		{
			std::map<std::string, std::vector<std::map<std::string, int>>> deeplyNested{
				{ "level1", { { { "a", 1 }, { "b", 2 } },
								{ { "c", 3 }, { "d", 4 } } } },
				{ "level2", { { { "x", 10 } },
								{ { "y", 20 }, { "z", 30 } } } } };

			testRoundTrip( deeplyNested );
		}
	}

	//----------------------------------------------
	// Custom data structures
	//----------------------------------------------

	TEST_F( JSONSerializerTest, CustomDataStructures )
	{
		// Test SimpleStruct with custom serialization
		{
			SimpleStruct original;
			original.name = "Test Structure";
			original.value = 42;
			original.enabled = true;

			testRoundTrip( original );
		}

		// Test SimpleStruct with different values
		{
			SimpleStruct original;
			original.name = "Another Test";
			original.value = -100;
			original.enabled = false;

			testRoundTrip( original );
		}

		// Test SimpleStruct with empty name
		{
			SimpleStruct original;
			original.name = "";
			original.value = 0;
			original.enabled = true;

			testRoundTrip( original );
		}

		// Test using convenience functions with custom struct
		{
			SimpleStruct original;
			original.name = "Convenience Test";
			original.value = 999;
			original.enabled = true;

			std::string jsonStr = Serializer<SimpleStruct>::toJson( original );
			EXPECT_FALSE( jsonStr.empty() ) << "JSON should not be empty for custom struct";

			SimpleStruct deserialized = Serializer<SimpleStruct>::fromJson( jsonStr );
			EXPECT_EQ( original, deserialized ) << "Custom data structures must preserve all field values";
		}

		// Test validation exception triggering
		{
			SimpleStruct original;
			original.name = ""; // Empty name
			original.value = 42;
			original.enabled = true;

			Serializer<SimpleStruct>::Options options;
			options.includeNullFields = true;	  // This will add metadata
			options.validateOnDeserialize = true; // This will enable validation

			Serializer<SimpleStruct> serializer( options );
			std::string jsonStr = serializer.serializeToString( original );

			// Now manually modify the JSON to create the inconsistency that triggers the exception
			// Replace empty name with non-empty name to create metadata inconsistency
			size_t pos = jsonStr.find( "\"name\": \"\"" ); // Handle formatted JSON with spaces
			if ( pos != std::string::npos )
			{
				jsonStr.replace( pos, 11, "\"name\": \"NotEmpty\"" );

				// This should throw the "Metadata inconsistency detected ! exception
				EXPECT_THROW(
					{
						SimpleStruct deserialized = serializer.deserializeFromString( jsonStr );
					},
					std::runtime_error );
			}
		}
	}

	//----------------------------------------------
	// Edge cases and stress test
	//----------------------------------------------

	TEST_F( JSONSerializerTest, ComplexStructuresWithNfxTypes )
	{
		using nfx::containers::StringMap;
		using nfx::containers::StringSet;
		using nfx::containers::HashMap;
		using nfx::datatypes::Int128;
		using nfx::datatypes::Decimal;
		using nfx::time::DateTime;
		using nfx::time::TimeSpan;
		using nfx::time::DateTimeOffset;

		// Test 1: StringMap with nfx datatypes
		{
			StringMap<Int128> int128Map;
			int128Map["small"] = Int128{ 42 };
			int128Map["large"] = Int128{ 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL };
			int128Map["negative"] = Int128{ -999999999 };
			int128Map["zero"] = Int128{ 0 };

			testRoundTrip( int128Map );
		}

		// Test 2: StringMap with Decimal values
		{
			StringMap<Decimal> decimalMap;
			decimalMap["pi"] = Decimal::parse( "3.14159265358979323846" );
			decimalMap["e"] = Decimal::parse( "2.71828182845904523536" );
			decimalMap["zero"] = Decimal::zero();
			decimalMap["one"] = Decimal::one();
			decimalMap["large"] = Decimal::parse( "123456789012345678901234567.89" );
			decimalMap["small"] = Decimal::parse( "0.000000000000000000000001" );

			testRoundTrip( decimalMap );
		}

		// Test 3: StringMap with DateTime values
		{
			StringMap<DateTime> dateTimeMap;
			dateTimeMap["epoch"] = DateTime{ 1970, 1, 1 };
			dateTimeMap["y2k"] = DateTime{ 2000, 1, 1 };
			dateTimeMap["current"] = DateTime{ 2024, 10, 4, 15, 30, 45, 123 };
			dateTimeMap["future"] = DateTime{ 2099, 12, 31, 23, 59, 59, 999 };
			dateTimeMap["min"] = DateTime::minValue();
			dateTimeMap["max"] = DateTime::maxValue();

			testRoundTrip( dateTimeMap );
		}

		// Test 4: HashMap with complex key-value pairs
		{
			HashMap<std::string, std::vector<Decimal>> hashMapComplex;
			hashMapComplex.insertOrAssign( "financial_data", { Decimal::parse( "1234.56" ),
																 Decimal::parse( "-9876.43" ),
																 Decimal::parse( "0.000001" ) } );
			hashMapComplex.insertOrAssign( "empty_values", {} );
			hashMapComplex.insertOrAssign( "single_value", { Decimal::one() } );

			testRoundTrip( hashMapComplex );
		}

		// Test 5: StringSet with various strings
		{
			StringSet stringSet;
			stringSet.insert( "alpha" );
			stringSet.insert( "beta" );
			stringSet.insert( "gamma" );
			stringSet.insert( "" ); // empty string
			stringSet.insert( "unicode_test_αβγ" );
			stringSet.insert( "numbers_123_test" );

			testRoundTrip( stringSet );
		}

		// Test 6: TimeSpan and DateTimeOffset combinations
		{
			std::map<std::string, DateTimeOffset> offsetMap;
			offsetMap["utc"] = DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( 0 ) };
			offsetMap["pst"] = DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( -8 ) };
			offsetMap["jst"] = DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( 9 ) };
			offsetMap["half_hour"] = DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromMinutes( 330 ) }; // +5:30

			testRoundTrip( offsetMap );
		}
	}

	TEST_F( JSONSerializerTest, DeeplyNestedComplexStructures )
	{
		using nfx::containers::StringMap;
		using nfx::containers::HashMap;
		using nfx::datatypes::Int128;
		using nfx::datatypes::Decimal;
		using nfx::time::DateTime;

		// Test 1: Nested StringMaps with different value types
		{
			StringMap<StringMap<Int128>> nestedStringMaps;

			StringMap<Int128> innerMap1;
			innerMap1["value1"] = Int128{ 1000 };
			innerMap1["value2"] = Int128{ -2000 };

			StringMap<Int128> innerMap2;
			innerMap2["big_number"] = Int128{ 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL };
			innerMap2["small_number"] = Int128{ 1 };

			nestedStringMaps["group1"] = std::move( innerMap1 );
			nestedStringMaps["group2"] = std::move( innerMap2 );
			nestedStringMaps["empty_group"] = StringMap<Int128>{};

			testRoundTrip( nestedStringMaps );
		}

		// Test 2: Vector of StringMaps with Decimal values
		{
			std::vector<StringMap<Decimal>> vectorOfStringMaps;

			StringMap<Decimal> map1;
			map1["price"] = Decimal::parse( "99.99" );
			map1["tax"] = Decimal::parse( "8.25" );
			map1["total"] = Decimal::parse( "108.24" );

			StringMap<Decimal> map2;
			map2["balance"] = Decimal::parse( "1000000.00" );
			map2["interest"] = Decimal::parse( "0.035" );

			vectorOfStringMaps.push_back( std::move( map1 ) );
			vectorOfStringMaps.push_back( std::move( map2 ) );
			vectorOfStringMaps.push_back( StringMap<Decimal>{} ); // empty map

			testRoundTrip( vectorOfStringMaps );
		}

		// Test 3: HashMap with vector values containing DateTime objects
		{
			HashMap<std::string, std::vector<DateTime>> dateTimeVectorMap;

			std::vector<DateTime> importantDates{
				DateTime{ 2024, 1, 1 },	 // New Year
				DateTime{ 2024, 7, 4 },	 // Independence Day
				DateTime{ 2024, 12, 25 } // Christmas
			};

			std::vector<DateTime> meetingDates{
				DateTime{ 2024, 10, 1, 9, 0, 0 },
				DateTime{ 2024, 10, 8, 14, 30, 0 },
				DateTime{ 2024, 10, 15, 16, 45, 30 } };

			dateTimeVectorMap.insertOrAssign( "holidays", std::move( importantDates ) );
			dateTimeVectorMap.insertOrAssign( "meetings", std::move( meetingDates ) );
			dateTimeVectorMap.insertOrAssign( "empty_schedule", std::vector<DateTime>{} );

			testRoundTrip( dateTimeVectorMap );
		}

		// Test 4: Complex mixed container with nfx types
		{
			struct ComplexMixedData
			{
				StringMap<Int128> integerData;
				HashMap<std::string, std::vector<Decimal>> financialData;
				std::unordered_set<std::string> stringSet;
				std::map<std::string, DateTime> dateMap;
				std::vector<std::unordered_map<std::string, bool>> boolMaps;

				bool operator==( const ComplexMixedData& other ) const
				{
					return integerData == other.integerData &&
						   financialData == other.financialData &&
						   stringSet == other.stringSet &&
						   dateMap == other.dateMap &&
						   boolMaps == other.boolMaps;
				}

				// Custom serialization method - no parameters
				Document serialize() const
				{
					Document doc;

					// Serialize StringMap<Int128> integerData using instance serializer
					{
						Document intDoc;
						Serializer<StringMap<Int128>> intSerializer;
						intDoc = intSerializer.serialize( integerData );
						doc.set<Document>( "/integerData", intDoc );
					}

					// Serialize HashMap<string, vector<Decimal>> financialData using instance serializer
					{
						Document finDoc;
						Serializer<HashMap<std::string, std::vector<Decimal>>> finSerializer;
						finDoc = finSerializer.serialize( financialData );
						doc.set<Document>( "/financialData", finDoc );
					}

					// Serialize unordered_set<string> stringSet using instance serializer
					{
						Document strDoc;
						Serializer<std::unordered_set<std::string>> strSerializer;
						strDoc = strSerializer.serialize( stringSet );
						doc.set<Document>( "/stringSet", strDoc );
					}

					// Serialize map<string, DateTime> dateMap using instance serializer
					{
						Document dateDoc;
						Serializer<std::map<std::string, DateTime>> dateSerializer;
						dateDoc = dateSerializer.serialize( dateMap );
						doc.set<Document>( "/dateMap", dateDoc );
					}

					// Serialize vector<unordered_map<string, bool>> boolMaps using instance serializer
					{
						Document boolDoc;
						Serializer<std::vector<std::unordered_map<std::string, bool>>> boolSerializer;
						boolDoc = boolSerializer.serialize( boolMaps );
						doc.set<Document>( "/boolMaps", boolDoc );
					}

					return doc;
				}

				// Parameter-based serialization method
				void serialize( const Serializer<ComplexMixedData>&, Document& doc ) const
				{
					doc = serialize(); // Delegate to the no-parameter version
				}

				void deserialize( const Serializer<ComplexMixedData>&, const Document& doc )
				{
					// Deserialize StringMap<Int128> integerData using instance serializer
					if ( doc.is<Document::Object>( "/integerData" ) )
					{
						if ( auto intDocOpt = doc.get<Document>( "/integerData" ) )
						{
							Serializer<StringMap<Int128>> intSerializer;
							integerData = intSerializer.deserialize( *intDocOpt );
						}
					}

					// Deserialize HashMap<string, vector<Decimal>> financialData using instance serializer
					if ( doc.is<Document::Object>( "/financialData" ) )
					{
						if ( auto finDocOpt = doc.get<Document>( "/financialData" ) )
						{
							Serializer<HashMap<std::string, std::vector<Decimal>>> finSerializer;
							financialData = finSerializer.deserialize( *finDocOpt );
						}
					}

					// Deserialize unordered_set<string> stringSet using instance serializer
					if ( doc.is<Document::Array>( "/stringSet" ) )
					{
						if ( auto strDocOpt = doc.get<Document>( "/stringSet" ) )
						{
							Serializer<std::unordered_set<std::string>> strSerializer;
							stringSet = strSerializer.deserialize( *strDocOpt );
						}
					}

					// Deserialize map<string, DateTime> dateMap using instance serializer
					if ( doc.is<Document::Object>( "/dateMap" ) )
					{
						if ( auto dateDocOpt = doc.get<Document>( "/dateMap" ) )
						{
							Serializer<std::map<std::string, DateTime>> dateSerializer;
							dateMap = dateSerializer.deserialize( *dateDocOpt );
						}
					}

					// Deserialize vector<unordered_map<string, bool>> boolMaps using instance serializer
					if ( doc.is<Document::Array>( "/boolMaps" ) )
					{
						if ( auto boolDocOpt = doc.get<Document>( "/boolMaps" ) )
						{
							Serializer<std::vector<std::unordered_map<std::string, bool>>> boolSerializer;
							boolMaps = boolSerializer.deserialize( *boolDocOpt );
						}
					}
				}
			};

			ComplexMixedData complexData;

			// Fill with test data
			complexData.integerData["max_int128"] = Int128{ 0xFFFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL };
			complexData.integerData["min_int128"] = Int128{ 0x0000000000000000ULL, 0x8000000000000000ULL };
			complexData.integerData["zero"] = Int128{ 0 };

			std::vector<Decimal> prices{
				Decimal::parse( "19.99" ),
				Decimal::parse( "299.50" ),
				Decimal::parse( "1599.00" ) };
			complexData.financialData.insertOrAssign( "product_prices", std::move( prices ) );

			complexData.stringSet.insert( "test1" );
			complexData.stringSet.insert( "test2" );
			complexData.stringSet.insert( "αβγδε" ); // Unicode test

			complexData.dateMap["start"] = DateTime{ 2024, 1, 1 };
			complexData.dateMap["end"] = DateTime{ 2024, 12, 31 };

			std::unordered_map<std::string, bool> flags1{ { "enabled", true }, { "debug", false } };
			std::unordered_map<std::string, bool> flags2{ { "active", false }, { "visible", true } };
			complexData.boolMaps.push_back( std::move( flags1 ) );
			complexData.boolMaps.push_back( std::move( flags2 ) );

			testRoundTrip( complexData );
		}
	}

	TEST_F( JSONSerializerTest, ChdHashMapEdgeCasesAndStressTest )
	{
		using nfx::containers::ChdHashMap;
		using nfx::datatypes::Int128;
		using nfx::datatypes::Decimal;
		using nfx::time::DateTime;
		using nfx::time::TimeSpan;
		using nfx::time::DateTimeOffset;

		// Test 1: ChdHashMap with single character keys
		{
			std::vector<std::pair<std::string, int>> items{
				{ "a", 1 },
				{ "b", 2 },
				{ "c", 3 },
				{ "x", 24 },
				{ "y", 25 },
				{ "z", 26 } };
			ChdHashMap<int> singleCharMap( std::move( items ) );
			testRoundTrip( singleCharMap );
		}

		// Test 2: ChdHashMap with numeric string keys
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "0", "zero" },
				{ "1", "one" },
				{ "42", "answer" },
				{ "100", "century" },
				{ "1000", "thousand" },
				{ "999999", "almost_million" } };
			ChdHashMap<std::string> numericKeyMap( std::move( items ) );
			testRoundTrip( numericKeyMap );
		}

		// Test 3: ChdHashMap with special character keys
		{
			std::vector<std::pair<std::string, double>> items{
				{ "key-with-dashes", 1.1 },
				{ "key_with_underscores", 2.2 },
				{ "key.with.dots", 3.3 },
				{ "key with spaces", 4.4 },
				{ "key@with@symbols", 5.5 },
				{ "key123with456numbers", 6.6 } };
			ChdHashMap<double> specialCharMap( std::move( items ), 200 );
			testRoundTrip( specialCharMap );
		}

		// Test 4: ChdHashMap with unicode keys
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "αlpha", "greek_alpha" },
				{ "βeta", "greek_beta" },
				{ "γamma", "greek_gamma" },
				{ "你好", "hello_chinese" },
				{ "مرحبا", "hello_arabic" },
				{ "🌍", "earth_emoji" },
				{ "🚀", "rocket_emoji" } };
			ChdHashMap<std::string> unicodeKeyMap( std::move( items ), 200 );
			testRoundTrip( unicodeKeyMap );
		}

		// Test 5: ChdHashMap with very long keys
		{
			std::vector<std::pair<std::string, int>> items;
			std::string longKey1( 100, 'A' );
			std::string longKey2( 200, 'B' );
			std::string longKey3( 500, 'C' );

			items.emplace_back( longKey1, 100 );
			items.emplace_back( longKey2, 200 );
			items.emplace_back( longKey3, 500 );
			items.emplace_back( "short", 1 );

			ChdHashMap<int> longKeyMap( std::move( items ), 200 );
			testRoundTrip( longKeyMap );
		}

		// Test 6: ChdHashMap with keys that might cause hash collisions
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "abc", "value1" },
				{ "acb", "value2" },
				{ "bac", "value3" },
				{ "bca", "value4" },
				{ "cab", "value5" },
				{ "cba", "value6" } };
			ChdHashMap<std::string> collisionTestMap( std::move( items ), 200 );
			testRoundTrip( collisionTestMap );
		}

		// Test 7: ChdHashMap with nested complex values (vectors of maps)
		{
			using NestedValue = std::vector<std::map<std::string, int>>;
			std::vector<std::pair<std::string, NestedValue>> items;

			NestedValue value1{
				{ { "a", 1 }, { "b", 2 } },
				{ { "x", 10 }, { "y", 20 } } };

			NestedValue value2{
				{ { "alpha", 100 }, { "beta", 200 } },
				{}, // empty map
				{ { "gamma", 300 } } };

			items.emplace_back( "complex1", std::move( value1 ) );
			items.emplace_back( "complex2", std::move( value2 ) );
			items.emplace_back( "empty", NestedValue{} );

			ChdHashMap<NestedValue> nestedComplexMap( std::move( items ) );
			testRoundTrip( nestedComplexMap );
		}

		// Test 8: ChdHashMap with nfx datatype values in one map
		{
			// Int128 values
			std::vector<std::pair<std::string, Int128>> int128Items{
				{ "small_positive", Int128{ 42 } },
				{ "small_negative", Int128{ -42 } },
				{ "zero", Int128{ 0 } },
				{ "large_positive", Int128{ 0x7FFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL } },
				{ "large_negative", Int128{ 0x8000000000000000ULL, 0x0000000000000000ULL } } };
			ChdHashMap<Int128> int128Map( std::move( int128Items ) );
			testRoundTrip( int128Map );

			// Decimal values
			std::vector<std::pair<std::string, Decimal>> decimalItems{
				{ "zero", Decimal::zero() },
				{ "one", Decimal::one() },
				{ "pi_approx", Decimal::parse( "3.14159265358979323846" ) },
				{ "e_approx", Decimal::parse( "2.71828182845904523536" ) },
				{ "very_small", Decimal::parse( "0.000000000000000000001" ) },
				{ "very_large", Decimal::parse( "999999999999999999999.999" ) } };
			ChdHashMap<Decimal> decimalMap( std::move( decimalItems ) );
			testRoundTrip( decimalMap );
		}

		// Test 9: ChdHashMap with DateTime edge cases
		{
			std::vector<std::pair<std::string, DateTime>> items{
				{ "unix_epoch", DateTime::epoch() },
				{ "min_value", DateTime::minValue() },
				{ "max_value", DateTime::maxValue() },
				{ "y2k", DateTime{ 2000, 1, 1, 0, 0, 0, 0 } },
				{ "leap_year", DateTime{ 2024, 2, 29, 12, 0, 0, 0 } },
				{ "end_of_year", DateTime{ 2024, 12, 31, 23, 59, 59, 999 } } };
			ChdHashMap<DateTime> dateTimeMap( std::move( items ) );
			testRoundTrip( dateTimeMap );
		}

		// Test 10: ChdHashMap with DateTimeOffset timezone edge cases
		{
			std::vector<std::pair<std::string, DateTimeOffset>> items{
				{ "utc", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( 0 ) } },
				{ "max_positive_offset", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( 14 ) } },
				{ "max_negative_offset", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromHours( -12 ) } },
				{ "half_hour_offset", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromMinutes( 330 ) } },	  // +5:30
				{ "quarter_hour_offset", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromMinutes( 315 ) } }, // +5:15
				{ "unusual_offset", DateTimeOffset{ DateTime{ 2024, 6, 15, 12, 0, 0 }, TimeSpan::fromMinutes( 345 ) } } };	  // +5:45
			ChdHashMap<DateTimeOffset> offsetMap( std::move( items ) );
			testRoundTrip( offsetMap );
		}

		// Test 11: Large ChdHashMap stress test with configurable seed search multiplier
		{
			std::vector<std::pair<std::string, int>> items;
			items.reserve( 250 );

			// Generate 250 unique keys with predictable values (reasonable size for CHD algorithm)
			for ( int i = 0; i < 250; ++i )
			{
				std::string key = "stress_key_" + std::to_string( i ) + "_suffix_" + std::to_string( i * 7 );
				items.emplace_back( std::move( key ), i * i );
			}

			using StressChdHashMap = ChdHashMap<int, core::hashing::DEFAULT_FNV_OFFSET_BASIS, core::hashing::DEFAULT_FNV_PRIME>;
			StressChdHashMap largeMap( std::move( items ), 500 );
			testRoundTrip( largeMap );
		}

		// Test 12: ChdHashMap with custom FNV parameters and constructor seed search multipliers
		{
			using CustomChdHashMap1 = ChdHashMap<std::string, 0x00000000, 0x00000001>; // Minimal FNV values
			using CustomChdHashMap2 = ChdHashMap<std::string, 0xFFFFFFFF, 0xFFFFFFFF>; // Maximum FNV values
			using CustomChdHashMap3 = ChdHashMap<std::string, 0x12345678, 0x87654321>; // Mixed FNV values

			std::vector<std::pair<std::string, std::string>> testItems{
				{ "test1", "value1" },
				{ "test2", "value2" },
				{ "test3", "value3" } };

			// Test with different FNV parameters and varying seed search multipliers
			{
				auto items1 = testItems;
				CustomChdHashMap1 customMap1( std::move( items1 ), 50 );
				testRoundTrip( customMap1 );
			}

			{
				auto items2 = testItems;
				CustomChdHashMap2 customMap2( std::move( items2 ), 50 );
				testRoundTrip( customMap2 );
			}

			{
				auto items3 = testItems;
				CustomChdHashMap3 customMap3( std::move( items3 ), 50 );
				testRoundTrip( customMap3 );
			}
		}

		// Test 13: ChdHashMap with empty string keys (edge case)
		{
			std::vector<std::pair<std::string, std::string>> items{
				{ "", "empty_key" },
				{ " ", "single_space" },
				{ "  ", "double_space" },
				{ "\t", "tab_key" },
				{ "\n", "newline_key" },
				{ "normal", "normal_value" } };
			ChdHashMap<std::string> emptyKeyMap( std::move( items ), 50 );
			testRoundTrip( emptyKeyMap );
		}

		// Test 14: ChdHashMap serialization with different serialization options
		{
			std::vector<std::pair<std::string, std::optional<std::string>>> items{
				{ "has_value", std::optional<std::string>{ "present" } },
				{ "no_value", std::optional<std::string>{} },
				{ "empty_string", std::optional<std::string>{ "" } } };
			ChdHashMap<std::optional<std::string>> optionalMap( std::move( items ) );

			// Test with different serialization options using testRoundTrip helper
			Serializer<ChdHashMap<std::optional<std::string>>>::Options options1;
			options1.includeNullFields = true;
			options1.prettyPrint = true;
			testRoundTrip( optionalMap, options1 );

			Serializer<ChdHashMap<std::optional<std::string>>>::Options options2;
			options2.includeNullFields = false;
			options2.validateOnDeserialize = true;
			testRoundTrip( optionalMap, options2 );
		}
	}

	TEST_F( JSONSerializerTest, StressTestLargeDataStructures )
	{
		using nfx::containers::StringMap;
		using nfx::containers::HashMap;
		using nfx::datatypes::Decimal;
		using nfx::time::DateTime;

		// Test 1: Large StringMap with many entries
		{
			StringMap<int> largeStringMap;
			for ( int i = 0; i < 1000; ++i )
			{
				std::string key = "key_" + std::to_string( i );
				largeStringMap[key] = i * i; // Square values
			}

			testRoundTrip( largeStringMap );
		}

		// Test 2: Large vector of complex objects
		{
			std::vector<std::map<std::string, Decimal>> largeVector;
			for ( int i = 0; i < 500; ++i )
			{
				std::map<std::string, Decimal> entry;
				entry["id"] = Decimal{ i };
				entry["value"] = Decimal::parse( std::to_string( i * 0.01 ) );
				entry["squared"] = Decimal{ i * i };
				largeVector.push_back( std::move( entry ) );
			}

			testRoundTrip( largeVector );
		}

		// Test 3: Deep nesting stress test
		{
			// Create a 5-level deep nested structure
			using Level5 = std::map<std::string, int>;
			using Level4 = std::map<std::string, Level5>;
			using Level3 = std::map<std::string, Level4>;
			using Level2 = std::map<std::string, Level3>;
			using Level1 = std::map<std::string, Level2>;

			Level1 deepNested;

			for ( int i = 0; i < 3; ++i )
			{
				std::string key1 = "level1_" + std::to_string( i );
				for ( int j = 0; j < 3; ++j )
				{
					std::string key2 = "level2_" + std::to_string( j );
					for ( int k = 0; k < 2; ++k )
					{
						std::string key3 = "level3_" + std::to_string( k );
						for ( int l = 0; l < 2; ++l )
						{
							std::string key4 = "level4_" + std::to_string( l );
							for ( int m = 0; m < 2; ++m )
							{
								std::string key5 = "level5_" + std::to_string( m );
								deepNested[key1][key2][key3][key4][key5] = i + j + k + l + m;
							}
						}
					}
				}
			}

			testRoundTrip( deepNested );
		}

		// Test 4: Mixed container types with large datasets
		{
			struct LargeDataset
			{
				HashMap<std::string, std::vector<DateTime>> dateCollections;
				std::unordered_set<std::string> uniqueStrings;
				StringMap<std::unordered_map<std::string, double>> nestedMaps;

				bool operator==( const LargeDataset& other ) const
				{
					return dateCollections == other.dateCollections &&
						   uniqueStrings == other.uniqueStrings &&
						   nestedMaps == other.nestedMaps;
				}

				// Custom serialization method - no parameters
				Document serialize() const
				{
					Document doc;

					// Serialize HashMap<string, vector<DateTime>> dateCollections using instance serializer
					{
						Document dateDoc;
						Serializer<HashMap<std::string, std::vector<DateTime>>> dateSerializer;
						dateDoc = dateSerializer.serialize( dateCollections );
						doc.set<Document>( "/dateCollections", dateDoc );
					}

					// Serialize unordered_set<string> uniqueStrings using instance serializer
					{
						Document stringDoc;
						Serializer<std::unordered_set<std::string>> stringSerializer;
						stringDoc = stringSerializer.serialize( uniqueStrings );
						doc.set<Document>( "/uniqueStrings", stringDoc );
					}

					// Serialize StringMap<unordered_map<string, double>> nestedMaps using instance serializer
					{
						Document mapDoc;
						Serializer<StringMap<std::unordered_map<std::string, double>>> mapSerializer;
						mapDoc = mapSerializer.serialize( nestedMaps );
						doc.set<Document>( "/nestedMaps", mapDoc );
					}

					return doc;
				}

				// Parameter-based serialization method
				void serialize( const Serializer<LargeDataset>&, Document& doc ) const
				{
					doc = serialize(); // Delegate to the no-parameter version
				}

				void deserialize( const Serializer<LargeDataset>&, const Document& doc )
				{
					// Deserialize HashMap<string, vector<DateTime>> dateCollections using instance serializer
					if ( doc.is<Document::Object>( "/dateCollections" ) )
					{
						if ( auto dateDocOpt = doc.get<Document>( "/dateCollections" ) )
						{
							Serializer<HashMap<std::string, std::vector<DateTime>>> dateSerializer;
							dateCollections = dateSerializer.deserialize( *dateDocOpt );
						}
					}

					// Deserialize unordered_set<string> uniqueStrings using instance serializer
					if ( doc.is<Document::Array>( "/uniqueStrings" ) )
					{
						if ( auto stringDocOpt = doc.get<Document>( "/uniqueStrings" ) )
						{
							Serializer<std::unordered_set<std::string>> stringSerializer;
							uniqueStrings = stringSerializer.deserialize( *stringDocOpt );
						}
					}

					// Deserialize StringMap<unordered_map<string, double>> nestedMaps using instance serializer
					if ( doc.is<Document::Object>( "/nestedMaps" ) )
					{
						if ( auto mapDocOpt = doc.get<Document>( "/nestedMaps" ) )
						{
							Serializer<StringMap<std::unordered_map<std::string, double>>> mapSerializer;
							nestedMaps = mapSerializer.deserialize( *mapDocOpt );
						}
					}
				}
			};

			LargeDataset dataset;

			// Create multiple date collections
			for ( int collection = 0; collection < 10; ++collection )
			{
				std::string collectionName = "collection_" + std::to_string( collection );
				std::vector<DateTime> dates;

				for ( int day = 1; day <= 30; ++day )
				{
					dates.emplace_back( 2024, collection % 12 + 1, day );
				}

				dataset.dateCollections.insertOrAssign( collectionName, std::move( dates ) );
			}

			// Create large set of unique strings
			for ( int i = 0; i < 200; ++i )
			{
				dataset.uniqueStrings.insert( "unique_string_" + std::to_string( i ) );
			}

			// Create nested maps with numerical data
			for ( int outer = 0; outer < 5; ++outer )
			{
				std::string outerKey = "outer_" + std::to_string( outer );
				std::unordered_map<std::string, double> innerMap;

				for ( int inner = 0; inner < 10; ++inner )
				{
					std::string innerKey = "inner_" + std::to_string( inner );
					innerMap[innerKey] = outer * 10.0 + inner * 0.1;
				}

				dataset.nestedMaps[outerKey] = std::move( innerMap );
			}

			testRoundTrip( dataset );
		}
	}
} // namespace nfx::serialization::json::test
