#==============================================================================
# NFX_CORE - CMake build configuration
#==============================================================================

#----------------------------------------------
# Build validation
#----------------------------------------------

# --- Validate CMake version ---
if(CMAKE_VERSION VERSION_LESS "3.20")
	message(FATAL_ERROR "CMake 3.20 or higher is required for reliable C++20 support")
endif()

# --- Prevent in-source builds ---
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# --- Ensure at least one library type is built ---
if(NOT NFX_CORE_BUILD_STATIC AND NOT NFX_CORE_BUILD_SHARED)
	message(WARNING "Neither NFX_CORE_BUILD_STATIC nor NFX_CORE_BUILD_SHARED is enabled.")
	
	if(DEFINED CACHE{NFX_CORE_BUILD_STATIC} AND DEFINED CACHE{NFX_CORE_BUILD_SHARED})
		message(STATUS "Both library types were explicitly disabled.")
		message(STATUS "Applying fallback: Enabling static library build")
		set(NFX_CORE_BUILD_STATIC ON CACHE BOOL "Build static library (fallback)" FORCE)
	else()
		message(STATUS "Defaulting to static library build")
		set(NFX_CORE_BUILD_STATIC ON)
	endif()
endif()

#----------------------------------------------
# Multi-config generator setup
#----------------------------------------------

# --- For multi-config generators (Visual Studio, Xcode), set available configurations ---
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES "Release;Debug;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build configurations" FORCE)
	message(STATUS "Multi-config generator detected. Available configurations: ${CMAKE_CONFIGURATION_TYPES}")
else()
	# --- For single-config generators (Makefiles, Ninja), set default build type ---
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
		message(STATUS "Single-config generator detected. Defaulting to optimized build type: ${CMAKE_BUILD_TYPE}")
	else()
		message(STATUS "Single-config generator detected. Using specified build type: ${CMAKE_BUILD_TYPE}")
	endif()
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

#----------------------------------------------
# Install prefix configuration
#----------------------------------------------

include(GNUInstallDirs)

#----------------------------------------------
# Directory configuration
#----------------------------------------------

set(NFX_CORE_ROOT_DIR           "${CMAKE_CURRENT_SOURCE_DIR}"   CACHE PATH  "Root directory"     )
set(NFX_CORE_INCLUDE_DIR        "${NFX_CORE_ROOT_DIR}/include"  CACHE PATH  "Include directory"  )
set(NFX_CORE_SOURCE_DIR         "${NFX_CORE_ROOT_DIR}/src"      CACHE PATH  "Source directory"   )
set(NFX_CORE_TEST_DIR           "${NFX_CORE_ROOT_DIR}/tests"    CACHE PATH  "Test directory"     )
set(NFX_CORE_SAMPLES_DIR        "${NFX_CORE_ROOT_DIR}/samples"  CACHE PATH  "Samples directory"  )
set(NFX_CORE_DOCUMENTATION_DIR  "${NFX_CORE_ROOT_DIR}/doc"      CACHE PATH  "Documentation dir"  )

#----------------------------------------------
# Compiler detection
#----------------------------------------------

set(COMPILER_NAME "Unknown")

if(WIN32)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(COMPILER_NAME "MSVC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_NAME "GCC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if(CMAKE_CXX_COMPILER MATCHES "clang-cl(\\.exe)?$")
			set(COMPILER_NAME "Clang-MSVC-CLI")
		else()
			set(COMPILER_NAME "Clang-GNU-CLI")
		endif()
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
		set(COMPILER_NAME "Intel")
	endif()
elseif(UNIX AND NOT APPLE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_NAME "GCC")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(COMPILER_NAME "Clang")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
		set(COMPILER_NAME "Intel")
	endif()
endif()

message(STATUS "Compiler resolved as: ${COMPILER_NAME}")

#----------------------------------------------
# Architecture detection
#----------------------------------------------

set(ARCH_NAME "Unknown")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCH_NAME "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ARCH_NAME "x86")
endif()

#----------------------------------------------
# Output directory configuration
#----------------------------------------------

set(COMPILER_DIR_NAME "${COMPILER_NAME}-${CMAKE_CXX_COMPILER_VERSION}-${ARCH_NAME}")
set(NFX_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}/${COMPILER_DIR_NAME}/$<CONFIG>)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NFX_CORE_BUILD_DIR}/bin)

#----------------------------------------------
# Compiler cache support
#----------------------------------------------

if(NFX_CORE_USE_CCACHE)
	find_program(CCACHE_PROGRAM ccache)
	if(CCACHE_PROGRAM)
		message(STATUS "Using compiler cache: ${CCACHE_PROGRAM}")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
	else()
		message(STATUS "ccache not found - install for faster builds")
	endif()
endif()

#----------------------------------------------
# CPU feature detection
#----------------------------------------------

# --- Detect CPU cores ---
include(ProcessorCount)
ProcessorCount(NFX_CORE_CPU_COUNT)
math(EXPR NFX_CORE_THREADS "(${NFX_CORE_CPU_COUNT} * 3 + 3) / 4") 
message(STATUS "CPU cores detected: ${NFX_CORE_CPU_COUNT}, using ${NFX_CORE_THREADS} threads for parallel builds")

# --- Detect CPU capabilities ---
include(CheckCXXSourceRuns)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_REQUIRED_FLAGS "/arch:AVX2")
else()
	set(CMAKE_REQUIRED_FLAGS "-mavx2")
endif()

check_cxx_source_runs("
	#include <immintrin.h>
	int main() {
		__m256i a = _mm256_set1_epi32(1);
		__m256i b = _mm256_add_epi32(a, a);
		return _mm256_extract_epi32(b, 0) == 2 ? 0 : 1;
	}
" NFX_CORE_HAS_AVX2_RUNTIME_SUPPORT)

if(NFX_CORE_HAS_AVX2_RUNTIME_SUPPORT)
	message(STATUS "CPU supports AVX2 - enabling advanced optimizations")
	set(NFX_CORE_ENABLE_AVX2 ON CACHE BOOL "AVX2 support enabled" FORCE)
else()
	message(STATUS "CPU does not support AVX2 - using SSE4.2 fallback")
	set(NFX_CORE_ENABLE_AVX2 OFF CACHE BOOL "AVX2 support disabled (not supported)" FORCE)
endif()

set(CMAKE_REQUIRED_FLAGS "")
