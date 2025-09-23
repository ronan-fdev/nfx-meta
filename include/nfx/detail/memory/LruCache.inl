/**
 * @file LruCache.inl
 * @brief Implementation of LruCache template methods
 * @details Template method implementations for thread-safe memory cache
 *          with LRU eviction and configurable expiration policies
 */

namespace nfx::memory
{
	//=====================================================================
	// LruCacheOptions
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	NFX_CORE_INLINE LruCacheOptions::LruCacheOptions(
		std::size_t sizeLimit,
		std::chrono::milliseconds defaultSlidingExpiration,
		std::chrono::milliseconds backgroundCleanupInterval )
		: m_sizeLimit{ sizeLimit },
		  m_slidingExpiration{ defaultSlidingExpiration },
		  m_backgroundCleanupInterval{ backgroundCleanupInterval }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	NFX_CORE_INLINE std::size_t LruCacheOptions::sizeLimit() const
	{
		return m_sizeLimit;
	}

	NFX_CORE_INLINE std::chrono::milliseconds LruCacheOptions::slidingExpiration() const
	{
		return m_slidingExpiration;
	}

	NFX_CORE_INLINE std::chrono::milliseconds LruCacheOptions::backgroundCleanupInterval() const
	{
		return m_backgroundCleanupInterval;
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
	// LruCache
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE LruCache<TKey, TValue>::LruCache( const LruCacheOptions& options )
		: m_options{ options },
		  m_lruHead{ nullptr },
		  m_lruTail{ nullptr },
		  m_lastCleanupTime{ std::chrono::steady_clock::now() }
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
	NFX_CORE_INLINE TValue& LruCache<TKey, TValue>::getOrCreate( const TKey& key, FactoryFunction factory, ConfigFunction configure )
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		// Check for background cleanup opportunity
		checkAndPerformBackgroundCleanup();

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
	NFX_CORE_INLINE std::optional<std::reference_wrapper<TValue>> LruCache<TKey, TValue>::tryGet( const TKey& key )
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		// Check for background cleanup opportunity
		checkAndPerformBackgroundCleanup();

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
	NFX_CORE_INLINE bool LruCache<TKey, TValue>::remove( const TKey& key )
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
	NFX_CORE_INLINE void LruCache<TKey, TValue>::clear()
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		m_cache.clear();
		m_lruHead = nullptr;
		m_lruTail = nullptr;
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE std::size_t LruCache<TKey, TValue>::size() const
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		return m_cache.size();
	}

	//----------------------------------------------
	// State inspection
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE bool LruCache<TKey, TValue>::isEmpty() const
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		return m_cache.empty();
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void LruCache<TKey, TValue>::cleanupExpired()
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
	LruCache<TKey, TValue>::CachedItem::CachedItem( TValue val, CacheEntry meta )
		: value{ std::move( val ) },
		  metadata{ std::move( meta ) }
	{
	}

	//----------------------------------------------
	// LRU list management
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void LruCache<TKey, TValue>::addToLruHead( CacheEntry* entry ) noexcept
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
	NFX_CORE_INLINE void LruCache<TKey, TValue>::removeFromLru( CacheEntry* entry ) noexcept
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
	NFX_CORE_INLINE void LruCache<TKey, TValue>::moveToLruHead( CacheEntry* entry ) noexcept
	{
		if ( entry == m_lruHead )
		{
			return;
		}

		removeFromLru( entry );
		addToLruHead( entry );
	}

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void LruCache<TKey, TValue>::evictLeastRecentlyUsed()
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

	//----------------------------------------------
	// Background cleanup implementation
	//----------------------------------------------

	template <typename TKey, typename TValue>
	NFX_CORE_INLINE void LruCache<TKey, TValue>::checkAndPerformBackgroundCleanup() const
	{
		// Skip if background cleanup is disabled
		if ( m_options.backgroundCleanupInterval().count() <= 0 )
		{
			return;
		}

		auto now = std::chrono::steady_clock::now();
		auto timeSinceLastCleanup = now - m_lastCleanupTime;

		if ( timeSinceLastCleanup >= m_options.backgroundCleanupInterval() )
		{
			m_lastCleanupTime = now;

			/*
			 * Perform incremental cleanup of expired entries
			 * Note: We need to cast away const since we're modifying the cache
			 * This is safe because the method is called from within locked operations
			 */
			auto* self = const_cast<LruCache<TKey, TValue>*>( this );

			size_t cleanedCount = 0;

			auto it = self->m_cache.begin();
			while ( it != self->m_cache.end() && cleanedCount < MAX_CLEANUP_PER_CYCLE )
			{
				if ( it->second.metadata.isExpired() )
				{
					self->removeFromLru( &it->second.metadata );
					it = self->m_cache.erase( it );
					++cleanedCount;
				}
				else
				{
					++it;
				}
			}
		}
	}
}
