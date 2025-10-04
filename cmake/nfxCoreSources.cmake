#==============================================================================
# NFX_CORE - C++ Library CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

# --- Always include core headers ---
set(PUBLIC_HEADERS
	# --- Core configuration ---
	${NFX_CORE_INCLUDE_DIR}/nfx/config.h

	#--- Core utilities ---
	${NFX_CORE_INCLUDE_DIR}/nfx/core/CPU.h
	${NFX_CORE_INCLUDE_DIR}/nfx/core/Hashing.h

	# --- Core utilities implementation ---
	${NFX_CORE_INCLUDE_DIR}/nfx/detail/core/Hashing.inl
)

set(PRIVATE_SOURCES)

# --- Container components ---
if(NFX_CORE_WITH_CONTAINERS)
	list(APPEND PUBLIC_HEADERS
		# --- Container functors ---
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/HashMapHashFunctor.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/functors/StringFunctors.h

		# --- Container headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/ChdHashMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/HashMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringMap.h
		${NFX_CORE_INCLUDE_DIR}/nfx/containers/StringSet.h

		# --- Container functors implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/functors/HashMapHashFunctor.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/functors/StringFunctors.inl

		# --- Container inline implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/ChdHashMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/HashMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/StringMap.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/containers/StringSet.inl
	)
	list(APPEND PRIVATE_SOURCES
	)
endif()

# --- Datatype components ---
if(NFX_CORE_WITH_DATATYPES)
	list(APPEND PUBLIC_HEADERS
		# --- Datatype headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/constants/DecimalConstants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/constants/Int128Constants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Decimal.h
		${NFX_CORE_INCLUDE_DIR}/nfx/datatypes/Int128.h

		# --- Datatype implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/datatypes/Decimal.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/datatypes/Int128.inl
	)
	list(APPEND PRIVATE_SOURCES
		# --- Datatype source files ---
		${NFX_CORE_SOURCE_DIR}/datatypes/Decimal.cpp
		${NFX_CORE_SOURCE_DIR}/datatypes/Int128.cpp
	)
endif()

# --- Container components ---
if(NFX_CORE_WITH_MEMORY)
	list(APPEND PUBLIC_HEADERS
		# --- Memory management headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/memory/LruCache.h

		# --- Memory management implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/memory/LruCache.inl
	)
	list(APPEND PRIVATE_SOURCES
	)
endif()

# --- Serialization components ---
if(NFX_CORE_WITH_JSON)
	list(APPEND PUBLIC_HEADERS
		# --- JSON serialization headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/ArrayEnumerator.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/Document.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/FieldEnumerator.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/SchemaValidator.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/SerializationTraits.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/SerializationTraits.h
		${NFX_CORE_INCLUDE_DIR}/nfx/serialization/json/Serializer.h

		# --- JSON serialization implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/serialization/json/Serializer.inl
	)

	list(APPEND PRIVATE_HEADERS
		# --- JSON serialization private headers ---
		${NFX_CORE_SOURCE_DIR}/serialization/json/ArrayEnumerator_impl.h
		${NFX_CORE_SOURCE_DIR}/serialization/json/Document_impl.h
		${NFX_CORE_SOURCE_DIR}/serialization/json/FieldEnumerator_impl.h
		${NFX_CORE_SOURCE_DIR}/serialization/json/SchemaValidator_impl.h
	)
	list(APPEND PRIVATE_SOURCES
		# --- JSON serialization implementations ---
		${NFX_CORE_SOURCE_DIR}/serialization/json/ArrayEnumerator_impl.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/ArrayEnumerator.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/Document_impl.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/Document.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/FieldEnumerator_impl.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/FieldEnumerator.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/SchemaValidator.cpp
		${NFX_CORE_SOURCE_DIR}/serialization/json/SchemaValidator_impl.cpp
	)
endif()

# --- String components ---
if(NFX_CORE_WITH_STRING)
	list(APPEND PUBLIC_HEADERS
		# --- String processing headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/string/StringBuilderPool.h
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Splitter.h
		${NFX_CORE_INCLUDE_DIR}/nfx/string/Utils.h

		# --- String processing implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/string/StringBuilderPool.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/string/Splitter.inl
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/string/Utils.inl
	)
	list(APPEND PRIVATE_HEADERS
		# --- String processing private headers ---
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBuffer_impl.h
	)
	list(APPEND PRIVATE_SOURCES
		# --- String processing source files ---
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBuffer_impl.cpp
		${NFX_CORE_SOURCE_DIR}/string/DynamicStringBufferPool.cpp
		${NFX_CORE_SOURCE_DIR}/string/StringBuilderPool.cpp
	)
endif()

# --- Temporal components ---
if(NFX_CORE_WITH_TIME)
	list(APPEND PUBLIC_HEADERS
		# --- Time handling headers ---
		${NFX_CORE_INCLUDE_DIR}/nfx/time/constants/DateTimeConstants.h
		${NFX_CORE_INCLUDE_DIR}/nfx/time/DateTime.h

		# --- Time handling implementations ---
		${NFX_CORE_INCLUDE_DIR}/nfx/detail/time/DateTime.inl
	)
	list(APPEND PRIVATE_SOURCES
		# --- Time handling source files ---
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
if(NFX_CORE_WITH_JSON)
    message(STATUS "  - Serialization (JSON Document)")
endif()
if(NFX_CORE_WITH_TIME)
	message(STATUS "  - Time utilities (DateTime, DateTimeOffset, TimeSpan)")
endif()


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

	if(NFX_CORE_WITH_JSON)
		target_link_libraries(${target_name} PRIVATE
			nlohmann_json::nlohmann_json
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
