/**
 * @file Sample_DateTime.cpp
 * @brief Demonstrates comprehensive usage of NFX C++ Core time utilities
 * @details This sample shows how to use DateTime, TimeSpan, and DateTimeOffset
 *          for high-precision temporal operations, ISO 8601 parsing, and timezone handling
 */

#include <iomanip>
#include <iostream>
#include <string>

#include <nfx/time/DateTime.h>

int main()
{
	std::cout << "=== NFX C++ Core - Time Utilities Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// TimeSpan - High-precision time intervals
	//=========================================================================

	std::cout << "--- TimeSpan Examples ---" << std::endl;

	// Factory methods for different time units
	auto one_day = nfx::time::TimeSpan::fromDays( 1.0 );
	auto six_hours = nfx::time::TimeSpan::fromHours( 6.5 );
	auto thirty_minutes = nfx::time::TimeSpan::fromMinutes( 30.0 );
	auto ten_seconds = nfx::time::TimeSpan::fromSeconds( 10.5 );
	auto hundred_ms = nfx::time::TimeSpan::fromMilliseconds( 100.0 );

	std::cout << "One day: " << one_day.totalHours() << " hours" << std::endl;
	std::cout << "Six and half hours: " << six_hours.totalMinutes() << " minutes" << std::endl;
	std::cout << "Thirty minutes: " << thirty_minutes.totalSeconds() << " seconds" << std::endl;
	std::cout << "Ten and half seconds: " << ten_seconds.totalMilliseconds() << " milliseconds" << std::endl;
	std::cout << "Hundred milliseconds: " << hundred_ms.ticks() << " ticks (100ns)" << std::endl;

	// TimeSpan arithmetic
	auto total_time = one_day + six_hours + thirty_minutes;
	std::cout << "Total time (1 day + 6.5 hours + 30 min): " << total_time.totalHours() << " hours" << std::endl;

	// TimeSpan parsing
	try
	{
		auto parsed_duration = nfx::time::TimeSpan::parse( "PT1H30M45S" ); // ISO 8601 duration
		std::cout << "Parsed ISO 8601 duration 'PT1H30M45S': " << parsed_duration.totalSeconds() << " seconds" << std::endl;

		auto simple_duration = nfx::time::TimeSpan::parse( "01:30:45" ); // H:M:S format
		std::cout << "Parsed H:M:S format '01:30:45': " << simple_duration.totalMinutes() << " minutes" << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cout << "TimeSpan parsing error: " << e.what() << std::endl;
	}

	std::cout << std::endl;

	//=========================================================================
	// DateTime - UTC date and time operations
	//=========================================================================

	std::cout << "--- DateTime Examples ---" << std::endl;

	// Current time
	auto now = nfx::time::DateTime::now();
	auto today = nfx::time::DateTime::today();

	std::cout << "Current time (UTC): " << now.toString() << std::endl;
	std::cout << "Today (date only): " << today.toString() << std::endl;

	// Different formatting options
	std::cout << "ISO 8601 Basic: " << now.toString( nfx::time::DateTime::Format::Iso8601Basic ) << std::endl;
	std::cout << "ISO 8601 Extended: " << now.toString( nfx::time::DateTime::Format::Iso8601Extended ) << std::endl;
	std::cout << "Date Only: " << now.toString( nfx::time::DateTime::Format::DateOnly ) << std::endl;
	std::cout << "Time Only: " << now.toString( nfx::time::DateTime::Format::TimeOnly ) << std::endl;

	// Component access
	std::cout << "Year: " << now.year() << ", Month: " << now.month() << ", Day: " << now.day() << std::endl;
	std::cout << "Hour: " << now.hour() << ", Minute: " << now.minute() << ", Second: " << now.second() << std::endl;
	std::cout << "Day of week: " << now.dayOfWeek() << ", Day of year: " << now.dayOfYear() << std::endl;

	// DateTime construction
	auto christmas_2025 = nfx::time::DateTime{ 2025, 12, 25, 0, 0, 0 };
	auto new_year_2026 = nfx::time::DateTime{ 2026, 1, 1, 0, 0, 0, 0 };

	std::cout << "Christmas 2025: " << christmas_2025.toString() << std::endl;
	std::cout << "New Year 2026: " << new_year_2026.toString() << std::endl;

	// Time arithmetic using TimeSpan
	auto tomorrow = now + nfx::time::TimeSpan::fromDays( 1 );
	auto next_week = now + nfx::time::TimeSpan::fromDays( 7 );
	auto last_hour = now - nfx::time::TimeSpan::fromHours( 1 );

	std::cout << "Tomorrow: " << tomorrow.toString() << std::endl;
	std::cout << "Next week: " << next_week.toString() << std::endl;
	std::cout << "Last hour: " << last_hour.toString() << std::endl;

	// Time differences
	auto time_until_christmas = christmas_2025 - now;
	if ( time_until_christmas.ticks() > 0 )
	{
		std::cout << "Days until Christmas 2025: " << time_until_christmas.totalDays() << std::endl;
	}
	else
	{
		std::cout << "Christmas 2025 has passed!" << std::endl;
	}

	// Unix timestamp conversion
	std::cout << "Current Unix seconds: " << now.toUnixSeconds() << std::endl;
	std::cout << "Current Unix milliseconds: " << now.toUnixMilliseconds() << std::endl;

	// DateTime from Unix timestamps
	auto epoch_time = nfx::time::DateTime::sinceEpochSeconds( 1640995200 ); // 2022-01-01 00:00:00
	std::cout << "From Unix timestamp: " << epoch_time.toString() << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// DateTime Parsing - ISO 8601 support
	//=========================================================================

	std::cout << "--- DateTime Parsing Examples ---" << std::endl;

	const char* iso_strings[] = {
		"2025-12-25T00:00:00Z",
		"2025-06-15T14:30:45Z",
		"2025-01-01T12:00:00.123Z" };

	for ( const auto& iso_str : iso_strings )
	{
		try
		{
			auto parsed_date = nfx::time::DateTime::parse( iso_str );
			std::cout << "Parsed '" << iso_str << "' → " << parsed_date.toString() << std::endl;
		}
		catch ( const std::exception& e )
		{
			std::cout << "Failed to parse '" << iso_str << "': " << e.what() << std::endl;
		}
	}

	// Try parse (no exceptions)
	nfx::time::DateTime result;
	if ( nfx::time::DateTime::tryParse( "2025-08-31T15:30:00Z", result ) )
	{
		std::cout << "Successfully parsed with tryParse: " << result.toString() << std::endl;
	}
	else
	{
		std::cout << "tryParse failed" << std::endl;
	}

	std::cout << std::endl;

	//=========================================================================
	// DateTimeOffset - Timezone-aware operations
	//=========================================================================

	std::cout << "--- DateTimeOffset Examples ---" << std::endl;

	// Current time with system timezone
	auto now_local = nfx::time::DateTimeOffset::now();
	auto now_utc = nfx::time::DateTimeOffset::utcNow();

	std::cout << "Current local time: " << now_local.toString() << std::endl;
	std::cout << "Current UTC time: " << now_utc.toString() << std::endl;
	std::cout << "Local timezone offset: " << now_local.totalOffsetMinutes() << " minutes" << std::endl;

	// DateTimeOffset with specific timezone
	auto utc_offset = nfx::time::TimeSpan::fromHours( 0 );	// UTC
	auto est_offset = nfx::time::TimeSpan::fromHours( -5 ); // EST
	auto pst_offset = nfx::time::TimeSpan::fromHours( -8 ); // PST

	auto meeting_utc = nfx::time::DateTimeOffset{ 2025, 9, 1, 14, 0, 0, utc_offset };
	auto meeting_est = meeting_utc.toOffset( est_offset );
	auto meeting_pst = meeting_utc.toOffset( pst_offset );

	std::cout << "Global meeting times:" << std::endl;
	std::cout << "  UTC: " << meeting_utc.toString() << std::endl;
	std::cout << "  EST: " << meeting_est.toString() << std::endl;
	std::cout << "  PST: " << meeting_pst.toString() << std::endl;

	// Convert between local and UTC
	auto local_meeting = nfx::time::DateTimeOffset{ 2025, 9, 1, 10, 0, 0, est_offset };
	auto utc_meeting = local_meeting.toUniversalTime();

	std::cout << "Meeting scheduled for 10:00 EST: " << local_meeting.toString() << std::endl;
	std::cout << "Same meeting in UTC: " << utc_meeting.toString() << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// std::chrono interoperability
	//=========================================================================

	std::cout << "--- std::chrono Interoperability ---" << std::endl;

	// Convert DateTime to std::chrono
	auto chrono_timepoint = now.toChrono();
	auto time_t_value = std::chrono::system_clock::to_time_t( chrono_timepoint );

	std::cout << "DateTime as std::chrono time_t: " << time_t_value << std::endl;

	// Convert from std::chrono back to DateTime
	auto current_chrono = std::chrono::system_clock::now();
	auto datetime_from_chrono = nfx::time::DateTime::fromChrono( current_chrono );

	std::cout << "std::chrono back to DateTime: " << datetime_from_chrono.toString() << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Advanced dateTime operations
	//=========================================================================

	std::cout << "--- Advanced Operations ---" << std::endl;

	// Date validation
	std::cout << "Is 2024 a leap year? " << ( nfx::time::DateTime::isLeapYear( 2024 ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Is 2025 a leap year? " << ( nfx::time::DateTime::isLeapYear( 2025 ) ? "Yes" : "No" ) << std::endl;
	std::cout << "Days in February 2024: " << nfx::time::DateTime::daysInMonth( 2024, 2 ) << std::endl;
	std::cout << "Days in February 2025: " << nfx::time::DateTime::daysInMonth( 2025, 2 ) << std::endl;

	// DateTime bounds
	auto min_datetime = nfx::time::DateTime::minValue();
	auto max_datetime = nfx::time::DateTime::maxValue();
	auto epoch = nfx::time::DateTime::epoch();

	std::cout << "DateTime range:" << std::endl;
	std::cout << "  Minimum: " << min_datetime.toString() << std::endl;
	std::cout << "  Maximum: " << max_datetime.toString() << std::endl;
	std::cout << "  Unix Epoch: " << epoch.toString() << std::endl;

	// Time of day operations
	auto date_part = now.date();
	auto time_part = now.timeOfDay();

	std::cout << "Current date part: " << date_part.toString() << std::endl;
	std::cout << "Current time part: " << time_part.totalHours() << " hours" << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Combined example - Event qcheduling across timezones
	//=========================================================================

	std::cout << "--- Real-World Example: Global Event Scheduling ---" << std::endl;

	// Schedule a webinar for September 15, 2025 at 2:00 PM EST
	auto webinar_est = nfx::time::DateTimeOffset{
		2025, 9, 15, 14, 0, 0,
		nfx::time::TimeSpan::fromHours( -5 ) }; // EST

	// Convert to different timezones for global participants
	auto webinar_utc = webinar_est.toUniversalTime();
	auto webinar_london = webinar_est.toOffset( nfx::time::TimeSpan::fromHours( 1 ) );	// BST
	auto webinar_tokyo = webinar_est.toOffset( nfx::time::TimeSpan::fromHours( 9 ) );	// JST
	auto webinar_sydney = webinar_est.toOffset( nfx::time::TimeSpan::fromHours( 10 ) ); // AEST

	std::cout << "Global Webinar Schedule - 'NFX C++ Core Launch':" << std::endl;
	std::cout << "  New York (EST): " << webinar_est.toString() << std::endl;
	std::cout << "  London (BST):   " << webinar_london.toString() << std::endl;
	std::cout << "  Tokyo (JST):    " << webinar_tokyo.toString() << std::endl;
	std::cout << "  Sydney (AEST):  " << webinar_sydney.toString() << std::endl;
	std::cout << "  UTC:            " << webinar_utc.toString() << std::endl;

	// Calculate time until webinar
	auto time_until_webinar = webinar_utc.utcDateTime() - now;
	if ( time_until_webinar.ticks() > 0 )
	{
		std::cout << "Time until webinar: " << time_until_webinar.totalDays() << " days" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "=== NFX Time Utilities sample completed successfully ===" << std::endl;
	std::cout << "This demonstrates the power of high-precision, timezone-aware temporal operations!" << std::endl;

	return 0;
}
