/**
 * @file TESTS_HashMap.cpp
 * @brief Unit tests for HashMap Robin Hood hashing container
 * @details Comprehensive test suite validating Robin Hood hashing algorithm,
 *          heterogeneous lookup operations, and cache-optimized performance
 */

#include <gtest/gtest.h>

#include <nfx/containers/HashMap.h>

namespace nfx::containers::test
{
	//=====================================================================
	// HashMap Tests - Robin Hood Hashing
	//=====================================================================

	//----------------------------------------------
	// Basic construction and operations
	//----------------------------------------------

	TEST( HashMapBasic, DefaultConstruction )
	{
		HashMap<std::string, int> map;

		EXPECT_TRUE( map.isEmpty() );
		EXPECT_EQ( map.size(), 0 );
		EXPECT_GE( map.capacity(), 32 ); // INITIAL_CAPACITY
	}

	TEST( HashMapBasic, CapacityConstruction )
	{
		HashMap<std::string, int> map( 64 );

		EXPECT_TRUE( map.isEmpty() );
		EXPECT_EQ( map.size(), 0 );
		EXPECT_GE( map.capacity(), 64 );
	}

	TEST( HashMapBasic, BasicInsertionAndLookup )
	{
		HashMap<std::string, int> map;

		map.insertOrAssign( "key1", 100 );
		map.insertOrAssign( "key2", 200 );
		map.insertOrAssign( "key3", 300 );

		EXPECT_EQ( map.size(), 3 );
		EXPECT_FALSE( map.isEmpty() );

		// Test tryGetValue
		int* value1 = nullptr;
		int* value2 = nullptr;
		int* value3 = nullptr;
		int* valueMissing = nullptr;

		EXPECT_TRUE( map.tryGetValue( "key1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "key2", value2 ) );
		EXPECT_TRUE( map.tryGetValue( "key3", value3 ) );
		EXPECT_FALSE( map.tryGetValue( "missing", valueMissing ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );
		EXPECT_EQ( valueMissing, nullptr );

		EXPECT_EQ( *value1, 100 );
		EXPECT_EQ( *value2, 200 );
		EXPECT_EQ( *value3, 300 );
	}

	TEST( HashMapBasic, InsertOrAssignUpdate )
	{
		HashMap<std::string, std::string> map;

		// Initial insertion
		map.insertOrAssign( "update_key", "initial_value" );
		std::string* value1 = nullptr;
		EXPECT_TRUE( map.tryGetValue( "update_key", value1 ) );
		ASSERT_NE( value1, nullptr );
		EXPECT_EQ( *value1, "initial_value" );
		EXPECT_EQ( map.size(), 1 );

		// Update existing key
		map.insertOrAssign( "update_key", "updated_value" );
		std::string* value2 = nullptr;
		EXPECT_TRUE( map.tryGetValue( "update_key", value2 ) );
		ASSERT_NE( value2, nullptr );
		EXPECT_EQ( *value2, "updated_value" );
		EXPECT_EQ( map.size(), 1 ); // Size should remain the same
	}

	//----------------------------------------------
	// Heterogeneous lookup operations
	//----------------------------------------------

	TEST( HashMapHeterogeneousLookup, StringTypes )
	{
		HashMap<std::string, int> map;

		map.insertOrAssign( "lookup_test", 42 );

		std::string strKey{ "lookup_test" };
		std::string_view svKey{ strKey };
		const char* cstrKey{ strKey.c_str() };

		// All lookup methods should work with different string types
		int* value1 = nullptr;
		int* value2 = nullptr;
		int* value3 = nullptr;

		EXPECT_TRUE( map.tryGetValue( strKey, value1 ) );
		EXPECT_TRUE( map.tryGetValue( svKey, value2 ) );
		EXPECT_TRUE( map.tryGetValue( cstrKey, value3 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );

		EXPECT_EQ( *value1, 42 );
		EXPECT_EQ( *value2, 42 );
		EXPECT_EQ( *value3, 42 );
	}

	TEST( HashMapHeterogeneousLookup, ZeroCopyOperations )
	{
		HashMap<std::string, std::string> map;

		map.insertOrAssign( "zero_copy_key", "zero_copy_value" );

		// These operations should not create temporary std::string objects
		std::string_view svKey{ "zero_copy_key" };
		const char* cstrKey{ "zero_copy_key" };

		std::string* result1 = nullptr;
		std::string* result2 = nullptr;

		EXPECT_TRUE( map.tryGetValue( svKey, result1 ) );
		EXPECT_TRUE( map.tryGetValue( cstrKey, result2 ) );

		ASSERT_NE( result1, nullptr );
		ASSERT_NE( result2, nullptr );
		EXPECT_EQ( *result1, "zero_copy_value" );
		EXPECT_EQ( *result2, "zero_copy_value" );
	}

	//----------------------------------------------
	// Erase operations
	//----------------------------------------------

	TEST( HashMapErase, BasicErase )
	{
		HashMap<std::string, int> map;

		map.insertOrAssign( "erase1", 1 );
		map.insertOrAssign( "erase2", 2 );
		map.insertOrAssign( "erase3", 3 );

		EXPECT_EQ( map.size(), 3 );

		// Erase existing key
		bool erased1 = map.erase( "erase2" );
		EXPECT_TRUE( erased1 );
		EXPECT_EQ( map.size(), 2 );

		// Verify key is gone
		int* value = nullptr;
		EXPECT_FALSE( map.tryGetValue( "erase2", value ) );
		EXPECT_EQ( value, nullptr );

		// Other keys should still exist
		int* value1 = nullptr;
		int* value3 = nullptr;
		EXPECT_TRUE( map.tryGetValue( "erase1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "erase3", value3 ) );
		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value3, nullptr );
		EXPECT_EQ( *value1, 1 );
		EXPECT_EQ( *value3, 3 );
	}

	TEST( HashMapErase, EraseNonExistent )
	{
		HashMap<std::string, int> map;

		map.insertOrAssign( "existing", 100 );

		// Try to erase non-existent key
		bool erased = map.erase( "non_existent" );
		EXPECT_FALSE( erased );
		EXPECT_EQ( map.size(), 1 );

		// Existing key should still be there
		int* value = nullptr;
		EXPECT_TRUE( map.tryGetValue( "existing", value ) );
		ASSERT_NE( value, nullptr );
		EXPECT_EQ( *value, 100 );
	}

	TEST( HashMapErase, HeterogeneousErase )
	{
		HashMap<std::string, int> map;

		map.insertOrAssign( "hetero_erase", 999 );

		std::string_view svKey{ "hetero_erase" };
		const char* cstrKey{ "hetero_erase" };

		// Erase with different key types
		bool erased1 = map.erase( svKey );
		EXPECT_TRUE( erased1 );
		EXPECT_EQ( map.size(), 0 );

		// Re-insert and erase with const char*
		map.insertOrAssign( "hetero_erase", 999 );
		bool erased2 = map.erase( cstrKey );
		EXPECT_TRUE( erased2 );
		EXPECT_EQ( map.size(), 0 );
	}

	//----------------------------------------------
	// Capacity and memory management
	//----------------------------------------------

	TEST( HashMapCapacity, ReserveCapacity )
	{
		HashMap<std::string, int> map;

		size_t initialCapacity = map.capacity();

		map.reserve( 128 );
		EXPECT_GE( map.capacity(), 128 );
		EXPECT_GE( map.capacity(), initialCapacity );
		EXPECT_EQ( map.size(), 0 ); // Size should remain 0
	}

	TEST( HashMapCapacity, AutomaticResize )
	{
		HashMap<std::string, int> map;

		size_t initialCapacity = map.capacity();

		// Insert enough items to trigger resize (75% load factor)
		size_t itemsToInsert = ( initialCapacity * 75 ) / 100 + 5;

		for ( size_t i = 0; i < itemsToInsert; ++i )
		{
			map.insertOrAssign( "key_" + std::to_string( i ), static_cast<int>( i ) );
		}

		EXPECT_GT( map.capacity(), initialCapacity );
		EXPECT_EQ( map.size(), itemsToInsert );

		// Verify all items are still accessible
		for ( size_t i = 0; i < itemsToInsert; ++i )
		{
			int* value = nullptr;
			EXPECT_TRUE( map.tryGetValue( "key_" + std::to_string( i ), value ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, static_cast<int>( i ) );
		}
	}

	//----------------------------------------------
	// Robin Hood algorithm specific tests
	//----------------------------------------------

	TEST( HashMapRobinHood, CollisionHandling )
	{
		HashMap<std::string, int> map;

		// Insert items that might cause hash collisions
		map.insertOrAssign( "collision1", 1 );
		map.insertOrAssign( "collision2", 2 );
		map.insertOrAssign( "collision3", 3 );
		map.insertOrAssign( "collision4", 4 );
		map.insertOrAssign( "collision5", 5 );

		EXPECT_EQ( map.size(), 5 );

		// All items should be retrievable
		int* value1 = nullptr;
		int* value2 = nullptr;
		int* value3 = nullptr;
		int* value4 = nullptr;
		int* value5 = nullptr;

		EXPECT_TRUE( map.tryGetValue( "collision1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "collision2", value2 ) );
		EXPECT_TRUE( map.tryGetValue( "collision3", value3 ) );
		EXPECT_TRUE( map.tryGetValue( "collision4", value4 ) );
		EXPECT_TRUE( map.tryGetValue( "collision5", value5 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );
		ASSERT_NE( value4, nullptr );
		ASSERT_NE( value5, nullptr );

		EXPECT_EQ( *value1, 1 );
		EXPECT_EQ( *value2, 2 );
		EXPECT_EQ( *value3, 3 );
		EXPECT_EQ( *value4, 4 );
		EXPECT_EQ( *value5, 5 );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( HashMapEdgeCases, EmptyStringKey )
	{
		HashMap<std::string, std::string> map;

		// Empty string as key
		map.insertOrAssign( "", "empty_key_value" );

		std::string* value = nullptr;
		EXPECT_TRUE( map.tryGetValue( "", value ) );
		ASSERT_NE( value, nullptr );
		EXPECT_EQ( *value, "empty_key_value" );

		// Test with empty string_view
		std::string_view emptySv{};
		std::string* value2 = nullptr;
		EXPECT_TRUE( map.tryGetValue( emptySv, value2 ) );
		ASSERT_NE( value2, nullptr );
		EXPECT_EQ( *value2, "empty_key_value" );
	}

	TEST( HashMapEdgeCases, SpecialCharacterKeys )
	{
		HashMap<std::string, int> map;

		// Keys with special characters
		map.insertOrAssign( "key with spaces", 1 );
		map.insertOrAssign( "key\twith\ttabs", 2 );
		map.insertOrAssign( "key\nwith\nnewlines", 3 );
		map.insertOrAssign( "key\"with\"quotes", 4 );
		map.insertOrAssign( "key\\with\\backslashes", 5 );

		EXPECT_EQ( map.size(), 5 );

		int* value1 = nullptr;
		int* value2 = nullptr;
		int* value3 = nullptr;
		int* value4 = nullptr;
		int* value5 = nullptr;

		EXPECT_TRUE( map.tryGetValue( "key with spaces", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "key\twith\ttabs", value2 ) );
		EXPECT_TRUE( map.tryGetValue( "key\nwith\nnewlines", value3 ) );
		EXPECT_TRUE( map.tryGetValue( "key\"with\"quotes", value4 ) );
		EXPECT_TRUE( map.tryGetValue( "key\\with\\backslashes", value5 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );
		ASSERT_NE( value4, nullptr );
		ASSERT_NE( value5, nullptr );

		EXPECT_EQ( *value1, 1 );
		EXPECT_EQ( *value2, 2 );
		EXPECT_EQ( *value3, 3 );
		EXPECT_EQ( *value4, 4 );
		EXPECT_EQ( *value5, 5 );
	}

	TEST( HashMapEdgeCases, UnicodeKeys )
	{
		HashMap<std::string, std::string> map;

		// Unicode keys
		map.insertOrAssign( "🔑", "key_emoji" );
		map.insertOrAssign( "clé", "french_key" );
		map.insertOrAssign( "键", "chinese_key" );
		map.insertOrAssign( "ключ", "russian_key" );

		EXPECT_EQ( map.size(), 4 );

		std::string* value1 = nullptr;
		std::string* value2 = nullptr;
		std::string* value3 = nullptr;
		std::string* value4 = nullptr;

		EXPECT_TRUE( map.tryGetValue( "🔑", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "clé", value2 ) );
		EXPECT_TRUE( map.tryGetValue( "键", value3 ) );
		EXPECT_TRUE( map.tryGetValue( "ключ", value4 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value3, nullptr );
		ASSERT_NE( value4, nullptr );

		EXPECT_EQ( *value1, "key_emoji" );
		EXPECT_EQ( *value2, "french_key" );
		EXPECT_EQ( *value3, "chinese_key" );
		EXPECT_EQ( *value4, "russian_key" );
	}

	//----------------------------------------------
	// Performance and stress tests
	//----------------------------------------------

	TEST( HashMapPerformance, LargeDataHandling )
	{
		HashMap<std::string, size_t> map;

		// Insert many items
		constexpr size_t numItems = 1000;
		for ( size_t i = 0; i < numItems; ++i )
		{
			std::string key{ "perf_key_" + std::to_string( i ) };
			map.insertOrAssign( key, i );
		}

		EXPECT_EQ( map.size(), numItems );

		// Verify all items are accessible
		for ( size_t i = 0; i < numItems; ++i )
		{
			std::string key{ "perf_key_" + std::to_string( i ) };
			size_t* value = nullptr;
			EXPECT_TRUE( map.tryGetValue( key, value ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );

			// Also test with string_view (zero-copy)
			std::string_view svKey{ key };
			size_t* value2 = nullptr;
			EXPECT_TRUE( map.tryGetValue( svKey, value2 ) );
			ASSERT_NE( value2, nullptr );
			EXPECT_EQ( *value2, i );
		}
	}

	TEST( HashMapPerformance, InsertErasePattern )
	{
		HashMap<std::string, int> map;

		// Pattern: Insert, then erase half, then insert more
		for ( int i = 0; i < 100; ++i )
		{
			map.insertOrAssign( "pattern_" + std::to_string( i ), i );
		}
		EXPECT_EQ( map.size(), 100 );

		// Erase every other item
		for ( int i = 0; i < 100; i += 2 )
		{
			bool erased = map.erase( "pattern_" + std::to_string( i ) );
			EXPECT_TRUE( erased );
		}
		EXPECT_EQ( map.size(), 50 );

		// Insert new items
		for ( int i = 100; i < 150; ++i )
		{
			map.insertOrAssign( "pattern_" + std::to_string( i ), i );
		}
		EXPECT_EQ( map.size(), 100 );

		// Verify remaining items
		for ( int i = 1; i < 100; i += 2 ) // Odd numbers from original
		{
			int* value = nullptr;
			EXPECT_TRUE( map.tryGetValue( "pattern_" + std::to_string( i ), value ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );
		}

		for ( int i = 100; i < 150; ++i ) // New items
		{
			int* value = nullptr;
			EXPECT_TRUE( map.tryGetValue( "pattern_" + std::to_string( i ), value ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );
		}
	}

	//----------------------------------------------
	// Value type tests
	//----------------------------------------------

	TEST( HashMapValueTypes, ComplexValues )
	{
		HashMap<std::string, std::vector<int>> map;

		map.insertOrAssign( "vector1", std::vector<int>{ 1, 2, 3 } );
		map.insertOrAssign( "vector2", std::vector<int>{ 4, 5, 6, 7 } );

		std::vector<int>* value1 = nullptr;
		std::vector<int>* value2 = nullptr;
		EXPECT_TRUE( map.tryGetValue( "vector1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "vector2", value2 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );

		EXPECT_EQ( value1->size(), 3 );
		EXPECT_EQ( value2->size(), 4 );
		EXPECT_EQ( ( *value1 )[0], 1 );
		EXPECT_EQ( ( *value2 )[3], 7 );
	}

	TEST( HashMapValueTypes, MoveSemantics )
	{
		HashMap<std::string, std::unique_ptr<int>> map;

		map.insertOrAssign( "unique1", std::make_unique<int>( 42 ) );
		map.insertOrAssign( "unique2", std::make_unique<int>( 84 ) );

		std::unique_ptr<int>* value1 = nullptr;
		std::unique_ptr<int>* value2 = nullptr;
		EXPECT_TRUE( map.tryGetValue( "unique1", value1 ) );
		EXPECT_TRUE( map.tryGetValue( "unique2", value2 ) );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value1->get(), nullptr );
		ASSERT_NE( value2->get(), nullptr );

		EXPECT_EQ( **value1, 42 );
		EXPECT_EQ( **value2, 84 );
	}
}
