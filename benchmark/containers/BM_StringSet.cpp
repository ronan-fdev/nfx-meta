/**
 * @file BM_StringSet.cpp
 * @brief Benchmark StringSet performance vs std::unordered_set<std::string>
 * @details Demonstrates zero-copy heterogeneous lookups and set operations performance
 */

#include <benchmark/benchmark.h>

#include <chrono>
#include <random>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <nfx/containers/StringSet.h>

namespace nfx::containers::benchmark
{
	//=====================================================================
	// StringSet benchmark suite
	//=====================================================================

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
	// Basic insertion
	//----------------------------------------------

	static void BM_std_unordered_set_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			for ( size_t i = 0; i < 100; ++i )
			{
				set.insert( testKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 100; ++i )
			{
				set.insert( testKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	//----------------------------------------------
	// Heterogeneous insertion
	//----------------------------------------------

	//----------------------------
	// c_str insertion
	//----------------------------

	static void BM_std_unordered_set_Insert_CStr( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			for ( size_t i = 0; i < 100; ++i )
			{
				set.insert( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_Insert_CStr( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy heterogeneous insert
				set.insert( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	//----------------------------
	// string_view insertion
	//----------------------------

	static void BM_std_unordered_set_Insert_StringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// std::unordered_set must create temporary string for string_view
				set.insert( std::string{ std::string_view{ c_strKeys[i] } } );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_Insert_StringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy string_view insert
				set.insert( std::string_view{ c_strKeys[i] } );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	//----------------------------------------------
	// Heterogeneous emplace
	//----------------------------------------------

	//----------------------------
	// emplace(c_str)
	//----------------------------

	static void BM_std_unordered_set_Emplace_CStr( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			for ( size_t i = 0; i < 100; ++i )
			{
				set.emplace( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_Emplace_CStr( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Heterogeneous emplace with const char*
				set.emplace( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	//----------------------------
	// emplace(string_view)
	//----------------------------

	static void BM_std_unordered_set_Emplace_StringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			std::unordered_set<std::string> set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// std::unordered_set must create temporary string for string_view
				set.emplace( std::string{ std::string_view{ c_strKeys[i] } } );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_Emplace_StringView( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Heterogeneous emplace with string_view
				set.emplace( std::string_view{ c_strKeys[i] } );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	//----------------------------------------------
	// Heterogeneous lookup
	//----------------------------------------------

	//----------------------------
	// find(c_str)
	//----------------------------

	static void BM_std_unordered_set_Find_CStr( ::benchmark::State& state )
	{
		std::unordered_set<std::string> set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				auto it = set.find( c_strKeys[i] );
				if ( it != set.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	static void BM_StringSet_Find_CStr( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy heterogeneous lookup
				auto it = set.find( c_strKeys[i] );
				if ( it != set.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------
	// find(string_view)
	//----------------------------

	static void BM_std_unordered_set_Find_StringView( ::benchmark::State& state )
	{
		std::unordered_set<std::string> set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// std::unordered_set must create temporary string for string_view
				auto it = set.find( std::string{ std::string_view{ c_strKeys[i] } } );
				if ( it != set.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	static void BM_StringSet_Find_StringView( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy string_view lookup
				auto it = set.find( std::string_view{ c_strKeys[i] } );
				if ( it != set.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------
	// contains(c_str)
	//----------------------------

	static void BM_std_unordered_set_Contains_CStr( ::benchmark::State& state )
	{
		std::unordered_set<std::string> set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				if ( set.count( c_strKeys[i] ) > 0 )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	static void BM_StringSet_Contains_CStr( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy heterogeneous contains
				if ( set.contains( c_strKeys[i] ) )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------
	// contains(string_view)
	//----------------------------

	static void BM_std_unordered_set_Contains_StringView( ::benchmark::State& state )
	{
		std::unordered_set<std::string> set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// std::unordered_set must create temporary string for string_view
				if ( set.count( std::string{ std::string_view{ c_strKeys[i] } } ) > 0 )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	static void BM_StringSet_Contains_StringView( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				// Zero-copy string_view contains
				if ( set.contains( std::string_view{ c_strKeys[i] } ) )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------------------------
	// Mixed operations (realistic usage)
	//----------------------------------------------

	static void BM_StringSet_Mixed_Operations( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;

			// Insert phase with different key types
			for ( size_t i = 0; i < 50; ++i )
			{
				if ( i % 3 == 0 )
				{
					set.insert( c_strKeys[i] ); // const char*
				}
				else if ( i % 3 == 1 )
				{
					set.emplace( std::string_view{ c_strKeys[i] } ); // string_view
				}
				else
				{
					set.insert( testKeys[i] ); // std::string
				}
			}

			// Lookup phase with different key types
			size_t found_count = 0;
			for ( size_t i = 0; i < 50; ++i )
			{
				// Mix of const char*, string_view, and string lookups
				if ( i % 3 == 0 )
				{
					if ( set.contains( c_strKeys[i] ) )
						++found_count; // const char*
				}
				else if ( i % 3 == 1 )
				{
					if ( set.find( std::string_view{ c_strKeys[i] } ) != set.end() )
						++found_count; // string_view
				}
				else
				{
					if ( set.count( testKeys[i] ) > 0 )
						++found_count; // std::string
				}
			}

			// Remove phase
			for ( size_t i = 0; i < 25; ++i )
			{
				set.erase( c_strKeys[i] );
			}

			::benchmark::DoNotOptimize( set );
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------------------------
	// Set operations
	//----------------------------------------------

	static void BM_StringSet_SetOperations( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set1, set2, result;

			// Populate first set
			for ( size_t i = 0; i < 50; ++i )
			{
				set1.insert( c_strKeys[i] );
			}

			// Populate second set with overlap
			for ( size_t i = 25; i < 75; ++i )
			{
				set2.insert( c_strKeys[i] );
			}

			// Intersection-like operation using heterogeneous lookup
			for ( const auto& key : set1 )
			{
				if ( set2.contains( std::string_view{ key } ) )
				{
					result.insert( key );
				}
			}

			::benchmark::DoNotOptimize( set1 );
			::benchmark::DoNotOptimize( set2 );
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// Large dataset
	//----------------------------------------------

	static void BM_StringSet_LargeDataset_Insert( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;
			for ( size_t i = 0; i < 1000; ++i )
			{
				set.insert( c_strKeys[i] );
			}
			::benchmark::DoNotOptimize( set );
		}
	}

	static void BM_StringSet_LargeDataset_Lookup( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 1000; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			size_t found_count = 0;
			for ( size_t i = 0; i < 1000; ++i )
			{
				if ( set.contains( c_strKeys[i] ) )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------------------------
	// Memory allocation
	//----------------------------------------------

	static void BM_StringSet_ZeroAlloc_Lookup( ::benchmark::State& state )
	{
		nfx::containers::StringSet set;
		for ( size_t i = 0; i < 100; ++i )
		{
			set.insert( testKeys[i] );
		}

		for ( auto _ : state )
		{
			// This should cause ZERO string allocations due to heterogeneous lookup
			size_t found_count = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				if ( set.find( c_strKeys[i] ) != set.end() )
				{
					++found_count;
				}
			}
			::benchmark::DoNotOptimize( found_count );
		}
	}

	//----------------------------------------------
	// Duplicate handling
	//----------------------------------------------

	static void BM_StringSet_DuplicateHandling( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			nfx::containers::StringSet set;

			// Insert same keys multiple times with different string types
			for ( size_t round = 0; round < 3; ++round )
			{
				for ( size_t i = 0; i < 50; ++i )
				{
					if ( round == 0 )
					{
						set.insert( c_strKeys[i] ); // const char*
					}
					else if ( round == 1 )
					{
						set.insert( std::string_view{ c_strKeys[i] } ); // string_view
					}
					else
					{
						set.insert( testKeys[i] ); // std::string
					}
				}
			}

			::benchmark::DoNotOptimize( set );
		}
	}
} // namespace nfx::containers::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Basic insertion
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Insert )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Heterogeneous insertion
//----------------------------------------------

//----------------------------
// c_str insertion
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Insert_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Insert_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// string_view insertion
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Insert_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Insert_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Heterogeneous emplace
//----------------------------------------------

//----------------------------
// emplace(c_str)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Emplace_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Emplace_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// emplace(string_view)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Emplace_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Emplace_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Heterogeneous lookup
//----------------------------------------------

//----------------------------
// find(c_str)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Find_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Find_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// find(string_view)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Find_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Find_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// contains(c_str)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Contains_CStr )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Contains_CStr )
	->Unit( benchmark::kMicrosecond );

//----------------------------
// contains(string_view)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_set_Contains_StringView )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_Contains_StringView )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Mixed operations (realistic usage)
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringSet_Mixed_Operations )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Set operations
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringSet_SetOperations )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Large dataset
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringSet_LargeDataset_Insert )
	->Unit( benchmark::kMicrosecond );
BENCHMARK( nfx::containers::benchmark::BM_StringSet_LargeDataset_Lookup )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Memory allocation
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_StringSet_ZeroAlloc_Lookup )
	->Unit( benchmark::kMicrosecond );

//----------------------------------------------
// Duplicate handling
//----------------------------------------------
BENCHMARK( nfx::containers::benchmark::BM_StringSet_DuplicateHandling )
	->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
