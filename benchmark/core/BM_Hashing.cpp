/**
 * @file BM_Hashing.cpp
 * @brief Benchmark core hash algorithms and infrastructure
 * @details Comprehensive benchmarks for FNV-1a, CRC32, string hashing,
 *          integer hashing, and CPU feature detection performance
 */

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <nfx/core/hashing/Hash.h>

namespace nfx::core::benchmark
{
	//=====================================================================
	// Hashing infrastructure benchmark suite
	//=====================================================================

	//=====================================================================
	// Test data generation
	//=====================================================================

	static std::vector<std::string> generateTestStrings( size_t count, size_t minLength = 5, size_t maxLength = 50 )
	{
		std::vector<std::string> strings;
		strings.reserve( count );

		std::mt19937 gen( 42 ); // Fixed seed for reproducibility
		std::uniform_int_distribution<> lengthDist( static_cast<int>( minLength ), static_cast<int>( maxLength ) );
		std::uniform_int_distribution<> charDist( 'a', 'z' );

		for ( size_t i = 0; i < count; ++i )
		{
			const size_t len = static_cast<size_t>( lengthDist( gen ) );
			std::string str;
			str.reserve( len );

			for ( size_t j = 0; j < len; ++j )
			{
				str.push_back( static_cast<char>( charDist( gen ) ) );
			}

			strings.emplace_back( std::move( str ) );
		}

		return strings;
	}

	static std::vector<uint32_t> generateTestIntegers( size_t count )
	{
		std::vector<uint32_t> integers;
		integers.reserve( count );

		std::mt19937 gen( 42 );
		std::uniform_int_distribution<uint32_t> dist( 0, UINT32_MAX );

		for ( size_t i = 0; i < count; ++i )
		{
			integers.push_back( dist( gen ) );
		}

		return integers;
	}

	// Test data sets
	static const auto shortStrings = generateTestStrings( 100, 3, 8 );
	static const auto mediumStrings = generateTestStrings( 100, 10, 25 );
	static const auto longStrings = generateTestStrings( 100, 50, 200 );
	static const auto testIntegers = generateTestIntegers( 1000 );

	//=====================================================================
	// CPU feature detection benchmarks
	//=====================================================================

	static void BM_HasSSE42Support( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			bool hasSSE42 = nfx::core::hashing::hasSSE42Support();
			::benchmark::DoNotOptimize( hasSSE42 );
		}
	}

	//=====================================================================
	// Low-level hash building blocks benchmarks
	//=====================================================================

	//----------------------------------------------
	// Single-step hash functions
	//----------------------------------------------

	static void BM_FNV1a_SingleStep( ::benchmark::State& state )
	{
		const uint32_t initialHash = nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS;
		const uint8_t testByte = 'A';

		for ( auto _ : state )
		{
			uint32_t hash = nfx::core::hashing::fnv1a<nfx::core::hashing::DEFAULT_FNV_PRIME>( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_CRC32_SingleStep( ::benchmark::State& state )
	{
		const uint32_t initialHash = 0;
		const uint8_t testByte = 'A';

		for ( auto _ : state )
		{
			uint32_t hash = nfx::core::hashing::crc32( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Larson_SingleStep( ::benchmark::State& state )
	{
		const uint32_t initialHash = 0;
		const uint8_t testByte = 'A';

		for ( auto _ : state )
		{
			uint32_t hash = nfx::core::hashing::Larson( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	//----------------------------------------------
	// CHD algorithm components
	//----------------------------------------------

	static void BM_SeedMix_Function( ::benchmark::State& state )
	{
		const uint32_t seed = 12345;
		const uint32_t hash = 0xABCDEF01;
		const size_t tableSize = 1024;

		for ( auto _ : state )
		{
			uint32_t result = nfx::core::hashing::seedMix( seed, hash, tableSize );
			::benchmark::DoNotOptimize( result );
		}
	}

	//=====================================================================
	// String hashing benchmarks
	//=====================================================================

	//----------------------------------------------
	// High-level string hashing
	//----------------------------------------------

	static void BM_HashStringView_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// Manual FNV-1a implementation for comparison
	//----------------------------------------------

	static void BM_ManualFNV1a_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				uint32_t hash = nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS;
				for ( char c : str )
				{
					hash = nfx::core::hashing::fnv1a<nfx::core::hashing::DEFAULT_FNV_PRIME>( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ManualFNV1a_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				uint32_t hash = nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS;
				for ( char c : str )
				{
					hash = nfx::core::hashing::fnv1a<nfx::core::hashing::DEFAULT_FNV_PRIME>( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ManualFNV1a_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				uint32_t hash = nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS;
				for ( char c : str )
				{
					hash = nfx::core::hashing::fnv1a<nfx::core::hashing::DEFAULT_FNV_PRIME>( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// Manual CRC32 implementation for comparison
	//----------------------------------------------

	static void BM_ManualCRC32_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::core::hashing::crc32( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ManualCRC32_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::core::hashing::crc32( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ManualCRC32_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::core::hashing::crc32( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// std::hash comparison
	//----------------------------------------------

	static void BM_StdHash_Short( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_Medium( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_Long( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//=====================================================================
	// Integer hashing benchmarks
	//=====================================================================

	static void BM_HashInteger_uint32( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				totalHash += nfx::core::hashing::hashInteger( value );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashInteger_uint64( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				uint64_t largeValue = static_cast<uint64_t>( value ) << 32 | value;
				totalHash += nfx::core::hashing::hashInteger( largeValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashInteger_int32( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				int32_t signedValue = static_cast<int32_t>( value );
				totalHash += nfx::core::hashing::hashInteger( signedValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// std::hash comparison for integers
	//----------------------------------------------

	static void BM_StdHash_uint32( ::benchmark::State& state )
	{
		std::hash<uint32_t> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				totalHash += hasher( value );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_uint64( ::benchmark::State& state )
	{
		std::hash<uint64_t> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				uint64_t largeValue = static_cast<uint64_t>( value ) << 32 | value;
				totalHash += hasher( largeValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//=====================================================================
	// Algorithm comparison benchmarks
	//=====================================================================

	//----------------------------------------------
	// Single string hashing comparison
	//----------------------------------------------

	static void BM_SingleString_HashStringView( ::benchmark::State& state )
	{
		const std::string testStr = "performance_test_string_for_comparison";

		for ( auto _ : state )
		{
			uint32_t hash = nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( testStr );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_SingleString_StdHash( ::benchmark::State& state )
	{
		const std::string testStr = "performance_test_string_for_comparison";
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t hash = hasher( testStr );
			::benchmark::DoNotOptimize( hash );
		}
	}

	//----------------------------------------------
	// Cache behavior tests
	//----------------------------------------------

	static void BM_HashStringView_CacheTest( ::benchmark::State& state )
	{
		// Test cache behavior with repeated hashing of same strings
		std::vector<std::string> repeatedStrings;
		for ( int i = 0; i < 10; ++i )
		{
			for ( const auto& str : shortStrings )
			{
				repeatedStrings.push_back( str );
			}
		}

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : repeatedStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//=====================================================================
	// Memory access pattern benchmarks
	//=====================================================================

	static void BM_Sequential_StringHashing( ::benchmark::State& state )
	{
		std::vector<std::string> sequentialStrings;
		for ( size_t i = 0; i < 1000; ++i )
		{
			sequentialStrings.push_back( "string_" + std::to_string( i ) );
		}

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : sequentialStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_Random_StringHashing( ::benchmark::State& state )
	{
		auto randomStrings = generateTestStrings( 1000, 8, 32 );

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : randomStrings )
			{
				totalHash += nfx::core::hashing::hashStringView<nfx::core::hashing::DEFAULT_FNV_OFFSET_BASIS, nfx::core::hashing::DEFAULT_FNV_PRIME>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}
}

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// CPU feature detection
//----------------------------------------------

BENCHMARK( nfx::core::benchmark::BM_HasSSE42Support );

//----------------------------------------------
// Low-level hash building blocks
//----------------------------------------------

BENCHMARK( nfx::core::benchmark::BM_FNV1a_SingleStep );
BENCHMARK( nfx::core::benchmark::BM_CRC32_SingleStep );
BENCHMARK( nfx::core::benchmark::BM_Larson_SingleStep );
BENCHMARK( nfx::core::benchmark::BM_SeedMix_Function );

//----------------------------------------------
// String hashing comparisons
//----------------------------------------------

//----------------------------
// High-level API
//----------------------------

BENCHMARK( nfx::core::benchmark::BM_HashStringView_Short );
BENCHMARK( nfx::core::benchmark::BM_HashStringView_Medium );
BENCHMARK( nfx::core::benchmark::BM_HashStringView_Long );

//----------------------------
// Manual FNV-1a
//----------------------------

BENCHMARK( nfx::core::benchmark::BM_ManualFNV1a_Short );
BENCHMARK( nfx::core::benchmark::BM_ManualFNV1a_Medium );
BENCHMARK( nfx::core::benchmark::BM_ManualFNV1a_Long );

//----------------------------
// Manual CRC32
//----------------------------

BENCHMARK( nfx::core::benchmark::BM_ManualCRC32_Short );
BENCHMARK( nfx::core::benchmark::BM_ManualCRC32_Medium );
BENCHMARK( nfx::core::benchmark::BM_ManualCRC32_Long );

//----------------------------
// std::hash comparison
//----------------------------

BENCHMARK( nfx::core::benchmark::BM_StdHash_Short );
BENCHMARK( nfx::core::benchmark::BM_StdHash_Medium );
BENCHMARK( nfx::core::benchmark::BM_StdHash_Long );

//----------------------------------------------
// Integer hashing comparisons
//----------------------------------------------

BENCHMARK( nfx::core::benchmark::BM_HashInteger_uint32 );
BENCHMARK( nfx::core::benchmark::BM_HashInteger_uint64 );
BENCHMARK( nfx::core::benchmark::BM_HashInteger_int32 );

BENCHMARK( nfx::core::benchmark::BM_StdHash_uint32 );
BENCHMARK( nfx::core::benchmark::BM_StdHash_uint64 );

//----------------------------------------------
// Single string comparisons
//----------------------------------------------

BENCHMARK( nfx::core::benchmark::BM_SingleString_HashStringView );
BENCHMARK( nfx::core::benchmark::BM_SingleString_StdHash );

//----------------------------------------------
// Cache and memory patterns
//----------------------------------------------

BENCHMARK( nfx::core::benchmark::BM_HashStringView_CacheTest );
BENCHMARK( nfx::core::benchmark::BM_Sequential_StringHashing );
BENCHMARK( nfx::core::benchmark::BM_Random_StringHashing );

BENCHMARK_MAIN();
