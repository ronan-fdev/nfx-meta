/**
 * @file BM_Splitter.cpp
 * @brief Benchmark Splitter string splitting performance vs manual approaches
 */

#include <benchmark/benchmark.h>
#include <string_view>
#include <vector>

#include <nfx/string/Splitter.h>

namespace nfx::string::benchmark
{
	//=====================================================================
	// Splitter benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Test data
	//----------------------------------------------

	static const std::string csv_data = "John,Doe,30,Engineer,NewYork,75000,Active,2023-01-15";
	static const std::string path_data = "VE/400a/400/C101.31/S206/H346.11112/meta";
	static const std::string config_data = "server=localhost;port=8080;database=mydb;timeout=30;ssl=true;debug=false";

	//----------------------------------------------
	// Manual vs Splitter with CSV data
	//----------------------------------------------

	class ManualSplitter
	{
	public:
		static void split( std::string_view input, char delimiter, std::vector<std::string_view>& output )
		{
			output.clear();

			size_t start = 0;
			size_t pos = 0;

			while ( ( pos = input.find( delimiter, start ) ) != std::string_view::npos )
			{
				output.emplace_back( input.substr( start, pos - start ) );
				start = pos + 1;
			}

			if ( start <= input.length() )
			{
				output.emplace_back( input.substr( start ) );
			}
		}
	};

	//----------------------------
	// Manual with CSV data
	//----------------------------

	static void BM_Manual_CSV( ::benchmark::State& state )
	{
		std::vector<std::string_view> segments;

		for ( auto _ : state )
		{
			ManualSplitter::split( csv_data, ',', segments );
			::benchmark::DoNotOptimize( segments );
		}
	}

	//----------------------------
	// Splitter with CSV data
	//----------------------------

	static void BM_Splitter_CSV( ::benchmark::State& state )
	{
		std::vector<std::string_view> segments;

		for ( auto _ : state )
		{
			segments.clear();
			for ( const auto segment : nfx::string::Splitter{ csv_data, ',' } )
			{
				segments.emplace_back( segment );
			}
			::benchmark::DoNotOptimize( segments );
		}
	}

	//----------------------------
	// SplitView with CSV data
	//----------------------------

	static void BM_SplitView_CSV( ::benchmark::State& state )
	{
		std::vector<std::string_view> segments;

		for ( auto _ : state )
		{
			segments.clear();
			for ( const auto segment : nfx::string::splitView( csv_data, ',' ) )
			{
				segments.emplace_back( segment );
			}
			::benchmark::DoNotOptimize( segments );
		}
	}

	//----------------------------------------------
	// Path splitting
	//----------------------------------------------

	static void BM_Splitter_Path( ::benchmark::State& state )
	{
		std::vector<std::string_view> segments;

		for ( auto _ : state )
		{
			segments.clear();
			for ( const auto segment : nfx::string::Splitter{ path_data, '/' } )
			{
				segments.emplace_back( segment );
			}
			::benchmark::DoNotOptimize( segments );
		}
	}

	//----------------------------------------------
	// Config data splitting
	//----------------------------------------------

	static void BM_Splitter_Config( ::benchmark::State& state )
	{
		std::vector<std::string_view> segments;

		for ( auto _ : state )
		{
			segments.clear();
			for ( const auto segment : nfx::string::Splitter{ config_data, ';' } )
			{
				segments.emplace_back( segment );
			}
			::benchmark::DoNotOptimize( segments );
		}
	}

	//----------------------------------------------
	// Zero-allocation benchmark
	//----------------------------------------------

	static void BM_Splitter_ZeroAlloc( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t count = 0;
			for ( const auto segment : nfx::string::Splitter{ csv_data, ',' } )
			{
				count += segment.length();
			}
			::benchmark::DoNotOptimize( count );
		}
	}
}

BENCHMARK( nfx::string::benchmark::BM_Manual_CSV );
BENCHMARK( nfx::string::benchmark::BM_Splitter_CSV );
BENCHMARK( nfx::string::benchmark::BM_SplitView_CSV );
BENCHMARK( nfx::string::benchmark::BM_Splitter_Path );
BENCHMARK( nfx::string::benchmark::BM_Splitter_Config );
BENCHMARK( nfx::string::benchmark::BM_Splitter_ZeroAlloc );

BENCHMARK_MAIN();
