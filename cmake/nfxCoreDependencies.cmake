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

if(NFX_CORE_WITH_STRING)
# --- {fmt} ---
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
		GIT_TAG        11.2.0
		GIT_SHALLOW    TRUE
	)
endif()

# --- Google test ---
if(NFX_CORE_BUILD_TESTS)
	set(BUILD_GMOCK         OFF  CACHE BOOL  "Build GoogleMock library"              FORCE)
	set(INSTALL_GTEST       OFF  CACHE BOOL  "Install GoogleTest targets"            FORCE)
	set(GTEST_HAS_ABSL      OFF  CACHE BOOL  "Disable Abseil support in GoogleTest"  FORCE)

	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        v1.17.0
		GIT_SHALLOW    TRUE
	)
endif()

#----------------------------
# Dependency fetching
#----------------------------

if(NFX_CORE_BUILD_TESTS)
	FetchContent_MakeAvailable(
		googletest
	)
endif()

if(NFX_CORE_WITH_STRING)
	FetchContent_MakeAvailable(
		fmt
	)
endif()
