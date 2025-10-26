#==============================================================================
# NFX_CORE - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(nfx-core
	VERSION 0.5.3
	DESCRIPTION "A modern C++ utility library featuring cross-platform 128-bit arithmetic, zero-copy containers, advanced string processing, thread-safe memory caching, and JSON serialization."
	HOMEPAGE_URL "https://github.com/ronan-fdev/nfx-core"
	LANGUAGES CXX
)

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(NFX_CORE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "nfx-core major version")
set(NFX_CORE_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "nfx-core minor version")
set(NFX_CORE_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "nfx-core patch version")
set(NFX_CORE_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "nfx-core version")

#----------------------------------------------
# Project metadata
#----------------------------------------------

set(NFX_CORE_PROJECT_LICENSE "MIT")
set(NFX_CORE_PROJECT_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

#----------------------------------------------
# Standalone project detection
#----------------------------------------------

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(NFX_CORE_STANDALONE_PROJECT ON)
else()
	set(NFX_CORE_STANDALONE_PROJECT OFF)
endif()

option(NFX_CORE_DEVELOPER_MODE "Enable developer mode" ON)

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Build optimization ---
option(NFX_CORE_USE_CACHE            "Enable compiler cache"               ON  )

# --- Components ---
option(NFX_CORE_WITH_CONTAINERS      "Enable container utilities"          ON  )
option(NFX_CORE_WITH_DATATYPES       "Enable mathematical datatypes"       ON  )
option(NFX_CORE_WITH_MEMORY          "Enable memory management utilities"  ON  )
option(NFX_CORE_WITH_JSON            "Enable JSON serialization support"   ON  )
option(NFX_CORE_WITH_STRING          "Enable string utilities"             ON  )
option(NFX_CORE_WITH_TIME            "Enable temporal classes"             ON  )

# --- Development ---
set(NFX_CORE_DEVELOPER_DEFAULT_SHARED      ${NFX_CORE_STANDALONE_PROJECT})
set(NFX_CORE_DEVELOPER_DEFAULT_TESTS       ${NFX_CORE_STANDALONE_PROJECT})
set(NFX_CORE_DEVELOPER_DEFAULT_SAMPLES     ${NFX_CORE_STANDALONE_PROJECT})
set(NFX_CORE_DEVELOPER_DEFAULT_BENCHMARKS  ${NFX_CORE_STANDALONE_PROJECT})
set(NFX_CORE_DEVELOPER_DEFAULT_DOCS        ${NFX_CORE_STANDALONE_PROJECT})

# Override defaults for developer mode (library-focused development)
if(NFX_CORE_DEVELOPER_MODE)
	set(NFX_CORE_DEVELOPER_DEFAULT_SHARED      OFF )
	set(NFX_CORE_DEVELOPER_DEFAULT_TESTS       ON  )
	set(NFX_CORE_DEVELOPER_DEFAULT_SAMPLES     ON )
	set(NFX_CORE_DEVELOPER_DEFAULT_BENCHMARKS  ON )
	set(NFX_CORE_DEVELOPER_DEFAULT_DOCS        OFF )
endif()

# --- Library build types ---
option(NFX_CORE_BUILD_STATIC         "Build static library"               ON  )
option(NFX_CORE_BUILD_SHARED         "Build shared library"               ${NFX_CORE_DEVELOPER_DEFAULT_SHARED})

option(NFX_CORE_BUILD_TESTS          "Build tests"                        ${NFX_CORE_DEVELOPER_DEFAULT_TESTS})
option(NFX_CORE_BUILD_SAMPLES        "Build samples"                      ${NFX_CORE_DEVELOPER_DEFAULT_SAMPLES})
option(NFX_CORE_BUILD_BENCHMARKS     "Build benchmarks"                   ${NFX_CORE_DEVELOPER_DEFAULT_BENCHMARKS})
option(NFX_CORE_BUILD_DOCUMENTATION  "Build Doxygen documentation"        ${NFX_CORE_DEVELOPER_DEFAULT_DOCS})

# --- Installation ---
option(NFX_CORE_INSTALL_PROJECT      "Install project"                    ${NFX_CORE_STANDALONE_PROJECT})

# --- Packaging ---
option(NFX_CORE_PACKAGE_SOURCE       "Enable source package generation"   ${NFX_CORE_STANDALONE_PROJECT})
option(NFX_CORE_PACKAGE_ARCHIVE      "Enable TGZ/ZIP package generation"  ${NFX_CORE_STANDALONE_PROJECT})
option(NFX_CORE_PACKAGE_DEB          "Enable DEB package generation"      ${NFX_CORE_STANDALONE_PROJECT})
option(NFX_CORE_PACKAGE_RPM          "Enable RPM package generation"      ${NFX_CORE_STANDALONE_PROJECT})
option(NFX_CORE_PACKAGE_NSIS         "Enable NSIS Windows installer"      ${NFX_CORE_STANDALONE_PROJECT})

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(nfxCoreBuildConfig)
include(nfxCoreDependencies)
include(nfxCoreSources)

#----------------------------------------------
# Installation and packaging
#----------------------------------------------

include(nfxCoreInstall)
include(nfxCorePackaging)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(test)
add_subdirectory(samples)
add_subdirectory(benchmark)
add_subdirectory(doc)
