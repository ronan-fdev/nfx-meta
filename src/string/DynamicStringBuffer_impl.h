/**
 * @file DynamicStringBuffer_impl.h
 * @brief Pimpl implementation for DynamicStringBuffer wrapper around fmt::memory_buffer
 * @details Hidden implementation details for the DynamicStringBuffer facade class
 */

#pragma once

#include <string>
#include <string_view>

#include <fmt/format.h>

namespace nfx::string
{
	/**
	 * @brief Pimpl implementation class for DynamicStringBuffer
	 * @details Encapsulates fmt::memory_buffer and provides controlled access to its functionality.
	 */
	class DynamicStringBuffer_impl final
	{
	public:
		//----------------------------------------------
		// Construction & Destruction
		//----------------------------------------------

		/** @brief Default constructor - creates empty buffer with default capacity */
		DynamicStringBuffer_impl();

		/**
		 * @brief Constructor with initial capacity
		 * @param initialCapacity Initial buffer capacity in bytes
		 */
		explicit DynamicStringBuffer_impl( size_t initialCapacity );

		/** @brief Destructor */
		~DynamicStringBuffer_impl() = default;

		/**
		 * @brief Copy constructor
		 * @param other The DynamicStringBuffer_impl to copy from
		 */
		DynamicStringBuffer_impl( const DynamicStringBuffer_impl& other );

		/**
		 * @brief Move constructor
		 * @param other The DynamicStringBuffer_impl to move from
		 */
		DynamicStringBuffer_impl( DynamicStringBuffer_impl&& other ) noexcept;

		/**
		 * @brief Copy assignment
		 * @param other The DynamicStringBuffer_impl to copy from
		 * @return Reference to this DynamicStringBuffer_impl after assignment
		 */
		DynamicStringBuffer_impl& operator=( const DynamicStringBuffer_impl& other );

		/**
		 * @brief Move assignment
		 * @param other The DynamicStringBuffer_impl to move from
		 * @return Reference to this DynamicStringBuffer_impl after assignment
		 */
		DynamicStringBuffer_impl& operator=( DynamicStringBuffer_impl&& other ) noexcept;

		//----------------------------------------------
		// Buffer access methods
		//----------------------------------------------

		/**
		 * @brief Gets reference to underlying fmt::memory_buffer
		 * @return Mutable reference to the internal fmt::memory_buffer
		 */
		fmt::memory_buffer& buffer() noexcept
		{
			return m_buffer;
		}

		/**
		 * @brief Gets const reference to underlying fmt::memory_buffer
		 * @return Const reference to the internal fmt::memory_buffer
		 */
		const fmt::memory_buffer& buffer() const noexcept
		{
			return m_buffer;
		}

		//----------------------------------------------
		// Capacity and size management
		//----------------------------------------------

		/**
		 * @brief Returns current buffer size in bytes
		 * @return Number of bytes currently stored in buffer
		 */
		size_t size() const noexcept;

		/**
		 * @brief Returns current buffer capacity in bytes
		 * @return Number of bytes allocated for buffer storage
		 */
		size_t capacity() const noexcept;

		/**
		 * @brief Checks if buffer is empty
		 * @return true if buffer contains no data, false otherwise
		 */
		bool empty() const noexcept;

		/** @brief Clears buffer contents (size becomes 0) */
		void clear() noexcept;

		/**
		 * @brief Reserves capacity for at least the specified number of bytes
		 * @param newCapacity Minimum desired capacity in bytes
		 */
		void reserve( size_t newCapacity );

		/**
		 * @brief Resizes buffer to specified size
		 * @param newSize New buffer size in bytes
		 */
		void resize( size_t newSize );

		//----------------------------------------------
		// Data access
		//----------------------------------------------

		/**
		 * @brief Returns pointer to buffer data
		 * @return Mutable pointer to first byte of buffer data
		 */
		char* data() noexcept;

		/**
		 * @brief Returns const pointer to buffer data
		 * @return Const pointer to first byte of buffer data
		 */
		const char* data() const noexcept;

		/**
		 * @brief Array access operator
		 * @param index Zero-based index of element to access
		 * @return Reference to element at specified index
		 */
		char& operator[]( size_t index );

		/**
		 * @brief Const array access operator
		 * @param index Zero-based index of element to access
		 * @return Const reference to element at specified index
		 */
		const char& operator[]( size_t index ) const;

		//----------------------------------------------
		// Content manipulation
		//----------------------------------------------

		/**
		 * @brief Appends string_view to buffer
		 * @param str String view to append
		 */
		void append( std::string_view str );

		/**
		 * @brief Appends std::string to buffer
		 * @param str String to append
		 */
		void append( const std::string& str );

		/**
		 * @brief Appends C-string to buffer
		 * @param str Null-terminated C-string to append
		 */
		void append( const char* str );

		/**
		 * @brief Appends single character to buffer
		 * @param c Character to append
		 */
		void push_back( char c );

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Converts buffer contents to std::string
		 * @return String copy of buffer content
		 */
		std::string toString() const;

		/**
		 * @brief Converts buffer contents to std::string_view
		 * @return String view referencing buffer data
		 */
		std::string_view toStringView() const noexcept;

		//----------------------------------------------
		// Iterator interface
		//----------------------------------------------

		/**
		 * @brief Returns iterator to beginning
		 * @return Mutable iterator pointing to first element
		 */
		char* begin() noexcept;

		/**
		 * @brief Returns const iterator to beginning
		 * @return Const iterator pointing to first element
		 */
		const char* begin() const noexcept;

		/**
		 * @brief Returns iterator to end
		 * @return Mutable iterator pointing one past last element
		 */
		char* end() noexcept;

		/**
		 * @brief Returns const iterator to end
		 * @return Const iterator pointing one past last element
		 */
		const char* end() const noexcept;

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		/** @brief The underlying fmt::memory_buffer */
		fmt::memory_buffer m_buffer;
	};
} // namespace nfx::string
