/**
 * @file BM_HashMap.cpp
 * @brief Benchmark HashMap performance vs std::unordered_map
 * @details Demonstrates Robin Hood hashing performance and heterogeneous string operations
 */

#include <benchmark/benchmark.h>

#include <chrono>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nfx/containers/HashMap.h>

namespace nfx::containers::benchmark
{
	//=====================================================================
	// HashMap benchmark suite
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

		Employee() = default;
		Employee( std::string n, uint32_t i, double s, std::string dept )
			: name{ std::move( n ) },
			  id{ i }, salary{ s },
			  department{ std::move( dept ) }
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
	// Integer values (simple type)
	//----------------------------------------------

	static void BM_std_unordered_map_Insert_Int( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_map<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insert_or_assign( testKeys[i], static_cast<int>( i ) );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_Insert_Int( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Lookup operations
	//----------------------------------------------

	//----------------------------
	// std::string lookup
	//----------------------------

	static void BM_std_unordered_map_Lookup_String( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insert_or_assign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				auto it = map.find( testKeys[i] );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_HashMap_Lookup_String( ::benchmark::State& state )
	{
		nfx::containers::HashMap<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				if ( map.tryGetValue( testKeys[i], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// c_str lookup
	//----------------------------

	static void BM_std_unordered_map_Lookup_CStr( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insert_or_assign( testKeys[i], static_cast<int>( i ) );
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

	static void BM_HashMap_Lookup_CStr( ::benchmark::State& state )
	{
		nfx::containers::HashMap<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				// Zero-copy heterogeneous lookup
				if ( map.tryGetValue( c_strKeys[i], value ) )
				{
					sum += *value;
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
			map.insert_or_assign( testKeys[i], static_cast<int>( i ) );
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

	static void BM_HashMap_Lookup_StringView( ::benchmark::State& state )
	{
		nfx::containers::HashMap<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				// Zero-copy string_view lookup
				if ( map.tryGetValue( std::string_view{ c_strKeys[i] }, value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Complex value types
	//----------------------------------------------

	static void BM_std_unordered_map_Complex_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_map<std::string, Employee> map;
			for ( size_t i = 0; i < 50; ++i )
			{
				map.insert_or_assign( testKeys[i],
					Employee{
						"Employee_" + std::to_string( i ),
						static_cast<uint32_t>( i + 1000 ),
						50000.0 + static_cast<double>( i ) * 1000.0,
						"Engineering" } );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_Complex_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, Employee> map;
			for ( size_t i = 0; i < 50; ++i )
			{
				map.insertOrAssign( testKeys[i],
					Employee{
						"Employee_" + std::to_string( i ),
						static_cast<uint32_t>( i + 1000 ),
						50000.0 + static_cast<double>( i ) * 1000.0,
						"Engineering" } );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Removal operations
	//----------------------------------------------

	static void BM_std_unordered_map_Erase( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			state.PauseTiming();
			std::unordered_map<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insert_or_assign( testKeys[i], static_cast<int>( i ) );
			}
			state.ResumeTiming();

			for ( size_t i = 0; i < 50; ++i )
			{
				map.erase( testKeys[i] );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_Erase( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			state.PauseTiming();
			nfx::containers::HashMap<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}
			state.ResumeTiming();

			for ( size_t i = 0; i < 50; ++i )
			{
				map.erase( testKeys[i] );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Heterogeneous erase
	//----------------------------------------------

	static void BM_HashMap_Erase_CStr( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			state.PauseTiming();
			nfx::containers::HashMap<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}
			state.ResumeTiming();

			for ( size_t i = 0; i < 50; ++i )
			{
				// Zero-copy heterogeneous erase
				map.erase( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_Erase_StringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			state.PauseTiming();
			nfx::containers::HashMap<std::string, int> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}
			state.ResumeTiming();

			for ( size_t i = 0; i < 50; ++i )
			{
				// Zero-copy string_view erase
				map.erase( std::string_view{ c_strKeys[i] } );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Mixed operations (realistic usage)
	//----------------------------------------------

	static void BM_HashMap_Mixed_Operations( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, int> map;

			// Insert phase
			for ( size_t i = 0; i < 75; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}

			// Lookup phase with different key types
			int sum = 0;
			for ( size_t i = 0; i < 75; ++i )
			{
				int* value = nullptr;
				if ( i % 3 == 0 )
				{
					// const char* lookup
					if ( map.tryGetValue( c_strKeys[i], value ) )
					{
						sum += *value;
					}
				}
				else if ( i % 3 == 1 )
				{
					// string_view lookup
					if ( map.tryGetValue( std::string_view{ c_strKeys[i] }, value ) )
					{
						sum += *value;
					}
				}
				else
				{
					// std::string lookup
					if ( map.tryGetValue( testKeys[i], value ) )
					{
						sum += *value;
					}
				}
			}

			// Remove phase with heterogeneous keys
			for ( size_t i = 0; i < 25; ++i )
			{
				if ( i % 2 == 0 )
				{
					map.erase( c_strKeys[i] );
				}
				else
				{
					map.erase( std::string_view{ c_strKeys[i] } );
				}
			}

			::benchmark::DoNotOptimize( map );
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Large dataset
	//----------------------------------------------

	static void BM_HashMap_LargeDataset_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, int> map;
			for ( size_t i = 0; i < 1000; ++i )
			{
				map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_LargeDataset_Lookup( ::benchmark::State& state )
	{
		nfx::containers::HashMap<std::string, int> map;
		for ( size_t i = 0; i < 1000; ++i )
		{
			map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 1000; ++i )
			{
				int* value = nullptr;
				if ( map.tryGetValue( testKeys[i], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Robin Hood specific - probe distance
	//----------------------------------------------

	static void BM_HashMap_WorstCase_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<std::string, int> map;

			/*
			 * Create keys that might cause hash collisions
			 * Strategy: Use a common prefix with numeric suffixes that may hash to similar values
			 * The multiplier (37) creates a pattern that can stress the hash table's collision resolution
			 * This tests Robin Hood hashing's ability to handle clustered insertions efficiently
			 */
			std::vector<std::string> collision_keys;
			for ( size_t i = 0; i < 100; ++i )
			{
				/*
				 * Pattern: "collision_key_0", "collision_key_37", "collision_key_74", etc.
				 * Common prefixes + arithmetic progression may create hash clustering
				 */
				collision_keys.emplace_back( "collision_key_" + std::to_string( i * 37 ) );
			}

			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( collision_keys[i], static_cast<int>( i ) );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	//----------------------------------------------
	// Memory and cache behavior
	//----------------------------------------------

	static void BM_HashMap_ZeroAlloc_Lookup( ::benchmark::State& state )
	{
		nfx::containers::HashMap<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insertOrAssign( testKeys[i], static_cast<int>( i ) );
		}

		for ( auto _ : state )
		{
			// This should cause ZERO string allocations due to heterogeneous lookup
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				if ( map.tryGetValue( c_strKeys[i], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Non-string keys (testing general HashMap performance)
	//----------------------------------------------

	static void BM_HashMap_IntKey_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::HashMap<int, std::string> map;
			for ( size_t i = 0; i < 100; ++i )
			{
				map.insertOrAssign( static_cast<int>( i ), testKeys[i] );
			}
			::benchmark::DoNotOptimize( map );
		}
	}

	static void BM_HashMap_IntKey_Lookup( ::benchmark::State& state )
	{
		nfx::containers::HashMap<int, std::string> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map.insertOrAssign( static_cast<int>( i ), testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t total_length = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				std::string* value = nullptr;
				if ( map.tryGetValue( static_cast<int>( i ), value ) )
				{
					total_length += value->length();
				}
			}
			::benchmark::DoNotOptimize( total_length );
		}
	}
}

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Integer values (simple type)
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Insert_Int )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Insert_Int )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Lookup operations
//----------------------------------------------

//----------------------------
// std::string lookup
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_String )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Lookup_String )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// c_str lookup
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Lookup_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// string_view lookup
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Lookup_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Complex value types
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Complex_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Complex_Insert )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Removal operations
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Erase )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Erase )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Heterogeneous erase
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_Erase_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Erase_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Mixed operations (realistic usage)
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_Mixed_Operations )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Large dataset
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_LargeDataset_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_LargeDataset_Lookup )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Robin Hood specific - probe distance
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_WorstCase_Insert )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Memory and cache behavior
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_ZeroAlloc_Lookup )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Non-string keys (testing general HashMap performance)
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_HashMap_IntKey_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_IntKey_Lookup )
	->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
