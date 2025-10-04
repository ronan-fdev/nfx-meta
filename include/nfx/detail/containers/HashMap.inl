/**
 * @file HashMap.inl
 * @brief Template implementation file for HashMap Robin Hood hashing container
 * @details Contains template method implementations for cache-friendly hash table
 *          with Robin Hood displacement algorithm, heterogeneous string lookup,
 *          and aggressive performance optimizations
 */

#include "nfx/config.h"

namespace nfx::containers
{
	//=====================================================================
	// HashMap class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::HashMap()
	{
		m_buckets.resize( INITIAL_CAPACITY );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::HashMap( size_t initialCapacity )
	{
		size_t capacity{ 1 };
		while ( capacity < initialCapacity )
		{
			capacity <<= 1;
		}
		m_capacity = capacity;
		m_mask = capacity - 1;
		m_buckets.resize( capacity );
	}

	//----------------------------------------------
	// Core operations
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	template <typename KeyType>
	NFX_CORE_INLINE bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::tryGetValue( const KeyType& key, TValue*& outValue ) noexcept
	{
		const std::uint32_t hash{ static_cast<std::uint32_t>( m_hasher( key ) ) };

		size_t pos{ hash & m_mask };

		for ( std::uint16_t distance = 0;; ++distance, pos = ( pos + 1 ) & m_mask )
		{
			Bucket& bucket{ m_buckets[pos] };

			// Check Robin Hood invariant and occupancy in single condition
			if ( !bucket.occupied || distance > bucket.distance )
			{
				outValue = nullptr;
				return false;
			}

			// Hot path: hash comparison first, then key equality
			if ( bucket.hash == hash && keysEqual( bucket.key, key ) )
			{
				outValue = &bucket.value;
				return true;
			}
		}
	}

	//----------------------------------------------
	// Insertion
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::insertOrAssign( const TKey& key, TValue&& value )
	{
		insertOrAssignInternal( key, std::forward<TValue>( value ) );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::insertOrAssign( const TKey& key, const TValue& value )
	{
		insertOrAssignInternal( key, value );
	}

	//----------------------------------------------
	// Capacity and memory management
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::reserve( size_t minCapacity )
	{
		if ( minCapacity > m_capacity )
		{
			size_t newCapacity{ 1 };
			while ( newCapacity < minCapacity )
			{
				newCapacity <<= 1;
			}

			if ( newCapacity > m_capacity )
			{
				std::vector<Bucket> oldBuckets{ std::move( m_buckets ) };
				const size_t oldCapacity{ m_capacity };

				m_capacity = newCapacity;
				m_mask = newCapacity - 1;
				m_buckets.clear();
				m_buckets.resize( newCapacity );
				m_size = 0;

				for ( size_t i = 0; i < oldCapacity; ++i )
				{
					if ( oldBuckets[i].occupied )
					{
						insertOrAssignInternal( std::move( oldBuckets[i].key ), std::move( oldBuckets[i].value ) );
					}
				}
			}
		}
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	template <typename KeyType>
	NFX_CORE_INLINE bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::erase( const KeyType& key ) noexcept
	{
		const std::uint32_t hash{ static_cast<std::uint32_t>( m_hasher( key ) ) };

		size_t pos{ hash & m_mask };
		std::uint16_t distance{ 0 };

		while ( distance <= m_buckets[pos].distance && m_buckets[pos].occupied )
		{
			if ( m_buckets[pos].hash == hash && keysEqual( m_buckets[pos].key, key ) )
			{
				eraseAtPosition( pos );
				--m_size;
				return true;
			}
			pos = ( pos + 1 ) & m_mask;
			++distance;
		}

		return false;
	}

	//----------------------------------------------
	// State insspection
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE size_t HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::size() const noexcept
	{
		return m_size;
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE size_t HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::capacity() const noexcept
	{
		return m_capacity;
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::isEmpty() const noexcept
	{
		return m_size == 0;
	}

	//----------------------------------------------
	// Internal implementation
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	template <typename ValueType>
	inline void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::insertOrAssignInternal( const TKey& key, ValueType&& value )
	{
		if ( shouldResize() )
		{
			resize();
		}

		const std::uint32_t hash{ static_cast<std::uint32_t>( m_hasher( key ) ) };

		size_t pos{ hash & m_mask };
		std::uint16_t distance{ 0 };

		// First pass: check for existing key or find insertion point
		while ( m_buckets[pos].occupied )
		{
			if ( m_buckets[pos].hash == hash && keysEqual( m_buckets[pos].key, key ) )
			{
				// Update existing key
				m_buckets[pos].value = std::forward<ValueType>( value );
				return;
			}

			if ( distance > m_buckets[pos].distance )
			{
				// Robin Hood: we need to displace this bucket
				break;
			}

			pos = ( pos + 1 ) & m_mask;
			++distance;
		}

		// If we're here, we need to insert a new bucket
		Bucket newBucket{ key, std::forward<ValueType>( value ), hash, distance, true };

		// Robin Hood displacement loop
		while ( m_buckets[pos].occupied )
		{
			if ( newBucket.distance > m_buckets[pos].distance )
			{
				std::swap( newBucket, m_buckets[pos] );
			}

			pos = ( pos + 1 ) & m_mask;
			++newBucket.distance;
		}

		// Insert the final bucket
		m_buckets[pos] = std::move( newBucket );
		++m_size;
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::shouldResize() const noexcept
	{
		return ( m_size * 100 ) >= ( m_capacity * MAX_LOAD_FACTOR_PERCENT );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	inline void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::resize()
	{
		const size_t oldCapacity{ m_capacity };
		m_capacity <<= 1;
		m_mask = m_capacity - 1;

		std::vector<Bucket> oldBuckets{ std::move( m_buckets ) };
		m_buckets.clear();
		m_buckets.resize( m_capacity );
		m_size = 0;

		for ( size_t i = 0; i < oldCapacity; ++i )
		{
			if ( oldBuckets[i].occupied )
			{
				insertOrAssignInternal( std::move( oldBuckets[i].key ), std::move( oldBuckets[i].value ) );
			}
		}
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE void HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::eraseAtPosition( size_t pos ) noexcept
	{
		size_t nextPos{ ( pos + 1 ) & m_mask };

		while ( m_buckets[nextPos].occupied && m_buckets[nextPos].distance > 0 )
		{
			m_buckets[pos] = std::move( m_buckets[nextPos] );
			--m_buckets[pos].distance; // Adjust distance!
			pos = nextPos;
			nextPos = ( nextPos + 1 ) & m_mask;
		}

		m_buckets[pos] = Bucket{};
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	template <typename KeyType1, typename KeyType2>
	NFX_CORE_INLINE bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::keysEqual( const KeyType1& k1, const KeyType2& k2 ) const noexcept
	{
		if constexpr ( std::is_same_v<KeyType1, std::string> && std::is_same_v<KeyType2, std::string_view> )
		{
			return StringViewEqual{}( k1, k2 );
		}
		else if constexpr ( std::is_same_v<KeyType1, std::string_view> && std::is_same_v<KeyType2, std::string> )
		{
			return StringViewEqual{}( k1, k2 );
		}
		else
		{
			return k1 == k2;
		}
	}

	//----------------------------------------------
	// STL-compatible iteration support
	//----------------------------------------------

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	typename HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::iterator
	HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::begin() noexcept
	{
		return iterator( m_buckets.data(), m_buckets.data() + m_capacity );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	typename HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::const_iterator
	HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::begin() const noexcept
	{
		return const_iterator( m_buckets.data(), m_buckets.data() + m_capacity );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	typename HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::iterator
	HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::end() noexcept
	{
		return iterator( m_buckets.data() + m_capacity, m_buckets.data() + m_capacity );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	typename HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::const_iterator
	HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::end() const noexcept
	{
		return const_iterator( m_buckets.data() + m_capacity, m_buckets.data() + m_capacity );
	}

	template <typename TKey, typename TValue, uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	bool HashMap<TKey, TValue, FnvOffsetBasis, FnvPrime>::operator==( const HashMap& other ) const noexcept
	{
		if ( m_size != other.m_size )
		{
			return false;
		}

		// Compare all key-value pairs
		for ( const auto& pair : *this )
		{
			// Find key in other HashMap and compare values
			auto it = std::find_if( other.begin(), other.end(),
				[&pair]( const auto& otherPair ) {
					return pair.first == otherPair.first;
				} );

			if ( it == other.end() || it->second != pair.second )
			{
				return false;
			}
		}

		return true;
	}
} // namespace nfx::containers
