#==============================================================================
# NFX_CORE - C++ Library CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

# --- Always include platform configuration ---
set(PUBLIC_HEADERS
	${NFX_CORE_INCLUDE_DIR}/nfx/core/hashing/Hash.h
	${NFX_CORE_INCLUDE_DIR}/nfx/core/hashing/Hash.inl
	${NFX_CORE_INCLUDE_DIR}/nfx/config.h
)

set(PRIVATE_SOURCES)

# --- Container components ---
if(NFX_CORE_WITH_CONTAINERS)
	list(APPEND PUBLIC_HEADERS
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/HashMapHashFunctor.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/HashMapHashFunctor.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/StringFunctors.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/StringFunctors.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/ChdHashMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/ChdHashMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/HashMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/HashMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringSet.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringSet.inl
	)
	list(APPEND PRIVATE_SOURCES
	)
endif()

# --- Datatype components ---
if(NFX_CORE_WITH_DATATYPES)
	list(APPEND PUBLIC_HEADERS
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/constants/DecimalConstants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/constants/Int128Constants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Decimal.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Decimal.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Int128.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Int128.inl
	)
	list(APPEND PRIVATE_SOURCES
		${NFX_CORE_SOURCE_DIR}/datatypes/Decimal.cpp
		${NFX_CORE_SOURCE_DIR}/datatypes/Int128.cpp
	)
endif()

# --- Container components ---
if(NFX_CORE_WITH_MEMORY)
	list(APPEND PUBLIC_HEADERS
		${NFX_CORE_INCLUDE_DIR}/nfx/memory/LruCache.h
		${NFX_CORE_INCLUDE_DIR}/nfx/memory/LruCache.inl
	)
	list(APPEND PRIVATE_SOURCES
	)
endif()

# --- String components ---
if(NFX_CORE_WITH_STRING)
	list(APPEND PUBLIC_HEADERS
		${NFX_CORE_INCLUDE_DIR}/nfx/string/StringBuilderPool.h
		${NFX_CORE_INCLUDE_DIR}/nfx/string/StringBuilderPool.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Splitter.h
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Splitter.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Utils.h
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Utils.inl
	)
	list(APPEND PRIVATE_HEADERS
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBuffer_impl.h
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBufferPool.cpp
	)
	list(APPEND PRIVATE_SOURCES
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBuffer_impl.cpp
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBufferPool.cpp
		${NFX_CORE_SOURCE_DIR}/string/StringBuilderPool.cpp
	)
endif()

# --- Temporal components ---
if(NFX_CORE_WITH_TIME)
	list(APPEND PUBLIC_HEADERS
		${NFX_CORE_INCLUDE_DIR}/nfx/time/constants/DateTimeConstants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/time/DateTime.h
		${NFX_CORE_INCLUDE_DIR}/nfx/time/DateTime.inl
	)
	list(APPEND PRIVATE_SOURCES
		${NFX_CORE_SOURCE_DIR}/time/DateTime.cpp
	)
endif()

# --- Show what components are being built ---
message(STATUS "Enabled components:")
if(NFX_CORE_WITH_CONTAINERS)
	message(STATUS "  - Containers (ChdHashMap, HashMap, StringMap, StringSet, StringFunctors)")
endif()
if(NFX_CORE_WITH_DATATYPES)
	message(STATUS "  - Datatypes (Int128, Decimal)")
endif()
if(NFX_CORE_WITH_MEMORY)
	message(STATUS "  - Memory (LruCache)")
endif()
if(NFX_CORE_WITH_STRING)
	message(STATUS "  - String utilities (StringBuilderPool, Splitter, Utils)")
endif()
if(NFX_CORE_WITH_TIME)
	message(STATUS "  - Time utilities (DateTime, DateTimeOffset, TimeSpan)")
endif()

list(LENGTH PUBLIC_HEADERS header_count)
list(LENGTH PRIVATE_SOURCES source_count)
message(STATUS "Building with ${header_count} headers and ${source_count} source files")

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Create shared library if requested ---
if(NFX_CORE_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PUBLIC
			${PUBLIC_HEADERS}
		PRIVATE
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${NFX_CORE_BUILD_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${NFX_CORE_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(NFX_CORE_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PUBLIC
			${PUBLIC_HEADERS}
		PRIVATE
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static
		ARCHIVE_OUTPUT_DIRECTORY ${NFX_CORE_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

function(configure_target target_name)
	# --- Include directories ---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${NFX_CORE_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${NFX_CORE_SOURCE_DIR}
	)

	# --- External dependencies ---
	if(NFX_CORE_WITH_STRING)
		target_link_libraries(${target_name} PRIVATE
			fmt::fmt-header-only
		)
	endif()

	# --- Properties ---
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
	)
endfunction()

# --- Apply configuration to both targets ---
if(NFX_CORE_BUILD_SHARED)
	configure_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfxCoreVersion.rc.in
			${CMAKE_BINARY_DIR}/nfxCoreVersion.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/nfxCoreVersion.rc)
	endif()
endif()

if(NFX_CORE_BUILD_STATIC)
	configure_target(${PROJECT_NAME}-static)
endif()
