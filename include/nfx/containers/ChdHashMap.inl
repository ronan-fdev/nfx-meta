/**
 * @file ChdHashMap.inl
 * @brief Template implementation of CHD Dictionary class
 *
 * @details Implementation file for ChdHashMap template class, derived from Vista.SDK's
 *          C# ChdDictionary with C++ adaptations and optimizations.
 *
 * **Original Source Attribution:**
 * - Vista.SDK C# implementation: Vista.SDK.Internal.ChdDictionary<T>
 * - Author: Vista SDK Team
 * - Repository: https://github.com/dnv-opensource/vista-sdk
 * - License: MIT License
 * - Copyright (c) 2024 DNV
 *
 * **License Compliance:**
 * This implementation is derivative work based on Vista.SDK's CHD algorithm.
 * The original MIT License terms apply to the core algorithmic foundation.
 * C++ template adaptations and performance optimizations are part of nfx-core.
 *
 * @see https://github.com/dnv-opensource/vista-sdk/blob/main/LICENSE (MIT License)
 */

#include <algorithm>
#include <array>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>

#if defined( __GNUC__ )
#	include <cpuid.h>
#endif

#include "nfx/containers/constants/ChdConstants.h"

namespace nfx::containers
{
	namespace
	{
		//=====================================================================
		// Internal helper components
		//=====================================================================

		//----------------------------------------------
		// CPU feature detection
		//----------------------------------------------

		NFX_CORE_INLINE bool hasSSE42Support() noexcept
		{
			static thread_local const bool s_hasSSE42 = []() {
				bool hasSupport = false;
#if defined( _MSC_VER )
				std::array<int, 4> cpuInfo{};
				__cpuid( cpuInfo.data(), 1 );
				hasSupport = ( cpuInfo[2] & ( 1 << 20 ) ) != 0;
#elif defined( __GNUC__ )
				unsigned int eax, ebx, ecx, edx;
				if ( __get_cpuid( 1, &eax, &ebx, &ecx, &edx ) )
				{
					hasSupport = ( ecx & ( 1 << 20 ) ) != 0;
				}
#endif
				return hasSupport;
			}();

			return s_hasSSE42;
		}

		//----------------------------------------------
		// ThrowHelper class
		//----------------------------------------------

		//----------------------------
		// Public static methods
		//----------------------------

		inline void ThrowHelper::throwKeyNotFoundException( std::string_view key )
		{
			throw KeyNotFoundException{ key };
		}

		inline void ThrowHelper::throwInvalidOperationException()
		{
			throw InvalidOperationException{};
		}

		//----------------------------------------------
		// Hashing class
		//----------------------------------------------

		//----------------------------
		// Public static methods
		//----------------------------

		[[maybe_unused]] NFX_CORE_INLINE constexpr uint32_t Hashing::Larson( uint32_t hash, uint8_t ch ) noexcept
		{
			return 37 * hash + ch;
		}

		NFX_CORE_INLINE constexpr uint32_t Hashing::fnv1a( uint32_t hash, uint8_t ch ) noexcept
		{
			return ( ch ^ hash ) * constants::chd::FNV_PRIME;
		}

		NFX_CORE_INLINE uint32_t Hashing::crc32( uint32_t hash, uint8_t ch ) noexcept
		{
			return _mm_crc32_u8( hash, ch );
		}

		NFX_CORE_INLINE constexpr uint32_t Hashing::seed( uint32_t seed, uint32_t hash, size_t size ) noexcept
		{
			// Mixes the primary hash with the seed to find the final table slot
			uint32_t x{ seed + hash };
			x ^= x >> 12;
			x ^= x << 25;
			x ^= x >> 27;

			return static_cast<uint32_t>( ( x * 0x2545F4914F6CDD1DUL ) & ( size - 1 ) );
		}
	}

	//=====================================================================
	// Exception class
	//=====================================================================

	inline KeyNotFoundException::KeyNotFoundException( std::string_view key )
		: std::runtime_error{ std::string{ "No value associated to key: " } + std::string{ key } }
	{
	}

	inline InvalidOperationException::InvalidOperationException()
		: std::runtime_error{ "Operation is not valid due to the current state of the object." }
	{
	}

	inline InvalidOperationException::InvalidOperationException( std::string_view message )
		: std::runtime_error{ std::string{ message } }
	{
	}

	//=====================================================================
	// ChdHashMap class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TValue>
	inline ChdHashMap<TValue>::ChdHashMap( std::vector<std::pair<std::string, TValue>>&& items )
		: m_table{},
		  m_seeds{}
	{
		if ( items.empty() )
		{
			return;
		}

		uint64_t size{ 1 };
		// Ensure table size is a power of 2 and at least 2x item count for efficient modulo operations (using '&')
		while ( size < items.size() )
		{
			size *= 2;
		}
		size *= 2;

		m_table.reserve( size );
		m_seeds.reserve( size );

		auto hashBuckets{ std::vector<std::vector<std::pair<unsigned, uint32_t>>>( size ) };
		for ( auto& bucket : hashBuckets )
		{
			bucket.reserve( 4 );
		}

		for ( size_t i{ 0 }; i < items.size(); ++i )
		{
			const auto& key{ items[i].first };
			uint32_t hashValue{ hash( key ) };
			auto bucketForItemIdx{ hashValue & ( size - 1 ) };
			hashBuckets[bucketForItemIdx].emplace_back( static_cast<unsigned int>( i + 1 ), hashValue );
		}

		std::sort( hashBuckets.begin(), hashBuckets.end(), []( const auto& a, const auto& b ) { return a.size() > b.size(); } );

		auto indices{ std::vector<unsigned int>( size, 0 ) };
		auto seeds{ std::vector<int>( size, 0 ) };

		size_t currentBucketIdx{ 0 };
		for ( ; currentBucketIdx < hashBuckets.size() && hashBuckets[currentBucketIdx].size() > 1; ++currentBucketIdx )
		{
			const auto& subKeys{ hashBuckets[currentBucketIdx] };
			auto entries{ std::unordered_map<size_t, unsigned>() };
			entries.reserve( subKeys.size() );
			uint32_t currentSeedValue{ 0 };

			// CHD ALGORITHM: Find perfect seed value for this collision bucket
			while ( true )
			{
				++currentSeedValue;
				entries.clear();
				bool seedValid{ true };

				for ( const auto& k : subKeys )
				{
					// Calculate final position using secondary hash with current seed
					auto finalHash{ Hashing::seed( currentSeedValue, k.second, size ) };
					bool slotOccupied = indices[finalHash] != 0;
					bool entryClaimedThisTry = entries.count( finalHash ) != 0;

					if ( !slotOccupied && !entryClaimedThisTry )
					{
						entries[finalHash] = k.first;
					}
					else
					{
						seedValid = false;
						break;
					}
				}

				if ( seedValid )
				{
					break;
				}

				if ( currentSeedValue > size * constants::chd::MAX_SEED_SEARCH_MULTIPLIER )
				{
					std::ostringstream oss;
					oss << "Bucket " << currentBucketIdx << ": Seed search exceeded threshold (" << currentSeedValue << "), aborting construction!";
					throw std::runtime_error{ oss.str() };
				}
			}

			for ( const auto& [finalHash, itemIdx] : entries )
			{
				indices[finalHash] = itemIdx;
			}
			seeds[subKeys[0].second & ( size - 1 )] = static_cast<int>( currentSeedValue );
		}

		m_table.resize( size );
		m_seeds.resize( size, 0 );

		std::vector<size_t> freeSlots;
		freeSlots.reserve( size );

		for ( size_t i{ 0 }; i < size; ++i )
		{
			if ( i < indices.size() && indices[i] != 0 )
			{
				auto itemIndex = indices[i] - 1;
				m_table[i] = std::move( items[itemIndex] );
			}
			else
			{
				m_table[i] = { std::string{}, TValue{} };
				if ( i < indices.size() )
				{
					freeSlots.push_back( i );
				}
			}
		}

		size_t freeSlotsIndex{ 0 };
		for ( ; currentBucketIdx < hashBuckets.size() && !hashBuckets[currentBucketIdx].empty(); ++currentBucketIdx )
		{
			const auto& k{ hashBuckets[currentBucketIdx][0] };
			auto slotIndexInMTable{ freeSlots[freeSlotsIndex++] };
			auto itemIndex = k.first - 1;

			m_table[slotIndexInMTable] = std::move( items[itemIndex] );

			// Use negative seed to directly encode the final table index for single-item buckets
			seeds[k.second & ( size - 1 )] = -static_cast<int>( slotIndexInMTable + 1 );
		}

		m_seeds = std::move( seeds );
	}

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	template <typename TValue>
	NFX_CORE_INLINE TValue& ChdHashMap<TValue>::operator[]( std::string_view key )
	{
		if ( isEmpty() )
		{
			ThrowHelper::throwKeyNotFoundException( key );
		}

		uint32_t hashValue = hash( key );
		const size_t tableSize = m_table.size();
		const uint32_t index = hashValue & ( tableSize - 1 );
		const int seed = m_seeds[index];

		size_t finalIndex;
		if ( seed < 0 )
		{
			finalIndex = static_cast<size_t>( -seed - 1 );
		}
		else
		{
			finalIndex = Hashing::seed( static_cast<uint32_t>( seed ), hashValue, tableSize );
		}

		const auto& kvp = m_table[finalIndex];

		if ( key == kvp.first )
		{
			return const_cast<TValue&>( kvp.second );
		}

		ThrowHelper::throwKeyNotFoundException( key );
	}

	//----------------------------------------------
	// Lookup methods
	//----------------------------------------------

	template <typename TValue>
	inline const TValue& ChdHashMap<TValue>::at( std::string_view key ) const
	{
		if ( isEmpty() )
		{
			ThrowHelper::throwKeyNotFoundException( key );
		}

		const TValue* outValue = nullptr;
		if ( tryGetValue( key, outValue ) && outValue != nullptr )
		{
			return *outValue;
		}

		ThrowHelper::throwKeyNotFoundException( key );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	template <typename TValue>
	inline size_t ChdHashMap<TValue>::size() const noexcept
	{
		return m_table.size();
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	template <typename TValue>
	inline bool ChdHashMap<TValue>::isEmpty() const noexcept
	{
		return m_table.empty();
	}

	//----------------------------------------------
	// Static query methods
	//----------------------------------------------

	template <typename TValue>
	NFX_CORE_INLINE bool ChdHashMap<TValue>::tryGetValue( std::string_view key, const TValue*& outValue ) const noexcept
	{
		if ( isEmpty() )
		{
			outValue = nullptr;
			return false;
		}

		const uint32_t hashValue = hash( key );
		const size_t tableSize = m_table.size();
		const uint32_t index = hashValue & ( tableSize - 1 );
		const int seed = m_seeds[index];

		const size_t finalIndex = ( seed < 0 ) ? static_cast<size_t>( -seed - 1 )
											   : Hashing::seed( static_cast<uint32_t>( seed ), hashValue, tableSize );

		const auto& kvp = m_table[finalIndex];
		const size_t keyLen = key.size();
		const size_t storedLen = kvp.first.size();

		if ( keyLen == storedLen && !kvp.first.empty() && key == kvp.first )
		{
			outValue = &kvp.second;

			return true;
		}

		outValue = nullptr;

		return false;
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	template <typename TValue>
	inline typename ChdHashMap<TValue>::Iterator ChdHashMap<TValue>::begin() const noexcept
	{
		for ( size_t i{ 0 }; i < m_table.size(); ++i )
		{
			if ( !m_table[i].first.empty() )
			{
				return Iterator{ &m_table, i };
			}
		}

		return end();
	}

	template <typename TValue>
	inline typename ChdHashMap<TValue>::Iterator ChdHashMap<TValue>::end() const noexcept
	{
		return Iterator{ &m_table, m_table.size() };
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	template <typename TValue>
	inline typename ChdHashMap<TValue>::Enumerator ChdHashMap<TValue>::enumerator() const noexcept
	{
		return Enumerator{ &m_table };
	}

	//----------------------------------------------
	// Private helper methods
	//----------------------------------------------

	//---------------------------
	// Hashing
	//---------------------------

	template <typename TValue>
	NFX_CORE_INLINE uint32_t ChdHashMap<TValue>::hash( std::string_view key ) noexcept
	{
		if ( key.empty() )
		{
			return constants::chd::FNV_OFFSET_BASIS;
		}

		uint32_t hashValue = constants::chd::FNV_OFFSET_BASIS;
		size_t length = key.length();

		static const bool hasSSE42 = hasSSE42Support();

		if ( hasSSE42 )
		{
			// Use SSE4.2 CRC32 - process each character's low byte only
			for ( size_t i = 0; i < length; ++i )
			{
				hashValue = Hashing::crc32( hashValue, static_cast<uint8_t>( key[i] ) );
			}
		}
		else
		{
			// FNV-1a software fallback - process each character's low byte only
			for ( size_t i = 0; i < length; ++i )
			{
				hashValue = Hashing::fnv1a( hashValue, static_cast<uint8_t>( key[i] ) );
			}
		}

		return hashValue;
	}

	//----------------------------------------------
	// ChdHashMap::Iterator class
	//----------------------------------------------

	//---------------------------
	// Construction
	//---------------------------

	template <typename TValue>
	inline ChdHashMap<TValue>::Iterator::Iterator( const std::vector<std::pair<std::string, TValue>>* table, size_t index ) noexcept
		: m_table{ table },
		  m_index{ index }
	{
	}

	//---------------------------
	// Operations
	//---------------------------

	template <typename TValue>
	inline const std::pair<std::string, TValue>& ChdHashMap<TValue>::Iterator::operator*() const
	{
		if ( m_index >= m_table->size() )
		{
			std::ostringstream oss;
			oss << "Iterator: Dereference out of bounds (index: " << m_index << ", table size: " << m_table->size() << ")";
			throw std::runtime_error{ oss.str() };
		}

		return ( *m_table )[m_index];
	}

	template <typename TValue>
	inline const std::pair<std::string, TValue>* ChdHashMap<TValue>::Iterator::operator->() const
	{
		if ( m_index >= m_table->size() )
		{
			std::ostringstream oss;
			oss << "Iterator: Arrow operator out of bounds (index: " << m_index << ", table size: " << m_table->size() << ")";
			throw std::runtime_error{ oss.str() };
		}

		return &( ( *m_table )[m_index] );
	}

	template <typename TValue>
	inline typename ChdHashMap<TValue>::Iterator& ChdHashMap<TValue>::Iterator::operator++() noexcept
	{
		if ( m_table == nullptr )
		{
			return *this;
		}

		while ( ++m_index < m_table->size() )
		{
			const auto& entry{ ( *m_table )[m_index] };
			if ( !entry.first.empty() )
			{
				return *this;
			}
		}

		m_index = m_table->size();

		return *this;
	}

	template <typename TValue>
	inline typename ChdHashMap<TValue>::Iterator ChdHashMap<TValue>::Iterator::operator++( int ) noexcept
	{
		auto tmp{ Iterator{ *this } };
		++( *this );

		return tmp;
	}

	//---------------------------
	// Comparison
	//---------------------------

	template <typename TValue>
	inline bool ChdHashMap<TValue>::Iterator::operator==( const Iterator& other ) const noexcept
	{
		return m_table == other.m_table && m_index == other.m_index;
	}

	template <typename TValue>
	inline bool ChdHashMap<TValue>::Iterator::operator!=( const Iterator& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// ChdHashMap::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	template <typename TValue>
	inline ChdHashMap<TValue>::Enumerator::Enumerator( const std::vector<std::pair<std::string, TValue>>* table ) noexcept
		: m_table{ table },
		  m_index{ std::numeric_limits<size_t>::max() }
	{
	}

	//----------------------------
	// Enumeration
	//----------------------------

	template <typename TValue>
	inline bool ChdHashMap<TValue>::Enumerator::next() noexcept
	{
		do
		{
			++m_index;
		} while ( m_index < m_table->size() && ( *m_table )[m_index].first.empty() );

		return m_index < m_table->size();
	}

	template <typename TValue>
	inline const std::pair<std::string, TValue>& ChdHashMap<TValue>::Enumerator::current() const
	{
		if ( !m_table || m_index == SIZE_MAX || m_index >= m_table->size() )
		{
			ThrowHelper::throwInvalidOperationException();
		}

		return ( *m_table )[m_index];
	}

	template <typename TValue>
	inline void ChdHashMap<TValue>::Enumerator::reset() noexcept
	{
		m_index = std::numeric_limits<size_t>::max();
	}
}
