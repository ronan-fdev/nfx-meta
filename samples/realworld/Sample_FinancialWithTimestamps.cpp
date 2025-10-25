/**
 * @file Sample_FinancialWithTimestamps.cpp
 * @brief Demonstrates advanced financial trading and portfolio management using NFX C++ Core
 * @details This sample showcases the integration of high-precision mathematical datatypes
			with temporal operations for real-world financial applications including:
 *              - Portfolio valuation with precise timestamps
 *              - Trade execution and audit trails
 *              - Interest calculations over time periods
 *              - Performance measurement and risk analysis
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nfx/datatypes/Int128.h>
#include <nfx/datatypes/Decimal.h>
#include <nfx/datetime/DateTime.h>

// Trade structure for financial operations
struct Trade
{
	std::string symbol;
	nfx::datatypes::Decimal quantity;
	nfx::datatypes::Decimal price;
	nfx::datetime::DateTime timestamp;
	std::string side; // "BUY" or "SELL"

	nfx::datatypes::Decimal value() const
	{
		return quantity * price;
	}
};

// Portfolio position tracking
struct Position
{
	std::string symbol;
	nfx::datatypes::Decimal quantity;
	nfx::datatypes::Decimal avg_cost;
	nfx::datetime::DateTime first_trade;
	nfx::datetime::DateTime last_update;

	nfx::datatypes::Decimal market_value( const nfx::datatypes::Decimal& current_price ) const
	{
		return quantity * current_price;
	}

	nfx::datatypes::Decimal unrealized_pnl( const nfx::datatypes::Decimal& current_price ) const
	{
		// Calculate actual P&L: (current_price - avg_cost) * quantity
		auto cost_basis = avg_cost * quantity;
		auto market_value = current_price * quantity;
		// P&L = market_value - cost_basis
		if ( market_value >= cost_basis )
		{
			return market_value - cost_basis;
		}
		else
		{
			// Handle negative P&L (loss)
			return nfx::datatypes::Decimal{ "0" } - ( cost_basis - market_value );
		}
	}
};

int main()
{
	std::cout << "=== NFX C++ Core - Financial Trading & Portfolio Management ===" << std::endl;
	std::cout << std::endl;

	auto trading_session_start = nfx::datetime::DateTime::now();
	std::cout << "Trading session started at: " << trading_session_start.toString() << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Trade execution with precise timestamps
	//=========================================================================

	std::cout << "--- Trade Execution Engine ---" << std::endl;

	std::vector<Trade> trades;

	trades.push_back( { "AAPL",
		nfx::datatypes::Decimal{ "1000" },
		nfx::datatypes::Decimal{ "175.50" },
		nfx::datetime::DateTime::now(),
		"BUY" } );

	// Simulate some processing time
	auto trade_2_time = nfx::datetime::DateTime::now() + nfx::datetime::TimeSpan::fromMilliseconds( 1.5 );
	trades.push_back( { "MSFT",
		nfx::datatypes::Decimal{ "500" },
		nfx::datatypes::Decimal{ "328.75" },
		trade_2_time,
		"BUY" } );

	auto trade_3_time = nfx::datetime::DateTime::now() + nfx::datetime::TimeSpan::fromMilliseconds( 3.2 );
	trades.push_back( { "GOOGL",
		nfx::datatypes::Decimal{ "200" },
		nfx::datatypes::Decimal{ "2650.25" },
		trade_3_time,
		"BUY" } );

	auto trade_4_time = nfx::datetime::DateTime::now() + nfx::datetime::TimeSpan::fromMilliseconds( 4.8 );
	trades.push_back( { "AAPL",
		nfx::datatypes::Decimal{ "500" },
		nfx::datatypes::Decimal{ "176.25" },
		trade_4_time,
		"BUY" } );

	// Display trade blotter with precise timestamps
	nfx::datatypes::Decimal total_traded_value{ "0" };
	std::cout << std::fixed << std::setprecision( 2 );
	std::cout << "Trade Blotter:" << std::endl;
	std::cout << "┌─────────┬──────────┬───────────┬─────────────────┬──────────────────────┬───────────────┐" << std::endl;
	std::cout << "│ Symbol  │ Side     │ Quantity  │ Price           │ Timestamp            │ Trade Value   │" << std::endl;
	std::cout << "├─────────┼──────────┼───────────┼─────────────────┼──────────────────────┼───────────────┤" << std::endl;

	for ( const auto& trade : trades )
	{
		total_traded_value = total_traded_value + trade.value();
		std::cout << "│ " << std::setw( 7 ) << std::left << trade.symbol
				  << " │ " << std::setw( 8 ) << trade.side
				  << " │ " << std::setw( 9 ) << trade.quantity
				  << " │ $" << std::setw( 14 ) << trade.price
				  << " │ " << trade.timestamp.toString()
				  << " │ $" << std::setw( 12 ) << trade.value() << " │" << std::endl;
	}
	std::cout << "└─────────┴──────────┴───────────┴─────────────────┴──────────────────────┴───────────────┘" << std::endl;
	std::cout << "Total Traded Value: $" << total_traded_value << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Portfolio position management
	//=========================================================================

	std::cout << "--- Portfolio Position Consolidation ---" << std::endl;

	std::vector<Position> positions;

	// Consolidate trades into positions
	for ( const auto& trade : trades )
	{
		auto pos_it = std::find_if( positions.begin(), positions.end(),
			[&trade]( const Position& pos ) { return pos.symbol == trade.symbol; } );

		if ( pos_it == positions.end() )
		{
			// New position
			positions.push_back( { trade.symbol,
				trade.quantity,
				trade.price,
				trade.timestamp,
				trade.timestamp } );
		}
		else
		{
			// Update existing position (weighted average cost)
			auto current_value = pos_it->quantity * pos_it->avg_cost;
			auto trade_value = trade.quantity * trade.price;
			auto new_quantity = pos_it->quantity + trade.quantity;

			pos_it->avg_cost = ( current_value + trade_value ) / new_quantity;
			pos_it->quantity = new_quantity;
			pos_it->last_update = trade.timestamp;
		}
	}

	// Display consolidated positions
	std::cout << "Portfolio Positions:" << std::endl;
	std::cout << "┌─────────┬───────────┬─────────────────┬──────────────────────┬───────────────┐" << std::endl;
	std::cout << "│ Symbol  │ Quantity  │ Avg Cost        │ Last Update          │ Book Value    │" << std::endl;
	std::cout << "├─────────┼───────────┼─────────────────┼──────────────────────┼───────────────┤" << std::endl;

	nfx::datatypes::Decimal total_book_value{ "0" };
	for ( const auto& pos : positions )
	{
		auto book_value = pos.quantity * pos.avg_cost;
		total_book_value = total_book_value + book_value;

		std::cout << "│ " << std::setw( 7 ) << std::left << pos.symbol
				  << " │ " << std::setw( 9 ) << pos.quantity
				  << " │ $" << std::setw( 14 ) << pos.avg_cost
				  << " │ " << pos.last_update.toString()
				  << " │ $" << std::setw( 12 ) << book_value << " │" << std::endl;
	}
	std::cout << "└─────────┴───────────┴─────────────────┴──────────────────────┴───────────────┘" << std::endl;
	std::cout << "Total Book Value: $" << total_book_value << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Mark-to-Market valuation with current prices
	//=========================================================================

	std::cout << "--- Mark-to-Market Portfolio Valuation ---" << std::endl;

	// Simulate current market prices (slightly different from trade prices)
	std::map<std::string, nfx::datatypes::Decimal> current_prices = {
		{ "AAPL", nfx::datatypes::Decimal{ "177.15" } },  // Up $1.65 from average
		{ "MSFT", nfx::datatypes::Decimal{ "330.25" } },  // Up $1.50
		{ "GOOGL", nfx::datatypes::Decimal{ "2645.80" } } // Down $4.45
	};

	auto valuation_time = nfx::datetime::DateTime::now();
	std::cout << "Market valuation as of: " << valuation_time.toString() << std::endl;
	std::cout << std::endl;

	std::cout << "Portfolio Valuation:" << std::endl;
	std::cout << "┌─────────┬───────────┬─────────────────┬─────────────────┬───────────────┬───────────────┐" << std::endl;
	std::cout << "│ Symbol  │ Quantity  │ Avg Cost        │ Current Price   │ Market Value  │ Unrealized P&L│" << std::endl;
	std::cout << "├─────────┼───────────┼─────────────────┼─────────────────┼───────────────┼───────────────┤" << std::endl;

	nfx::datatypes::Decimal total_market_value{ "0" };
	nfx::datatypes::Decimal total_unrealized_pnl{ "0" };

	for ( const auto& pos : positions )
	{
		auto current_price = current_prices[pos.symbol];
		auto market_value = pos.market_value( current_price );
		auto unrealized_pnl = pos.unrealized_pnl( current_price );

		total_market_value = total_market_value + market_value;
		total_unrealized_pnl = total_unrealized_pnl + unrealized_pnl;

		std::cout << "│ " << std::setw( 7 ) << std::left << pos.symbol
				  << " │ " << std::setw( 9 ) << pos.quantity
				  << " │ $" << std::setw( 14 ) << pos.avg_cost
				  << " │ $" << std::setw( 14 ) << current_price
				  << " │ $" << std::setw( 12 ) << market_value
				  << " │ $" << std::setw( 12 ) << unrealized_pnl << " │" << std::endl;
	}
	std::cout << "└─────────┴───────────┴─────────────────┴─────────────────┴───────────────┴───────────────┘" << std::endl;
	std::cout << "Total Market Value: $" << total_market_value << std::endl;
	std::cout << "Total Unrealized P&L: $" << total_unrealized_pnl << std::endl;

	auto portfolio_return_percentage = ( total_unrealized_pnl / total_book_value ) * nfx::datatypes::Decimal{ "100" };
	std::cout << "Portfolio Return: " << portfolio_return_percentage << "%" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Interest calculation over time periods
	//=========================================================================

	std::cout << "--- Interest & Yield Calculations ---" << std::endl;

	// Portfolio cash management - calculate interest on cash positions
	nfx::datatypes::Decimal cash_balance{ "2500000.00" };	  // $2.5M cash
	nfx::datatypes::Decimal annual_interest_rate{ "0.0475" }; // 4.75% annual

	// Calculate interest for different time periods
	auto interest_start_date = nfx::datetime::DateTime{ 2025, 1, 1, 0, 0, 0 };
	auto current_date = nfx::datetime::DateTime::now();
	auto days_elapsed_span = current_date - interest_start_date;
	auto days_elapsed = nfx::datatypes::Decimal{ std::to_string( days_elapsed_span.totalDays() ) };

	auto daily_interest_rate = annual_interest_rate / nfx::datatypes::Decimal{ "365" };
	auto accrued_interest = cash_balance * daily_interest_rate * days_elapsed;

	std::cout << "Cash Position Interest Calculation:" << std::endl;
	std::cout << "Cash Balance: $" << cash_balance << std::endl;

	auto annual_rate_percentage = annual_interest_rate * nfx::datatypes::Decimal{ "100" };
	std::cout << "Annual Interest Rate: " << annual_rate_percentage << "%" << std::endl;
	std::cout << "Days Since Jan 1, 2025: " << days_elapsed << std::endl;
	std::cout << "Accrued Interest: $" << accrued_interest << std::endl;
	std::cout << "Cash + Interest: $" << ( cash_balance + accrued_interest ) << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Performance analytics with timestamps
	//=========================================================================

	std::cout << "--- Performance Analytics ---" << std::endl;

	auto analytics_start = nfx::datetime::DateTime::now();

	// Calculate various performance metrics
	auto total_portfolio_value = total_market_value + cash_balance + accrued_interest;

	// Trading return should be based on unrealized P&L from positions only
	auto trading_return = total_unrealized_pnl;			   // This is the actual trading profit/loss
	auto total_return = trading_return + accrued_interest; // Total return includes interest
	auto trading_return_percentage = ( trading_return / total_book_value ) * nfx::datatypes::Decimal{ "100" };

	// Risk metrics (simplified)
	auto largest_position_value = nfx::datatypes::Decimal{ "0" };
	std::string largest_position_symbol;

	for ( const auto& pos : positions )
	{
		auto position_value = pos.market_value( current_prices[pos.symbol] );
		if ( position_value > largest_position_value )
		{
			largest_position_value = position_value;
			largest_position_symbol = pos.symbol;
		}
	}

	auto concentration_risk = ( largest_position_value / total_portfolio_value ) * nfx::datatypes::Decimal{ "100" };

	auto analytics_end = nfx::datetime::DateTime::now();
	auto analytics_duration = analytics_end - analytics_start;

	// Format numbers for clean financial display (round to reasonable precision)
	auto portfolio_display = nfx::datatypes::Decimal::round( total_portfolio_value, 2 );
	auto book_display = nfx::datatypes::Decimal::round( total_book_value, 2 );
	auto cash_display = nfx::datatypes::Decimal::round( cash_balance, 2 );
	auto interest_display = nfx::datatypes::Decimal::round( accrued_interest, 2 );
	auto return_display = nfx::datatypes::Decimal::round( trading_return, 2 );
	auto total_return_display = nfx::datatypes::Decimal::round( total_return, 2 );
	auto percentage_display = nfx::datatypes::Decimal::round( trading_return_percentage, 3 );
	auto position_display = nfx::datatypes::Decimal::round( largest_position_value, 2 );
	auto risk_display = nfx::datatypes::Decimal::round( concentration_risk, 1 );

	std::cout << "Portfolio Analytics Summary:" << std::endl;
	std::cout << "╔══════════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║                       PORTFOLIO DASHBOARD                        ║" << std::endl;
	std::cout << "╠══════════════════════════════════════════════════════════════════╣" << std::endl;
	std::cout << "║ Total Portfolio Value     │ $" << std::setw( 17 ) << std::right << portfolio_display << "                   ║" << std::endl;
	std::cout << "║ Total Book Value          │ $" << std::setw( 17 ) << std::right << book_display << "                   ║" << std::endl;
	std::cout << "║ Cash Position             │ $" << std::setw( 17 ) << std::right << cash_display << "                   ║" << std::endl;
	std::cout << "║ Accrued Interest          │ $" << std::setw( 17 ) << std::right << interest_display << "                   ║" << std::endl;
	std::cout << "║ Trading P&L               │ $" << std::setw( 17 ) << std::right << return_display << "                   ║" << std::endl;
	std::cout << "║ Trading Return %          │ " << std::setw( 17 ) << std::right << percentage_display << "%                   ║" << std::endl;
	std::cout << "║ Total Return              │ $" << std::setw( 17 ) << std::right << total_return_display << "                   ║" << std::endl;
	std::cout << "║ Largest Position          │ " << std::setw( 5 ) << std::left << largest_position_symbol << " $     " << std::right << position_display << "                   ║" << std::endl;
	std::cout << "║ Concentration Risk        │ " << std::setw( 17 ) << std::right << risk_display << "%                   ║" << std::endl;
	std::cout << "╚══════════════════════════════════════════════════════════════════╝" << std::endl;
	std::cout << std::endl;

	//=========================================================================
	// Trading session summary with timing
	//=========================================================================

	auto trading_session_end = nfx::datetime::DateTime::now();
	auto total_session_time = trading_session_end - trading_session_start;

	std::cout << "--- Trading Session Summary ---" << std::endl;
	std::cout << "Session Start: " << trading_session_start.toString() << std::endl;
	std::cout << "Session End: " << trading_session_end.toString() << std::endl;
	std::cout << "Total Session Duration: " << total_session_time.totalMilliseconds() << " milliseconds" << std::endl;
	std::cout << "Analytics Processing Time: " << analytics_duration.totalMilliseconds() << " milliseconds" << std::endl;
	std::cout << std::endl;

	std::cout << "Operations Completed:" << std::endl;
	std::cout << "    Executed " << trades.size() << " trades with precision timestamps" << std::endl;
	std::cout << "    Consolidated into " << positions.size() << " portfolio positions" << std::endl;
	std::cout << "    Performed mark-to-market valuation" << std::endl;
	std::cout << "    Calculated accrued interest over " << days_elapsed << " days" << std::endl;
	std::cout << "    Generated comprehensive risk and performance analytics" << std::endl;
	std::cout << std::endl;

	std::cout << "=== NFX Financial Trading & Portfolio Management sample completed successfully ===" << std::endl;
	std::cout << "This demonstrates enterprise-grade financial computing with exact precision" << std::endl;
	std::cout << "and nanosecond-accurate timestamps suitable for institutional trading systems!" << std::endl;

	return 0;
}
