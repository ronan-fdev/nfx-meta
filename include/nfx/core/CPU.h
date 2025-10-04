/**
 * @file CPU.h
 * @brief CPU feature detection and capabilities
 * @details Runtime detection of processor features including SSE4.2, AVX2, and other
 *          instruction set extensions for optimized algorithm selection
 */

#pragma once

#include <array>

#if defined( __GNUC__ )
#	include <cpuid.h>
#endif
#if defined( _MSC_VER ) || defined( __SSE4_2__ )
#	include <nmmintrin.h>
#endif

#include "nfx/config.h"

namespace nfx::core::cpu
{
	//=====================================================================
	// CPU feature detection
	//=====================================================================

	//----------------------------
	// SSE4.2 Detection
	//----------------------------

	/**
	 * @brief Gets the cached SSE4.2 support status.
	 * @details Checks CPU capabilities for SSE4.2 CRC32 instructions, which provide
	 *          3-5x faster hashing compared to software fallback. Result is cached
	 *          via static initialization for zero runtime overhead.
	 * @return `true` if SSE4.2 is supported, `false` otherwise.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 * @note Requires CPUID leaf 1, ECX bit 20
	 */
	[[nodiscard]] NFX_CORE_INLINE bool hasSSE42Support() noexcept
	{
		static const bool s_hasSSE42 = []() {
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

	//----------------------------
	// AVX Detection
	//----------------------------

	/**
	 * @brief Gets the cached AVX support status.
	 * @details Checks CPU capabilities for AVX (Advanced Vector Extensions) instructions,
	 *          which provide 256-bit SIMD operations for floating-point processing. AVX enables:
	 *          - 256-bit floating-point operations (vs 128-bit SSE)
	 *          - Vectorized mathematical computations
	 *          - SIMD-accelerated floating-point algorithms
	 *          Result is cached via static initialization for zero runtime overhead.
	 * @return `true` if AVX is supported, `false` otherwise.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 * @note Requires CPUID leaf 1, ECX bit 28
	 */
	[[nodiscard]] NFX_CORE_INLINE bool hasAVXSupport() noexcept
	{
		static const bool s_hasAVX = []() {
			bool hasSupport = false;
#if defined( _MSC_VER )
			std::array<int, 4> cpuInfo{};
			__cpuid( cpuInfo.data(), 1 );					// Basic features leaf
			hasSupport = ( cpuInfo[2] & ( 1 << 28 ) ) != 0; // ECX bit 28 = AVX
#elif defined( __GNUC__ )
			unsigned int eax, ebx, ecx, edx;
			if ( __get_cpuid( 1, &eax, &ebx, &ecx, &edx ) )
			{
				hasSupport = ( ecx & ( 1 << 28 ) ) != 0; // ECX bit 28 = AVX
			}
#endif
			return hasSupport;
		}();

		return s_hasAVX;
	}

	//----------------------------
	// AVX2 Detection
	//----------------------------

	/**
	 * @brief Gets the cached AVX2 support status.
	 * @details Checks CPU capabilities for AVX2 (Advanced Vector Extensions 2) instructions,
	 *          which provide 256-bit SIMD operations for vectorized processing. AVX2 enables:
	 *          - 256-bit integer operations (vs 128-bit SSE)
	 *          - Vectorized string processing and comparison
	 *          - Parallel hash computation for multiple keys
	 *          - SIMD-accelerated mathematical operations
	 *          Result is cached via static initialization for zero runtime overhead.
	 * @return `true` if AVX2 is supported, `false` otherwise.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 * @note Requires CPUID leaf 7, subfunction 0, EBX bit 5
	 */
	[[nodiscard]] NFX_CORE_INLINE bool hasAVX2Support() noexcept
	{
		static const bool s_hasAVX2 = []() {
			bool hasSupport = false;
#if defined( _MSC_VER )
			std::array<int, 4> cpuInfo{};
			__cpuid( cpuInfo.data(), 7 );				   // Extended features leaf
			hasSupport = ( cpuInfo[1] & ( 1 << 5 ) ) != 0; // EBX bit 5 = AVX2
#elif defined( __GNUC__ )
			unsigned int eax, ebx, ecx, edx;
			if ( __get_cpuid_count( 7, 0, &eax, &ebx, &ecx, &edx ) )
			{
				hasSupport = ( ebx & ( 1 << 5 ) ) != 0; // EBX bit 5 = AVX2
			}
#endif
			return hasSupport;
		}();

		return s_hasAVX2;
	}
} // namespace nfx::core::cpu
