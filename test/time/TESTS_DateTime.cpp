/**
 * @file TESTS_DateTime.cpp
 * @brief Comprehensive tests for DateTime, TimeSpan, and DateTimeOffset classes
 * @details Tests UTC-only datetime operations with 100-nanosecond precision,
 *          timezone-aware DateTimeOffset operations, ISO 8601 parsing/formatting,
 *          and cross-platform compatibility
 */

#include <gtest/gtest.h>

#include <nfx/time/constants/DateTimeConstants.h>
#include <nfx/time/DateTime.h>

namespace nfx::time::test
{
	//=====================================================================
	// TimeSpan type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( TimeSpanConstruction, Construction )
	{
		// Default constructor
		time::TimeSpan ts1;
		EXPECT_EQ( 0, ts1.ticks() );

		// Constructor with ticks
		time::TimeSpan ts2{ 1234567890 };
		EXPECT_EQ( 1234567890, ts2.ticks() );

		// Copy constructor
		time::TimeSpan ts3{ ts2 };
		EXPECT_EQ( ts2.ticks(), ts3.ticks() );

		// Move constructor
		time::TimeSpan ts4{ time::TimeSpan{ 9876543210 } };
		EXPECT_EQ( 9876543210, ts4.ticks() );
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	TEST( TimeSpanOperations, ComparisonOperators )
	{
		time::TimeSpan ts1{ 1000 };
		time::TimeSpan ts2{ 2000 };
		time::TimeSpan ts3{ 1000 };

		// Equality
		EXPECT_TRUE( ts1 == ts3 );
		EXPECT_FALSE( ts1 == ts2 );
		EXPECT_TRUE( ts1 != ts2 );
		EXPECT_FALSE( ts1 != ts3 );

		// Ordering
		EXPECT_TRUE( ts1 < ts2 );
		EXPECT_FALSE( ts2 < ts1 );
		EXPECT_TRUE( ts1 <= ts2 );
		EXPECT_TRUE( ts1 <= ts3 );
		EXPECT_TRUE( ts2 > ts1 );
		EXPECT_FALSE( ts1 > ts2 );
		EXPECT_TRUE( ts2 >= ts1 );
		EXPECT_TRUE( ts3 >= ts1 );
	}

	TEST( TimeSpanOperations, ArithmeticOperators )
	{
		time::TimeSpan ts1{ 1000 };
		time::TimeSpan ts2{ 500 };

		// Addition
		time::TimeSpan sum = ts1 + ts2;
		EXPECT_EQ( 1500, sum.ticks() );

		// Subtraction
		time::TimeSpan diff = ts1 - ts2;
		EXPECT_EQ( 500, diff.ticks() );

		// Unary minus
		time::TimeSpan neg = -ts1;
		EXPECT_EQ( -1000, neg.ticks() );

		// In-place addition
		ts1 += ts2;
		EXPECT_EQ( 1500, ts1.ticks() );

		// In-place subtraction
		ts1 -= ts2;
		EXPECT_EQ( 1000, ts1.ticks() );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	TEST( TimeSpanAccessors, PropertyAccessors )
	{
		// 1 day = 24 hours = 1440 minutes = 86400 seconds = 864000000000 ticks
		time::TimeSpan oneDay{ constants::datetime::TICKS_PER_DAY };

		EXPECT_DOUBLE_EQ( 1.0, oneDay.totalDays() );
		EXPECT_DOUBLE_EQ( 24.0, oneDay.totalHours() );
		EXPECT_DOUBLE_EQ( 1440.0, oneDay.totalMinutes() );
		EXPECT_DOUBLE_EQ( 86400.0, oneDay.totalSeconds() );
		EXPECT_DOUBLE_EQ( 86400000.0, oneDay.totalMilliseconds() );

		// Test fractional values
		time::TimeSpan halfDay{ constants::datetime::TICKS_PER_DAY / 2 };
		EXPECT_DOUBLE_EQ( 0.5, halfDay.totalDays() );
		EXPECT_DOUBLE_EQ( 12.0, halfDay.totalHours() );
	}

	//----------------------------------------------
	// Factory
	//----------------------------------------------

	TEST( TimeSpanFactory, StaticFactoryMethods )
	{
		auto fromDays{ time::TimeSpan::fromDays( 2.5 ) };
		EXPECT_EQ( static_cast<std::int64_t>( 2.5 * constants::datetime::TICKS_PER_DAY ), fromDays.ticks() );

		auto fromHours{ time::TimeSpan::fromHours( 3.0 ) };
		EXPECT_EQ( 3 * constants::datetime::TICKS_PER_HOUR, fromHours.ticks() );

		auto fromMinutes{ time::TimeSpan::fromMinutes( 90.0 ) };
		EXPECT_EQ( 90 * constants::datetime::TICKS_PER_MINUTE, fromMinutes.ticks() );

		auto fromSeconds{ time::TimeSpan::fromSeconds( 30.5 ) };
		EXPECT_EQ( static_cast<std::int64_t>( 30.5 * constants::datetime::TICKS_PER_SECOND ), fromSeconds.ticks() );

		auto fromMs{ time::TimeSpan::fromMilliseconds( 1500.0 ) };
		EXPECT_EQ( 1500 * constants::datetime::TICKS_PER_MILLISECOND, fromMs.ticks() );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( TimeSpanStringParsing, TryParseMethod )
	{
		time::TimeSpan result;

		// Valid ISO 8601 duration formats
		EXPECT_TRUE( time::TimeSpan::tryParse( "PT1H", result ) );
		EXPECT_EQ( constants::datetime::TICKS_PER_HOUR, result.ticks() );

		EXPECT_TRUE( time::TimeSpan::tryParse( "PT30M", result ) );
		EXPECT_EQ( 30 * constants::datetime::TICKS_PER_MINUTE, result.ticks() );

		EXPECT_TRUE( time::TimeSpan::tryParse( "PT45S", result ) );
		EXPECT_EQ( 45 * constants::datetime::TICKS_PER_SECOND, result.ticks() );

		EXPECT_TRUE( time::TimeSpan::tryParse( "PT1H30M45S", result ) );
		EXPECT_EQ( constants::datetime::TICKS_PER_HOUR + 30 * constants::datetime::TICKS_PER_MINUTE + 45 * constants::datetime::TICKS_PER_SECOND, result.ticks() );

		// Valid H:M:S format
		EXPECT_TRUE( time::TimeSpan::tryParse( "01:30:45", result ) );
		EXPECT_EQ( constants::datetime::TICKS_PER_HOUR + 30 * constants::datetime::TICKS_PER_MINUTE + 45 * constants::datetime::TICKS_PER_SECOND, result.ticks() );

		EXPECT_TRUE( time::TimeSpan::tryParse( "00:05:30.5", result ) );
		EXPECT_EQ( 5 * constants::datetime::TICKS_PER_MINUTE + 30.5 * constants::datetime::TICKS_PER_SECOND, result.ticks() );

		// Valid numeric seconds format
		EXPECT_TRUE( time::TimeSpan::tryParse( "123.45", result ) );
		EXPECT_EQ( static_cast<std::int64_t>( 123.45 * constants::datetime::TICKS_PER_SECOND ), result.ticks() );

		EXPECT_TRUE( time::TimeSpan::tryParse( "60", result ) );
		EXPECT_EQ( 60 * constants::datetime::TICKS_PER_SECOND, result.ticks() );

		// Invalid formats
		EXPECT_FALSE( time::TimeSpan::tryParse( "", result ) );
		EXPECT_FALSE( time::TimeSpan::tryParse( "invalid", result ) );
		EXPECT_FALSE( time::TimeSpan::tryParse( "25:00:00", result ) ); // Invalid hour
		EXPECT_FALSE( time::TimeSpan::tryParse( "01:60:00", result ) ); // Invalid minute
		EXPECT_FALSE( time::TimeSpan::tryParse( "01:30:60", result ) ); // Invalid second
	}

	TEST( TimeSpanStringParsing, ParseMethod )
	{
		// Valid parsing
		auto result1{ time::TimeSpan::parse( "PT1H30M" ) };
		EXPECT_EQ( constants::datetime::TICKS_PER_HOUR + 30 * constants::datetime::TICKS_PER_MINUTE, result1.ticks() );

		auto result2{ time::TimeSpan::parse( "02:15:30" ) };
		EXPECT_EQ( 2 * constants::datetime::TICKS_PER_HOUR + 15 * constants::datetime::TICKS_PER_MINUTE + 30 * constants::datetime::TICKS_PER_SECOND, result2.ticks() );

		auto result3{ time::TimeSpan::parse( "90.5" ) };
		EXPECT_EQ( static_cast<std::int64_t>( 90.5 * constants::datetime::TICKS_PER_SECOND ), result3.ticks() );

		// Invalid parsing should throw
		EXPECT_THROW( (void)time::TimeSpan::parse( "" ), std::invalid_argument );
		EXPECT_THROW( (void)time::TimeSpan::parse( "invalid" ), std::invalid_argument );
		EXPECT_THROW( (void)time::TimeSpan::parse( "25:00:00" ), std::invalid_argument );
	}

	//=====================================================================
	// DateTime type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( DateTimeConstruction, Construction )
	{
		// Default constructor
		time::DateTime dt1;
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, dt1.ticks() );

		// Constructor with ticks
		time::DateTime dt2{ constants::datetime::UNIX_EPOCH_TICKS };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, dt2.ticks() );

		// Constructor from date components
		time::DateTime dt3{ 2024, 1, 15 };
		EXPECT_EQ( 2024, dt3.year() );
		EXPECT_EQ( 1, dt3.month() );
		EXPECT_EQ( 15, dt3.day() );
		EXPECT_EQ( 0, dt3.hour() );
		EXPECT_EQ( 0, dt3.minute() );
		EXPECT_EQ( 0, dt3.second() );

		// Constructor from date and time components
		time::DateTime dt4{ 2024, 6, 15, 14, 30, 45 };
		EXPECT_EQ( 2024, dt4.year() );
		EXPECT_EQ( 6, dt4.month() );
		EXPECT_EQ( 15, dt4.day() );
		EXPECT_EQ( 14, dt4.hour() );
		EXPECT_EQ( 30, dt4.minute() );
		EXPECT_EQ( 45, dt4.second() );

		// Constructor with milliseconds
		time::DateTime dt5{ 2024, 12, 25, 23, 59, 59, 999 };
		EXPECT_EQ( 2024, dt5.year() );
		EXPECT_EQ( 12, dt5.month() );
		EXPECT_EQ( 25, dt5.day() );
		EXPECT_EQ( 23, dt5.hour() );
		EXPECT_EQ( 59, dt5.minute() );
		EXPECT_EQ( 59, dt5.second() );
		EXPECT_EQ( 999, dt5.millisecond() );
	}

	TEST( DateTimeConstruction, SystemClockConstructor )
	{
		auto now{ std::chrono::system_clock::now() };
		time::DateTime dt{ now };

		// Should be within reasonable range of current time
		auto currentTime{ time::DateTime::now() };
		time::TimeSpan diff = currentTime - dt;

		// Should be within 1 second difference
		EXPECT_LT( std::abs( diff.totalSeconds() ), 1.0 );
	}

	TEST( DateTimeConstruction, ISO8601StringConstructor )
	{
		// Basic ISO 8601 format
		time::DateTime dt1{ "2024-06-15T14:30:45Z" };
		EXPECT_EQ( 2024, dt1.year() );
		EXPECT_EQ( 6, dt1.month() );
		EXPECT_EQ( 15, dt1.day() );
		EXPECT_EQ( 14, dt1.hour() );
		EXPECT_EQ( 30, dt1.minute() );
		EXPECT_EQ( 45, dt1.second() );

		// ISO 8601 with fractional seconds
		time::DateTime dt2{ "2024-01-01T00:00:00.1234567Z" };
		EXPECT_EQ( 2024, dt2.year() );
		EXPECT_EQ( 1, dt2.month() );
		EXPECT_EQ( 1, dt2.day() );
		EXPECT_EQ( 0, dt2.hour() );
		EXPECT_EQ( 0, dt2.minute() );
		EXPECT_EQ( 0, dt2.second() );

		// Invalid format should throw
		EXPECT_THROW( time::DateTime( "invalid-date-string" ), std::invalid_argument );
		EXPECT_THROW( time::DateTime( "2024-13-01T00:00:00Z" ), std::invalid_argument );
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	TEST( DateTimeOperations, ComparisonOperators )
	{
		time::DateTime dt1{ 2024, 1, 1, 12, 0, 0 };
		time::DateTime dt2{ 2024, 1, 1, 13, 0, 0 };
		time::DateTime dt3{ 2024, 1, 1, 12, 0, 0 };

		EXPECT_TRUE( dt1 == dt3 );
		EXPECT_FALSE( dt1 == dt2 );
		EXPECT_TRUE( dt1 != dt2 );
		EXPECT_FALSE( dt1 != dt3 );

		EXPECT_TRUE( dt1 < dt2 );
		EXPECT_FALSE( dt2 < dt1 );
		EXPECT_TRUE( dt1 <= dt2 );
		EXPECT_TRUE( dt1 <= dt3 );
		EXPECT_TRUE( dt2 > dt1 );
		EXPECT_FALSE( dt1 > dt2 );
		EXPECT_TRUE( dt2 >= dt1 );
		EXPECT_TRUE( dt3 >= dt1 );
	}

	TEST( DateTimeOperations, ArithmeticOperators )
	{
		time::DateTime dt{ 2024, 6, 15, 12, 0, 0 };
		auto oneHour{ time::TimeSpan::fromHours( 1.0 ) };
		auto oneDay{ time::TimeSpan::fromDays( 1.0 ) };

		// Addition
		time::DateTime dtPlusHour = dt + oneHour;
		EXPECT_EQ( 13, dtPlusHour.hour() );

		// Subtraction
		time::DateTime dtMinusHour = dt - oneHour;
		EXPECT_EQ( 11, dtMinusHour.hour() );

		// DateTime difference
		time::TimeSpan diff = dtPlusHour - dt;
		EXPECT_DOUBLE_EQ( 1.0, diff.totalHours() );

		// In-place operations
		dt += oneDay;
		EXPECT_EQ( 16, dt.day() );

		dt -= oneDay;
		EXPECT_EQ( 15, dt.day() );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	TEST( DateTimeAccessors, PropertyAccessors )
	{
		time::DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };

		EXPECT_EQ( 2024, dt.year() );
		EXPECT_EQ( 6, dt.month() );
		EXPECT_EQ( 15, dt.day() );
		EXPECT_EQ( 14, dt.hour() );
		EXPECT_EQ( 30, dt.minute() );
		EXPECT_EQ( 45, dt.second() );
		EXPECT_EQ( 123, dt.millisecond() );

		// Day of week (0=Sunday, 6=Saturday) - June 15, 2024 is a Saturday
		EXPECT_EQ( 6, dt.dayOfWeek() );

		// Day of year calculation - Jan+Feb(leap)+Mar+Apr+May+15days
		int expectedDayOfYear{ 31 + 29 + 31 + 30 + 31 + 15 };
		EXPECT_EQ( expectedDayOfYear, dt.dayOfYear() );
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	TEST( DateTimeOperations, ConversionMethods )
	{
		time::DateTime epoch = time::DateTime::epoch();

		// Unix timestamp conversions
		EXPECT_EQ( 0, epoch.toUnixSeconds() );
		EXPECT_EQ( 0, epoch.toUnixMilliseconds() );

		// 2021-01-01T00:00:00Z
		time::DateTime dt = time::DateTime::sinceEpochSeconds( 1609459200 );
		EXPECT_EQ( 2021, dt.year() );
		EXPECT_EQ( 1, dt.month() );
		EXPECT_EQ( 1, dt.day() );

		// Date extraction
		time::DateTime dtWithTime{ 2024, 6, 15, 14, 30, 45 };
		time::DateTime dateOnly = dtWithTime.date();
		EXPECT_EQ( 2024, dateOnly.year() );
		EXPECT_EQ( 6, dateOnly.month() );
		EXPECT_EQ( 15, dateOnly.day() );
		EXPECT_EQ( 0, dateOnly.hour() );
		EXPECT_EQ( 0, dateOnly.minute() );
		EXPECT_EQ( 0, dateOnly.second() );

		// Time of day extraction
		time::TimeSpan timeOfDay = dtWithTime.timeOfDay();
		EXPECT_DOUBLE_EQ( 14.0 + 30.0 / 60.0 + 45.0 / 3600.0, timeOfDay.totalHours() );
	}

	TEST( DateTimeOperations, StringFormatting )
	{
		time::DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };

		// Basic ISO 8601
		std::string basic{ dt.toString() };
		EXPECT_EQ( "2024-06-15T14:30:45Z", basic );

		// Specific formats
		EXPECT_EQ( "2024-06-15T14:30:45Z", dt.toString( time::DateTime::Format::Iso8601Basic ) );
		EXPECT_EQ( "2024-06-15", dt.toString( time::DateTime::Format::DateOnly ) );
		EXPECT_EQ( "14:30:45", dt.toString( time::DateTime::Format::TimeOnly ) );
		EXPECT_EQ( "2024-06-15T14:30:45+00:00", dt.toString( time::DateTime::Format::Iso8601WithOffset ) );

		// Extended format with fractional seconds
		std::string extended{ dt.toString( time::DateTime::Format::Iso8601Extended ) };
		EXPECT_TRUE( extended.find( "2024-06-15T14:30:45." ) != std::string::npos );
		EXPECT_TRUE( extended.find( "Z" ) != std::string::npos );

		// Unix timestamps
		time::DateTime epoch = time::DateTime::epoch();
		EXPECT_EQ( "0", epoch.toString( time::DateTime::Format::UnixSeconds ) );
		EXPECT_EQ( "0", epoch.toString( time::DateTime::Format::UnixMilliseconds ) );
	}

	TEST( DateTimeOperations, ValidationMethods )
	{
		// Valid DateTime
		time::DateTime validDt{ 2024, 6, 15, 12, 30, 45 };
		EXPECT_TRUE( validDt.isValid() );

		// Leap year tests
		EXPECT_TRUE( time::DateTime::isLeapYear( 2024 ) );
		EXPECT_TRUE( time::DateTime::isLeapYear( 2000 ) );
		EXPECT_FALSE( time::DateTime::isLeapYear( 1900 ) );
		EXPECT_FALSE( time::DateTime::isLeapYear( 2023 ) );

		// Days in month tests
		EXPECT_EQ( 31, time::DateTime::daysInMonth( 2024, 1 ) );
		EXPECT_EQ( 29, time::DateTime::daysInMonth( 2024, 2 ) );
		EXPECT_EQ( 28, time::DateTime::daysInMonth( 2023, 2 ) );
		EXPECT_EQ( 30, time::DateTime::daysInMonth( 2024, 4 ) );
		EXPECT_EQ( 31, time::DateTime::daysInMonth( 2024, 12 ) );
		EXPECT_EQ( 0, time::DateTime::daysInMonth( 2024, 13 ) );
	}

	//----------------------------------------------
	// Factory
	//----------------------------------------------

	TEST( DateTimeFactory, StaticFactoryMethods )
	{
		// Min and max values
		auto minVal{ time::DateTime::minValue() };
		auto maxVal{ time::DateTime::maxValue() };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, minVal.ticks() );
		EXPECT_EQ( constants::datetime::MAX_DATETIME_TICKS, maxVal.ticks() );

		// Epoch
		auto epoch{ time::DateTime::epoch() };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, epoch.ticks() );
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );

		// Now and today (basic smoke tests)
		auto now{ time::DateTime::now() };
		auto today{ time::DateTime::today() };
		EXPECT_TRUE( now.isValid() );
		EXPECT_TRUE( today.isValid() );
		EXPECT_EQ( 0, today.hour() );
		EXPECT_EQ( 0, today.minute() );
		EXPECT_EQ( 0, today.second() );

		// Factory from timestamps
		auto fromSeconds{ time::DateTime::sinceEpochSeconds( 86400 ) };
		EXPECT_EQ( 1970, fromSeconds.year() );
		EXPECT_EQ( 1, fromSeconds.month() );
		EXPECT_EQ( 2, fromSeconds.day() );

		auto fromMs{ time::DateTime::sinceEpochMilliseconds( 86400000 ) };
		EXPECT_EQ( 1970, fromMs.year() );
		EXPECT_EQ( 1, fromMs.month() );
		EXPECT_EQ( 2, fromMs.day() );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( DateTimeStringParsing, TryParseMethod )
	{
		time::DateTime result;

		// Valid parsing
		EXPECT_TRUE( time::DateTime::tryParse( "2024-06-15T14:30:45Z", result ) );
		EXPECT_EQ( 2024, result.year() );
		EXPECT_EQ( 6, result.month() );
		EXPECT_EQ( 15, result.day() );
		EXPECT_EQ( 14, result.hour() );
		EXPECT_EQ( 30, result.minute() );
		EXPECT_EQ( 45, result.second() );

		// Valid parsing with fractional seconds
		EXPECT_TRUE( time::DateTime::tryParse( "2024-01-01T00:00:00.123Z", result ) );
		EXPECT_EQ( 2024, result.year() );

		// Invalid parsing
		EXPECT_FALSE( time::DateTime::tryParse( "invalid-date", result ) );
		EXPECT_FALSE( time::DateTime::tryParse( "2024-13-01T00:00:00Z", result ) );
		EXPECT_FALSE( time::DateTime::tryParse( "2024-01-32T00:00:00Z", result ) );
		EXPECT_FALSE( time::DateTime::tryParse( "2024-01-01T25:00:00Z", result ) );
	}

	TEST( DateTimeStringParsing, ParseMethod )
	{
		// Valid parsing
		auto result1{ time::DateTime::parse( "2024-06-15T14:30:45Z" ) };
		EXPECT_EQ( 2024, result1.year() );
		EXPECT_EQ( 6, result1.month() );
		EXPECT_EQ( 15, result1.day() );
		EXPECT_EQ( 14, result1.hour() );
		EXPECT_EQ( 30, result1.minute() );
		EXPECT_EQ( 45, result1.second() );

		auto result2{ time::DateTime::parse( "2024-01-01" ) };
		EXPECT_EQ( 2024, result2.year() );
		EXPECT_EQ( 1, result2.month() );
		EXPECT_EQ( 1, result2.day() );
		EXPECT_EQ( 0, result2.hour() );
		EXPECT_EQ( 0, result2.minute() );
		EXPECT_EQ( 0, result2.second() );

		auto result3{ time::DateTime::parse( "2024-12-31T23:59:59.999Z" ) };
		EXPECT_EQ( 2024, result3.year() );
		EXPECT_EQ( 12, result3.month() );
		EXPECT_EQ( 31, result3.day() );
		EXPECT_EQ( 23, result3.hour() );
		EXPECT_EQ( 59, result3.minute() );
		EXPECT_EQ( 59, result3.second() );
		EXPECT_EQ( 999, result3.millisecond() );

		// Invalid parsing should throw
		EXPECT_THROW( (void)time::DateTime::parse( "" ), std::invalid_argument );
		EXPECT_THROW( (void)time::DateTime::parse( "invalid" ), std::invalid_argument );
		EXPECT_THROW( (void)time::DateTime::parse( "2024-13-01" ), std::invalid_argument );			  // Invalid month
		EXPECT_THROW( (void)time::DateTime::parse( "2024-01-32" ), std::invalid_argument );			  // Invalid day
		EXPECT_THROW( (void)time::DateTime::parse( "2024-01-01T25:00:00Z" ), std::invalid_argument ); // Invalid hour
	}

	TEST( DateTimeStringParsing, StreamOperators )
	{
		time::DateTime dt{ 2024, 6, 15, 14, 30, 45 };

		// Output stream operator
		std::ostringstream oss;
		oss << dt;
		EXPECT_EQ( "2024-06-15T14:30:45Z", oss.str() );

		// Input stream operator
		std::istringstream iss( "2024-12-25T23:59:59Z" );
		time::DateTime parsed;
		iss >> parsed;
		EXPECT_EQ( 2024, parsed.year() );
		EXPECT_EQ( 12, parsed.month() );
		EXPECT_EQ( 25, parsed.day() );
		EXPECT_EQ( 23, parsed.hour() );
		EXPECT_EQ( 59, parsed.minute() );
		EXPECT_EQ( 59, parsed.second() );

		// Invalid input should set fail bit
		std::istringstream issInvalid( "invalid-date" );
		time::DateTime invalidParsed;
		issInvalid >> invalidParsed;
		EXPECT_TRUE( issInvalid.fail() );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( DateTimeEdgeCase, EdgeCases )
	{
		// Boundary values
		auto minDateTime{ time::DateTime::minValue() };
		auto maxDateTime{ time::DateTime::maxValue() };

		EXPECT_EQ( constants::datetime::MIN_YEAR, minDateTime.year() );
		EXPECT_EQ( constants::datetime::MAX_YEAR, maxDateTime.year() );

		// Invalid date construction should clamp to minimum
		time::DateTime invalidDate{ 0, 0, 0 };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, invalidDate.ticks() );

		// Invalid time construction should clamp to minimum
		time::DateTime invalidTime{ 2024, 1, 1, -1, -1, -1, -1 };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, invalidTime.ticks() );

		// Leap year edge cases
		time::DateTime leapDay2024{ 2024, 2, 29 };
		EXPECT_EQ( 29, leapDay2024.day() );

		// End of year
		time::DateTime endOfYear{ 2024, 12, 31, 23, 59, 59, 999 };
		EXPECT_EQ( 366, endOfYear.dayOfYear() );
	}

	//----------------------------------------------
	// Integration
	//----------------------------------------------

	TEST( DateTimeIntegration, DateTimeTimeSpanIntegration )
	{
		time::DateTime start{ 2024, 1, 1, 0, 0, 0 };
		time::DateTime end{ 2024, 1, 2, 0, 0, 0 };

		time::TimeSpan duration = end - start;
		EXPECT_DOUBLE_EQ( 1.0, duration.totalDays() );
		EXPECT_DOUBLE_EQ( 24.0, duration.totalHours() );

		time::DateTime calculated = start + duration;
		EXPECT_EQ( end.ticks(), calculated.ticks() );
	}

	TEST( DateTimeIntegration, RoundTripSerialization )
	{
		time::DateTime original{ 2024, 6, 15, 14, 30, 45, 123 };

		// Round trip through ISO 8601 string
		std::string serialized = original.toString( time::DateTime::Format::Iso8601Extended );
		time::DateTime deserialized;
		EXPECT_TRUE( time::DateTime::tryParse( serialized, deserialized ) );

		// Should be very close (within millisecond precision)
		time::TimeSpan diff = original - deserialized;
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
	}

	TEST( DateTimeIntegration, CrossPlatformConsistency )
	{
		// Test that key values are consistent across platforms
		auto epoch{ time::DateTime::epoch() };
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.hour() );
		EXPECT_EQ( 0, epoch.minute() );
		EXPECT_EQ( 0, epoch.second() );

		// Known Unix timestamp - 2021-01-01T00:00:00Z
		auto knownDate{ time::DateTime::sinceEpochSeconds( 1609459200 ) };
		EXPECT_EQ( 2021, knownDate.year() );
		EXPECT_EQ( 1, knownDate.month() );
		EXPECT_EQ( 1, knownDate.day() );
	}

	TEST( DateTimeIntegration, ISO8601FormatValidationEdgeCases )
	{
		time::DateTime result;

		// Invalid ISO 8601 format variations that ACTUALLY fail based on parser implementation

		// Wrong separators - parser expects exact dashes for dates and colons for times
		EXPECT_FALSE( time::DateTime::tryParse( "1994/11/20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994/11/20T10:25:33Z" ), std::invalid_argument );

		// 2-digit year - parser expects exactly 4 digits
		EXPECT_FALSE( time::DateTime::tryParse( "94-11-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "94-11-20T10:25:33Z" ), std::invalid_argument );

		// Missing date separators - parser expects exact format
		EXPECT_FALSE( time::DateTime::tryParse( "19941120T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "19941120T10:25:33Z" ), std::invalid_argument );

		// Wrong time datatypes::separators - parser expects colons
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T10.25.33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T10.25.33Z" ), std::invalid_argument );

		// Missing time separators
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T102533Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T102533Z" ), std::invalid_argument );

		// Non-numeric characters in date/time components
		EXPECT_FALSE( time::DateTime::tryParse( "ABCD-11-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "ABCD-11-20T10:25:33Z" ), std::invalid_argument );

		EXPECT_FALSE( time::DateTime::tryParse( "1994-AB-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-AB-20T10:25:33Z" ), std::invalid_argument );

		// Empty or too short strings
		EXPECT_FALSE( time::DateTime::tryParse( "", result ) );
		EXPECT_THROW( time::DateTime( "" ), std::invalid_argument );

		EXPECT_FALSE( time::DateTime::tryParse( "123", result ) );
		EXPECT_THROW( time::DateTime( "123" ), std::invalid_argument );

		// Completely malformed strings
		EXPECT_FALSE( time::DateTime::tryParse( "not-a-date", result ) );
		EXPECT_THROW( time::DateTime( "not-a-date" ), std::invalid_argument );

		EXPECT_FALSE( time::DateTime::tryParse( "random text", result ) );
		EXPECT_THROW( time::DateTime( "random text" ), std::invalid_argument );
	}

	TEST( DateTimeIntegration, DateTimeBoundaryViolations )
	{
		time::DateTime result;

		// Date/time boundary violations

		// Invalid month - months must be 1-12
		EXPECT_FALSE( time::DateTime::tryParse( "1994-13-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-13-20T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTime::tryParse( "1994-00-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-00-20T10:25:33Z" ), std::invalid_argument );

		// Invalid day - February 30th doesn't exist
		EXPECT_FALSE( time::DateTime::tryParse( "1994-02-30T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-02-30T10:25:33Z" ), std::invalid_argument );
		// Day 32 doesn't exist in any month
		EXPECT_FALSE( time::DateTime::tryParse( "1994-01-32T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-01-32T10:25:33Z" ), std::invalid_argument );
		// Day 0 is invalid
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-00T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-00T10:25:33Z" ), std::invalid_argument );

		// Invalid hour - hours must be 0-23
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T25:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T25:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T24:00:00Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T24:00:00Z" ), std::invalid_argument );

		// Invalid minute/second - minutes and seconds must be 0-59
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T10:60:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T10:60:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T10:25:60Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T10:25:60Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTime::tryParse( "1994-11-20T10:61:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-11-20T10:61:33Z" ), std::invalid_argument );

		// Additional boundary tests
		// Leap year edge cases
		EXPECT_FALSE( time::DateTime::tryParse( "1900-02-29T10:25:33Z", result ) ); // 1900 is not a leap year
		EXPECT_THROW( time::DateTime( "1900-02-29T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTime::tryParse( "2100-02-29T10:25:33Z", result ) ); // 2100 is not a leap year
		EXPECT_THROW( time::DateTime( "2100-02-29T10:25:33Z" ), std::invalid_argument );

		// Valid leap year should work
		EXPECT_TRUE( time::DateTime::tryParse( "2024-02-29T10:25:33Z", result ) ); // 2024 is a leap year
		EXPECT_NO_THROW( time::DateTime( "2024-02-29T10:25:33Z" ) );

		// April has only 30 days
		EXPECT_FALSE( time::DateTime::tryParse( "1994-04-31T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTime( "1994-04-31T10:25:33Z" ), std::invalid_argument );
	}

	//=====================================================================
	// DateTimeOffset type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( DateTimeOffsetTest, Construction )
	{
		// Default constructor
		time::DateTimeOffset dto1;
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, dto1.ticks() );
		EXPECT_EQ( 0, dto1.totalOffsetMinutes() );

		// Constructor from DateTime and offset
		time::DateTime dt{ 2024, 6, 15, 14, 30, 45 };
		auto offset{ time::TimeSpan::fromHours( 2.0 ) };
		time::DateTimeOffset dto2{ dt, offset };
		EXPECT_EQ( dt.ticks(), dto2.ticks() );
		EXPECT_EQ( 120, dto2.totalOffsetMinutes() );

		// Constructor from ticks and offset
		time::DateTimeOffset dto3{ constants::datetime::UNIX_EPOCH_TICKS, time::TimeSpan::fromHours( -5.0 ) };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, dto3.ticks() );
		EXPECT_EQ( -300, dto3.totalOffsetMinutes() );

		// Constructor from date components and offset
		time::DateTimeOffset dto4{ 2024, 1, 15, time::TimeSpan::fromMinutes( 90 ) };
		EXPECT_EQ( 2024, dto4.year() );
		EXPECT_EQ( 1, dto4.month() );
		EXPECT_EQ( 15, dto4.day() );
		EXPECT_EQ( 90, dto4.totalOffsetMinutes() );

		// Constructor with time components
		time::DateTimeOffset dto5{ 2024, 6, 15, 14, 30, 45, time::TimeSpan::fromHours( -8.0 ) };
		EXPECT_EQ( 2024, dto5.year() );
		EXPECT_EQ( 6, dto5.month() );
		EXPECT_EQ( 15, dto5.day() );
		EXPECT_EQ( 14, dto5.hour() );
		EXPECT_EQ( 30, dto5.minute() );
		EXPECT_EQ( 45, dto5.second() );
		EXPECT_EQ( -480, dto5.totalOffsetMinutes() );

		// Constructor with milliseconds
		time::DateTimeOffset dto6{ 2024, 12, 25, 23, 59, 59, 999, time::TimeSpan::fromHours( 9.0 ) };
		EXPECT_EQ( 2024, dto6.year() );
		EXPECT_EQ( 999, dto6.millisecond() );
		EXPECT_EQ( 540, dto6.totalOffsetMinutes() );

		// Constructor with microseconds
		time::DateTimeOffset dto7{ 2024, 1, 1, 0, 0, 0, 0, 500, time::TimeSpan::fromMinutes( 30 ) };
		EXPECT_EQ( 500, dto7.microsecond() );
		EXPECT_EQ( 30, dto7.totalOffsetMinutes() );
	}

	TEST( DateTimeOffsetTest, ISO8601StringConstructor )
	{
		// Basic ISO 8601 format with UTC
		time::DateTimeOffset dto1{ "2024-06-15T14:30:45Z" };
		EXPECT_EQ( 2024, dto1.year() );
		EXPECT_EQ( 6, dto1.month() );
		EXPECT_EQ( 15, dto1.day() );
		EXPECT_EQ( 14, dto1.hour() );
		EXPECT_EQ( 30, dto1.minute() );
		EXPECT_EQ( 45, dto1.second() );
		EXPECT_EQ( 0, dto1.totalOffsetMinutes() );

		// ISO 8601 with positive offset
		time::DateTimeOffset dto2{ "2024-01-01T12:00:00+02:00" };
		EXPECT_EQ( 2024, dto2.year() );
		EXPECT_EQ( 12, dto2.hour() );
		EXPECT_EQ( 120, dto2.totalOffsetMinutes() );

		// ISO 8601 with negative offset
		time::DateTimeOffset dto3{ "2024-07-04T16:00:00-05:00" };
		EXPECT_EQ( 2024, dto3.year() );
		EXPECT_EQ( 7, dto3.month() );
		EXPECT_EQ( 4, dto3.day() );
		EXPECT_EQ( 16, dto3.hour() );
		EXPECT_EQ( -300, dto3.totalOffsetMinutes() );

		// Invalid format should throw
		EXPECT_THROW( time::DateTimeOffset( "invalid-date-string" ), std::invalid_argument );
		EXPECT_THROW( time::DateTimeOffset( "2024-13-01T00:00:00Z" ), std::invalid_argument );
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	TEST( DateTimeOffsetTest, ComparisonOperators )
	{
		// Same UTC time with different offsets should be equal
		time::DateTimeOffset dto1{ 2024, 1, 1, 14, 0, 0, time::TimeSpan::fromHours( 2.0 ) };
		time::DateTimeOffset dto2{ 2024, 1, 1, 12, 0, 0, time::TimeSpan::fromHours( 0.0 ) };
		time::DateTimeOffset dto3{ 2024, 1, 1, 7, 0, 0, time::TimeSpan::fromHours( -5.0 ) };

		// All represent 12:00 UTC
		EXPECT_TRUE( dto1 == dto2 );
		EXPECT_TRUE( dto2 == dto3 );
		EXPECT_TRUE( dto1 == dto3 );

		// Different UTC times
		time::DateTimeOffset dto4{ 2024, 1, 1, 13, 0, 0, time::TimeSpan::fromHours( 0.0 ) };
		EXPECT_FALSE( dto1 == dto4 );
		EXPECT_TRUE( dto1 != dto4 );
		EXPECT_TRUE( dto1 < dto4 );
		EXPECT_TRUE( dto4 > dto1 );

		// Ordering based on UTC time
		EXPECT_TRUE( dto1 <= dto2 );
		EXPECT_TRUE( dto4 >= dto1 );
	}

	TEST( DateTimeOffsetTest, ComparisonMethods )
	{
		time::DateTimeOffset dto1{ 2024, 1, 1, 14, 0, 0, time::TimeSpan::fromHours( 2.0 ) };
		time::DateTimeOffset dto2{ 2024, 1, 1, 12, 0, 0, time::TimeSpan::fromHours( 0.0 ) };
		time::DateTimeOffset dto3{ 2024, 1, 1, 13, 0, 0, time::TimeSpan::fromHours( 0.0 ) };

		// Compare to method (based on UTC time)
		EXPECT_EQ( 0, dto1.compareTo( dto2 ) );
		EXPECT_LT( 0, dto3.compareTo( dto1 ) );
		EXPECT_GT( 0, dto1.compareTo( dto3 ) );

		// Equals methods
		EXPECT_TRUE( dto1.equals( dto2 ) );
		EXPECT_FALSE( dto1.equals( dto3 ) );

		// Exact equals (checks both local time and offset)
		EXPECT_FALSE( dto1.equalsExact( dto2 ) );
		time::DateTimeOffset dto1Copy{ 2024, 1, 1, 14, 0, 0, time::TimeSpan::fromHours( 2.0 ) };
		EXPECT_TRUE( dto1.equalsExact( dto1Copy ) );

		// Static compare method
		EXPECT_EQ( 0, time::DateTimeOffset::compare( dto1, dto2 ) );
		EXPECT_LT( 0, time::DateTimeOffset::compare( dto3, dto1 ) );
	}

	TEST( DateTimeOffsetTest, ArithmeticOperators )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 12, 0, 0, time::TimeSpan::fromHours( 3.0 ) };
		auto oneHour{ time::TimeSpan::fromHours( 1.0 ) };
		auto oneDay{ time::TimeSpan::fromDays( 1.0 ) };

		// Addition preserves offset
		time::DateTimeOffset dtoPlusHour = dto + oneHour;
		EXPECT_EQ( 13, dtoPlusHour.hour() );
		EXPECT_EQ( 180, dtoPlusHour.totalOffsetMinutes() );

		// Subtraction preserves offset
		time::DateTimeOffset dtoMinusHour = dto - oneHour;
		EXPECT_EQ( 11, dtoMinusHour.hour() );
		EXPECT_EQ( 180, dtoMinusHour.totalOffsetMinutes() );

		// DateTimeOffset difference (based on UTC time)
		time::TimeSpan diff = dtoPlusHour - dto;
		EXPECT_DOUBLE_EQ( 1.0, diff.totalHours() );

		// In-place operations
		dto += oneDay;
		EXPECT_EQ( 16, dto.day() );
		EXPECT_EQ( 180, dto.totalOffsetMinutes() );

		dto -= oneDay;
		EXPECT_EQ( 15, dto.day() );
	}

	TEST( DateTimeOffsetTest, ArithmeticMethods )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 12, 0, 0, time::TimeSpan::fromHours( 3.0 ) };

		// Add methods
		auto plusDays{ dto.addDays( 5.5 ) };
		EXPECT_EQ( 21, plusDays.day() );
		EXPECT_EQ( 0, plusDays.hour() );
		EXPECT_EQ( 180, plusDays.totalOffsetMinutes() );

		auto plusHours{ dto.addHours( 25.0 ) };
		EXPECT_EQ( 16, plusHours.day() );
		EXPECT_EQ( 13, plusHours.hour() );

		auto plusMinutes{ dto.addMinutes( 90.0 ) };
		EXPECT_EQ( 13, plusMinutes.hour() );
		EXPECT_EQ( 30, plusMinutes.minute() );

		auto plusSeconds{ dto.addSeconds( 3665.0 ) };
		EXPECT_EQ( 13, plusSeconds.hour() );
		EXPECT_EQ( 1, plusSeconds.minute() );
		EXPECT_EQ( 5, plusSeconds.second() );

		auto plusMs{ dto.addMilliseconds( 2500.0 ) };
		EXPECT_EQ( 2, plusMs.second() );
		EXPECT_EQ( 500, plusMs.millisecond() );

		// Add months with calendar logic
		time::DateTimeOffset jan31{ 2024, 1, 31, 12, 0, 0, time::TimeSpan::fromHours( 2.0 ) };
		time::DateTimeOffset feb = jan31.addMonths( 1 );
		EXPECT_EQ( 2, feb.month() );
		EXPECT_EQ( 29, feb.day() );

		time::DateTimeOffset plusYear = dto.addYears( 1 );
		EXPECT_EQ( 2025, plusYear.year() );
		EXPECT_EQ( 6, plusYear.month() );
		EXPECT_EQ( 15, plusYear.day() );

		// Add ticks
		time::DateTimeOffset plusTicks = dto.addTicks( 10000000LL );
		EXPECT_EQ( 1, plusTicks.second() );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	TEST( DateTimeOffsetTest, PropertyAccessors )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, 123, 456, time::TimeSpan::fromMinutes( 90 ) };

		// Date and time components
		EXPECT_EQ( 2024, dto.year() );
		EXPECT_EQ( 6, dto.month() );
		EXPECT_EQ( 15, dto.day() );
		EXPECT_EQ( 14, dto.hour() );
		EXPECT_EQ( 30, dto.minute() );
		EXPECT_EQ( 45, dto.second() );
		EXPECT_EQ( 123, dto.millisecond() );
		EXPECT_EQ( 456, dto.microsecond() );

		// Offset properties
		EXPECT_EQ( 90, dto.totalOffsetMinutes() );
		auto offset{ dto.offset() };
		EXPECT_DOUBLE_EQ( 1.5, offset.totalHours() );

		// DateTime properties
		auto localDt{ dto.localDateTime() };
		auto utcDt{ dto.utcDateTime() };
		EXPECT_EQ( dto.ticks(), localDt.ticks() );
		EXPECT_NE( localDt.ticks(), utcDt.ticks() );

		// Day calculations
		EXPECT_EQ( 6, dto.dayOfWeek() );
		int expectedDayOfYear{ 31 + 29 + 31 + 30 + 31 + 15 };
		EXPECT_EQ( expectedDayOfYear, dto.dayOfYear() );
	}

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	TEST( DateTimeOffsetConversion, ConversionMethods )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, time::TimeSpan::fromHours( 2.0 ) };

		// Date extraction
		time::DateTimeOffset dateOnly = dto.date();
		EXPECT_EQ( 2024, dateOnly.year() );
		EXPECT_EQ( 6, dateOnly.month() );
		EXPECT_EQ( 15, dateOnly.day() );
		EXPECT_EQ( 0, dateOnly.hour() );
		EXPECT_EQ( 0, dateOnly.minute() );
		EXPECT_EQ( 0, dateOnly.second() );
		EXPECT_EQ( 120, dateOnly.totalOffsetMinutes() );

		// Offset conversion
		auto dtoUtc{ dto.toUniversalTime() };
		EXPECT_EQ( 0, dtoUtc.totalOffsetMinutes() );
		EXPECT_EQ( 12, dtoUtc.hour() );

		auto dtoNewOffset{ dto.toOffset( time::TimeSpan::fromHours( -5.0 ) ) };
		EXPECT_EQ( -300, dtoNewOffset.totalOffsetMinutes() );
		EXPECT_EQ( 7, dtoNewOffset.hour() );

		// Unix timestamp conversions
		auto epoch{ time::DateTimeOffset::unixEpoch() };
		EXPECT_EQ( 0, epoch.toUnixSeconds() );
		EXPECT_EQ( 0, epoch.toUnixMilliseconds() );

		// File time conversion
		auto fileTime{ dto.toFILETIME() };
		auto fromFileTime{ time::DateTimeOffset::fromFileTime( fileTime ) };
		EXPECT_EQ( dto.utcDateTime().ticks(), fromFileTime.utcDateTime().ticks() );

		// Time of day
		time::TimeSpan timeOfDay = dto.timeOfDay();
		EXPECT_DOUBLE_EQ( 14.5125, timeOfDay.totalHours() );
	}

	TEST( DateTimeOffsetConversion, StringFormatting )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, 123, time::TimeSpan::fromHours( 2.0 ) };

		// Basic ISO 8601
		auto basic{ dto.toString() };
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", basic );

		// Specific formats
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", dto.toString( time::DateTime::Format::Iso8601Basic ) );
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", dto.toString( time::DateTime::Format::Iso8601WithOffset ) );
		EXPECT_EQ( "2024-06-15", dto.toString( time::DateTime::Format::DateOnly ) );
		EXPECT_EQ( "14:30:45+02:00", dto.toString( time::DateTime::Format::TimeOnly ) );

		// Extended format with fractional seconds
		auto extended{ dto.toString( time::DateTime::Format::Iso8601Extended ) };
		EXPECT_TRUE( extended.find( "2024-06-15T14:30:45." ) != std::string::npos );
		EXPECT_TRUE( extended.find( "+02:00" ) != std::string::npos );

		// UTC offset formatting
		time::DateTimeOffset utc{ 2024, 1, 1, 12, 0, 0, time::TimeSpan::fromHours( 0.0 ) };
		EXPECT_EQ( "2024-01-01T12:00:00Z", utc.toString() );

		// Negative offset
		time::DateTimeOffset negative{ 2024, 1, 1, 8, 0, 0, time::TimeSpan::fromHours( -5.0 ) };
		EXPECT_EQ( "2024-01-01T08:00:00-05:00", negative.toString() );

		// Unix timestamps
		auto epoch{ time::DateTimeOffset::unixEpoch() };
		EXPECT_EQ( "0", epoch.toString( time::DateTime::Format::UnixSeconds ) );
		EXPECT_EQ( "0", epoch.toString( time::DateTime::Format::UnixMilliseconds ) );

		// ISO 8601 extended method
		auto iso8601Ext{ dto.toIso8601Extended() };
		EXPECT_EQ( extended, iso8601Ext );
	}

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	TEST( DateTimeOffsetValidation, ValidationMethods )
	{
		// Valid DateTimeOffset
		time::DateTimeOffset validDto{ 2024, 6, 15, 12, 30, 45, time::TimeSpan::fromHours( 3.0 ) };
		EXPECT_TRUE( validDto.isValid() );
	}

	//----------------------------------------------
	// Factory
	//----------------------------------------------

	TEST( DateTimeOffsetFactory, StaticFactoryMethods )
	{
		// Min and max values
		auto minVal{ time::DateTimeOffset::minValue() };
		auto maxVal{ time::DateTimeOffset::maxValue() };
		EXPECT_EQ( constants::datetime::MIN_DATETIME_TICKS, minVal.ticks() );
		EXPECT_EQ( constants::datetime::MAX_DATETIME_TICKS, maxVal.ticks() );
		EXPECT_EQ( 0, minVal.totalOffsetMinutes() );
		EXPECT_EQ( 0, maxVal.totalOffsetMinutes() );

		// Unix epoch
		auto epoch{ time::DateTimeOffset::unixEpoch() };
		EXPECT_EQ( constants::datetime::UNIX_EPOCH_TICKS, epoch.ticks() );
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.totalOffsetMinutes() );

		// Now, utcNow, and today (basic smoke tests)
		auto now{ time::DateTimeOffset::now() };
		auto utcNow{ time::DateTimeOffset::utcNow() };
		auto today{ time::DateTimeOffset::today() };

		EXPECT_TRUE( now.isValid() );
		EXPECT_TRUE( utcNow.isValid() );
		EXPECT_TRUE( today.isValid() );

		EXPECT_EQ( 0, utcNow.totalOffsetMinutes() );
		EXPECT_EQ( 0, today.hour() );
		EXPECT_EQ( 0, today.minute() );
		EXPECT_EQ( 0, today.second() );

		// Factory from timestamps
		auto fromSeconds{ time::DateTimeOffset::fromUnixTimeSeconds( 86400 ) };
		EXPECT_EQ( 1970, fromSeconds.year() );
		EXPECT_EQ( 1, fromSeconds.month() );
		EXPECT_EQ( 2, fromSeconds.day() );
		EXPECT_EQ( 0, fromSeconds.totalOffsetMinutes() );

		auto fromMs{ time::DateTimeOffset::fromUnixTimeMilliseconds( 86400000 ) };
		EXPECT_EQ( 1970, fromMs.year() );
		EXPECT_EQ( 1, fromMs.month() );
		EXPECT_EQ( 2, fromMs.day() );
		EXPECT_EQ( 0, fromMs.totalOffsetMinutes() );

		// From file time
		std::int64_t fileTime{ 132679392000000000LL };
		auto fromFileTime{ time::DateTimeOffset::fromFileTime( fileTime ) };
		EXPECT_TRUE( fromFileTime.isValid() );
		EXPECT_EQ( 0, fromFileTime.totalOffsetMinutes() );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( DateTimeOffsetStringParsing, TryParseMethod )
	{
		time::DateTimeOffset result;

		// Valid parsing with UTC
		EXPECT_TRUE( time::DateTimeOffset::tryParse( "2024-06-15T14:30:45Z", result ) );
		EXPECT_EQ( 2024, result.year() );
		EXPECT_EQ( 6, result.month() );
		EXPECT_EQ( 15, result.day() );
		EXPECT_EQ( 14, result.hour() );
		EXPECT_EQ( 30, result.minute() );
		EXPECT_EQ( 45, result.second() );
		EXPECT_EQ( 0, result.totalOffsetMinutes() );

		// Valid parsing with positive offset
		EXPECT_TRUE( time::DateTimeOffset::tryParse( "2024-01-01T12:00:00+02:30", result ) );
		EXPECT_EQ( 12, result.hour() );
		EXPECT_EQ( 150, result.totalOffsetMinutes() );

		// Valid parsing with negative offset
		EXPECT_TRUE( time::DateTimeOffset::tryParse( "2024-07-04T16:00:00-05:00", result ) );
		EXPECT_EQ( 16, result.hour() );
		EXPECT_EQ( -300, result.totalOffsetMinutes() );

		// Invalid parsing
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "invalid-date", result ) );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "2024-13-01T00:00:00Z", result ) );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "2024-01-01T25:00:00Z", result ) );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "2024-01-01T12:00:00+15:00", result ) );
	}

	TEST( DateTimeOffsetStringParsing, ParseMethod )
	{
		// Valid parsing with UTC timezone
		auto result1{ time::DateTimeOffset::parse( "2024-06-15T14:30:45Z" ) };
		EXPECT_EQ( 2024, result1.year() );
		EXPECT_EQ( 6, result1.month() );
		EXPECT_EQ( 15, result1.day() );
		EXPECT_EQ( 14, result1.hour() );
		EXPECT_EQ( 30, result1.minute() );
		EXPECT_EQ( 45, result1.second() );
		EXPECT_EQ( 0, result1.totalOffsetMinutes() );

		// Valid parsing with positive offset
		auto result2{ time::DateTimeOffset::parse( "2024-06-15T14:30:45+02:00" ) };
		EXPECT_EQ( 2024, result2.year() );
		EXPECT_EQ( 6, result2.month() );
		EXPECT_EQ( 15, result2.day() );
		EXPECT_EQ( 14, result2.hour() );
		EXPECT_EQ( 30, result2.minute() );
		EXPECT_EQ( 45, result2.second() );
		EXPECT_EQ( 120, result2.totalOffsetMinutes() ); // +02:00 = 120 minutes

		// Valid parsing with negative offset
		auto result3{ time::DateTimeOffset::parse( "2024-06-15T14:30:45-05:00" ) };
		EXPECT_EQ( 2024, result3.year() );
		EXPECT_EQ( 6, result3.month() );
		EXPECT_EQ( 15, result3.day() );
		EXPECT_EQ( 14, result3.hour() );
		EXPECT_EQ( 30, result3.minute() );
		EXPECT_EQ( 45, result3.second() );
		EXPECT_EQ( -300, result3.totalOffsetMinutes() ); // -05:00 = -300 minutes

		// Valid parsing with fractional seconds
		auto result4{ time::DateTimeOffset::parse( "2024-12-31T23:59:59.999Z" ) };
		EXPECT_EQ( 2024, result4.year() );
		EXPECT_EQ( 12, result4.month() );
		EXPECT_EQ( 31, result4.day() );
		EXPECT_EQ( 23, result4.hour() );
		EXPECT_EQ( 59, result4.minute() );
		EXPECT_EQ( 59, result4.second() );
		EXPECT_EQ( 999, result4.millisecond() );
		EXPECT_EQ( 0, result4.totalOffsetMinutes() );

		// Invalid parsing should throw
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "" ), std::invalid_argument );
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "invalid" ), std::invalid_argument );
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "2024-13-01T12:00:00Z" ), std::invalid_argument );		 // Invalid month
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "2024-01-32T12:00:00Z" ), std::invalid_argument );		 // Invalid day
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "2024-01-01T25:00:00Z" ), std::invalid_argument );		 // Invalid hour
		EXPECT_THROW( (void)time::DateTimeOffset::parse( "2024-01-01T12:00:00+15:00" ), std::invalid_argument ); // Invalid offset (>14:00)
	}

	//----------------------------------------------
	// Stream
	//----------------------------------------------

	TEST( DateTimeOffsetStream, StreamOperators )
	{
		time::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, time::TimeSpan::fromHours( 2.0 ) };

		// Output stream operator
		std::ostringstream oss;
		oss << dto;
		EXPECT_EQ( "2024-06-15T14:30:45+02:00", oss.str() );

		// Input stream operator
		std::istringstream iss( "2024-12-25T23:59:59-08:00" );
		time::DateTimeOffset parsed;
		iss >> parsed;
		EXPECT_EQ( 2024, parsed.year() );
		EXPECT_EQ( 12, parsed.month() );
		EXPECT_EQ( 25, parsed.day() );
		EXPECT_EQ( 23, parsed.hour() );
		EXPECT_EQ( 59, parsed.minute() );
		EXPECT_EQ( 59, parsed.second() );
		EXPECT_EQ( -480, parsed.totalOffsetMinutes() );

		// Invalid input should set fail bit
		std::istringstream issInvalid( "invalid-date" );
		time::DateTimeOffset invalidParsed;
		issInvalid >> invalidParsed;
		EXPECT_TRUE( issInvalid.fail() );
	}

	//----------------------------------------------
	// EdgeCases
	//----------------------------------------------

	TEST( DateTimeOffsetEdgeCases, EdgeCases )
	{
		// Maximum positive offset
		auto maxOffset{ time::TimeSpan::fromHours( 14.0 ) };
		time::DateTimeOffset maxOffsetDto{ 2024, 1, 1, 12, 0, 0, maxOffset };
		EXPECT_TRUE( maxOffsetDto.isValid() );
		EXPECT_EQ( 840, maxOffsetDto.totalOffsetMinutes() );

		// Maximum negative offset
		auto minOffset{ time::TimeSpan::fromHours( -14.0 ) };
		time::DateTimeOffset minOffsetDto{ 2024, 1, 1, 12, 0, 0, minOffset };
		EXPECT_TRUE( minOffsetDto.isValid() );
		EXPECT_EQ( -840, minOffsetDto.totalOffsetMinutes() );

		// Boundary datetime values
		auto minDateTime{ time::DateTimeOffset::minValue() };
		auto maxDateTime{ time::DateTimeOffset::maxValue() };
		EXPECT_TRUE( minDateTime.isValid() );
		EXPECT_TRUE( maxDateTime.isValid() );

		// Cross day boundary with offset
		time::DateTimeOffset endOfDay{ 2024, 1, 1, 23, 30, 0, time::TimeSpan::fromHours( -6.0 ) };
		auto nextDayUtc{ endOfDay.toUniversalTime() };
		EXPECT_EQ( 2, nextDayUtc.day() );
		EXPECT_EQ( 5, nextDayUtc.hour() );

		// Leap year with offset
		time::DateTimeOffset leapDay{ 2024, 2, 29, 12, 0, 0, time::TimeSpan::fromHours( 1.0 ) };
		EXPECT_EQ( 29, leapDay.day() );
		EXPECT_TRUE( leapDay.isValid() );
	}

	//----------------------------------------------
	// Integration
	//----------------------------------------------

	TEST( DateTimeOffsetIntegration, DateTimeIntegration )
	{
		// Conversion between DateTime and DateTimeOffset
		time::DateTime dt{ 2024, 6, 15, 14, 30, 45 };
		time::DateTimeOffset dto{ dt, time::TimeSpan::fromHours( 3.0 ) };

		EXPECT_EQ( dt.ticks(), dto.ticks() );
		EXPECT_EQ( dt.year(), dto.year() );
		EXPECT_EQ( dt.hour(), dto.hour() );

		// Extract DateTime from DateTimeOffset
		auto localDt{ dto.localDateTime() };
		auto utcDt{ dto.utcDateTime() };

		EXPECT_EQ( dt.ticks(), localDt.ticks() );
		EXPECT_NE( dt.ticks(), utcDt.ticks() );
	}

	TEST( DateTimeOffsetIntegration, RoundTripSerialization )
	{
		time::DateTimeOffset original{ 2024, 6, 15, 14, 30, 45, 123, time::TimeSpan::fromMinutes( 150 ) };

		// Round trip through ISO 8601 string
		std::string serialized = original.toString( time::DateTime::Format::Iso8601Extended );
		time::DateTimeOffset deserialized;
		EXPECT_TRUE( time::DateTimeOffset::tryParse( serialized, deserialized ) );

		// Should be very close (within millisecond precision)
		auto diff{ original - deserialized };
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
		EXPECT_EQ( original.totalOffsetMinutes(), deserialized.totalOffsetMinutes() );
	}

	TEST( DateTimeOffsetIntegration, CrossPlatformConsistency )
	{
		// Test that key values are consistent across platforms
		auto epoch{ time::DateTimeOffset::unixEpoch() };
		EXPECT_EQ( 1970, epoch.year() );
		EXPECT_EQ( 1, epoch.month() );
		EXPECT_EQ( 1, epoch.day() );
		EXPECT_EQ( 0, epoch.hour() );
		EXPECT_EQ( 0, epoch.minute() );
		EXPECT_EQ( 0, epoch.second() );
		EXPECT_EQ( 0, epoch.totalOffsetMinutes() );

		// Known Unix timestamp with offset
		auto knownDate{ time::DateTimeOffset::fromUnixTimeSeconds( 1609459200 ) };
		EXPECT_EQ( 2021, knownDate.year() );
		EXPECT_EQ( 1, knownDate.month() );
		EXPECT_EQ( 1, knownDate.day() );
		EXPECT_EQ( 0, knownDate.totalOffsetMinutes() );

		// File time round trip
		time::DateTimeOffset testDto{ 2024, 6, 15, 14, 30, 45, time::TimeSpan::fromHours( 2.0 ) };
		std::int64_t fileTime{ testDto.toFILETIME() };
		auto fromFileTime{ time::DateTimeOffset::fromFileTime( fileTime ) };

		// Should match in UTC time
		EXPECT_EQ( testDto.utcDateTime().ticks(), fromFileTime.utcDateTime().ticks() );
	}

	TEST( DateTimeOffsetIntegration, ISO8601FormatValidationEdgeCases )
	{
		time::DateTimeOffset result;

		// Invalid ISO 8601 format variations for DateTimeOffset - based on actual parser behavior

		// Wrong separators - should use dashes and colons, not slashes
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994/11/20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994/11/20T10:25:33Z" ), std::invalid_argument );

		// Invalid timezone offsets - beyond ±14:00 range (841+ minutes)
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33+25:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33+25:00" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33-15:30", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33-15:30" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33+14:01", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33+14:01" ), std::invalid_argument );

		// Invalid timezone minute values - must be 0-59
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33+02:60", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33+02:60" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33-05:75", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33-05:75" ), std::invalid_argument );

		// Malformed timezone format with invalid characters
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:33+AB:CD", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:33+AB:CD" ), std::invalid_argument );

		// 2-digit year - parser expects exactly 4 digits
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "94-11-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "94-11-20T10:25:33Z" ), std::invalid_argument );

		// Missing date separators
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "19941120T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "19941120T10:25:33Z" ), std::invalid_argument );

		// Wrong time separators
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10.25.33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10.25.33Z" ), std::invalid_argument );

		// Non-numeric characters in date/time components
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "ABCD-11-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "ABCD-11-20T10:25:33Z" ), std::invalid_argument );

		// Empty or too short strings
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "", result ) );
		EXPECT_THROW( time::DateTimeOffset( "" ), std::invalid_argument );

		EXPECT_FALSE( time::DateTimeOffset::tryParse( "123", result ) );
		EXPECT_THROW( time::DateTimeOffset( "123" ), std::invalid_argument );

		// Completely malformed strings
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "not-a-date", result ) );
		EXPECT_THROW( time::DateTimeOffset( "not-a-date" ), std::invalid_argument );
	}

	TEST( DateTimeOffsetIntegration, DateTimeBoundaryViolations )
	{
		time::DateTimeOffset result;

		// Date/time boundary violations for DateTimeOffset

		// Invalid month - months must be 1-12
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-13-20T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-13-20T10:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-00-20T10:25:33+02:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-00-20T10:25:33+02:00" ), std::invalid_argument );

		// Invalid day - February 30th doesn't exist
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-02-30T10:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-02-30T10:25:33Z" ), std::invalid_argument );
		// Day 32 doesn't exist in any month
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-01-32T10:25:33-05:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-01-32T10:25:33-05:00" ), std::invalid_argument );
		// Day 0 is invalid
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-00T10:25:33+08:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-00T10:25:33+08:00" ), std::invalid_argument );

		// Invalid hour - hours must be 0-23
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T25:25:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T25:25:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T24:00:00+02:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T24:00:00+02:00" ), std::invalid_argument );

		// Invalid minute/second - minutes and seconds must be 0-59
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:60:33Z", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:60:33Z" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:25:60-03:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:25:60-03:00" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-11-20T10:61:33+07:00", result ) );
		EXPECT_THROW( time::DateTimeOffset( "1994-11-20T10:61:33+07:00" ), std::invalid_argument );

		// Leap year edge cases with timezone offsets
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1900-02-29T10:25:33+01:00", result ) ); // 1900 is not a leap year
		EXPECT_THROW( time::DateTimeOffset( "1900-02-29T10:25:33+01:00" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "2100-02-29T10:25:33-02:00", result ) ); // 2100 is not a leap year
		EXPECT_THROW( time::DateTimeOffset( "2100-02-29T10:25:33-02:00" ), std::invalid_argument );

		// Valid leap year should work with timezone
		EXPECT_TRUE( time::DateTimeOffset::tryParse( "2024-02-29T10:25:33+05:30", result ) ); // 2024 is a leap year
		EXPECT_NO_THROW( time::DateTimeOffset( "2024-02-29T10:25:33+05:30" ) );

		// Month-specific day validation with timezones
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-04-31T10:25:33-08:00", result ) ); // April has only 30 days
		EXPECT_THROW( time::DateTimeOffset( "1994-04-31T10:25:33-08:00" ), std::invalid_argument );
		EXPECT_FALSE( time::DateTimeOffset::tryParse( "1994-06-31T10:25:33+09:00", result ) ); // June has only 30 days
		EXPECT_THROW( time::DateTimeOffset( "1994-06-31T10:25:33+09:00" ), std::invalid_argument );
	}

	//----------------------------------------------
	// std::chrono interoperability Tests
	//----------------------------------------------

	TEST( DateTimeChrono, ConstructorFromTimePoint )
	{
		// Test construction from std::chrono::system_clock::time_point
		auto now{ std::chrono::system_clock::now() };
		time::DateTime dt{ now };

		// Should be valid and close to current time
		EXPECT_TRUE( dt.isValid() );

		// Convert back and compare
		auto converted{ dt.toChrono() };
		auto diff{ std::chrono::duration_cast<std::chrono::milliseconds>( converted - now ).count() };

		// Within 1 second due to precision differences
		EXPECT_LT( std::abs( diff ), 1000 );
	}

	TEST( DateTimeChrono, ToChronoConversion )
	{
		// Test known DateTime to chrono conversion
		auto epoch{ time::DateTime::epoch() };
		auto chronoEpoch{ epoch.toChrono() };

		// Should match Unix epoch
		auto duration{ chronoEpoch.time_since_epoch() };
		auto seconds{ std::chrono::duration_cast<std::chrono::seconds>( duration ).count() };
		EXPECT_EQ( 0, seconds );

		// Test specific date
		time::DateTime specificDate{ 2024, 6, 15, 14, 30, 45, 123 };
		auto chronoPoint{ specificDate.toChrono() };

		// Convert back to verify round-trip
		time::DateTime roundTrip{ chronoPoint };
		EXPECT_EQ( specificDate.year(), roundTrip.year() );
		EXPECT_EQ( specificDate.month(), roundTrip.month() );
		EXPECT_EQ( specificDate.day(), roundTrip.day() );
		EXPECT_EQ( specificDate.hour(), roundTrip.hour() );
		EXPECT_EQ( specificDate.minute(), roundTrip.minute() );
		EXPECT_EQ( specificDate.second(), roundTrip.second() );

		// Millisecond precision might have small differences
		EXPECT_LT( std::abs( specificDate.millisecond() - roundTrip.millisecond() ), 2 );
	}

	TEST( DateTimeChrono, FromChronoStaticMethod )
	{
		// Test static fromChrono method
		auto now{ std::chrono::system_clock::now() };
		time::DateTime dt = time::DateTime::fromChrono( now );

		EXPECT_TRUE( dt.isValid() );

		// Should be equivalent to constructor
		time::DateTime dtConstructor{ now };
		EXPECT_EQ( dt.ticks(), dtConstructor.ticks() );
	}

	TEST( DateTimeChrono, RoundTripConversions )
	{
		// Test round-trip: DateTime -> chrono -> DateTime
		time::DateTime original{ 2024, 1, 1, 12, 0, 0, 0 };
		auto chronoPoint{ original.toChrono() };
		auto roundTrip{ time::DateTime::fromChrono( chronoPoint ) };

		EXPECT_EQ( original.ticks(), roundTrip.ticks() );
		EXPECT_EQ( original.year(), roundTrip.year() );
		EXPECT_EQ( original.month(), roundTrip.month() );
		EXPECT_EQ( original.day(), roundTrip.day() );
		EXPECT_EQ( original.hour(), roundTrip.hour() );
		EXPECT_EQ( original.minute(), roundTrip.minute() );
		EXPECT_EQ( original.second(), roundTrip.second() );

		// Test with fractional seconds
		time::DateTime withMs{ 2024, 6, 15, 14, 30, 45, 999 };
		auto chronoMs{ withMs.toChrono() };
		auto roundTripMs{ time::DateTime::fromChrono( chronoMs ) };

		// Should preserve most precision
		time::TimeSpan diff = withMs - roundTripMs;
		EXPECT_LT( std::abs( diff.totalMilliseconds() ), 1.0 );
	}

	TEST( DateTimeChrono, PrecisionHandling )
	{
		// Test precision boundaries
		time::DateTime dt{ 2024, 1, 1, 12, 0, 0, 0 };

		// Add specific number of ticks (100-nanosecond precision)
		// 123.4567 milliseconds
		std::int64_t extraTicks{ 1234567 };
		time::DateTime dtWithTicks{ dt.ticks() + extraTicks };

		auto chronoPoint{ dtWithTicks.toChrono() };
		time::DateTime converted{ chronoPoint };

		// Should preserve tick-level precision
		EXPECT_EQ( dtWithTicks.ticks(), converted.ticks() );
	}

	TEST( DateTimeChrono, BoundaryValues )
	{
		// Min and max DateTime values cannot round-trip through chrono on Linux
		auto minDt{ time::DateTime::minValue() };
		auto minChrono{ minDt.toChrono() };
		time::DateTime minRoundTrip{ minChrono };

		// Expect clamping behavior - not equality for extreme values
		EXPECT_NE( minDt.ticks(), minRoundTrip.ticks() );
		EXPECT_TRUE( minRoundTrip.isValid() );

		// Test maximum DateTime value
		time::DateTime maxDt = time::DateTime::maxValue();
		auto maxChrono{ maxDt.toChrono() };
		time::DateTime maxRoundTrip{ maxChrono };
		EXPECT_NE( maxDt.ticks(), maxRoundTrip.ticks() );
		EXPECT_TRUE( maxRoundTrip.isValid() );

		// Test Unix epoch specifically - this should round-trip perfectly
		auto epoch{ time::DateTime::epoch() };
		auto epochChrono{ epoch.toChrono() };
		time::DateTime epochRoundTrip{ epochChrono };
		EXPECT_EQ( epoch.ticks(), epochRoundTrip.ticks() );

		// Test a safe modern date that should round-trip
		auto modernDate{ time::DateTime( 2024, 1, 1, 12, 0, 0 ) };
		auto modernChrono{ modernDate.toChrono() };
		time::DateTime modernRoundTrip{ modernChrono };
		EXPECT_EQ( modernDate.ticks(), modernRoundTrip.ticks() );
	}

	TEST( DateTimeChrono, ChronoArithmetic )
	{
		// Test compatibility with chrono arithmetic
		time::DateTime dt{ 2024, 6, 15, 12, 0, 0 };
		auto chronoPoint{ dt.toChrono() };

		// Add time using chrono
		auto chronoPlus1Hour{ chronoPoint + std::chrono::hours( 1 ) };
		time::DateTime dtPlus1Hour{ chronoPlus1Hour };

		EXPECT_EQ( 13, dtPlus1Hour.hour() );
		EXPECT_EQ( dt.year(), dtPlus1Hour.year() );
		EXPECT_EQ( dt.month(), dtPlus1Hour.month() );
		EXPECT_EQ( dt.day(), dtPlus1Hour.day() );

		// Add fractional time
		auto chronoPlus30Min{ chronoPoint + std::chrono::minutes( 30 ) };
		time::DateTime dtPlus30Min{ chronoPlus30Min };

		EXPECT_EQ( 12, dtPlus30Min.hour() );
		EXPECT_EQ( 30, dtPlus30Min.minute() );
	}

	TEST( DateTimeChrono, ChronoCompatibilityWithStandardLibrary )
	{
		// Test that our DateTime works with standard chrono utilities
		time::DateTime dt1{ 2024, 1, 1, 12, 0, 0 };
		time::DateTime dt2{ 2024, 1, 1, 13, 0, 0 };

		auto chrono1{ dt1.toChrono() };
		auto chrono2{ dt2.toChrono() };

		// Test chrono comparison
		EXPECT_TRUE( chrono1 < chrono2 );
		EXPECT_FALSE( chrono1 > chrono2 );
		EXPECT_TRUE( chrono1 != chrono2 );

		// Test chrono duration calculation
		auto chronoDiff{ chrono2 - chrono1 };
		auto hoursDiff{ std::chrono::duration_cast<std::chrono::hours>( chronoDiff ).count() };
		EXPECT_EQ( 1, hoursDiff );

		// Test with standard time formatting (C++20 feature when available)
		auto timeT{ std::chrono::system_clock::to_time_t( chrono1 ) };

		// Should be a valid time_t
		EXPECT_GT( timeT, 0 );
	}

	TEST( DateTimeChrono, TimeZoneIndependence )
	{
		// DateTime is UTC-only, chrono conversions should maintain UTC semantics
		time::DateTime utcTime{ 2024, 6, 15, 14, 30, 45 };
		auto chronoPoint{ utcTime.toChrono() };

		// Convert to time_t and back to verify UTC handling
		auto timeT{ std::chrono::system_clock::to_time_t( chronoPoint ) };
		auto chronoFromTimeT{ std::chrono::system_clock::from_time_t( timeT ) };
		time::DateTime convertedBack{ chronoFromTimeT };

		// Should match original( within second precision due to time_t )
		EXPECT_EQ( utcTime.year(), convertedBack.year() );
		EXPECT_EQ( utcTime.month(), convertedBack.month() );
		EXPECT_EQ( utcTime.day(), convertedBack.day() );
		EXPECT_EQ( utcTime.hour(), convertedBack.hour() );
		EXPECT_EQ( utcTime.minute(), convertedBack.minute() );
		EXPECT_EQ( utcTime.second(), convertedBack.second() );
	}
} // namespace nfx::time::test
