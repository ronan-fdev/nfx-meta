/**
 * @file LruCache.h
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
	// LruCacheOptions struct
	//=====================================================================

	/**
	 * @brief Configuration options for LruCache behavior
	 */
	struct LruCacheOptions final
	{
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct LruCacheOptions with specified parameters
		 * @param sizeLimit Maximum number of entries (0 = unlimited)
		 * @param slidingExpiration Default expiration time after last access
		 * @param backgroundCleanupInterval Interval for automatic expired entry cleanup (0 = disabled)
		 */
		NFX_CORE_INLINE LruCacheOptions(
			std::size_t sizeLimit = 0,
			std::chrono::milliseconds slidingExpiration = std::chrono::hours{ 1 },
			std::chrono::milliseconds backgroundCleanupInterval = std::chrono::milliseconds{ 0 } );

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get the maximum number of cache entries allowed
		 * @return Size limit (0 = unlimited)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE std::size_t sizeLimit() const;

		/**
		 * @brief Get the default sliding expiration time
		 * @return Sliding expiration duration after last access
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE std::chrono::milliseconds slidingExpiration() const;

		/**
		 * @brief Get the background cleanup interval
		 * @return Cleanup interval (0 = disabled)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] NFX_CORE_INLINE std::chrono::milliseconds backgroundCleanupInterval() const;

	private:
		/** Maximum number of entries allowed in cache (0 = unlimited) */
		std::size_t m_sizeLimit{ 0 };

		/** Default time after last access before entries expire */
		std::chrono::milliseconds m_slidingExpiration{ std::chrono::minutes{ 60 } };

		/*
		 * Background cleanup design:
		 * - When enabled (interval > 0), cache tracks last cleanup time
		 * - During getOrCreate/tryGet operations, checks if cleanup interval has elapsed
		 * - If elapsed, performs incremental cleanup of expired entries
		 * - Amortizes cleanup cost across normal operations without requiring separate thread
		 * - Ideal for write-heavy scenarios with unique keys (logging, batch processing)
		 * - For very low-activity caches, still requires occasional manual cleanupExpired() calls
		 */
		std::chrono::milliseconds m_backgroundCleanupInterval{ std::chrono::milliseconds{ 0 } };
	};

	//=====================================================================
	// CacheEntry struct
	//=====================================================================

	/** @brief Cache entry metadata with intrusive LRU list support */
	struct CacheEntry final
	{
		/** @brief Timestamp of the last access to this cache entry */
		std::chrono::steady_clock::time_point lastAccessed;

		/** @brief Sliding expiration time for this specific entry */
		std::chrono::milliseconds slidingExpiration;

		/** @brief Size of this cache entry for memory accounting */
		std::size_t size{ 1 };

		/** @brief Previous entry in the LRU doubly-linked list */
		CacheEntry* lruPrev{ nullptr };

		/** @brief Next entry in the LRU doubly-linked list */
		CacheEntry* lruNext{ nullptr };

		/** @brief Pointer to the key for this cache entry */
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
	// LruCache class
	//=====================================================================

	/**
	 * @brief Thread-safe memory cache with size limits and expiration policies
	 * @tparam TKey Key type for cache entries
	 * @tparam TValue Value type for cached objects
	 */
	template <typename TKey, typename TValue>
	class LruCache final
	{
	public:
		//----------------------------------------------
		// Type aliases
		//----------------------------------------------

		/** @brief Function type for creating cache values when not found */
		using FactoryFunction = std::function<TValue()>;

		/** @brief Function type for configuring cache entry metadata */
		using ConfigFunction = std::function<void( CacheEntry& )>;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct memory cache with specified options
		 * @param options Configuration options for cache behavior
		 */
		NFX_CORE_INLINE explicit LruCache( const LruCacheOptions& options = {} );

		//----------------------------------------------
		// Copy and move operations
		//----------------------------------------------

		LruCache( const LruCache& ) = delete;
		LruCache( LruCache&& ) = delete;

		//----------------------------------------------
		// Assignment operations
		//----------------------------------------------

		LruCache& operator=( const LruCache& ) = delete;
		LruCache& operator=( LruCache&& ) = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		// Default destructor
		~LruCache() = default;

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
		// Background cleanup
		//----------------------------------------------

		/**
		 * @brief Maximum number of expired entries to clean up per opportunistic cleanup cycle
		 * @details Limits cleanup work per operation to prevent blocking normal cache access.
		 *          This ensures cleanup cost remains bounded and amortized across operations.
		 */
		static constexpr size_t MAX_CLEANUP_PER_CYCLE = 10;

		/**
		 * @brief Check if background cleanup should run and perform it if needed
		 * @details Called during normal operations to amortize cleanup cost
		 */
		NFX_CORE_INLINE void checkAndPerformBackgroundCleanup() const;

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
		LruCacheOptions m_options;

		/** @brief Head of the LRU doubly-linked list (most recently used) */
		CacheEntry* m_lruHead;

		/** @brief Tail of the LRU doubly-linked list (least recently used) */
		CacheEntry* m_lruTail;

		/** @brief Last time background cleanup was performed */
		mutable std::chrono::steady_clock::time_point m_lastCleanupTime;

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
} // namespace nfx::memory

#include "nfx/detail/memory/LruCache.inl"
