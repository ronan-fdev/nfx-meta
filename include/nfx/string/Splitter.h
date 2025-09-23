/**
 * @file Splitter.h
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
	// Splitter class
	//=====================================================================

	/**
	 * @brief Zero-allocation string splitting iterator for performance-critical paths
	 * @details Provides efficient string_view-based splitting without heap allocations
	 */
	class Splitter
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
		 * @brief Constructs a Splitter for the given string and delimiter
		 * @details Accepts any string-like type that can be converted to std::string_view
		 * @tparam StringType Any type convertible to std::string_view (std::string, const char*, etc.)
		 * @param str String to split
		 * @param delimiter Character to split on
		 */
		template <typename String>
		NFX_CORE_INLINE explicit Splitter( String&& str, char delimiter ) noexcept;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Returns iterator to first segment
		 * @return Iterator pointing to the first string segment
		 */
		NFX_CORE_INLINE Iterator begin() const noexcept;

		/**
		 * @brief Returns end iterator for range-based loops
		 * @return End iterator for range-based iteration
		 */
		NFX_CORE_INLINE Iterator end() const noexcept;

		//----------------------------------------------
		// Splitter::Iterator class
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
			 * @param splitter Reference to the parent Splitter object
			 * @param at_end Whether to position iterator at end (default: false for begin)
			 */
			NFX_CORE_INLINE explicit Iterator( const Splitter& splitter, bool at_end = false ) noexcept;

			//-----------------------------
			// Iterator operators
			//-----------------------------

			/**
			 * @brief Dereferences iterator to get current string segment
			 * @return String view of the current segment
			 */
			NFX_CORE_INLINE std::string_view operator*() const noexcept;

			/**
			 * @brief Pre-increment operator to advance to next segment
			 * @return Reference to this iterator after advancement
			 */
			NFX_CORE_INLINE Iterator& operator++() noexcept;

			//-----------------------------
			// Comparison operators
			//-----------------------------

			/**
			 * @brief Compares iterators for equality
			 * @param other Iterator to compare with
			 * @return true if iterators are equal, false otherwise
			 */
			NFX_CORE_INLINE bool operator==( const Iterator& other ) const noexcept;

		private:
			//-----------------------------
			// Private member variables
			//-----------------------------

			const Splitter& m_splitter;
			size_t m_start;
			size_t m_end;
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
	 * @brief Templated factory function for zero-copy string splitting
	 * @details Creates a Splitter for efficient iteration over string segments
	 *          without heap allocations. Accepts any string-like type that can
	 *          be converted to std::string_view.
	 * @tparam StringType Any type convertible to std::string_view (std::string, const char*, etc.)
	 * @param str String to split
	 * @param delimiter Character to split on
	 * @return Splitter object for range-based iteration
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <typename String>
	[[nodiscard]] static NFX_CORE_INLINE Splitter splitView( String&& str, char delimiter ) noexcept;
}

#include "nfx/detail/string/Splitter.inl"
