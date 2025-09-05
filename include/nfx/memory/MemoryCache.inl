/**
 * @file MemoryCache.inl
 * @brief Implementation of MemoryCache template methods
 * @details Template method implementations for thread-safe memory cache
 *          with LRU eviction and configurable expiration policies
 */

namespace nfx::memory
{
	//=====================================================================
	// MemoryCacheOptions
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	NFX_CORE_INLINE MemoryCacheOptions::MemoryCacheOptions( std::size_t sizeLimit, std::chrono::milliseconds defaultSlidingExpiration )
		: m_sizeLimit{ sizeLimit },
		  m_slidingExpiration{ defaultSlidingExpiration }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	NFX_CORE_INLINE std::size_t MemoryCacheOptions::sizeLimit() const
	{
		return m_sizeLimit;
	}

	NFX_CORE_INLINE std::chrono::milliseconds MemoryCacheOptions::slidingExpiration() const
	{
		return m_slidingExpiration;
	}

	//=====================================================================
	// CacheEntry
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	NFX_CORE_INLINE CacheEntry::CacheEntry( std::chrono::milliseconds expiration )
		: lastAccessed{ std::chrono::steady_clock::now() },
		  slidingExpiration{ expiration }
	{
	}

	//----------------------------------------------
	// Expiration checking
	//----------------------------------------------

	/**
	 * @brief Check if this cache entry has expired based on sliding expiration
	 * @return True if the entry has expired and should be evicted, false otherwise
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	NFX_CORE_INLINE bool CacheEntry::isExpired() const noexcept
	{
		auto now{ std::chrono::steady_clock::now() };
		return ( now - lastAccessed ) > slidingExpiration;
	}

	//----------------------------------------------
	// Access management
	//----------------------------------------------

	void NFX_CORE_INLINE CacheEntry::updateAccess() noexcept
	{
		lastAccessed = std::chrono::steady_clock::now();
	}

	//=====================================================================
	// MemoryCache
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE MemoryCache<TKey, TValue>::MemoryCache( const MemoryCacheOptions& options )
		: m_options{ options },
		  m_lruHead{ nullptr },
		  m_lruTail{ nullptr }
	{
		if ( m_options.sizeLimit() > 0 )
		{
			m_cache.reserve( m_options.sizeLimit() );
		}
	}

	//----------------------------------------------
	// Cache operations
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE TValue& MemoryCache<TKey, TValue>::getOrCreate( const TKey& key, FactoryFunction factory, ConfigFunction configure )
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		auto it = m_cache.find( key );
		if ( it != m_cache.end() )
		{
			if ( !it->second.metadata.isExpired() )
			{
				it->second.metadata.updateAccess();	   // Reset expiration
				moveToLruHead( &it->second.metadata ); // Mark as recent

				return it->second.value;
			}
			else
			{
				removeFromLru( &it->second.metadata ); // Clean expired

				m_cache.erase( it );
			}
		}

		TValue value{ factory() };
		CacheEntry metadata{ m_options.slidingExpiration() };

		if ( configure )
		{
			configure( metadata );
		}

		if ( m_options.sizeLimit() > 0 && m_cache.size() >= m_options.sizeLimit() )
		{
			evictLeastRecentlyUsed();
		}

		auto [insert_it, inserted]{ m_cache.try_emplace( key, std::move( value ), std::move( metadata ) ) };
		insert_it->second.metadata.keyPtr = &insert_it->first;
		addToLruHead( &insert_it->second.metadata );

		return insert_it->second.value;
	}

	//----------------------------------------------
	// Lookup operations
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE std::optional<std::reference_wrapper<TValue>> MemoryCache<TKey, TValue>::tryGet( const TKey& key )
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		auto it{ m_cache.find( key ) };
		if ( it != m_cache.end() && !it->second.metadata.isExpired() )
		{
			it->second.metadata.updateAccess();
			moveToLruHead( &it->second.metadata );

			return std::ref( it->second.value );
		}

		if ( it != m_cache.end() )
		{
			removeFromLru( &it->second.metadata );

			m_cache.erase( it );
		}

		return std::nullopt;
	}

	//----------------------------------------------
	// Modification operations
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE bool MemoryCache<TKey, TValue>::remove( const TKey& key )
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		auto it = m_cache.find( key );
		if ( it != m_cache.end() )
		{
			removeFromLru( &it->second.metadata );
			m_cache.erase( it );
			return true;
		}

		return false;
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::clear()
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		m_cache.clear();
		m_lruHead = nullptr;
		m_lruTail = nullptr;
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE std::size_t MemoryCache<TKey, TValue>::size() const
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		return m_cache.size();
	}

	//----------------------------------------------
	// State inspection
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE bool MemoryCache<TKey, TValue>::isEmpty() const
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		return m_cache.empty();
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::cleanupExpired()
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		auto it = m_cache.begin();
		while ( it != m_cache.end() )
		{
			if ( it->second.metadata.isExpired() )
			{
				removeFromLru( &it->second.metadata );
				it = m_cache.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	//----------------------------------------------
	// Internal data structures
	//----------------------------------------------

	template <typename TKey, typename TValue>
	MemoryCache<TKey, TValue>::CachedItem::CachedItem( TValue val, CacheEntry meta )
		: value{ std::move( val ) },
		  metadata{ std::move( meta ) }
	{
	}

	//----------------------------------------------
	// LRU list management
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::addToLruHead( CacheEntry* entry ) noexcept
	{
		entry->lruNext = m_lruHead;
		entry->lruPrev = nullptr;

		if ( m_lruHead != nullptr )
		{
			m_lruHead->lruPrev = entry;
		}
		else
		{
			m_lruTail = entry;
		}

		m_lruHead = entry;
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::removeFromLru( CacheEntry* entry ) noexcept
	{
		if ( entry->lruPrev != nullptr )
		{
			entry->lruPrev->lruNext = entry->lruNext;
		}
		else
		{
			m_lruHead = entry->lruNext;
		}

		if ( entry->lruNext != nullptr )
		{
			entry->lruNext->lruPrev = entry->lruPrev;
		}
		else
		{
			m_lruTail = entry->lruPrev;
		}

		entry->lruNext = nullptr;
		entry->lruPrev = nullptr;
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::moveToLruHead( CacheEntry* entry ) noexcept
	{
		if ( entry == m_lruHead )
		{
			return;
		}

		removeFromLru( entry );
		addToLruHead( entry );
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void MemoryCache<TKey, TValue>::evictLeastRecentlyUsed()
	{
		if ( m_lruTail == nullptr )
		{
			return;
		}

		const TKey* keyPtr{ static_cast<const TKey*>( m_lruTail->keyPtr ) };
		if ( keyPtr != nullptr )
		{
			removeFromLru( m_lruTail );
			m_cache.erase( *keyPtr );
		}
	}
}
