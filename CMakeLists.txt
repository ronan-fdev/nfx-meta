#==============================================================================
# nfx-meta - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(nfx-meta
	VERSION 0.8.0
	DESCRIPTION "A modern C++ utility library featuring cross-platform 128-bit arithmetic, zero-copy containers, advanced string processing, thread-safe memory caching, and JSON serialization."
	HOMEPAGE_URL "https://github.com/ronan-fdev/nfx-meta"
	LANGUAGES CXX
)

#----------------------------------------------
# Version configuration
#----------------------------------------------

set(NFX_META_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "nfx-meta major version")
set(NFX_META_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "nfx-meta minor version")
set(NFX_META_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "nfx-meta patch version")
set(NFX_META_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "nfx-meta version")

#----------------------------------------------
# Project metadata
#----------------------------------------------

set(NFX_META_PROJECT_LICENSE "MIT")
set(NFX_META_PROJECT_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

#----------------------------------------------
# Standalone project detection
#----------------------------------------------

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(NFX_META_STANDALONE_PROJECT ON)
else()
	set(NFX_META_STANDALONE_PROJECT OFF)
endif()

option(NFX_META_DEVELOPER_MODE "Enable developer mode" ON)

#----------------------------------------------
# Testing setup
#----------------------------------------------

enable_testing()

#----------------------------------------------
# Build options
#----------------------------------------------

# --- Build optimization ---
option(NFX_META_USE_CACHE            "Enable compiler cache"               ON  )

# --- Components ---
option(NFX_META_WITH_CONTAINERS      "Enable container utilities"          ON  )
option(NFX_META_WITH_DATATYPES       "Enable mathematical datatypes"       ON  )
option(NFX_META_WITH_MEMORY          "Enable memory management utilities"  ON  )
option(NFX_META_WITH_JSON            "Enable JSON serialization support"   ON  )
option(NFX_META_WITH_STRING          "Enable string utilities"             ON  )
option(NFX_META_WITH_TIME            "Enable temporal classes"             ON  )

# --- Development ---
set(NFX_META_DEVELOPER_DEFAULT_SHARED      ${NFX_META_STANDALONE_PROJECT})
set(NFX_META_DEVELOPER_DEFAULT_TESTS       ${NFX_META_STANDALONE_PROJECT})
set(NFX_META_DEVELOPER_DEFAULT_SAMPLES     ${NFX_META_STANDALONE_PROJECT})
set(NFX_META_DEVELOPER_DEFAULT_BENCHMARKS  ${NFX_META_STANDALONE_PROJECT})
set(NFX_META_DEVELOPER_DEFAULT_DOCS        ${NFX_META_STANDALONE_PROJECT})

# Override defaults for developer mode (library-focused development)
if(NFX_META_DEVELOPER_MODE)
	set(NFX_META_DEVELOPER_DEFAULT_SHARED      ON )
	set(NFX_META_DEVELOPER_DEFAULT_TESTS       ON  )
	set(NFX_META_DEVELOPER_DEFAULT_SAMPLES     ON )
	set(NFX_META_DEVELOPER_DEFAULT_BENCHMARKS  ON )
	set(NFX_META_DEVELOPER_DEFAULT_DOCS        OFF)
endif()

# --- Library build types ---
option(NFX_META_BUILD_STATIC         "Build static library"               ON  )
option(NFX_META_BUILD_SHARED         "Build shared library"               ${NFX_META_DEVELOPER_DEFAULT_SHARED})

option(NFX_META_BUILD_TESTS          "Build tests"                        ${NFX_META_DEVELOPER_DEFAULT_TESTS})
option(NFX_META_BUILD_SAMPLES        "Build samples"                      ${NFX_META_DEVELOPER_DEFAULT_SAMPLES})
option(NFX_META_BUILD_BENCHMARKS     "Build benchmarks"                   ${NFX_META_DEVELOPER_DEFAULT_BENCHMARKS})
option(NFX_META_BUILD_DOCUMENTATION  "Build Doxygen documentation"        ${NFX_META_DEVELOPER_DEFAULT_DOCS})

# --- Installation ---
option(NFX_META_INSTALL_PROJECT      "Install project"                    ${NFX_META_STANDALONE_PROJECT})

# --- Packaging ---
option(NFX_META_PACKAGE_SOURCE       "Enable source package generation"   ${NFX_META_STANDALONE_PROJECT})
option(NFX_META_PACKAGE_ARCHIVE      "Enable TGZ/ZIP package generation"  ${NFX_META_STANDALONE_PROJECT})
option(NFX_META_PACKAGE_DEB          "Enable DEB package generation"      ${NFX_META_STANDALONE_PROJECT})
option(NFX_META_PACKAGE_RPM          "Enable RPM package generation"      ${NFX_META_STANDALONE_PROJECT})
option(NFX_META_PACKAGE_NSIS         "Enable NSIS Windows installer"      ${NFX_META_STANDALONE_PROJECT})

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#----------------------------------------------
# Main configuration
#----------------------------------------------

include(nfxMetaBuildConfig)
include(nfxMetaDependencies)
include(nfxMetaSources)

#----------------------------------------------
# Installation and packaging
#----------------------------------------------

include(nfxMetaInstall)
include(nfxMetaPackaging)

#----------------------------------------------
# Targets
#----------------------------------------------

add_subdirectory(test)
add_subdirectory(samples)
add_subdirectory(benchmark)
add_subdirectory(doc)
