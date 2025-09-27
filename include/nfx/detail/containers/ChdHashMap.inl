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

namespace nfx::containers
{
	//=====================================================================
	// ChdHashMap class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::ChdHashMap( std::vector<std::pair<std::string, TValue>>&& items )
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
					auto finalHash{ core::hashing::seedMix( currentSeedValue, k.second, size ) };
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

				if ( currentSeedValue > size * MAX_SEED_SEARCH_MULTIPLIER )
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

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE TValue& ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::operator[]( std::string_view key )
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
			finalIndex = core::hashing::seedMix( static_cast<uint32_t>( seed ), hashValue, tableSize );
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

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline const TValue& ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::at( std::string_view key )
	{
		if ( isEmpty() )
		{
			ThrowHelper::throwKeyNotFoundException( key );
		}

		TValue* outValue = nullptr;
		if ( tryGetValue( key, outValue ) && outValue != nullptr )
		{
			return *outValue;
		}

		ThrowHelper::throwKeyNotFoundException( key );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline size_t ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::size() const noexcept
	{
		return m_table.size();
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline bool ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::isEmpty() const noexcept
	{
		return m_table.empty();
	}

	//----------------------------------------------
	// Static query methods
	//----------------------------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE bool ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::tryGetValue( std::string_view key, TValue*& outValue ) noexcept
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
											   : core::hashing::seedMix( static_cast<uint32_t>( seed ), hashValue, tableSize );

		auto& kvp = m_table[finalIndex];
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

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline typename ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::begin() const noexcept
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

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline typename ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::end() const noexcept
	{
		return Iterator{ &m_table, m_table.size() };
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline typename ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Enumerator ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::enumerator() const noexcept
	{
		return Enumerator{ &m_table };
	}

	//---------------------------
	// Hashing
	//---------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE uint32_t ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::hash( std::string_view key ) noexcept
	{
		return core::hashing::hashStringView<FnvOffsetBasis, FnvPrime>( key );
	}

	//----------------------------------------------
	// Exception classes
	//----------------------------------------------

	//----------------------------
	// ChdHashMap::KeyNotFoundException
	//----------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::KeyNotFoundException::KeyNotFoundException( std::string_view key )
		: std::runtime_error{ std::string{ "No value associated to key: " } + std::string{ key } }
	{
	}

	//----------------------------
	// ChdHashMap::InvalidOperationException
	//----------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::InvalidOperationException::InvalidOperationException()
		: std::runtime_error{ "Operation is not valid due to the current state of the object." }
	{
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::InvalidOperationException::InvalidOperationException( std::string_view message )
		: std::runtime_error{ std::string{ message } }
	{
	}

	//----------------------------------------------
	// ChdHashMap::Iterator class
	//----------------------------------------------

	//---------------------------
	// Construction
	//---------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::Iterator( const std::vector<std::pair<std::string, TValue>>* table, size_t index ) noexcept
		: m_table{ table },
		  m_index{ index }
	{
	}

	//---------------------------
	// Operations
	//---------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline const std::pair<std::string, TValue>& ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator*() const
	{
		if ( m_index >= m_table->size() )
		{
			std::ostringstream oss;
			oss << "Iterator: Dereference out of bounds (index: " << m_index << ", table size: " << m_table->size() << ")";
			throw std::runtime_error{ oss.str() };
		}

		return ( *m_table )[m_index];
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline const std::pair<std::string, TValue>* ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator->() const
	{
		if ( m_index >= m_table->size() )
		{
			std::ostringstream oss;
			oss << "Iterator: Arrow operator out of bounds (index: " << m_index << ", table size: " << m_table->size() << ")";
			throw std::runtime_error{ oss.str() };
		}

		return &( ( *m_table )[m_index] );
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline typename ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator& ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator++() noexcept
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

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline typename ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator++( int ) noexcept
	{
		auto tmp{ Iterator{ *this } };
		++( *this );

		return tmp;
	}

	//---------------------------
	// Comparison
	//---------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline bool ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator==( const Iterator& other ) const noexcept
	{
		return m_table == other.m_table && m_index == other.m_index;
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline bool ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Iterator::operator!=( const Iterator& other ) const noexcept
	{
		return !( *this == other );
	}

	//----------------------------------------------
	// ChdHashMap::Enumerator class
	//----------------------------------------------

	//----------------------------
	// Construction
	//----------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Enumerator::Enumerator( const std::vector<std::pair<std::string, TValue>>* table ) noexcept
		: m_table{ table },
		  m_index{ std::numeric_limits<size_t>::max() }
	{
	}

	//----------------------------
	// Enumeration
	//----------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline bool ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Enumerator::next() noexcept
	{
		do
		{
			++m_index;
		} while ( m_index < m_table->size() && ( *m_table )[m_index].first.empty() );

		return m_index < m_table->size();
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline const std::pair<std::string, TValue>& ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Enumerator::current() const
	{
		if ( !m_table || m_index == SIZE_MAX || m_index >= m_table->size() )
		{
			ThrowHelper::throwInvalidOperationException();
		}

		return ( *m_table )[m_index];
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline void ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::Enumerator::reset() noexcept
	{
		m_index = std::numeric_limits<size_t>::max();
	}

	//----------------------------------------------
	// ChdHashMap::ThrowHelper
	//----------------------------------------------

	//----------------------------
	// Static exception methods
	//----------------------------

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline void ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::ThrowHelper::throwKeyNotFoundException( std::string_view key )
	{
		throw KeyNotFoundException{ key };
	}

	template <typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline void ChdHashMap<TValue, FnvOffsetBasis, FnvPrime>::ThrowHelper::throwInvalidOperationException()
	{
		throw InvalidOperationException{};
	}
}
