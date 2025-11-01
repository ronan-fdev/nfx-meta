#==============================================================================
# nfx-meta - Dependencies configuration
#==============================================================================

#----------------------------
# Output configuration
#----------------------------

set(_SAVED_CMAKE_REQUIRED_QUIET     ${CMAKE_REQUIRED_QUIET})
set(_SAVED_CMAKE_MESSAGE_LOG_LEVEL  ${CMAKE_MESSAGE_LOG_LEVEL})
set(_SAVED_CMAKE_FIND_QUIETLY       ${CMAKE_FIND_QUIETLY})

set(CMAKE_REQUIRED_QUIET    ON         )
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE    ) # [ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG]
set(CMAKE_FIND_QUIETLY      ON         )

#----------------------------------------------
# FetchContent dependencies
#----------------------------------------------

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/.deps/${COMPILER_DIR_NAME}")

if(DEFINED ENV{CI})
	set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
else()
	set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
set(FETCHCONTENT_QUIET OFF)

#----------------------------
# Dependency version requirements
#----------------------------

set(NFX_META_NLOHMANN_JSON_MIN_VERSION  "3.11.0")
set(NFX_META_GTEST_MIN_VERSION          "1.12.1")
set(NFX_META_BENCHMARK_MIN_VERSION      "1.9.1" )

#----------------------------
# Dependency declarations
#----------------------------

FetchContent_Declare(
	nfx-core
	GIT_REPOSITORY https://github.com/ronan-fdev/nfx-core.git
	GIT_TAG        2.0.0
	GIT_SHALLOW    TRUE
)

if(NFX_META_WITH_TIME)
	FetchContent_Declare(
		nfx-datetime
		GIT_REPOSITORY https://github.com/ronan-fdev/nfx-datetime.git
		GIT_TAG        1.0.1
		GIT_SHALLOW    TRUE
	)
endif()

if(NFX_META_WITH_STRING)
	FetchContent_Declare(
		nfx-stringutils
		GIT_REPOSITORY https://github.com/ronan-fdev/nfx-stringutils.git
		GIT_TAG        1.1.0
		GIT_SHALLOW    TRUE
	)
	FetchContent_Declare(
		nfx-stringbuilderpool
		GIT_REPOSITORY https://github.com/ronan-fdev/nfx-stringbuilderpool.git
		GIT_TAG        1.0.0
		GIT_SHALLOW    TRUE
	)
endif()

if(NFX_META_WITH_DATATYPES)
	FetchContent_Declare(
		nfx-datatypes
		GIT_REPOSITORY https://github.com/ronan-fdev/nfx-datatypes.git
		GIT_TAG        1.0.2
		GIT_SHALLOW    TRUE
	)
endif()

if(NFX_META_WITH_MEMORY)
	FetchContent_Declare(
		nfx-lrucache
		GIT_REPOSITORY https://github.com/ronan-fdev/nfx-lrucache.git
		GIT_TAG        1.0.7
		GIT_SHALLOW    TRUE
	)
endif()

# --- nlohmann/json ---
if(NFX_META_WITH_JSON)
	find_package(nlohmann_json ${NFX_META_NLOHMANN_JSON_MIN_VERSION} QUIET)
	if(NOT nlohmann_json_FOUND)
		message(STATUS "nlohmann/json not found on system, using FetchContent")
	
		FetchContent_Declare(nlohmann_json
			URL https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip
			URL_HASH SHA256=b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
	else()
		message(STATUS "Using system-installed nlohmann/json version ${nlohmann_json_VERSION}")
	endif()
endif()

# --- Google test ---
if(NFX_META_BUILD_TESTS)
	find_package(GTest ${NFX_META_GTEST_MIN_VERSION} QUIET)
	
	if(NOT GTest_FOUND)
		message(STATUS "GoogleTest not found on system, using FetchContent")
		
		set(BUILD_GMOCK         OFF  CACHE BOOL  "Build GoogleMock library"              FORCE)
		set(INSTALL_GTEST       OFF  CACHE BOOL  "Install GoogleTest targets"            FORCE)
		set(GTEST_HAS_ABSL      OFF  CACHE BOOL  "Disable Abseil support in GoogleTest"  FORCE)

		FetchContent_Declare(
			googleTest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG        v1.17.0
			GIT_SHALLOW    TRUE
		)
	else()
		message(STATUS "Using system-installed GoogleTest version ${GTest_VERSION}")
	endif()
endif()

# --- Google benchmark ---
if(NFX_META_BUILD_BENCHMARKS)
	find_package(benchmark ${NFX_META_BENCHMARK_MIN_VERSION} QUIET)
	
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
			googleBenchmark
			GIT_REPOSITORY https://github.com/google/benchmark.git
			GIT_TAG        v1.9.4
			GIT_SHALLOW    TRUE
		)
	else()
		message(STATUS "Using system-installed Google Benchmark version ${benchmark_VERSION}")
	endif()
endif()

#----------------------------
# Dependency fetching
#----------------------------

FetchContent_MakeAvailable(nfx-core)

if(NFX_META_WITH_TIME)
	FetchContent_MakeAvailable(nfx-datetime)
endif()

if(NFX_META_WITH_STRING)
	FetchContent_MakeAvailable(nfx-stringutils nfx-stringbuilderpool)
endif()

if(NFX_META_WITH_DATATYPES)
	FetchContent_MakeAvailable(nfx-datatypes)
endif()

if(NFX_META_WITH_MEMORY)
	FetchContent_MakeAvailable(nfx-lrucache)
endif()

if(NFX_META_WITH_JSON)
	if(NOT nlohmann_json_FOUND)
		FetchContent_MakeAvailable(nlohmann_json)
	endif()
endif()

if(NFX_META_BUILD_TESTS)
	if(NOT GTest_FOUND)
		FetchContent_MakeAvailable(
			googleTest
		)
	endif()
endif()

if(NFX_META_BUILD_BENCHMARKS)
	if(NOT benchmark_FOUND)
		FetchContent_MakeAvailable(
			googleBenchmark
		)
	endif()
endif()

#----------------------------
# Dependencies Summary
#----------------------------

message(STATUS "NFX Dependencies:")

message(STATUS "  nfx-core             : ${NFX_CORE_VERSION}")

if(NFX_META_WITH_TIME)
	message(STATUS "  nfx-datetime         : ${NFX_DATETIME_VERSION}")
endif()
if(NFX_META_WITH_STRING)
	message(STATUS "  nfx-stringutils      : ${NFX_STRINGUTILS_VERSION}")
	message(STATUS "  nfx-stringbuilderpool: ${NFX_STRINGBUILDERPOOL_VERSION}")
endif()
if(NFX_META_WITH_DATATYPES)
	message(STATUS "  nfx-datatypes        : ${NFX_DATATYPES_VERSION}")
endif()
if(NFX_META_WITH_MEMORY)
	message(STATUS "  nfx-lrucache         : ${NFX_LRUCACHE_VERSION}")
endif()
if(NFX_META_WITH_JSON)
	if(nlohmann_json_FOUND)
		message(STATUS "  nlohmann_json        : ${nlohmann_json_VERSION} (system)")
	else()
		file(READ "${nlohmann_json_SOURCE_DIR}/single_include/nlohmann/json.hpp" NLOHMANN_JSON_HEADER)
		string(REGEX MATCH "#define NLOHMANN_JSON_VERSION_MAJOR ([0-9]+)" _ "${NLOHMANN_JSON_HEADER}")
		set(NLOHMANN_JSON_VERSION_MAJOR ${CMAKE_MATCH_1})
		string(REGEX MATCH "#define NLOHMANN_JSON_VERSION_MINOR ([0-9]+)" _ "${NLOHMANN_JSON_HEADER}")
		set(NLOHMANN_JSON_VERSION_MINOR ${CMAKE_MATCH_1})
		string(REGEX MATCH "#define NLOHMANN_JSON_VERSION_PATCH ([0-9]+)" _ "${NLOHMANN_JSON_HEADER}")
		set(NLOHMANN_JSON_VERSION_PATCH ${CMAKE_MATCH_1})
		set(NLOHMANN_JSON_DETECTED_VERSION "${NLOHMANN_JSON_VERSION_MAJOR}.${NLOHMANN_JSON_VERSION_MINOR}.${NLOHMANN_JSON_VERSION_PATCH}")
		message(STATUS "  nlohmann_json        : ${NLOHMANN_JSON_DETECTED_VERSION}")
	endif()
endif()

#----------------------------
# Cleanup
#----------------------------

set(CMAKE_REQUIRED_QUIET ${_SAVED_CMAKE_REQUIRED_QUIET})
set(CMAKE_MESSAGE_LOG_LEVEL ${_SAVED_CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_FIND_QUIETLY ${_SAVED_CMAKE_FIND_QUIETLY})
