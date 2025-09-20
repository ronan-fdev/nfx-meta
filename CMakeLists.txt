#==============================================================================
# NFX_CORE - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(nfx-core
	VERSION 0.0.5
	DESCRIPTION "C++ utility library with cross-platform 128-bit arithmetic, temporal calculations, zero-copy containers, and advanced string processing"
	LANGUAGES CXX
)

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Library build types ---
option(NFX_CORE_BUILD_STATIC     "Build static library"                ON  )
option(NFX_CORE_BUILD_SHARED     "Build shared library"                ON  )

# --- Components ---
option(NFX_CORE_WITH_CONTAINERS  "Enable container utilities"          ON  )
option(NFX_CORE_WITH_DATATYPES   "Enable mathematical datatypes"       ON  )
option(NFX_CORE_WITH_MEMORY      "Enable memory management utilities"  ON  )
option(NFX_CORE_WITH_STRING      "Enable string utilities"             ON  )
option(NFX_CORE_WITH_TIME        "Enable temporal classes"             ON  )

# --- Development ---
option(NFX_CORE_BUILD_TESTS      "Build tests"                         ON  )
option(NFX_CORE_BUILD_SAMPLES    "Build samples"                       ON  )

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(nfxCoreConfig)
include(nfxCoreDependencies)
include(nfxCoreSources)

#----------------------------------------------
# Tests target
#----------------------------------------------

if(NFX_CORE_BUILD_TESTS)
	add_subdirectory(tests)
endif()

#----------------------------------------------
# Samples target
#----------------------------------------------

if(NFX_CORE_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

