/**
 * @file StringViewSplitter.h
 * @brief Zero-allocation string splitting utilities for high-performance string processing
 * @details Provides efficient string_view-based splitting without heap allocations,
 *          perfect for performance-critical paths and CSV parsing applications
 */

#pragma once

#include <string_view>

#include "nfx/config.h"

namespace nfx::string
{
	//=====================================================================
	// StringViewSplitter class
	//=====================================================================

	/**
	 * @brief Zero-allocation string splitting iterator for performance-critical paths
	 * @details Provides efficient string_view-based splitting without heap allocations
	 */
	class StringViewSplitter
	{
	public:
		//----------------------------------------------
		// Forward declarations
		//----------------------------------------------

		class Iterator;

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs a StringViewSplitter for the given string and delimiter
		 */
		NFX_CORE_INLINE explicit StringViewSplitter( std::string_view str, char delimiter ) noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Returns iterator to first segment
		 */
		NFX_CORE_INLINE Iterator begin() const noexcept;

		/**
		 * @brief Returns end iterator for range-based loops
		 */
		NFX_CORE_INLINE Iterator end() const noexcept;

		//----------------------------------------------
		// StringViewSplitter::Iterator class
		//----------------------------------------------

		/**
		 * @brief Forward iterator for string segments
		 */
		class Iterator
		{
		public:
			//-----------------------------
			// Construction
			//-----------------------------

			/**
			 * @brief Constructs iterator at beginning or end position
			 */
			NFX_CORE_INLINE explicit Iterator( const StringViewSplitter* splitter, bool at_end = false ) noexcept;

			//-----------------------------
			// Iterator operators
			//-----------------------------

			/**
			 * @brief Dereferences iterator to get current string segment
			 */
			NFX_CORE_INLINE std::string_view operator*() const noexcept;

			/**
			 * @brief Pre-increment operator to advance to next segment
			 */
			NFX_CORE_INLINE Iterator& operator++() noexcept;

			//-----------------------------
			// Comparison operators
			//-----------------------------

			/**
			 * @brief Compares iterators for equality
			 */
			NFX_CORE_INLINE bool operator==( const Iterator& other ) const noexcept;

			/**
			 * @brief Compares iterators for range-based loops
			 */
			NFX_CORE_INLINE bool operator!=( const Iterator& other ) const noexcept;

		private:
			//-----------------------------
			// Private methods
			//-----------------------------

			/**
			 * @brief Advances to next segment using efficient string_view operations
			 */
			NFX_CORE_INLINE void advance() noexcept;

		private:
			//-----------------------------
			// Private member variables
			//-----------------------------

			const StringViewSplitter* m_splitter;
			size_t m_currentPos;
			std::string_view m_currentSegment;
			bool m_isAtEnd;
		};

	private:
		std::string_view m_str;
		char m_delimiter;
	};

	//=====================================================================
	// String splitting factory functions
	//=====================================================================

	/**
	 * @brief Factory function for zero-copy string splitting
	 * @details Creates a StringViewSplitter for efficient iteration over string segments
	 *          without heap allocations.
	 * @param str String to split
	 * @param delimiter Character to split on
	 * @return StringViewSplitter object for range-based iteration
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] static NFX_CORE_INLINE StringViewSplitter splitView( std::string_view str, char delimiter ) noexcept;
}

#include "StringViewSplitter.inl"
