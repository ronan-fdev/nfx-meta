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
		auto* value1 = map.tryGetValue( "key1" );
		auto* value2 = map.tryGetValue( "key2" );
		auto* value3 = map.tryGetValue( "key3" );
		auto* valueMissing = map.tryGetValue( "missing" );

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
		auto* value1 = map.tryGetValue( "update_key" );
		ASSERT_NE( value1, nullptr );
		EXPECT_EQ( *value1, "initial_value" );
		EXPECT_EQ( map.size(), 1 );

		// Update existing key
		map.insertOrAssign( "update_key", "updated_value" );
		auto* value2 = map.tryGetValue( "update_key" );
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
		auto* value1 = map.tryGetValue( strKey );
		auto* value2 = map.tryGetValue( svKey );
		auto* value3 = map.tryGetValue( cstrKey );

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

		auto* result1 = map.tryGetValue( svKey );
		auto* result2 = map.tryGetValue( cstrKey );

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
		auto* value = map.tryGetValue( "erase2" );
		EXPECT_EQ( value, nullptr );

		// Other keys should still exist
		auto* value1 = map.tryGetValue( "erase1" );
		auto* value3 = map.tryGetValue( "erase3" );
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
		auto* value = map.tryGetValue( "existing" );
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
			auto* value = map.tryGetValue( "key_" + std::to_string( i ) );
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
		auto* value1 = map.tryGetValue( "collision1" );
		auto* value2 = map.tryGetValue( "collision2" );
		auto* value3 = map.tryGetValue( "collision3" );
		auto* value4 = map.tryGetValue( "collision4" );
		auto* value5 = map.tryGetValue( "collision5" );

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

		auto* value = map.tryGetValue( "" );
		ASSERT_NE( value, nullptr );
		EXPECT_EQ( *value, "empty_key_value" );

		// Test with empty string_view
		std::string_view emptySv{};
		auto* value2 = map.tryGetValue( emptySv );
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

		auto* value1 = map.tryGetValue( "key with spaces" );
		auto* value2 = map.tryGetValue( "key\twith\ttabs" );
		auto* value3 = map.tryGetValue( "key\nwith\nnewlines" );
		auto* value4 = map.tryGetValue( "key\"with\"quotes" );
		auto* value5 = map.tryGetValue( "key\\with\\backslashes" );

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

		auto* value1 = map.tryGetValue( "🔑" );
		auto* value2 = map.tryGetValue( "clé" );
		auto* value3 = map.tryGetValue( "键" );
		auto* value4 = map.tryGetValue( "ключ" );

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
			auto* value = map.tryGetValue( key );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );

			// Also test with string_view (zero-copy)
			std::string_view svKey{ key };
			auto* value2 = map.tryGetValue( svKey );
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
			auto* value = map.tryGetValue( "pattern_" + std::to_string( i ) );
			ASSERT_NE( value, nullptr );
			EXPECT_EQ( *value, i );
		}

		for ( int i = 100; i < 150; ++i ) // New items
		{
			auto* value = map.tryGetValue( "pattern_" + std::to_string( i ) );
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

		auto* value1 = map.tryGetValue( "vector1" );
		auto* value2 = map.tryGetValue( "vector2" );

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

		auto* value1 = map.tryGetValue( "unique1" );
		auto* value2 = map.tryGetValue( "unique2" );

		ASSERT_NE( value1, nullptr );
		ASSERT_NE( value2, nullptr );
		ASSERT_NE( value1->get(), nullptr );
		ASSERT_NE( value2->get(), nullptr );

		EXPECT_EQ( **value1, 42 );
		EXPECT_EQ( **value2, 84 );
	}
}
