/**
 * @file TESTS_StringFunctors.cpp
 * @brief Unit tests for StringViewHash and StringViewEqual functors
 * @details Comprehensive test suite validating heterogeneous lookup functors
 *          for zero-copy string operations in unordered containers
 */

#include <gtest/gtest.h>

#include <nfx/containers/functors/StringFunctors.h>

namespace nfx::containers::test
{
	//=====================================================================
	// StringFunctors tests
	//=====================================================================

	//----------------------------------------------
	// StringViewHash
	//----------------------------------------------

	TEST( StringViewHash, ConsistentHashing )
	{
		StringViewHash hasher;

		std::string str{ "test_string" };
		std::string_view sv{ str };
		const char* cstr{ str.c_str() };

		// All forms should produce same hash
		size_t hashString{ hasher( str ) };
		size_t hashStringView{ hasher( sv ) };
		size_t hashCstring{ hasher( cstr ) };

		EXPECT_EQ( hashString, hashStringView );
		EXPECT_EQ( hashString, hashCstring );
		EXPECT_EQ( hashStringView, hashCstring );
	}

	TEST( StringViewHash, EmptyStringHashing )
	{
		StringViewHash hasher;

		std::string emptyStr{};
		std::string_view emptySv{};
		const char* emptyCstr{ "" };

		size_t hashString{ hasher( emptyStr ) };
		size_t hashStringView{ hasher( emptySv ) };
		size_t hashCstring{ hasher( emptyCstr ) };

		EXPECT_EQ( hashString, hashStringView );
		EXPECT_EQ( hashString, hashCstring );
	}

	TEST( StringViewHash, DifferentContentHashing )
	{
		StringViewHash hasher;

		size_t hash1{ hasher( "string1" ) };
		size_t hash2{ hasher( "string2" ) };
		size_t hash3{ hasher( "completely_different" ) };

		// Different strings should produce different hashes
		EXPECT_NE( hash1, hash2 );
		EXPECT_NE( hash1, hash3 );
		EXPECT_NE( hash2, hash3 );
	}

	//----------------------------------------------
	// StringViewEqual
	//----------------------------------------------

	TEST( StringViewEqual, StringStringComparison )
	{
		StringViewEqual eq;

		std::string str1{ "equal_content" };
		std::string str2{ "equal_content" };
		std::string str3{ "different_content" };

		EXPECT_TRUE( eq( str1, str2 ) );
		EXPECT_FALSE( eq( str1, str3 ) );
		EXPECT_FALSE( eq( str2, str3 ) );
	}

	TEST( StringViewEqual, StringStringViewComparison )
	{
		StringViewEqual eq;

		std::string str{ "test_content" };
		std::string_view svSame{ str };
		std::string_view svDifferent{ "other_content" };

		EXPECT_TRUE( eq( str, svSame ) );
		EXPECT_TRUE( eq( svSame, str ) );
		EXPECT_FALSE( eq( str, svDifferent ) );
		EXPECT_FALSE( eq( svDifferent, str ) );
	}

	TEST( StringViewEqual, StringViewStringViewComparison )
	{
		StringViewEqual eq;

		std::string_view sv1{ "identical" };
		std::string_view sv2{ "identical" };
		std::string_view sv3{ "different" };

		EXPECT_TRUE( eq( sv1, sv2 ) );
		EXPECT_FALSE( eq( sv1, sv3 ) );
		EXPECT_FALSE( eq( sv2, sv3 ) );
	}

	TEST( StringViewEqual, CStringComparisons )
	{
		StringViewEqual eq;

		const char* cstr1{ "c_string_content" };
		const char* cstr2{ "c_string_content" };
		const char* cstr3{ "different_content" };
		std::string str{ "c_string_content" };
		std::string_view sv{ str };

		// C-string to C-string
		EXPECT_TRUE( eq( cstr1, cstr2 ) );
		EXPECT_FALSE( eq( cstr1, cstr3 ) );

		// C-string to string
		EXPECT_TRUE( eq( cstr1, str ) );
		EXPECT_TRUE( eq( str, cstr1 ) );

		// C-string to string_view
		EXPECT_TRUE( eq( cstr1, sv ) );
		EXPECT_TRUE( eq( sv, cstr1 ) );
	}

	TEST( StringViewEqual, EmptyStringComparisons )
	{
		StringViewEqual eq;

		std::string emptyStr{};
		std::string_view emptySv{};
		const char* emptyCstr{ "" };

		EXPECT_TRUE( eq( emptyStr, emptySv ) );
		EXPECT_TRUE( eq( emptyStr, emptyCstr ) );
		EXPECT_TRUE( eq( emptySv, emptyCstr ) );
		EXPECT_TRUE( eq( emptyCstr, emptyStr ) );
		EXPECT_TRUE( eq( emptySv, emptyStr ) );
		EXPECT_TRUE( eq( emptyCstr, emptySv ) );
	}
} // namespace nfx::containers::test
