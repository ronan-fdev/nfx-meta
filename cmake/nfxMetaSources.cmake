#==============================================================================
# NFX_META - CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

# --- Always include core headers ---
set(PUBLIC_HEADERS
	${NFX_META_INCLUDE_DIR}/nfx/config.h
)

set(PRIVATE_SOURCES)

# --- Container components ---
if(NFX_META_WITH_CONTAINERS)
	list(APPEND PUBLIC_HEADERS
		# --- Container functors ---
		${NFX_META_INCLUDE_DIR}/nfx/containers/functors/HashMapHashFunctor.h
		${NFX_META_INCLUDE_DIR}/nfx/containers/functors/StringFunctors.h

		# --- Container headers ---
		${NFX_META_INCLUDE_DIR}/nfx/containers/ChdHashMap.h
		${NFX_META_INCLUDE_DIR}/nfx/containers/HashMap.h
		${NFX_META_INCLUDE_DIR}/nfx/containers/StringMap.h
		${NFX_META_INCLUDE_DIR}/nfx/containers/StringSet.h

		# --- Container functors implementations ---
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/functors/HashMapHashFunctor.inl
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/functors/StringFunctors.inl

		# --- Container inline implementations ---
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/ChdHashMap.inl
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/HashMap.inl
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/StringMap.inl
		${NFX_META_INCLUDE_DIR}/nfx/detail/containers/StringSet.inl
	)
endif()

# --- Serialization components ---
if(NFX_META_WITH_JSON)
	list(APPEND PUBLIC_HEADERS
		# --- JSON serialization headers ---
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/ArrayEnumerator.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/Document.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/FieldEnumerator.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/SchemaValidator.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/SerializationTraits.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/SerializationTraits.h
		${NFX_META_INCLUDE_DIR}/nfx/serialization/json/Serializer.h

		# --- JSON serialization implementations ---
		${NFX_META_INCLUDE_DIR}/nfx/detail/serialization/json/Serializer.inl
	)

	list(APPEND PRIVATE_HEADERS
		# --- JSON serialization private headers ---
		${NFX_META_SOURCE_DIR}/serialization/json/ArrayEnumerator_impl.h
		${NFX_META_SOURCE_DIR}/serialization/json/Document_impl.h
		${NFX_META_SOURCE_DIR}/serialization/json/FieldEnumerator_impl.h
		${NFX_META_SOURCE_DIR}/serialization/json/SchemaValidator_impl.h
	)
	list(APPEND PRIVATE_SOURCES
		# --- JSON serialization implementations ---
		${NFX_META_SOURCE_DIR}/serialization/json/ArrayEnumerator_impl.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/ArrayEnumerator.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/Document_impl.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/Document.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/FieldEnumerator_impl.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/FieldEnumerator.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/SchemaValidator.cpp
		${NFX_META_SOURCE_DIR}/serialization/json/SchemaValidator_impl.cpp
	)
endif()

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Create shared library if requested ---
if(NFX_META_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${NFX_META_BUILD_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${NFX_META_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(NFX_META_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static-${PROJECT_VERSION}
		ARCHIVE_OUTPUT_DIRECTORY ${NFX_META_BUILD_DIR}/lib
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
			$<BUILD_INTERFACE:${NFX_META_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${NFX_META_SOURCE_DIR}
	)

	# --- External dependencies ---
	if(NFX_META_WITH_JSON)
		if(DEFINED nlohmann_json_SOURCE_DIR)
			target_include_directories(${target_name} PRIVATE ${nlohmann_json_SOURCE_DIR}/include)
		endif()
	endif()

	# --- Link external component libraries ---
	if(NFX_META_WITH_TIME)
		if(TARGET nfx-datetime::static)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-datetime::static>)
		elseif(TARGET nfx-datetime::nfx-datetime)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-datetime::nfx-datetime>)
		endif()
	endif()

	if(NFX_META_WITH_STRING)
		if(TARGET nfx-stringutils::nfx-stringutils)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-stringutils::nfx-stringutils>)
		endif()
		if(TARGET nfx-stringbuilderpool::static)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-stringbuilderpool::static>)
		elseif(TARGET nfx-stringbuilderpool::nfx-stringbuilderpool)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-stringbuilderpool::nfx-stringbuilderpool>)
		endif()
	endif()

	if(NFX_META_WITH_DATATYPES)
		if(TARGET nfx-datatypes::static)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-datatypes::static>)
		elseif(TARGET nfx-datatypes::nfx-datatypes)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-datatypes::nfx-datatypes>)
		endif()
	endif()

	if(NFX_META_WITH_MEMORY)
		if(TARGET nfx-lrucache::nfx-lrucache)
			target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-lrucache::nfx-lrucache>)
		endif()
	endif()

	# --- Link nfx-core ---
	if(TARGET nfx-core::nfx-core)
		target_link_libraries(${target_name} PUBLIC $<BUILD_INTERFACE:nfx-core::nfx-core>)
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
if(NFX_META_BUILD_SHARED)
	configure_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfxMetaVersion.rc.in
			${CMAKE_BINARY_DIR}/nfxMetaVersion.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/nfxMetaVersion.rc)
	endif()
endif()

if(NFX_META_BUILD_STATIC)
	configure_target(${PROJECT_NAME}-static)
endif()
