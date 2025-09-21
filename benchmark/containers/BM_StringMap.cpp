/**
 * @file BM_StringMap.cpp
 * @brief Benchmark StringMap performance vs std::unordered_map<std::string, T>
 * @details Demonstrates zero-copy heterogeneous lookups and performance with complex types
 */

#include <benchmark/benchmark.h>

#include <chrono>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nfx/containers/StringMap.h>

namespace nfx::containers::benchmark
{
	//=====================================================================
	// StringMap benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Test classes
	//----------------------------------------------

	struct Employee
	{
		std::string name;
		uint32_t id;
		double salary;
		std::string department;
		std::chrono::system_clock::time_point hireDate;

		Employee() = default;
		Employee( std::string n, uint32_t i, double s, std::string dept )
			: name{ std::move( n ) },
			  id{ i },
			  salary{ s },
			  department{ std::move( dept ) },
			  hireDate{ std::chrono::system_clock::now() }
		{
		}
	};

	struct CacheEntry
	{
		std::vector<uint8_t> data;
		std::chrono::steady_clock::time_point timestamp;
		uint64_t access_count;
		bool isDirty;

		CacheEntry() = default;
		CacheEntry( std::vector<uint8_t> d )
			: data{ std::move( d ) },
			  timestamp{ std::chrono::steady_clock::now() },
			  access_count{ 0 },
			  isDirty{ false }
		{
		}
	};

	//----------------------------------------------
	// Test data generation
	//----------------------------------------------

	static std::vector<std::string> generateKeys( size_t count )
	{
		std::vector<std::string> keys;
		keys.reserve( count );

		std::mt19937 gen( 42 ); // Fixed seed for reproducibility
		std::uniform_int_distribution<> lengthDist( 5, 20 );
		std::uniform_int_distribution<> charDist( 'a', 'z' );

		for ( size_t i = 0; i < count; ++i )
		{
			const size_t len = static_cast<size_t>( lengthDist( gen ) );
			std::string key;
			key.reserve( len );

			for ( size_t j = 0; j < len; ++j )
			{
				key.push_back( static_cast<char>( charDist( gen ) ) );
			}

			keys.emplace_back( std::move( key ) );
		}

		return keys;
	}

	static const auto testKeys = generateKeys( 1000 );

	static const std::vector<const char*> c_strKeys = []() {
		std::vector<const char*> result;
		result.reserve( testKeys.size() );
		for ( const auto& key : testKeys )
		{
			result.push_back( key.c_str() );
		}
		return result;
	}();

	//----------------------------------------------
	// Basic integer
	//----------------------------------------------

	static void BM_std_unordered_map_Insert_Int( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_map<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map[testKeys[i]] = static_cast<int>( i );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_StringMap_Insert_Int( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringMap<int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map[testKeys[i]] = static_cast<int>( i );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Heterogeneous lookup
	//----------------------------------------------

	//----------------------------
	// c_str lookup
	//----------------------------

	static void BM_std_unordered_map_Lookup_CStr( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// This creates temporary strings
				auto it = map.find( std::string{ c_strKeys[i] } );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_StringMap_Lookup_CStr( ::benchmark::State& state )
	{
		nfx::containers::StringMap<int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy heterogeneous lookup
				auto it = map.find( c_strKeys[i] );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// string_view lookup
	//----------------------------

	static void BM_std_unordered_map_Lookup_StringView( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				auto it = map.find( c_strKeys[i] );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_StringMap_Lookup_StringView( ::benchmark::State& state )
	{
		nfx::containers::StringMap<int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy string_view lookup
				auto it = map.find( std::string_view{ c_strKeys[i] } );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Complex type - Employee
	//----------------------------------------------

	static void BM_std_unordered_map_Employee_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_map<std::string, Employee> map;
			for ( size_t i = 0; i < 50; ++i )
			{
				map.emplace( testKeys[i], Employee{
											  "Employee_" + std::to_string( i ),
											  static_cast<uint32_t>( i + 1000 ),
											  50000.0 + static_cast<double>( i ) * 1000.0,
											  "Engineering" } );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_StringMap_Employee_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringMap<Employee> map;
			for ( size_t i = 0; i < 50; ++i )
			{
				map.emplace( testKeys[i], Employee{
											  "Employee_" + std::to_string( i ),
											  static_cast<uint32_t>( i + 1000 ),
											  50000.0 + static_cast<double>( i ) * 1000.0,
											  "Engineering" } );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_StringMap_Employee_TryEmplace( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringMap<Employee> map;
			for ( size_t i = 0; i < 50; ++i )
			{
				// Test heterogeneous try_emplace with const char*
				map.try_emplace( c_strKeys[i],
					"Employee_" + std::to_string( i ),
					static_cast<uint32_t>( i + 1000 ),
					50000.0 + static_cast<double>( i ) * 1000.0,
					"Engineering" );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Complex type - CacheEntry with large data
	//----------------------------------------------

	static void BM_StringMap_Cache_InsertOrAssign( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringMap<CacheEntry> cache;
			for ( size_t i = 0; i < 20; ++i )
			{
				std::vector<uint8_t> data( 1024, static_cast<uint8_t>( i % 256 ) );

				// Test heterogeneous insert_or_assign with string_view
				cache.insert_or_assign( std::string_view{ c_strKeys[i] }, CacheEntry{ std::move( data ) } );
			}
			::benchmark::DoNotOptimize( cache );
		}
	}

	//----------------------------------------------
	// Mixed operations (realistic usage)
	//----------------------------------------------

	static void BM_StringMap_Mixed_Operations( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringMap<int> map;

			// Insert phase
			for ( size_t i = 0; i < 50; ++i )
			{
				map[c_strKeys[i]] = static_cast<int>( i );
			}

			// Lookup phase with different key types
			int sum = 0;
			for ( size_t i = 0; i < 50; ++i )
			{
				// Mix of const char*, string_view, and string lookups
				if ( i % 3 == 0 )
				{
					sum += map[c_strKeys[i]]; // const char*
				}
				else if ( i % 3 == 1 )
				{
					sum += map[std::string_view{ c_strKeys[i] }]; // string_view
				}
				else
				{
					sum += map[testKeys[i]]; // std::string
				}
			}

			// Update phase
			for ( size_t i = 0; i < 25; ++i )
			{
				map.insert_or_assign( c_strKeys[i], sum + static_cast<int>( i ) );
			}

			::benchmark::DoNotOptimize( map );
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Memory allocation
	//----------------------------------------------

	static void BM_StringMap_ZeroAlloc_Lookup( ::benchmark::State& state )
	{
		nfx::containers::StringMap<int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			// This should cause ZERO string allocations due to heterogeneous lookup
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				if ( map.find( c_strKeys[i] ) != map.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}
}

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Basic integer
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Insert_Int )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringMap_Insert_Int )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Heterogeneous lookup
//----------------------------------------------

//----------------------------
// c_str lookup
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringMap_Lookup_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// string_view lookup
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringMap_Lookup_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Complex type - Employee
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Employee_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringMap_Employee_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringMap_Employee_TryEmplace )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Complex type - CacheEntry with large data
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringMap_Cache_InsertOrAssign )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Mixed operations (realistic usage)
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringMap_Mixed_Operations )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Memory allocation
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringMap_ZeroAlloc_Lookup )
	->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
