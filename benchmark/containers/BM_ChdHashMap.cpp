/**
 * @file BM_ChdHashMap.cpp
 * @brief Benchmark ChdHashMap performance vs std::unordered_map and HashMap
 * @details Demonstrates perfect hashing performance and comparison with traditional hash maps
 */

#include <benchmark/benchmark.h>

#include <chrono>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nfx/containers/ChdHashMap.h>
#include <nfx/containers/HashMap.h>

namespace nfx::containers::benchmark
{
	//=====================================================================
	// ChdHashMap benchmark suite
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
			  id{ i },
			  salary{ s }, department{ std::move( dept ) }
		{
		}
	};

	struct Config
	{
		std::string value;
		bool isValid;
		std::chrono::system_clock::time_point timestamp;

		Config() = default;

		Config( std::string v, bool valid = true )
			: value{ std::move( v ) },
			  isValid{ valid },
			  timestamp{ std::chrono::system_clock::now() }
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
	// Dataset preparation for CHD construction
	//----------------------------------------------

	static std::vector<std::pair<std::string, int>> createIntDataset( size_t count )
	{
		std::vector<std::pair<std::string, int>> dataset;
		dataset.reserve( count );

		for ( size_t i = 0; i < count; ++i )
		{
			dataset.emplace_back( testKeys[i], static_cast<int>( i ) );
		}

		return dataset;
	}

	static std::vector<std::pair<std::string, Employee>> createEmployeeDataset( size_t count )
	{
		std::vector<std::pair<std::string, Employee>> dataset;
		dataset.reserve( count );

		const std::vector<std::string> departments = { "Engineering", "Sales", "Marketing", "HR", "Finance" };

		for ( size_t i = 0; i < count; ++i )
		{
			Employee emp{
				"Employee_" + std::to_string( i ),
				static_cast<uint32_t>( i ),
				50000.0 + ( static_cast<double>( i ) * 1000.0 ),
				departments[i % departments.size()] };

			dataset.emplace_back( testKeys[i], std::move( emp ) );
		}

		return dataset;
	}

	//----------------------------------------------
	// CHD Construction benchmarks
	//----------------------------------------------

	static void BM_ChdHashMap_Construction_Small( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 10 );

		for ( auto _ : state )
		{
			auto dataCopy = dataset; // CHD takes ownership
			nfx::containers::ChdHashMap<int> chd{ std::move( dataCopy ) };
			::benchmark::DoNotOptimize( chd );
		}
	}

	static void BM_ChdHashMap_Construction_Medium( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );

		for ( auto _ : state )
		{
			auto dataCopy = dataset;
			nfx::containers::ChdHashMap<int> chd{ std::move( dataCopy ) };
			::benchmark::DoNotOptimize( chd );
		}
	}

	static void BM_ChdHashMap_Construction_Large( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 500 );

		for ( auto _ : state )
		{
			auto dataCopy = dataset;
			nfx::containers::ChdHashMap<int> chd{ std::move( dataCopy ) };
			::benchmark::DoNotOptimize( chd );
		}
	}

	//----------------------------------------------
	// Lookup performance comparisons
	//----------------------------------------------

	//----------------------------
	// Integer values (simple type)
	//----------------------------

	static void BM_std_unordered_map_Lookup_Int( ::benchmark::State& state )
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
				auto it = map.find( testKeys[i] );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_HashMap_Lookup_Int( ::benchmark::State& state )
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

	static void BM_ChdHashMap_Lookup_Int( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				if ( chd.tryGetValue( testKeys[i], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// Operator[] access patterns
	//----------------------------

	static void BM_std_unordered_map_Access_Bracket( ::benchmark::State& state )
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
				sum += map[testKeys[i]];
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Access_Bracket( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				sum += chd[testKeys[i]];
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// c_str lookups
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
				// This creates temporary strings for C-string lookups
				auto it = map.find( std::string{ c_strKeys[i] } );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Lookup_CStr( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				std::string_view key_view{ c_strKeys[i] };
				int* value = nullptr;
				if ( chd.tryGetValue( key_view, value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// String view lookups
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
				// std::unordered_map doesn't support heterogeneous lookup by default
				// so we need to create temporary strings from string_view
				std::string_view key_view{ testKeys[i] };
				auto it = map.find( std::string{ key_view } );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Lookup_StringView( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				std::string_view key_view{ testKeys[i] };
				int* value = nullptr;
				if ( chd.tryGetValue( key_view, value ) )
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

	static void BM_std_unordered_map_Lookup_Employee( ::benchmark::State& state )
	{
		std::unordered_map<std::string, Employee> map;
		for ( size_t i = 0; i < 50; ++i )
		{
			Employee emp{
				"Employee_" + std::to_string( i ),
				static_cast<uint32_t>( i ),
				50000.0 + ( static_cast<double>( i ) * 1000.0 ),
				"Engineering" };
			map[testKeys[i]] = std::move( emp );
		}

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			for ( size_t i = 0; i < 50; ++i )
			{
				auto it = map.find( testKeys[i] );
				if ( it != map.end() )
				{
					totalSalary += it->second.salary;
				}
			}
			::benchmark::DoNotOptimize( totalSalary );
		}
	}

	static void BM_ChdHashMap_Lookup_Employee( ::benchmark::State& state )
	{
		auto dataset = createEmployeeDataset( 50 );
		nfx::containers::ChdHashMap<Employee> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			for ( size_t i = 0; i < 50; ++i )
			{
				Employee* emp = nullptr;
				if ( chd.tryGetValue( testKeys[i], emp ) )
				{
					totalSalary += emp->salary;
				}
			}
			::benchmark::DoNotOptimize( totalSalary );
		}
	}

	//----------------------------------------------
	// Memory access patterns
	//----------------------------------------------

	//----------------------------
	// Sequential access
	//----------------------------

	static void BM_std_unordered_map_Sequential_Access( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		for ( auto _ : state )
		{
			int sum = 0;
			for ( const auto& pair : map )
			{
				sum += pair.second;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Sequential_Access( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( const auto& pair : chd )
			{
				sum += pair.second;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// Random access
	//----------------------------

	static void BM_std_unordered_map_Random_Access( ::benchmark::State& state )
	{
		std::unordered_map<std::string, int> map;
		for ( size_t i = 0; i < 100; ++i )
		{
			map[testKeys[i]] = static_cast<int>( i );
		}

		// Create shuffled indices for random access
		std::vector<size_t> indices( 100 );
		std::iota( indices.begin(), indices.end(), 0 );
		std::shuffle( indices.begin(), indices.end(), std::mt19937{ 42 } );

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t idx : indices )
			{
				auto it = map.find( testKeys[idx] );
				if ( it != map.end() )
				{
					sum += it->second;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Random_Access( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		// Create shuffled indices for random access
		std::vector<size_t> indices( 100 );
		std::iota( indices.begin(), indices.end(), 0 );
		std::shuffle( indices.begin(), indices.end(), std::mt19937{ 42 } );

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t idx : indices )
			{
				int* value = nullptr;
				if ( chd.tryGetValue( testKeys[idx], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Cache behavior and miss patterns
	//----------------------------------------------

	static void BM_ChdHashMap_Hit_Rate_100( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int hits = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				if ( chd.tryGetValue( testKeys[i], value ) )
				{
					hits++;
				}
			}
			::benchmark::DoNotOptimize( hits );
		}
	}

	static void BM_ChdHashMap_Hit_Rate_50( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 50 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int hits = 0;

			/*
			 * Cache miss test: CHD contains only 50 keys (testKeys[0-49])
			 * but we query 100 keys (testKeys[0-99]), creating 50% miss rate
			 * This measures performance impact of failed lookups vs successful ones
			 */
			for ( size_t i = 0; i < 100; ++i )
			{
				int* value = nullptr;
				if ( chd.tryGetValue( testKeys[i], value ) )
				{
					hits++;
				}
			}
			::benchmark::DoNotOptimize( hits );
		}
	}

	//----------------------------------------------
	// Perfect hash properties
	//----------------------------------------------

	static void BM_ChdHashMap_Hash_Function( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t sum = 0;
			for ( size_t i = 0; i < 100; ++i )
			{
				sum += nfx::containers::ChdHashMap<int>::hash( testKeys[i] );
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Configuration-like workloads
	//----------------------------------------------

	static void BM_ChdHashMap_Config_Lookup( ::benchmark::State& state )
	{
		std::vector<std::pair<std::string, Config>> configData;
		configData.reserve( 20 );

		const std::vector<std::string> configKeys = {
			"server.host", "server.port", "database.url", "database.timeout",
			"cache.size", "cache.ttl", "logging.level", "logging.file",
			"security.ssl", "security.cert", "auth.method", "auth.timeout",
			"api.version", "api.rate_limit", "storage.path", "storage.backup",
			"monitor.enabled", "monitor.interval", "debug.enabled", "debug.verbose" };

		for ( size_t i = 0; i < configKeys.size(); ++i )
		{
			configData.emplace_back( configKeys[i], Config{ "value_" + std::to_string( i ) } );
		}

		nfx::containers::ChdHashMap<Config> configMap{ std::move( configData ) };

		for ( auto _ : state )
		{
			size_t validConfigs = 0;
			for ( const auto& key : configKeys )
			{
				Config* config = nullptr;
				if ( configMap.tryGetValue( key, config ) && config->isValid )
				{
					validConfigs++;
				}
			}
			::benchmark::DoNotOptimize( validConfigs );
		}
	}

	//----------------------------------------------
	// Large dataset stress test
	//----------------------------------------------

	static void BM_ChdHashMap_Large_Dataset( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 500 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( size_t i = 0; i < 500; ++i )
			{
				int* value = nullptr;
				if ( chd.tryGetValue( testKeys[i], value ) )
				{
					sum += *value;
				}
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Iterator vs. Enumerator
	//----------------------------------------------

	static void BM_ChdHashMap_Iterator_RangeBasedFor( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( const auto& [key, value] : chd )
			{
				sum += value;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Iterator_ManualIteration( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( auto it = chd.begin(); it != chd.end(); ++it )
			{
				sum += it->second;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Enumerator_BasicIteration( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				sum += enumerator.current().second;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// Complex value types iteration
	//----------------------------

	static void BM_ChdHashMap_Iterator_Employee_RangeFor( ::benchmark::State& state )
	{
		auto dataset = createEmployeeDataset( 50 );
		nfx::containers::ChdHashMap<Employee> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			uint32_t totalIds = 0;
			for ( const auto& [key, employee] : chd )
			{
				totalSalary += employee.salary;
				totalIds += employee.id;
			}
			::benchmark::DoNotOptimize( totalSalary );
			::benchmark::DoNotOptimize( totalIds );
		}
	}

	static void BM_ChdHashMap_Enumerator_Employee_Iteration( ::benchmark::State& state )
	{
		auto dataset = createEmployeeDataset( 50 );
		nfx::containers::ChdHashMap<Employee> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			uint32_t totalIds = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				const auto& employee = enumerator.current().second;
				totalSalary += employee.salary;
				totalIds += employee.id;
			}
			::benchmark::DoNotOptimize( totalSalary );
			::benchmark::DoNotOptimize( totalIds );
		}
	}

	//----------------------------
	// Conditional iteration patterns
	//----------------------------

	static void BM_ChdHashMap_Iterator_ConditionalProcessing( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int evenSum = 0;
			int oddCount = 0;
			for ( const auto& [key, value] : chd )
			{
				if ( value % 2 == 0 )
				{
					evenSum += value;
				}
				else
				{
					oddCount++;
				}
			}
			::benchmark::DoNotOptimize( evenSum );
			::benchmark::DoNotOptimize( oddCount );
		}
	}

	static void BM_ChdHashMap_Enumerator_ConditionalProcessing( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int evenSum = 0;
			int oddCount = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				const auto& [key, value] = enumerator.current();
				if ( value % 2 == 0 )
				{
					evenSum += value;
				}
				else
				{
					oddCount++;
				}
			}
			::benchmark::DoNotOptimize( evenSum );
			::benchmark::DoNotOptimize( oddCount );
		}
	}

	//----------------------------
	// Early termination patterns
	//----------------------------

	static void BM_ChdHashMap_Iterator_EarlyTermination( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int count = 0;
			for ( const auto& pair : chd )
			{
				(void)pair;

				count++;
				if ( count >= 10 ) // Stop after processing 10 items
				{
					break;
				}
			}
			::benchmark::DoNotOptimize( count );
		}
	}

	static void BM_ChdHashMap_Enumerator_EarlyTermination( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int count = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				count++;
				if ( count >= 10 ) // Stop after processing 10 items
				{
					break;
				}
			}
			::benchmark::DoNotOptimize( count );
		}
	}

	//----------------------------
	// Key filtering patterns
	//----------------------------

	static void BM_ChdHashMap_Iterator_KeyFiltering( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int matchingValues = 0;
			for ( const auto& [key, value] : chd )
			{
				// Process only keys that contain a specific character pattern
				if ( key.find( 'a' ) != std::string::npos )
				{
					matchingValues += value;
				}
			}
			::benchmark::DoNotOptimize( matchingValues );
		}
	}

	static void BM_ChdHashMap_Enumerator_KeyFiltering( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int matchingValues = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				const auto& [key, value] = enumerator.current();
				// Process only keys that contain a specific character pattern
				if ( key.find( 'a' ) != std::string::npos )
				{
					matchingValues += value;
				}
			}
			::benchmark::DoNotOptimize( matchingValues );
		}
	}

	//----------------------------
	// Reset and reuse patterns
	//----------------------------

	static void BM_ChdHashMap_Enumerator_ResetReuse( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 50 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			auto enumerator = chd.enumerator();
			int totalSum = 0;

			// First pass: sum all values
			while ( enumerator.next() )
			{
				totalSum += enumerator.current().second;
			}

			// Reset and second pass: count items
			enumerator.reset();
			int itemCount = 0;
			while ( enumerator.next() )
			{
				itemCount++;
			}

			::benchmark::DoNotOptimize( totalSum );
			::benchmark::DoNotOptimize( itemCount );
		}
	}

	static void BM_ChdHashMap_Iterator_MultiplePassComparison( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 50 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int totalSum = 0;

			// First pass: sum all values
			for ( const auto& [key, value] : chd )
			{
				totalSum += value;
			}

			// Second pass: count items (requires new iteration)
			int itemCount = 0;
			for ( const auto& pair : chd )
			{
				(void)pair;

				itemCount++;
			}

			::benchmark::DoNotOptimize( totalSum );
			::benchmark::DoNotOptimize( itemCount );
		}
	}

	//----------------------------
	// Large dataset iteration
	//----------------------------

	static void BM_ChdHashMap_Iterator_LargeDataset( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 500 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			for ( const auto& [key, value] : chd )
			{
				sum += value;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	static void BM_ChdHashMap_Enumerator_LargeDataset( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 500 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			int sum = 0;
			auto enumerator = chd.enumerator();
			while ( enumerator.next() )
			{
				sum += enumerator.current().second;
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------
	// STL algorithm patterns
	//----------------------------

	static void BM_ChdHashMap_Iterator_STLAlgorithm( ::benchmark::State& state )
	{
		auto dataset = createIntDataset( 100 );
		nfx::containers::ChdHashMap<int> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			// Use STL algorithm to find first element with value > 50
			auto found = std::find_if( chd.begin(), chd.end(),
				[]( const auto& pair ) { return pair.second > 50; } );

			bool hasResult = ( found != chd.end() );
			::benchmark::DoNotOptimize( hasResult );
		}
	}

	//----------------------------
	// Access pattern comparison
	//----------------------------

	static void BM_ChdHashMap_Iterator_ArrowOperator( ::benchmark::State& state )
	{
		auto dataset = createEmployeeDataset( 50 );
		nfx::containers::ChdHashMap<Employee> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			for ( auto it = chd.begin(); it != chd.end(); ++it )
			{
				// Access via arrow operator
				totalSalary += it->second.salary;
			}
			::benchmark::DoNotOptimize( totalSalary );
		}
	}

	static void BM_ChdHashMap_Iterator_DereferenceOperator( ::benchmark::State& state )
	{
		auto dataset = createEmployeeDataset( 50 );
		nfx::containers::ChdHashMap<Employee> chd{ std::move( dataset ) };

		for ( auto _ : state )
		{
			double totalSalary = 0.0;
			for ( auto it = chd.begin(); it != chd.end(); ++it )
			{
				// Access via dereference operator
				const auto& [key, employee] = *it;
				totalSalary += employee.salary;
			}
			::benchmark::DoNotOptimize( totalSalary );
		}
	}
} // namespace nfx::containers::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// CHD Construction benchmarks
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Construction_Small );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Construction_Medium );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Construction_Large );

//----------------------------------------------
// Lookup performance comparisons
//----------------------------------------------

//----------------------------
// Integer values (simple type)
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_Int );
BENCHMARK( nfx::containers::benchmark::BM_HashMap_Lookup_Int );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Lookup_Int );

//----------------------------
// Operator[] access patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Access_Bracket );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Access_Bracket );

//----------------------------
// c_str lookups
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_CStr );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Lookup_CStr );

//----------------------------
// String view lookups
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_StringView );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Lookup_StringView );

//----------------------------------------------
// Complex value types
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Lookup_Employee );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Lookup_Employee );

//----------------------------------------------
// Memory access patterns
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Sequential_Access );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Sequential_Access );

BENCHMARK( nfx::containers::benchmark::BM_std_unordered_map_Random_Access );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Random_Access );

//----------------------------------------------
// Cache behavior and miss patterns
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Hit_Rate_100 );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Hit_Rate_50 );

//----------------------------------------------
// Perfect hash properties
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Hash_Function );

//----------------------------------------------
// Configuration-like workloads
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Config_Lookup );

//----------------------------------------------
// Large dataset stress test
//----------------------------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Large_Dataset );

//----------------------------------------------
// Iterator vs. Enumerator
//----------------------------------------------

//----------------------------
// Basic iteration patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_RangeBasedFor );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_ManualIteration );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_BasicIteration );

//----------------------------
// Complex value types iteration
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_Employee_RangeFor );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_Employee_Iteration );

//----------------------------
// Conditional iteration patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_ConditionalProcessing );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_ConditionalProcessing );

//----------------------------
// Early termination patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_EarlyTermination );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_EarlyTermination );

//----------------------------
// Key filtering patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_KeyFiltering );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_KeyFiltering );

//----------------------------
// Reset and reuse patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_ResetReuse );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_MultiplePassComparison );

//----------------------------
// Large dataset iteration
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_LargeDataset );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Enumerator_LargeDataset );

//----------------------------
// STL algorithm patterns
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_STLAlgorithm );

//----------------------------
// Access pattern comparison
//----------------------------

BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_ArrowOperator );
BENCHMARK( nfx::containers::benchmark::BM_ChdHashMap_Iterator_DereferenceOperator );

BENCHMARK_MAIN();
