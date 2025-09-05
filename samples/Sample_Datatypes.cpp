/**
 * @file Sample_Datatypes.cpp
 * @brief Demonstrates comprehensive usage of NFX C++ Core mathematical datatypes
 * @details This sample shows how to use Int128 and Decimal for high-precision
 *          mathematical operations, exact arithmetic, and business calculations
 */

#include <iomanip>
#include <iostream>
#include <string>

#include <nfx/datatypes/Int128.h>
#include <nfx/datatypes/Decimal.h>

int main()
{
	std::cout << "=== NFX C++ Core - Mathematical Datatypes Usage ===" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Int128 - High-precision 128-bit integer arithmetic
	//=========================================================================

	std::cout << "--- Int128 Examples ---" << std::endl;

	// Construction from various types
	nfx::datatypes::Int128 small_int{ 12345 };
	nfx::datatypes::Int128 large_int{ static_cast<std::int64_t>(9223372036854775807LL) }; // max int64

	// Parse very large numbers from strings
	nfx::datatypes::Int128 huge_number = nfx::datatypes::Int128::parse( "123456789012345678901234567890" );

	std::cout << "Small integer: " << small_int << std::endl;
	std::cout << "Large integer (max int64): " << large_int << std::endl;
	std::cout << "Huge number from string: " << huge_number << std::endl;

	// Arithmetic operations
	auto sum = small_int + large_int;
	auto product = huge_number * nfx::datatypes::Int128{ 2 };
	auto difference = huge_number - large_int;

	std::cout << "Sum of small + large: " << sum << std::endl;
	std::cout << "Huge number × 2: " << product << std::endl;
	std::cout << "Huge - large: " << difference << std::endl;

	// Comparisons
	if ( huge_number > large_int )
	{
		std::cout << "Huge number is greater than large integer" << std::endl;
	}

	// State checking
	std::cout << "Is huge number negative? " << ( huge_number.isNegative() ? "Yes" : "No" ) << std::endl;
	std::cout << "Is huge number zero? " << ( huge_number.isZero() ? "Yes" : "No" ) << std::endl;

	// Division and modulo operations
	auto quotient = huge_number / nfx::datatypes::Int128{ 7 };
	auto remainder = huge_number % nfx::datatypes::Int128{ 7 };
	std::cout << "Huge number ÷ 7: " << quotient << std::endl;
	std::cout << "Huge number % 7: " << remainder << std::endl;

	// Sign operations and mathematical functions
	nfx::datatypes::Int128 negative_val{ -98765 };
	auto absolute_val = negative_val.isNegative() ? -negative_val : negative_val;
	std::cout << "Negative value: " << negative_val << std::endl;
	std::cout << "Absolute value: " << absolute_val << std::endl;

	// String conversion examples
	std::cout << "Huge number as string: " << huge_number.toString() << std::endl;
	std::cout << "Small int as string: " << small_int.toString() << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Decimal - High-precision decimal arithmetic (28-29 digits)
	//=========================================================================

	std::cout << "--- Decimal Examples ---" << std::endl;

	// Construction from various types
	nfx::datatypes::Decimal price_from_string{ "999.99" };
	nfx::datatypes::Decimal quantity_from_int{ 1000000 };
	nfx::datatypes::Decimal rate_from_double{ 0.0825 }; // 8.25% tax rate

	std::cout << std::fixed << std::setprecision( 6 );
	std::cout << "Price: " << price_from_string << std::endl;
	std::cout << "Quantity: " << quantity_from_int << std::endl;
	std::cout << "Tax rate: " << rate_from_double << std::endl;

	// Exact decimal arithmetic (no floating-point errors)
	auto subtotal = price_from_string * quantity_from_int;
	auto tax = subtotal * rate_from_double;
	auto total = subtotal + tax;

	std::cout << "Subtotal (price × quantity): " << subtotal << std::endl;
	std::cout << "Tax (subtotal × rate): " << tax << std::endl;
	std::cout << "Total (subtotal + tax): " << total << std::endl;

	// High-precision calculations
	nfx::datatypes::Decimal precise_pi{ "3.1415926535897932384626433832795" };
	nfx::datatypes::Decimal radius{ "5.5" };
	auto area = precise_pi * radius * radius;

	std::cout << "π (high precision): " << precise_pi << std::endl;
	std::cout << "Circle area (π × r²) with r=5.5: " << area << std::endl;

	// Rounding operations
	nfx::datatypes::Decimal unrounded{ "123.456789" };
	auto rounded_2dp = nfx::datatypes::Decimal::round( unrounded, 2 );
	auto truncated = nfx::datatypes::Decimal::truncate( unrounded );
	auto ceiling_val = nfx::datatypes::Decimal::ceiling( unrounded );
	auto floor_val = nfx::datatypes::Decimal::floor( unrounded );

	std::cout << "Original: " << unrounded << std::endl;
	std::cout << "Rounded to 2 dp: " << rounded_2dp << std::endl;
	std::cout << "Truncated: " << truncated << std::endl;
	std::cout << "Ceiling: " << ceiling_val << std::endl;
	std::cout << "Floor: " << floor_val << std::endl;

	// State checking
	std::cout << "Is price zero? " << ( price_from_string.isZero() ? "Yes" : "No" ) << std::endl;
	std::cout << "Is tax negative? " << ( tax.isNegative() ? "Yes" : "No" ) << std::endl;
	std::cout << "Price scale (decimal places): " << static_cast<int>( price_from_string.scale() ) << std::endl;

	std::cout << std::endl;

	//=========================================================================
	// Advanced mathematical operations
	//=========================================================================

	std::cout << "--- Advanced Mathematical Examples ---" << std::endl;

	// Compound interest calculation using Decimal for precision
	nfx::datatypes::Decimal principal{ "10000.00" };  // $10,000 initial
	nfx::datatypes::Decimal annual_rate{ "0.05" };	  // 5% annual interest
	nfx::datatypes::Decimal periods_per_year{ "12" }; // Monthly compounding
	nfx::datatypes::Decimal years{ "10" };			  // 10 years

	// A = P(1 + r/n)^(nt) - compound interest formula
	auto rate_per_period = annual_rate / periods_per_year;
	auto total_periods = periods_per_year * years;

	// Calculate (1 + r/n)^(nt) step by step for demonstration
	auto one_plus_rate = nfx::datatypes::Decimal{ "1.0" } + rate_per_period;
	auto annual_rate_percentage = annual_rate * nfx::datatypes::Decimal{ "100" };

	std::cout << "Principal: $" << principal << std::endl;
	std::cout << "Annual rate: " << annual_rate << " (" << annual_rate_percentage << "%)" << std::endl;
	std::cout << "Rate per period: " << rate_per_period << std::endl;
	std::cout << "Total periods: " << total_periods << std::endl;
	std::cout << "Base (1 + r/n): " << one_plus_rate << std::endl;

	// High-precision factorial calculation with Int128
	nfx::datatypes::Int128 factorial_input{ 20 };
	nfx::datatypes::Int128 factorial_result{ 1 };
	nfx::datatypes::Int128 counter{ 1 };

	while ( counter <= factorial_input )
	{
		factorial_result = factorial_result * counter;
		counter = counter + nfx::datatypes::Int128{ 1 };
	}
	std::cout << "20! (factorial) = " << factorial_result << std::endl;

	// Large number arithmetic - Fibonacci sequence with Int128
	nfx::datatypes::Int128 fib_a{ 0 };
	nfx::datatypes::Int128 fib_b{ 1 };
	std::cout << "Fibonacci sequence (first 15 terms with Int128):" << std::endl;
	std::cout << "F(0) = " << fib_a << std::endl;
	std::cout << "F(1) = " << fib_b << std::endl;

	for ( int i = 2; i < 15; ++i )
	{
		auto fib_next = fib_a + fib_b;
		std::cout << "F(" << i << ") = " << fib_next << std::endl;
		fib_a = fib_b;
		fib_b = fib_next;
	}

	// Financial precision demonstration
	std::cout << std::endl;
	std::cout << "--- Financial Precision Comparison ---" << std::endl;

	// Show why Decimal is better than double for financial calculations
	double double_price = 0.1;
	double double_quantity = 3.0;
	double double_result = double_price * double_quantity;

	nfx::datatypes::Decimal decimal_price{ "0.1" };
	nfx::datatypes::Decimal decimal_quantity{ "3" };
	auto decimal_result = decimal_price * decimal_quantity;

	std::cout << std::fixed << std::setprecision( 17 );
	std::cout << "Double precision: 0.1 × 3 = " << double_result << std::endl;
	std::cout << std::setprecision( 6 );
	std::cout << "Decimal precision: 0.1 × 3 = " << decimal_result << std::endl;

	bool is_exact = ( decimal_result == nfx::datatypes::Decimal{ "0.3" } );
	std::cout << "Decimal is exact: " << ( is_exact ? "Yes" : "No" ) << std::endl;

	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "=== NFX Mathematical Datatypes sample completed successfully ===" << std::endl;
	std::cout << "This demonstrates high-precision arithmetic suitable for" << std::endl;
	std::cout << "financial calculations, scientific computing, and large-scale mathematical operations!" << std::endl;

	return 0;
}
