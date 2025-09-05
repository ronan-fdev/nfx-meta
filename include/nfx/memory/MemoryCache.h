/**
 * @file MemoryCache.h
 * @brief Thread-safe LRU cache with sliding expiration (inspired by .NET IMemoryCache)
 */

#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "nfx/config.h"

namespace nfx::memory
{
	//=====================================================================
	// MemoryCacheOptions struct
	//=====================================================================

	/**
	 * @brief Configuration options for MemoryCache behavior
	 */
	struct MemoryCacheOptions final
	{
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct MemoryCacheOptions with specified parameters
		 * @param sizeLimit Maximum number of entries (0 = unlimited)
		 * @param slidingExpiration Default expiration time after last access
		 */
		NFX_CORE_INLINE MemoryCacheOptions( std::size_t sizeLimit = 0, std::chrono::milliseconds slidingExpiration = std::chrono::hours{ 1 } );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		NFX_CORE_INLINE std::size_t sizeLimit() const;

		NFX_CORE_INLINE std::chrono::milliseconds slidingExpiration() const;

	private:
		/** Maximum number of entries allowed in cache (0 = unlimited) */
		std::size_t m_sizeLimit{ 0 };

		/** Default time after last access before entries expire */
		std::chrono::milliseconds m_slidingExpiration{ std::chrono::minutes{ 60 } };

		/*
		 * TODO: Implement automatic background cleanup (std::chrono::milliseconds expirationScanFrequency{ 0 };)
		 * Currently expired entries only get cleaned up when accessed or when cache hits size limit.
		 * For applications with unique keys (logging, batch processing), expired entries can accumulate.
		 * This feature would add periodic cleanup to prevent memory growth in low-reaccess scenarios.
		 * Workaround: Set sizeLimit > 0 or call cleanupExpired() manually.
		 */
	};

	//=====================================================================
	// CacheEntry struct
	//=====================================================================

	/** @brief Cache entry metadata with intrusive LRU list support */
	struct CacheEntry final
	{
		std::chrono::steady_clock::time_point lastAccessed;
		std::chrono::milliseconds slidingExpiration;
		std::size_t size{ 1 };

		CacheEntry* lruPrev{ nullptr };
		CacheEntry* lruNext{ nullptr };
		const void* keyPtr{ nullptr };

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct cache entry with specified expiration time
		 * @param expiration Sliding expiration time for this entry
		 */
		NFX_CORE_INLINE CacheEntry( std::chrono::milliseconds expiration = std::chrono::hours( 1 ) );

		//----------------------------------------------
		// Expiration checking
		//----------------------------------------------

		/**
		 * @brief Check if this cache entry has expired based on sliding expiration
		 * @return True if the entry has expired and should be evicted, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE bool isExpired() const noexcept;

		//----------------------------------------------
		// Access management
		//----------------------------------------------

		/**
		 * @brief Update the last accessed timestamp to current time
		 * @details Resets the sliding expiration timer for this cache entry
		 */
		void NFX_CORE_INLINE updateAccess() noexcept;
	};

	//=====================================================================
	// MemoryCache class
	//=====================================================================

	/**
	 * @brief Thread-safe memory cache with size limits and expiration policies
	 * @tparam TKey Key type for cache entries
	 * @tparam TValue Value type for cached objects
	 */
	template <typename TKey, typename TValue>
	class MemoryCache final
	{
	public:
		//----------------------------------------------
		// Type aliases
		//----------------------------------------------

		using FactoryFunction = std::function<TValue()>;
		using ConfigFunction = std::function<void( CacheEntry& )>;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct memory cache with specified options
		 * @param options Configuration options for cache behavior
		 */
		NFX_CORE_INLINE explicit MemoryCache( const MemoryCacheOptions& options = {} );

		//----------------------------------------------
		// Copy and move operations
		//----------------------------------------------

		MemoryCache( const MemoryCache& ) = delete;
		MemoryCache( MemoryCache&& ) = delete;

		//----------------------------------------------
		// Assignment operations
		//----------------------------------------------

		MemoryCache& operator=( const MemoryCache& ) = delete;
		MemoryCache& operator=( MemoryCache&& ) = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		// Default destructor
		~MemoryCache() = default;

		//----------------------------------------------
		// Cache operations
		//----------------------------------------------

		/**
		 * @brief Get or create a cache entry using factory function
		 * @param key The cache key
		 * @param factory Function to create the value if not cached
		 * @param configure Optional function to configure cache entry
		 * @return Reference to the cached value
		 */
		NFX_CORE_INLINE TValue& getOrCreate( const TKey& key, FactoryFunction factory, ConfigFunction configure = nullptr );

		//----------------------------------------------
		// Lookup operations
		//----------------------------------------------

		/**
		 * @brief Try to get a cached value without creating it
		 * @param key The cache key
		 * @return Optional containing the value if found and not expired
		 */
		NFX_CORE_INLINE std::optional<std::reference_wrapper<TValue>> tryGet( const TKey& key );

		//----------------------------------------------
		// Modification operations
		//----------------------------------------------

		/**
		 * @brief Remove an entry from the cache
		 * @param key The cache key to remove
		 * @return True if entry was removed, false if not found
		 */
		NFX_CORE_INLINE bool remove( const TKey& key );

		/**
		 * @brief Clear all cache entries
		 */
		NFX_CORE_INLINE void clear();

		/**
		 * @brief Get current cache size
		 * @return Number of entries in cache
		 */
		NFX_CORE_INLINE std::size_t size() const;

		//----------------------------------------------
		// State inspection
		//----------------------------------------------

		/**
		 * @brief Check if cache is empty
		 * @return True if cache contains no entries
		 */
		NFX_CORE_INLINE bool isEmpty() const;

		/**
		 * @brief Manually trigger cleanup of expired entries
		 */
		NFX_CORE_INLINE void cleanupExpired();

	private:
		//----------------------------------------------
		// Internal data structures
		//----------------------------------------------

		/** @brief Internal cache item containing value and metadata */
		struct CachedItem
		{
			/** @brief The cached value */
			TValue value;

			/** @brief Cache entry metadata and LRU information */
			CacheEntry metadata;

			/** @brief Construct cache item with value and metadata */
			CachedItem( TValue val, CacheEntry meta );
		};

		mutable std::mutex m_mutex;
		std::unordered_map<TKey, CachedItem> m_cache;
		MemoryCacheOptions m_options;

		/** @brief Head of the LRU doubly-linked list (most recently used) */
		CacheEntry* m_lruHead;

		/** @brief Tail of the LRU doubly-linked list (least recently used) */
		CacheEntry* m_lruTail;

		//----------------------------------------------
		// LRU list management
		//----------------------------------------------

		/**
		 * @brief Add entry to head of LRU list (most recently used)
		 * @param entry Entry to add to LRU list head
		 */
		NFX_CORE_INLINE void addToLruHead( CacheEntry* entry ) noexcept;

		/**
		 * @brief Remove entry from LRU list
		 * @param entry Entry to remove from LRU list
		 */
		NFX_CORE_INLINE void removeFromLru( CacheEntry* entry ) noexcept;

		/**
		 * @brief Move entry to head of LRU list (mark as most recently used)
		 * @param entry Entry to move to LRU list head
		 */
		NFX_CORE_INLINE void moveToLruHead( CacheEntry* entry ) noexcept;

		/**
		 * @brief Evict least recently used entry in O(1) time
		 */
		NFX_CORE_INLINE void evictLeastRecentlyUsed();
	};
}

#include "MemoryCache.inl"
