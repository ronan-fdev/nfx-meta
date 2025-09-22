/**
 * @file DateTime.h
 * @brief Cross-platform UTC DateTime class with ISO 8601 support
 * @details Provides UTC-only datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing/formatting, and compatibility with system clocks
 *
 * @note Design inspired by .NET DateTime/DateTimeOffset and TimeSpan semantics
 *       (100 ns ticks, ranges, and formatting) for cross-platform consistency.
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace nfx::time
{
	//=====================================================================
	// TimeSpan class
	//=====================================================================

	/**
	 * @brief Represents a time interval in 100-nanosecond ticks
	 * @details Provides high-precision time duration representation and arithmetic operations.
	 *          Compatible with .NET TimeSpan semantics for cross-platform consistency.
	 *
	 *          Key features:
	 *          - 100-nanosecond tick precision for maximum accuracy
	 *          - Range: ±10,675,199 days (approximately ±29,247 years)
	 *          - Arithmetic operations (addition, subtraction, comparison)
	 *          - ISO 8601 duration parsing and formatting (P[n]Y[n]M[n]DT[n]H[n]M[n]S)
	 *          - Factory methods for common time units (days, hours, minutes, seconds)
	 *          - Conversion methods to various time units with fractional support
	 *
	 *          This class represents a duration or elapsed time interval, not a specific
	 *          point in time. For absolute time values, use DateTime or DateTimeOffset.
	 *
	 * @note Design inspired by .NET TimeSpan for cross-platform API consistency.
	 */
	class TimeSpan final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct from ticks (100-nanosecond units)
		 * @param ticks Number of 100-nanosecond intervals for this TimeSpan (defaults to 0)
		 */
		explicit inline constexpr TimeSpan( std::int64_t ticks = 0 ) noexcept;

		/** @brief Copy constructor */
		TimeSpan( const TimeSpan& ) = default;

		/** @brief Move constructor */
		TimeSpan( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeSpan() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this TimeSpan after assignment
		 */
		TimeSpan& operator=( const TimeSpan& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this TimeSpan after assignment
		 */
		TimeSpan& operator=( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison
		 * @param other The TimeSpan to compare with
		 * @return true if both TimeSpans represent the same duration, false otherwise
		 */
		inline constexpr bool operator==( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other The TimeSpan to compare with
		 * @return true if TimeSpans represent different durations, false otherwise
		 */
		inline constexpr bool operator!=( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Less than comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is shorter than other, false otherwise
		 */
		inline constexpr bool operator<( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is shorter than or equal to other, false otherwise
		 */
		inline constexpr bool operator<=( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Greater than comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is longer than other, false otherwise
		 */
		inline constexpr bool operator>( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is longer than or equal to other, false otherwise
		 */
		inline constexpr bool operator>=( const TimeSpan& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time durations
		 * @param other The TimeSpan to add to this TimeSpan
		 * @return New TimeSpan representing the sum of both durations
		 */
		inline constexpr TimeSpan operator+( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Subtract time durations
		 * @param other The TimeSpan to subtract from this TimeSpan
		 * @return New TimeSpan representing the difference between durations
		 */
		inline constexpr TimeSpan operator-( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Negate time duration
		 * @return New TimeSpan representing the negated duration
		 */
		inline constexpr TimeSpan operator-() const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param other The TimeSpan to add to this TimeSpan
		 * @return Reference to this TimeSpan after adding the duration
		 */
		inline constexpr TimeSpan& operator+=( const TimeSpan& other ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param other The TimeSpan to subtract from this TimeSpan
		 * @return Reference to this TimeSpan after subtracting the duration
		 */
		inline constexpr TimeSpan& operator-=( const TimeSpan& other ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get tick count
		 * @return The number of 100-nanosecond intervals in this TimeSpan
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get total days
		 * @return The total number of days represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr double totalDays() const noexcept;

		/**
		 * @brief Get total hours
		 * @return The total number of hours represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr double totalHours() const noexcept;

		/**
		 * @brief Get total minutes
		 * @return The total number of minutes represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr double totalMinutes() const noexcept;

		/**
		 * @brief Get total seconds
		 * @return The total number of seconds represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr double totalSeconds() const noexcept;

		/**
		 * @brief Get total milliseconds
		 * @return The total number of milliseconds represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr double totalMilliseconds() const noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Create a TimeSpan from a number of days
		 * @param days The number of days for the TimeSpan duration
		 * @return TimeSpan representing the specified number of days
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr TimeSpan fromDays( double days ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of hours
		 * @param hours The number of hours for the TimeSpan duration
		 * @return TimeSpan representing the specified number of hours
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr TimeSpan fromHours( double hours ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of minutes
		 * @param minutes The number of minutes for the TimeSpan duration
		 * @return TimeSpan representing the specified number of minutes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr TimeSpan fromMinutes( double minutes ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of seconds
		 * @param seconds The number of seconds for the TimeSpan duration
		 * @return TimeSpan representing the specified number of seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr TimeSpan fromSeconds( double seconds ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of milliseconds
		 * @param milliseconds The number of milliseconds for the TimeSpan duration
		 * @return TimeSpan representing the specified number of milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr TimeSpan fromMilliseconds( double milliseconds ) noexcept;

		/**
		 * @brief Parse a TimeSpan from an ISO 8601 duration string
		 * @param str The ISO 8601 duration string to parse
		 * @return TimeSpan parsed from the ISO 8601 duration string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan parse( std::string_view str );

		/**
		 * @brief Try to parse a TimeSpan from an ISO 8601 duration string without throwing
		 * @param str The ISO 8601 duration string to parse
		 * @param result Reference to store the parsed TimeSpan if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, TimeSpan& result ) noexcept;

	private:
		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;
	};

	//=====================================================================
	// DateTime class
	//=====================================================================

	/**
	 * @brief Cross-platform UTC DateTime type with 100-nanosecond precision
	 * @details Implements UTC-only datetime operations with:
	 *          - 100-nanosecond tick precision (matching .NET DateTime)
	 *          - Range: January 1, 0001 to December 31, 9999 UTC
	 *          - ISO 8601 string parsing and formatting
	 *          - System clock interoperability
	 *          - Arithmetic operations with time intervals
	 *
	 *          Note: This is UTC-only. For timezone-aware operations, use DateTimeOffset.
	 */
	class DateTime final
	{
	public:
		//----------------------------------------------
		// Enumerations
		//----------------------------------------------

		/**
		 * @brief DateTime string format options
		 * @details Provides type-safe format selection with self-documenting format names
		 */
		enum class Format : std::uint8_t
		{
			/** @brief ISO 8601 basic format: "2024-01-01T12:00:00Z" */
			Iso8601Basic,

			/** @brief ISO 8601 extended format with fractional seconds: "2024-01-01T12:00:00.1234567Z" */
			Iso8601Extended,

			/** @brief Date and time with timezone: "2024-01-01T12:00:00+02:00" */
			Iso8601WithOffset,

			/** @brief Date only format: "2024-01-01" */
			DateOnly,

			/** @brief Time only: "12:00:00" */
			TimeOnly,

			/** @brief Unix timestamp format: "1704110400" (seconds since epoch) */
			UnixSeconds,

			/** @brief Unix timestamp with milliseconds: "1704110400123" */
			UnixMilliseconds,
		};

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTime value) */
		constexpr DateTime() noexcept;

		/**
		 * @brief Construct from tick count (100-nanosecond units since year 1)
		 * @param ticks Number of 100-nanosecond intervals since January 1, 0001 UTC
		 */
		explicit constexpr DateTime( std::int64_t ticks ) noexcept;

		/**
		 * @brief Construct from system clock time point
		 * @param timePoint System clock time point to convert from
		 */
		explicit DateTime( std::chrono::system_clock::time_point timePoint ) noexcept;

		/**
		 * @brief Construct from date components (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/**
		 * @brief Construct from date and time components (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second ) noexcept;

		/**
		 * @brief Construct from date and time components with milliseconds (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond ) noexcept;

		/**
		 * @brief Parse from ISO 8601 string
		 * @param iso8601String ISO 8601 formatted string to parse
		 */
		explicit DateTime( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTime( const DateTime& ) = default;

		/** @brief Move constructor */
		DateTime( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTime() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this DateTime after assignment
		 */
		DateTime& operator=( const DateTime& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this DateTime after assignment
		 */
		DateTime& operator=( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison
		 * @param other The DateTime to compare with
		 * @return true if both DateTimes represent the same instant, false otherwise
		 */
		bool operator==( const DateTime& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other The DateTime to compare with
		 * @return true if DateTimes represent different instants, false otherwise
		 */
		bool operator!=( const DateTime& other ) const noexcept;

		/**
		 * @brief Less than comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is earlier than other, false otherwise
		 */
		bool operator<( const DateTime& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is earlier than or equal to other, false otherwise
		 */
		bool operator<=( const DateTime& other ) const noexcept;

		/**
		 * @brief Greater than comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is later than other, false otherwise
		 */
		bool operator>( const DateTime& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is later than or equal to other, false otherwise
		 */
		bool operator>=( const DateTime& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time duration
		 * @param duration The TimeSpan to add to this DateTime
		 * @return New DateTime representing this DateTime plus the duration
		 */
		DateTime operator+( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Subtract time duration
		 * @param duration The TimeSpan to subtract from this DateTime
		 * @return New DateTime representing this DateTime minus the duration
		 */
		DateTime operator-( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Get time difference between DateTimes
		 * @param other The DateTime to subtract from this DateTime
		 * @return TimeSpan representing the difference (this - other)
		 */
		TimeSpan operator-( const DateTime& other ) const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param duration The TimeSpan to add to this DateTime
		 * @return Reference to this DateTime after adding the duration
		 */
		DateTime& operator+=( const TimeSpan& duration ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param duration The TimeSpan to subtract from this DateTime
		 * @return Reference to this DateTime after subtracting the duration
		 */
		DateTime& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get tick count (100-nanosecond units since year 1)
		 * @return The number of 100-nanosecond intervals since January 1, 0001 UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get year component (1-9999)
		 * @return The year component of this DateTime (1-9999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t year() const noexcept;

		/**
		 * @brief Get month component (1-12)
		 * @return The month component of this DateTime (1-12)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t month() const noexcept;

		/**
		 * @brief Get day component (1-31)
		 * @return The day component of this DateTime (1-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t day() const noexcept;

		/**
		 * @brief Get hour component (0-23)
		 * @return The hour component of this DateTime (0-23)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t hour() const noexcept;

		/**
		 * @brief Get minute component (0-59)
		 * @return The minute component of this DateTime (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t minute() const noexcept;

		/**
		 * @brief Get second component (0-59)
		 * @return The second component of this DateTime (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t second() const noexcept;

		/**
		 * @brief Get millisecond component (0-999)
		 * @return The millisecond component of this DateTime (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t millisecond() const noexcept;

		/**
		 * @brief Get day of week (0=Sunday, 6=Saturday)
		 * @return The day of week as an integer (0=Sunday, 1=Monday, ..., 6=Saturday)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t dayOfWeek() const noexcept;

		/**
		 * @brief Get day of year (1-366)
		 * @return The day of year as an integer (1-366, where 366 occurs in leap years)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t dayOfYear() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/**
		 * @brief Convert to Unix timestamp (seconds since epoch)
		 * @return Number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toUnixSeconds() const noexcept;

		/**
		 * @brief Convert to Unix timestamp (milliseconds since epoch)
		 * @return Number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toUnixMilliseconds() const noexcept;

		/**
		 * @brief Get date component (time set to 00:00:00)
		 * @return New DateTime with the same date but time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime date() const noexcept;

		/**
		 * @brief Get time of day as duration since midnight
		 * @return TimeSpan representing the elapsed time since midnight (00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] TimeSpan timeOfDay() const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/**
		 * @brief Convert to ISO 8601 string (basic format)
		 * @return String representation in ISO 8601 basic format (e.g., "2024-01-01T12:00:00Z")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Convert to string using specified format
		 * @param format The format to use for string conversion
		 * @return String representation using the specified format
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString( Format format ) const;

		/**
		 * @brief Convert to ISO 8601 extended format with full precision
		 * @return String representation in ISO 8601 extended format with fractional seconds (e.g., "2024-01-01T12:00:00.1234567Z")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/**
		 * @brief Check if this DateTime is valid
		 * @return true if this DateTime represents a valid date and time, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isValid() const noexcept;

		/**
		 * @brief Check if given year is a leap year
		 * @param year The year to check for leap year status
		 * @return true if the year is a leap year, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr bool isLeapYear( std::int32_t year ) noexcept;

		/**
		 * @brief Get days in month for given year and month
		 * @param year The year to check (1-9999)
		 * @param month The month to check (1-12)
		 * @return Number of days in the specified month (28-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr std::int32_t daysInMonth( std::int32_t year, std::int32_t month ) noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Get current UTC time
		 * @return DateTime representing the current UTC date and time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime now() noexcept;

		/**
		 * @brief Get current UTC date (time set to 00:00:00)
		 * @return DateTime representing the current UTC date with time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime today() noexcept;

		/**
		 * @brief Get minimum DateTime value
		 * @return DateTime representing the minimum representable date (January 1, 0001 00:00:00.0000000 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr DateTime minValue() noexcept;

		/**
		 * @brief Get maximum DateTime value
		 * @return DateTime representing the maximum representable date (December 31, 9999 23:59:59.9999999 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr DateTime maxValue() noexcept;

		/**
		 * @brief Get Unix epoch DateTime (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static constexpr DateTime epoch() noexcept;

		/**
		 * @brief Parse ISO 8601 string
		 * @param str The ISO 8601 formatted string to parse
		 * @return DateTime parsed from the ISO 8601 string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime parse( std::string_view str );

		/**
		 * @brief Try parse ISO 8601 string without throwing
		 * @param str The ISO 8601 formatted string to parse
		 * @param result Reference to store the parsed DateTime if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, DateTime& result ) noexcept;

		/**
		 * @brief Create from Unix timestamp (seconds since epoch)
		 * @param seconds The number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the specified Unix timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime sinceEpochSeconds( std::int64_t seconds ) noexcept;

		/**
		 * @brief Create from Unix timestamp (milliseconds since epoch)
		 * @param milliseconds The number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the specified Unix timestamp in milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime sinceEpochMilliseconds( std::int64_t milliseconds ) noexcept;

		//----------------------------------------------
		// std::chrono interoperability
		//----------------------------------------------

		/**
		 * @brief Convert to std::chrono::system_clock::time_point
		 * @details Values outside the representable range of std::chrono::system_clock
		 *          (approximately years 1677-2262 on 64-bit Linux systems) will be
		 *          clamped to the nearest representable value. This means extreme dates
		 *          (near year 1 or year 9999) cannot round-trip through chrono.
		 * @return A system_clock::time_point representing this DateTime
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::chrono::system_clock::time_point toChrono() const noexcept;

		/**
		 * @brief Create DateTime from std::chrono::system_clock::time_point
		 * @details Values outside the representable range of DateTime
		 *          will be clamped to the nearest valid DateTime value.
		 * @param timePoint The system_clock time point to convert
		 * @return A DateTime representing the same instant in time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime fromChrono( const std::chrono::system_clock::time_point& timePoint ) noexcept;

	private:
		//----------------------------------------------
		// Internal data
		//----------------------------------------------

		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;

		//----------------------------------------------
		// Internal helper methods
		//----------------------------------------------

		/** @brief Convert ticks to date components */
		void dateComponents( std::int32_t& year, std::int32_t& month, std::int32_t& day ) const noexcept;

		/** @brief Convert ticks to time components */
		void timeComponents( std::int32_t& hour, std::int32_t& minute, std::int32_t& second, std::int32_t& millisecond ) const noexcept;

		/** @brief Convert date components to ticks */
		static std::int64_t dateToTicks( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/** @brief Convert time components to ticks */
		static std::int64_t timeToTicks( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept;

		/** @brief Validate date components */
		static bool isValidDate( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/** @brief Validate time components */
		static bool isValidTime( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept;
	};

	//=====================================================================
	// DateTimeOffset class
	//=====================================================================

	/**
	 * @brief Timezone-aware DateTime with UTC offset
	 * @details Represents a point in time, typically expressed as a date and time of day,
	 *          relative to Coordinated Universal Time (UTC). Composed of a DateTime value
	 *          and a TimeSpan offset that defines the difference from UTC.
	 *
	 *          Key features:
	 *          - 100-nanosecond tick precision for maximum accuracy
	 *          - Range: January 1, 0001 to December 31, 9999 with offset ±14:00:00
	 *          - ISO 8601 string parsing and formatting with timezone offset
	 *          - Cross-platform timezone-aware operations
	 *          - Arithmetic operations that account for timezone offsets
	 *          - Unix timestamp support with timezone awareness
	 */
	class DateTimeOffset final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTimeOffset value with zero offset) */
		inline constexpr DateTimeOffset() noexcept;

		/**
		 * @brief Construct from DateTime and offset
		 * @param dateTime The DateTime component (local time)
		 * @param offset The timezone offset from UTC
		 */
		inline constexpr DateTimeOffset( const DateTime& dateTime, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from DateTime (assumes local timezone offset)
		 * @param dateTime The DateTime to use (local timezone offset will be determined automatically)
		 */
		explicit DateTimeOffset( const DateTime& dateTime ) noexcept;

		/**
		 * @brief Construct from tick count and offset
		 * @param ticks Number of 100-nanosecond intervals since January 1, 0001 UTC (local time)
		 * @param offset The timezone offset from UTC
		 */
		inline constexpr DateTimeOffset( std::int64_t ticks, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date components and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components with milliseconds and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components with microseconds and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 * @param microsecond Microsecond component (0-999)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, std::int32_t microsecond, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Parse from ISO 8601 string with timezone offset
		 * @param iso8601String ISO 8601 formatted string with timezone offset to parse
		 */
		explicit DateTimeOffset( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTimeOffset( const DateTimeOffset& ) = default;

		/** @brief Move constructor */
		DateTimeOffset( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTimeOffset() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this DateTimeOffset after assignment
		 */
		DateTimeOffset& operator=( const DateTimeOffset& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this DateTimeOffset after assignment
		 */
		DateTimeOffset& operator=( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets represent the same UTC instant, false otherwise
		 */
		inline bool operator==( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Inequality comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if DateTimeOffsets represent different UTC instants, false otherwise
		 */
		inline bool operator!=( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Less than comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is earlier in UTC than other, false otherwise
		 */
		inline bool operator<( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is earlier than or equal to other in UTC, false otherwise
		 */
		inline bool operator<=( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Greater than comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is later in UTC than other, false otherwise
		 */
		inline bool operator>( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is later than or equal to other in UTC, false otherwise
		 */
		inline bool operator>=( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time duration
		 * @param duration The TimeSpan to add to this DateTimeOffset
		 * @return New DateTimeOffset representing this DateTimeOffset plus the duration
		 */
		inline DateTimeOffset operator+( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Subtract time duration
		 * @param duration The TimeSpan to subtract from this DateTimeOffset
		 * @return New DateTimeOffset representing this DateTimeOffset minus the duration
		 */
		inline DateTimeOffset operator-( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Get time difference between DateTimeOffsets (in UTC)
		 * @param other The DateTimeOffset to subtract from this DateTimeOffset
		 * @return TimeSpan representing the difference (this - other) in UTC
		 */
		inline TimeSpan operator-( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param duration The TimeSpan to add to this DateTimeOffset
		 * @return Reference to this DateTimeOffset after adding the duration
		 */
		inline DateTimeOffset& operator+=( const TimeSpan& duration ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param duration The TimeSpan to subtract from this DateTimeOffset
		 * @return Reference to this DateTimeOffset after subtracting the duration
		 */
		inline DateTimeOffset& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get the DateTime component (local time)
		 * @return The DateTime component representing the local time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr const DateTime& dateTime() const noexcept;

		/**
		 * @brief Get the offset from UTC
		 * @return The TimeSpan representing the offset from UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr const TimeSpan& offset() const noexcept;

		/**
		 * @brief Get UTC DateTime equivalent
		 * @return The DateTime representing this time in UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime utcDateTime() const noexcept;

		/**
		 * @brief Get local DateTime equivalent
		 * @return The DateTime representing this time in local timezone
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime localDateTime() const noexcept;

		/**
		 * @brief Get tick count (100-nanosecond units of local time)
		 * @return The number of 100-nanosecond intervals representing the local time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get UTC tick count
		 * @return The number of 100-nanosecond intervals representing the UTC time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t utcTicks() const noexcept;

		/**
		 * @brief Get year component (1-9999)
		 * @return The year component of this DateTimeOffset (1-9999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t year() const noexcept;

		/**
		 * @brief Get month component (1-12)
		 * @return The month component of this DateTimeOffset (1-12)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t month() const noexcept;

		/**
		 * @brief Get day component (1-31)
		 * @return The day component of this DateTimeOffset (1-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t day() const noexcept;

		/**
		 * @brief Get hour component (0-23)
		 * @return The hour component of this DateTimeOffset (0-23)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t hour() const noexcept;

		/**
		 * @brief Get minute component (0-59)
		 * @return The minute component of this DateTimeOffset (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t minute() const noexcept;

		/**
		 * @brief Get second component (0-59)
		 * @return The second component of this DateTimeOffset (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t second() const noexcept;

		/**
		 * @brief Get millisecond component (0-999)
		 * @return The millisecond component of this DateTimeOffset (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t millisecond() const noexcept;

		/**
		 * @brief Get microsecond component (0-999)
		 * @return The microsecond component of this DateTimeOffset (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t microsecond() const noexcept;

		/**
		 * @brief Get nanosecond component (0-900, in hundreds)
		 * @return The nanosecond component of this DateTimeOffset in hundreds (0-900)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t nanosecond() const noexcept;

		/**
		 * @brief Get day of week (0=Sunday, 6=Saturday)
		 * @return The day of week as an integer (0=Sunday, 1=Monday, ..., 6=Saturday)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t dayOfWeek() const noexcept;

		/**
		 * @brief Get day of year (1-366)
		 * @return The day of year as an integer (1-366, where 366 occurs in leap years)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t dayOfYear() const noexcept;

		/**
		 * @brief Get offset in total minutes
		 * @return The total minutes offset from UTC (positive for East, negative for West)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t totalOffsetMinutes() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/**
		 * @brief Convert to Unix timestamp (seconds since epoch)
		 * @return Number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t toUnixSeconds() const noexcept;

		/**
		 * @brief Convert to Unix timestamp (milliseconds since epoch)
		 * @return Number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t toUnixMilliseconds() const noexcept;

		/**
		 * @brief Get date component (time set to 00:00:00)
		 * @return DateTimeOffset with the same date but time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset date() const noexcept;

		/**
		 * @brief Get time of day as duration since midnight
		 * @return TimeSpan representing the elapsed time since midnight (00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline TimeSpan timeOfDay() const noexcept;

		/**
		 * @brief Convert to specified offset
		 * @param newOffset The new timezone offset to convert to
		 * @return DateTimeOffset representing the same instant in time with the specified offset
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toOffset( const TimeSpan& newOffset ) const noexcept;

		/**
		 * @brief Convert to UTC (offset = 00:00:00)
		 * @return DateTimeOffset representing the same instant in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toUniversalTime() const noexcept;

		/**
		 * @brief Convert to local time (system timezone)
		 * @return DateTimeOffset representing the same instant in local timezone
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toLocalTime() const noexcept;

		/**
		 * @brief Convert to Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for interoperability with Windows-originated data on any system.
		 * @return 100-nanosecond intervals since January 1, 1601 UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toFILETIME() const noexcept;

		//----------------------------------------------
		// Arithmetic methods
		//----------------------------------------------

		/**
		 * @brief Add time span
		 * @param value The TimeSpan to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the time span
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset add( const TimeSpan& value ) const noexcept;

		/**
		 * @brief Add days
		 * @param days The number of days to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified days
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addDays( double days ) const noexcept;

		/**
		 * @brief Add hours
		 * @param hours The number of hours to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified hours
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addHours( double hours ) const noexcept;

		/**
		 * @brief Add milliseconds
		 * @param milliseconds The number of milliseconds to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMilliseconds( double milliseconds ) const noexcept;

		/**
		 * @brief Add minutes
		 * @param minutes The number of minutes to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified minutes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMinutes( double minutes ) const noexcept;

		/**
		 * @brief Add months
		 * @param months The number of months to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified months
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMonths( std::int32_t months ) const noexcept;

		/**
		 * @brief Add seconds
		 * @param seconds The number of seconds to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addSeconds( double seconds ) const noexcept;

		/**
		 * @brief Add ticks
		 * @param ticks The number of 100-nanosecond ticks to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified ticks
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset addTicks( std::int64_t ticks ) const noexcept;

		/**
		 * @brief Add years
		 * @param years The number of years to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified years
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addYears( std::int32_t years ) const noexcept;

		/**
		 * @brief Subtract DateTimeOffset and return TimeSpan
		 * @param value The DateTimeOffset to subtract from this DateTimeOffset
		 * @return TimeSpan representing the difference between the two DateTimeOffsets
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline TimeSpan subtract( const DateTimeOffset& value ) const noexcept;

		/**
		 * @brief Subtract TimeSpan and return DateTimeOffset
		 * @param value The TimeSpan to subtract from this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset minus the time span
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset subtract( const TimeSpan& value ) const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/**
		 * @brief Convert to ISO 8601 string with offset
		 * @return String representation in ISO 8601 format with timezone offset (e.g., "2024-01-01T12:00:00+02:00")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Convert to string using specified format
		 * @param format The format to use for string conversion
		 * @return String representation using the specified format
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString( DateTime::Format format ) const;

		/**
		 * @brief Convert to ISO 8601 extended format with full precision and offset
		 * @return String representation in ISO 8601 extended format with fractional seconds and offset (e.g., "2024-01-01T12:00:00.1234567+02:00")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Comparison methods
		//----------------------------------------------

		/**
		 * @brief Compare to another DateTimeOffset
		 * @param other The DateTimeOffset to compare with
		 * @return Negative value if this is earlier, zero if equal, positive if this is later
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t compareTo( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Check if this DateTimeOffset equals another (same UTC time)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets represent the same UTC time, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equals( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Check if this DateTimeOffset equals another exactly (same local time and offset)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets have the same local time and offset, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equalsExact( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/**
		 * @brief Check if this DateTimeOffset is valid
		 * @return true if this DateTimeOffset represents a valid date and time with valid offset, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isValid() const noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Get current local time with system timezone offset
		 * @return DateTimeOffset representing the current local date and time with system timezone offset
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset now() noexcept;

		/**
		 * @brief Get current UTC time (offset = 00:00:00)
		 * @return DateTimeOffset representing the current UTC date and time (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset utcNow() noexcept;

		/**
		 * @brief Get current local date (time set to 00:00:00)
		 * @return DateTimeOffset representing the current local date with time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset today() noexcept;

		/**
		 * @brief Get minimum DateTimeOffset value
		 * @return DateTimeOffset representing the minimum representable date (January 1, 0001 00:00:00.0000000 with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr DateTimeOffset minValue() noexcept;

		/**
		 * @brief Get maximum DateTimeOffset value
		 * @return DateTimeOffset representing the maximum representable date (December 31, 9999 23:59:59.9999999 with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr DateTimeOffset maxValue() noexcept;

		/**
		 * @brief Get Unix epoch DateTimeOffset (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the Unix epoch (January 1, 1970 00:00:00 UTC with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr DateTimeOffset unixEpoch() noexcept;

		/**
		 * @brief Compare two DateTimeOffset values
		 * @param left The first DateTimeOffset to compare
		 * @param right The second DateTimeOffset to compare
		 * @return Negative value if left is earlier, zero if equal, positive if left is later
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static std::int32_t compare( const DateTimeOffset& left, const DateTimeOffset& right ) noexcept;

		/**
		 * @brief Parse ISO 8601 string with timezone offset
		 * @param str The ISO 8601 formatted string with timezone offset to parse
		 * @return DateTimeOffset parsed from the ISO 8601 string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset parse( std::string_view str );

		/**
		 * @brief Try parse ISO 8601 string without throwing
		 * @param str The ISO 8601 formatted string with timezone offset to parse
		 * @param result Reference to store the parsed DateTimeOffset if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, DateTimeOffset& result ) noexcept;

		/**
		 * @brief Create from Unix timestamp seconds with UTC offset
		 * @param seconds The number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the specified Unix timestamp in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromUnixTimeSeconds( std::int64_t seconds ) noexcept;

		/**
		 * @brief Create from Unix timestamp milliseconds with UTC offset
		 * @param milliseconds The number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the specified Unix timestamp in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromUnixTimeMilliseconds( std::int64_t milliseconds ) noexcept;

		/**
		 * @brief Create DateTimeOffset from Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for processing Windows-originated data on any system.
		 * @param fileTime 100-nanosecond intervals since January 1, 1601 UTC
		 * @return DateTimeOffset representing the same instant in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromFileTime( std::int64_t fileTime ) noexcept;

	private:
		//----------------------------------------------
		// Internal data
		//----------------------------------------------

		/** @brief Local date and time */
		DateTime m_dateTime;

		/** @brief Offset from UTC */
		TimeSpan m_offset;

		//----------------------------------------------
		// Internal helper methods
		//----------------------------------------------

		/** @brief Validate offset range (±14:00:00) */
		static bool isValidOffset( const TimeSpan& offset ) noexcept;

		/** @brief Get system timezone offset for given DateTime */
		static TimeSpan systemTimezoneOffset( const DateTime& dateTime ) noexcept;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator (ISO 8601 format)
	 * @param os Output stream to write to
	 * @param dateTime DateTime value to format and output
	 * @return Reference to the output stream for chaining
	 */
	std::ostream& operator<<( std::ostream& os, const DateTime& dateTime );

	/**
	 * @brief Input stream operator (ISO 8601 format)
	 * @param is Input stream to read from
	 * @param dateTime DateTime reference to store parsed value
	 * @return Reference to the input stream for chaining
	 */
	std::istream& operator>>( std::istream& is, DateTime& dateTime );

	/**
	 * @brief Output stream operator (ISO 8601 format with offset)
	 * @param os Output stream to write to
	 * @param dateTimeOffset DateTimeOffset value to format and output
	 * @return Reference to the output stream for chaining
	 */
	std::ostream& operator<<( std::ostream& os, const DateTimeOffset& dateTimeOffset );

	/**
	 * @brief Input stream operator (ISO 8601 format with offset)
	 * @param is Input stream to read from
	 * @param dateTimeOffset DateTimeOffset reference to store parsed value
	 * @return Reference to the input stream for chaining
	 */
	std::istream& operator>>( std::istream& is, DateTimeOffset& dateTimeOffset );
}

#include "DateTime.inl"
