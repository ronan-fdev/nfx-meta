/**
 * @file Hash.inl
 * @brief Implementation of core hash algorithms and infrastructure
 * @details Contains optimized hash functions with SSE4.2/FNV-1a for strings,
 *          multiplicative hashing for integers, and CPU feature detection
 */

#include <array>

#if defined( __GNUC__ )
#	include <cpuid.h>
#endif

namespace nfx::core::hashing
{
	//=====================================================================
	// Hash infrastructure
	//=====================================================================

	//----------------------------
	// CPU feature detection
	//----------------------------

	NFX_CORE_INLINE bool hasSSE42Support() noexcept
	{
		static thread_local const bool s_hasSSE42 = []() {
			bool hasSupport = false;
#if defined( _MSC_VER )
			std::array<int, 4> cpuInfo{};
			__cpuid( cpuInfo.data(), 1 );
			hasSupport = ( cpuInfo[2] & ( 1 << 20 ) ) != 0;
#elif defined( __GNUC__ )
			unsigned int eax, ebx, ecx, edx;
			if ( __get_cpuid( 1, &eax, &ebx, &ecx, &edx ) )
			{
				hasSupport = ( ecx & ( 1 << 20 ) ) != 0;
			}
#endif
			return hasSupport;
		}();

		return s_hasSSE42;
	}

	//----------------------------------------------
	// Low-level hash building blocks
	//----------------------------------------------

	[[maybe_unused]] NFX_CORE_INLINE constexpr uint32_t Larson( uint32_t hash, uint8_t ch ) noexcept
	{
		return 37 * hash + ch;
	}

	template <uint32_t FnvPrime>
	NFX_CORE_INLINE constexpr uint32_t fnv1a( uint32_t hash, uint8_t ch ) noexcept
	{
		return ( ch ^ hash ) * FnvPrime;
	}

	NFX_CORE_INLINE uint32_t crc32( uint32_t hash, uint8_t ch ) noexcept
	{
#if defined( _MSC_VER )
		return _mm_crc32_u8( hash, ch );
#elif defined( __SSE4_2__ )
		return __builtin_ia32_crc32qi( hash, ch );
#endif
	}

	NFX_CORE_INLINE constexpr uint32_t seedMix( uint32_t seed, uint32_t hash, size_t size ) noexcept
	{
		// Mixes the primary hash with the seed to find the final table slot
		uint32_t x{ seed + hash }; // Mix seed with original hash
		x ^= x >> 12;			   // Avalanche mixing
		x ^= x << 25;			   // More mixing
		x ^= x >> 27;			   // Final mixing

		/*
		 * Final step: Multiplicative hashing with 64-bit magic constant followed by modulo reduction.
		 *
		 * - Multiply by 0x2545F4914F6CDD1D: A carefully chosen 64-bit odd constant that ensures
		 *   good bit distribution and avalanche properties. The multiplication spreads the mixed
		 *   bits of 'x' across the full 64-bit range, maximizing entropy.
		 *
		 * - Bitwise AND with (size - 1): Fast modulo operation that works because 'size' is
		 *   guaranteed to be a power of 2. This maps the large hash value to a valid table
		 *   index in range [0, size-1]. Equivalent to (x * constant) % size but much faster.
		 *
		 * - Cast to uint32_t: Return type matches the expected table index size.
		 */
		return static_cast<uint32_t>( ( x * DEFAULT_HASH_MIX_64 ) & ( size - 1 ) );
	}

	//----------------------------------------------
	// High-level complete hash operations
	//----------------------------------------------

	//----------------------------
	// String hashing
	//----------------------------

	template <uint32_t FnvOffsetBasis, uint32_t FnvPrime>
	NFX_CORE_INLINE uint32_t hashStringView( std::string_view key ) noexcept
	{
		if ( key.empty() )
		{
			return FnvOffsetBasis;
		}

		uint32_t hashValue = FnvOffsetBasis;

		// Check for SSE4.2 support
		static const bool hasSSE42 = hasSSE42Support();

		if ( hasSSE42 )
		{
			// Use SSE4.2 CRC32 for hardware acceleration
			for ( size_t i = 0; i < key.length(); ++i )
			{
#if defined( _MSC_VER )
				hashValue = _mm_crc32_u8( hashValue, static_cast<uint8_t>( key[i] ) );
#elif defined( __SSE4_2__ )
				hashValue = __builtin_ia32_crc32qi( hashValue, static_cast<uint8_t>( key[i] ) );
#endif
			}
		}
		else
		{
			// FNV-1a software fallback
			for ( size_t i = 0; i < key.length(); ++i )
			{
				hashValue ^= static_cast<uint8_t>( key[i] );
				hashValue *= FnvPrime;
			}
		}

		return hashValue;
	}

	//----------------------------
	// Integer hashing
	//----------------------------

	template <typename T>
	NFX_CORE_INLINE constexpr std::enable_if_t<std::is_integral_v<T>, size_t> hashInteger( T value ) noexcept
	{
		/*
		 * Integer hashing using avalanche mixing for uniform bit distribution.
		 *
		 * Purpose: Transform integer input into well-distributed hash values where small
		 * changes in input produce large, unpredictable changes in output (avalanche effect).
		 *
		 * 32-bit Algorithm (Knuth-style):
		 * - Mix high/low bits with XOR and right shift to spread bit dependencies
		 * - Multiply by carefully chosen odd constant for mathematical guarantees
		 * - Repeat mixing steps to ensure full avalanche across all bit positions
		 *
		 * 64-bit Algorithm (Wang's method):
		 * - Use different shift amounts (30, 27, 31) to mix bit regions
		 * - Apply two different multiplicative constants for maximum entropy
		 * - Each step ensures bits from different input regions influence output
		 *
		 * Both provide O(1) performance with excellent statistical properties.
		 */
		if constexpr ( sizeof( T ) <= 4 )
		{
			/* 32-bit integer hashing */
			uint32_t x = static_cast<uint32_t>( value );
			x = ( ( x >> 16 ) ^ x ) * DEFAULT_INTEGER_HASH_32;
			x = ( ( x >> 16 ) ^ x ) * DEFAULT_INTEGER_HASH_32;
			x = ( x >> 16 ) ^ x;
			return static_cast<size_t>( x );
		}
		else
		{
			/* 64-bit integer hashing */
			uint64_t x = static_cast<uint64_t>( value );
			x = ( x ^ ( x >> 30 ) ) * DEFAULT_INTEGER_HASH_64_C1;
			x = ( x ^ ( x >> 27 ) ) * DEFAULT_INTEGER_HASH_64_C2;
			x = x ^ ( x >> 31 );
			return static_cast<size_t>( x );
		}
	}
}
