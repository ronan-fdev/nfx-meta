#==============================================================================
# NFX_CORE - C++ Library dependencies configuration
#==============================================================================

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${NFX_CORE_ROOT_DIR}/.deps/${COMPILER_DIR_NAME}")

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

#----------------------------
# Dependency declarations
#----------------------------

# --- {fmt} ---
if(NFX_CORE_WITH_STRING)
	find_package(fmt QUIET)
	
	if(NOT fmt_FOUND)
		message(STATUS "fmt not found on system, using FetchContent")
		
		set(FMT_FUZZ            OFF  CACHE BOOL  "Build fmt fuzzing tests"               FORCE)
		set(FMT_TEST            OFF  CACHE BOOL  "Build fmt unit tests"                  FORCE)
		set(FMT_CUDA_TEST       OFF  CACHE BOOL  "Build fmt cuda tests"                  FORCE)
		set(FMT_DOC             OFF  CACHE BOOL  "Build fmt documentation"               FORCE)
		set(FMT_INSTALL         OFF  CACHE BOOL  "Install fmt targets"                   FORCE)
		set(FMT_HEADER_ONLY     ON   CACHE BOOL  "Build fmt as header-only library"      FORCE)
		set(FMT_MODULE          OFF  CACHE BOOL  "Enable fmt C++20 module support"       FORCE)
		set(FMT_OS              OFF  CACHE BOOL  "Enable fmt OS-specific features"       FORCE)
		set(FMT_SYSTEM_HEADERS  OFF  CACHE BOOL  "Use system headers for fmt"            FORCE)
		set(FMT_UNICODE         ON   CACHE BOOL  "Enable Unicode support in fmt"         FORCE)

		FetchContent_Declare(
			fmt
			GIT_REPOSITORY https://github.com/fmtlib/fmt.git
			GIT_TAG        12.0.0
			GIT_SHALLOW    TRUE
		)
	else()
		message(STATUS "Using system-installed fmt")
	endif()
endif()

# --- Google test ---
if(NFX_CORE_BUILD_TESTS)
	find_package(GTest QUIET)
	
	if(NOT GTest_FOUND)
		message(STATUS "GoogleTest not found on system, using FetchContent")
		
		set(BUILD_GMOCK         OFF  CACHE BOOL  "Build GoogleMock library"              FORCE)
		set(INSTALL_GTEST       OFF  CACHE BOOL  "Install GoogleTest targets"            FORCE)
		set(GTEST_HAS_ABSL      OFF  CACHE BOOL  "Disable Abseil support in GoogleTest"  FORCE)

		FetchContent_Declare(
			googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG        v1.17.0
			GIT_SHALLOW    TRUE
		)
	else()
		message(STATUS "Using system-installed GoogleTest")
	endif()
endif()

# --- Google benchmark ---
if(NFX_CORE_BUILD_BENCHMARKS)
	find_package(benchmark QUIET)
	
	if(NOT benchmark_FOUND)
		message(STATUS "Google Benchmark not found on system, using FetchContent")
		
		set(BENCHMARK_ENABLE_TESTING         OFF  CACHE BOOL  "Build benchmark tests"                          FORCE)
		set(BENCHMARK_ENABLE_EXCEPTIONS      ON   CACHE BOOL  "Enable exceptions in benchmark library"         FORCE)
		set(BENCHMARK_ENABLE_LTO             OFF  CACHE BOOL  "Enable link time optimization"                  FORCE)
		set(BENCHMARK_USE_LIBCXX             OFF  CACHE BOOL  "Use libc++ as the standard library"             FORCE)
		set(BENCHMARK_ENABLE_WERROR          OFF  CACHE BOOL  "Treat warnings as errors"                       FORCE)
		set(BENCHMARK_FORCE_WERROR           OFF  CACHE BOOL  "Force warnings as errors regardless of issues"  FORCE)
		set(BENCHMARK_BUILD_32_BITS          OFF  CACHE BOOL  "Build a 32-bit version of the library"          FORCE)
		set(BENCHMARK_ENABLE_INSTALL         OFF  CACHE BOOL  "Install benchmark targets"                      FORCE)
		set(BENCHMARK_ENABLE_DOXYGEN         OFF  CACHE BOOL  "Build documentation with Doxygen"               FORCE)
		set(BENCHMARK_INSTALL_DOCS           OFF  CACHE BOOL  "Install documentation"                          FORCE)
		set(BENCHMARK_DOWNLOAD_DEPENDENCIES  OFF  CACHE BOOL  "Download and build unmet dependencies"          FORCE)
		set(BENCHMARK_ENABLE_GTEST_TESTS     OFF  CACHE BOOL  "Build benchmark GTest-based tests"              FORCE)
		set(BENCHMARK_USE_BUNDLED_GTEST      OFF  CACHE BOOL  "Use bundled GoogleTest for benchmark"           FORCE)
		set(BENCHMARK_ENABLE_LIBPFM          OFF  CACHE BOOL  "Enable performance counters via libpfm"         FORCE)
		set(ENABLE_ASSEMBLY_TESTS_DEFAULT    OFF  CACHE BOOL  "Enable assembly tests by default"               FORCE)

		FetchContent_Declare(
			googlebenchmark
			GIT_REPOSITORY https://github.com/google/benchmark.git
			GIT_TAG        v1.9.4
			GIT_SHALLOW    TRUE
		)
	else()
		message(STATUS "Using system-installed Google Benchmark")
	endif()
endif()

#----------------------------
# Dependency fetching
#----------------------------

if(NFX_CORE_WITH_STRING)
	if(NOT fmt_FOUND)
		FetchContent_MakeAvailable(
			fmt
		)
	endif()
endif()

if(NFX_CORE_BUILD_TESTS)
	if(NOT GTest_FOUND)
		FetchContent_MakeAvailable(
			googletest
		)
	endif()
endif()

if(NFX_CORE_BUILD_BENCHMARKS)
	if(NOT benchmark_FOUND)
		FetchContent_MakeAvailable(
			googlebenchmark
		)
	endif()
endif()
