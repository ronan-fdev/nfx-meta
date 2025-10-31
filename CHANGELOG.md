# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- NIL

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL

## [0.7.0] - 2025-10-31

### Changed

#### Major Architecture Restructuring (Breaking Change)

- **Project Rename**: `nfx-core` → `nfx-meta` for better semantic clarity

  - Repository renamed to reflect its role as a meta-library aggregating specialized NFX components
  - All CMake variables renamed: `NFX_CORE_*` → `NFX_META_*`
  - All C++ macros renamed: `NFX_CORE_INLINE` → `NFX_META_INLINE`, `NFX_CORE_NO_UNIQUE_ADDRESS` → `NFX_META_NO_UNIQUE_ADDRESS`, etc.

- **Core Utilities Externalization**: Moved low-level infrastructure to dedicated `nfx-core` repository

  - Extracted core hashing functions (`CPU.h`, `Hashing.h`, `Hashing.inl`) to standalone `nfx-core` library
  - `nfx-core` now serves as foundational dependency providing CPU feature detection and hash algorithms
  - External dependency: `https://github.com/ronan-fdev/nfx-core.git` (main branch)
  - Hash constants moved to `core::hashing::constants::*` namespace for better organization

**BREAKING CHANGES**:

- Project name changed from `nfx-core` to `nfx-meta` - update all package references
- All `NFX_CORE_*` CMake variables renamed to `NFX_META_*`
- All `NFX_CORE_*` C++ macros renamed to `NFX_META_*`
- Core hashing utilities moved to external `nfx-core` dependency - update includes from `nfx/core/` if used directly
- Hash constants now accessed via `core::hashing::constants::*` namespace

## [0.6.0] - 2025-10-31

### Changed

- **External Dependency Updates**: Updated external library dependencies to latest versions
  - `nfx-stringuti  - String builder pool now available as optional external dependency via `NFX_CORE_WITH_STRING` option
  - External dependency: `https://github.com/ronan-fdev/nfx-stringbuilderpool.git` (v1.0.0)

## [0.5.9] - 2025-10-30

### Changed

- **External Dependency Updates**: Updated external library dependencies to latest versions
  - `nfx-stringutils`: 1.0.3 → 1.1.0 (now includes `Splitter` functionality)
- **Dependency Cleanup**: Removed obsolete `nfx-stringsplitter` dependency

## [0.5.8] - 2025-10-30

### Changed

- **External Dependency Updates**: Updated external library dependencies to latest versions
  - `nfx-stringutils`: 1.0.2 → 1.0.3

## [0.5.7] - 2025-10-30

### Changed

- **StringBuilderLease API Improvement**: Renamed `builder()` method to `create()` for better semantic clarity
  - `StringBuilderLease::builder()` → `StringBuilderLease::create()`
  - More accurately reflects that the method creates a new `StringBuilder` wrapper instance
  - Updated all samples, tests, and benchmarks to use the new method name

## [0.5.6] - 2025-10-30

### Changed

- **External Dependency Updates**: Updated external library dependencies to latest versions
  - `nfx-datetime`: 1.0.0 → 1.0.1
  - `nfx-stringutils`: 1.0.1 → 1.0.2
  - `nfx-stringsplitter`: 1.0.0 → 1.0.2
  - `nfx-datatypes`: 1.0.1 → 1.0.2
  - `nfx-lrucache`: 1.0.6 → 1.0.7

## [0.5.5] - 2025-10-29

### Changed

- **External Dependency Updates**: Updated external library dependencies to latest versions
  - `nfx-stringutils`: 1.0.0 → 1.0.1
  - `nfx-datatypes`: 1.0.0 → 1.0.1

## [0.5.4] - 2025-10-27

### Changed

#### Memory Module Externalization

- **Memory Module Migration**: Moved LRU Cache to external dependency `nfx-lrucache`
  - Memory component now available as optional external dependency via `NFX_CORE_WITH_MEMORY` option
  - External dependency: `https://github.com/ronan-fdev/nfx-lrucache.git` (v1.0.5)
  - Removed internal implementation files:
    - `include/nfx/memory/LruCache.h`
    - `include/nfx/detail/memory/LruCache.inl`
    - `samples/memory/Sample_LruCache.cpp`
    - `test/memory/TESTS_LruCache.cpp`

## [0.5.3] - 2025-10-26

### Changed

#### String Splitter Module Externalization

- **String Splitter Module Split**: Separated `Splitter` from `nfx-stringutils` to dedicated `nfx-stringsplitter` repository
  - String splitter now available as separate optional dependency
  - External dependency: `https://github.com/ronan-fdev/nfx-stringsplitter.git` (v1.0.0)
  - Enables granular dependency management for string processing components

## [0.5.2] - 2025-10-26

### Changed

#### Datatypes Module Externalization

- **Datatypes Module Migration**: Moved datatypes (`Int128`, `Decimal`) to external dependency `nfx-datatypes`
  - Datatypes now available as optional external dependency via `NFX_CORE_WITH_DATATYPES` option
  - External dependency: `https://github.com/ronan-fdev/nfx-datatypes.git` (v1.0.0)
  - Removed internal implementation files:
    - `include/nfx/datatypes/Int128.h`
    - `include/nfx/datatypes/Decimal.h`
    - `include/nfx/datatypes/constants/Int128Constants.h`
    - `include/nfx/datatypes/constants/DecimalConstants.h`
    - `include/nfx/detail/datatypes/Int128.inl`
    - `include/nfx/detail/datatypes/Decimal.inl`
    - `src/datatypes/Int128.cpp`
    - `src/datatypes/Decimal.cpp`
    - `samples/datatypes/Sample_Datatypes.cpp`
    - `test/datatypes/TESTS_Int128.cpp`
    - `test/datatypes/TESTS_Decimal.cpp`

## [0.5.1] - 2025-10-25

### Changed

#### String Utilities Externalization

- **String Utilities Module Migration**: Moved string utilities (`Utils.h`, `Splitter.h`) to external dependency `nfx-stringutils`
  - String utility functions now available as optional external dependency via `NFX_CORE_WITH_STRING` option
  - External dependency: `https://github.com/ronan-fdev/nfx-stringutils.git` (v1.0.0)
  - Removed internal implementation files:
    - `include/nfx/string/Utils.h`
    - `include/nfx/string/Splitter.h`
    - `include/nfx/detail/string/Utils.inl`
    - `include/nfx/detail/string/Splitter.inl`
    - `samples/string/Sample_StringUtils.cpp`
    - `samples/string/Sample_StringSplitter.cpp`
    - `test/string/TESTS_StringUtils.cpp`
    - `test/string/TESTS_StringSplitter.cpp`
    - `benchmark/string/BM_StringUtilities.cpp`
    - `benchmark/string/BM_Splitter.cpp`

## [0.5.0] - 2025-10-25

### Changed

#### DateTime Module Externalization

- **DateTime Module Migration**: Moved the entire `nfx::time` namespace to external dependency `nfx-datetime`
  - DateTime functionality now available as optional external dependency via `NFX_CORE_WITH_TIME` option
  - External dependency: `https://github.com/ronan-fdev/nfx-datetime.git` (v1.0.0)
  - Removed internal implementation files:
    - `include/nfx/time/DateTime.h`
    - `include/nfx/time/constants/DateTimeConstants.h`
    - `include/nfx/detail/time/DateTime.inl`
    - `src/time/DateTime.cpp`
    - `samples/time/Sample_DateTime.cpp`
    - `test/time/TESTS_DateTime.cpp`

#### Configuration and Build System Updates

- Updated project configuration and build system files
- Enhanced JSON serialization traits and test coverage
- Updated sample applications to remove DateTime dependencies
- Refined CMake configuration and dependency management

## [0.4.1] - 2025-10-18

### Added

#### JSON Document API Enhancement

- **Complete Integer Type Support**: Added support for all integer types (`int8_t`, `int16_t`, `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`) in template constraints and implementations
- **Floating-Point Type Support**: Added native `float` type support alongside existing `double` support across all JSON Document APIs

## [0.4.0] - 2025-10-17

> **WORK IN PROGRESS**: This release introduces a major API modernization that is still undergoing refinement, stabilization, and documentation updates.

### Changed

#### JSON Document API Modernization (Breaking Change)

- **Unified Template API**: Complete replacement of individual typed methods with generic template interface

  - **Before**: `getString(path)`, `setInt(path, value)`, `addToArray(path, value)`, `hasStringByPointer(pointer)`
  - **After**: `get<std::string>(path)`, `set<int>(path, value)`, `array.add(value)`, `is<std::string>(path)`
  - **Benefits**: Type safety, reduced API surface, better performance through dual overload pattern

- **Dual Overload Pattern**: Optimal performance with both copy and move semantics

  - **Copy Semantics**: `set<T>(path, const T& value)` for lvalue references and const objects
  - **Move Semantics**: `set<T>(path, T&& value)` for temporaries and moved objects
  - **Performance**: Automatic selection of optimal operation based on value category

- **Document::Array and Document::Object Classes**: New nested classes for improved type safety
  - **Array Operations**: `array.add<T>(value)`, `array.set<T>(index, value)`, `array.get<T>(index)`
  - **Object Operations**: `object.set<T>(key, value)`, `object.get<T>(key)`, `object.hasField(key)`
  - **Fluent Interface**: Direct manipulation of nested structures without path navigation

#### C++20 Template Constraints

- **Concepts Integration**: Template methods now use `requires` clauses for better compile-time validation
  - **Supported Types**: `std::string`, `std::string_view`, `char`, `bool`, `int`, `int32_t`, `int64_t`, `double`, `Document`, `Document::Object`, `Document::Array`
  - **Compile-Time Safety**: Invalid type usage caught at compilation with clear error messages

#### API Simplification

- **Factory Method Removal**: Eliminated verbose factory methods in favor of template-based creation
  - **Removed**: `Document::createObject()`, `Document::createArray()`
  - **Replaced**: `doc.set<Document::Object>(path)`, `doc.set<Document::Array>(path)`
  - **Benefits**: Consistent API patterns, reduced cognitive load

#### Serializer Framework Updates

- **Template API Adoption**: Updated `Serializer<T>` implementation to use new unified template interface
  - **Internal Changes**: All `setBoolByPointer`, `hasStringByPointer` calls replaced with `set<bool>`, `is<std::string>`
  - **Backward Compatibility**: Public `Serializer<T>` API remains unchanged
  - **Performance**: Benefits from improved dual overload architecture in Document layer

## [0.3.5] - 2025-10-12

### Changed

- **JSON Document**: Character (type char) methods integrated with standard type API

## [0.3.4] - 2025-10-11

### Changed

- **JSON Document Field Ordering**: Switched from `nlohmann::json` to `nlohmann::ordered_json` for deterministic field ordering

## [0.3.3] - 2025-10-10

### Changed

- **StringBuilderPool Architecture Modernization**: Complete removal of PIMPL pattern

### Removed

- **fmt Library Dependency**: Removed fmt library dependency from StringBuilderPool implementation

## [0.3.2] - 2025-10-09

### Changed

- **CMake Integration Improvements**: Enhanced CMake variable caching and dependency management
  - Version variables now use `CACHE INTERNAL` for better CMake subproject integration
  - Package config template now exports version information for downstream projects
  - FetchContent base directory relocated from project root to build directory for improved build isolation

## [0.3.1] - 2025-10-07

### Changed

- **CMake Build System**: Changed fallback preference from shared to static libraries for simpler deployment
- **Build Configuration**: Enhanced multi-config generator support with Release mode default for optimal performance

## [0.3.0] - 2025-10-05

### Added

- **Cross-Platform Packaging**: CPack configuration for TGZ, ZIP, DEB, RPM, and NSIS packages

  - Linux packages include automatic dependency resolution
  - Windows NSIS installer
  - Component-based installation (Runtime, Development, Documentation)
  - CMake package configuration with `find_package()` support

- **Build System Enhancements**:
  - Standalone vs submodule detection with automatic feature control
  - Git submodule integration with workflow documentation
  - New `NFX_CORE_DEVELOPER_MODE` option for library-focused development

### Changed

- **CMake Module Reorganization** (Breaking):

  - `nfxCoreConfig.cmake` renamed to `nfxCoreBuildConfig.cmake`
  - Added `nfxCoreInstall.cmake` and `nfxCorePackaging.cmake`
  - Unified target naming across all build modes

- **Integration Methods**: Updated FetchContent, find_package, and new submodule integration documentation
- **Documentation**: Updated README with integration examples

### Fixed

- Enhanced dependency handling for nlohmann/json and fmt libraries

## [0.2.1] - 2025-10-05

### Added

- **JSON Document API Enhancements:**
  - **Generic Document Methods:** Type-agnostic operations for flexible JSON manipulation
    - `getDocument(path)` - Generic getter that returns any JSON structure as a Document (primitives, objects, arrays)
    - `setDocument(path, document)` - Generic setter for Document objects with full dot notation support
    - `addToArray(path, document)` - Generic array append method for Document objects with nested structure support
  - **Character Utility Methods:** Complete convenience API for single-character JSON operations
    - **Basic Operations:** `setChar(path, char)` and `getChar(path)` with `std::optional<char>` return type
    - **JSON Pointer Support:** `setCharByPointer(pointer, char)` and `getCharByPointer(pointer)` for RFC 6901 compliance
    - **Array Operations:** `addCharToArray(path, char)` and `getArrayElementChar(path, index)` for character arrays
    - **Type Validation:** `isChar(path)` and `hasCharByPointer(pointer)` for character detection and validation
    - **Implementation Details:** Characters stored as single-character JSON strings with proper type validation

## [0.2.0] - 2025-10-04

### Added

- **JSON Serialization Framework:**

  - **Document Processing:**
    - `Document` class for JSON parsing, manipulation, and serialization with RFC 6901 JSON Pointer support
    - `ArrayEnumerator` and `FieldEnumerator` for efficient JSON traversal and iteration
    - Integrated [nlohmann/json](https://github.com/nlohmann/json) with automatic dependency resolution
  - **Type System:**
    - `Serializer<T>` templated serialization with compile-time reflection and automatic type detection
    - `SerializationTraits<T>` extensible framework with specializations for nfx-core types (Int128, Decimal, DateTime, etc.)
    - Built-in support for containers, POD types, and custom objects with reflection capabilities
  - **Validation & Quality:**
    - `SchemaValidator` with JSON Schema Draft 7 compliance and detailed error reporting
    - `ValidationResult` and `ValidationError` classes for comprehensive validation feedback
  - **Build Integration:**
    - Conditional compilation via `NFX_CORE_WITH_JSON` CMake option
    - Comprehensive samples and test coverage for all serialization components

- **CPU Feature Detection (Breaking Change):**

  - Added `nfx::core::cpu` module for runtime detection of CPU instruction set extensions.
  - Provides `hasSSE42Support()`, `hasAVXSupport()`, and `hasAVX2Support()`.
  - Enables optimized algorithm selection based on available CPU features.

- **CMake & Build System Improvements:**
  - Modular CMake sources for all major components (containers, datatypes, memory, string, time, serialization).
  - Conditional inclusion of sources, tests, samples, and benchmarks based on feature flags.

### Changed

- **Hashing/Core Utilities:**
  - Moved core hash utilities to `nfx/core/Hashing.h` (was `nfx/core/hashing/Hash.h`).
  - CPU feature detection moved to dedicated `nfx/core/CPU.h` module.
- **Header File Reorganization (Breaking Change):**
  - `nfx/core/hashing/Hash.h` moved to `nfx/core/Hash.h` - update include paths accordingly
  - This change affects any code that directly includes the hashing utilities header

### Fixed

- **Thread-related crash in `Thread Safety Demonstration section` of `Sample_LruCache.cpp` :) :**
  - Fixed memory corruption caused by incorrect vector initialization in threading code
  - Changed `std::vector<std::size_t> hits_per_thread{ num_threads, 0 };` to `std::vector<std::size_t> hits_per_thread(num_threads, 0);`

## [0.1.9] - 2025-10-01

### Added

- **64-bit Hash Combination Function**: New `combine(size_t, size_t)` overload for 64-bit hash composition

## [0.1.8] - 2025-09-29

### Added

#### Cross-Platform CPU Architecture Detection

- **Dynamic CPU Feature Detection**: Runtime CPU capability detection in CMake configuration
  - **Sandy Bridge Support**: Automatic detection and fallback for older CPUs (i7-2xxx series, 2011-2012) that support AVX but not AVX2/FMA
  - **Haswell+ Optimization**: Enables AVX2/FMA optimizations on supported CPUs (i7-4xxx series, 2013+)

### Changed

#### Build System Architecture Modernization

- **Conditional Compiler Flags**:
  - **MSVC**: Conditional `/arch:AVX2` vs `/arch:SSE2` based on CPU support
  - **GCC/Clang**: Conditional `-mavx2` and `-mfma` flags only when CPU supports AVX2
  - **Fallback Safety**: Automatic degradation to SSE4.2 baseline on unsupported hardware

### Fixed

#### High-Precision Arithmetic Edge Case Handling

- **Int128 Minimum Value Overflow Protection**: Fixed critical two's complement arithmetic edge case
  - **toString() Fix**: Special case handling in `Int128::toString()` prevents overflow when calling `abs()` on minimum value
  - **Decimal Constructor Fix**: Enhanced `Decimal(const Int128&)` constructor to handle minimum value without overflow

#### Test Suite Reliability Improvements

- **DateTime Test Precision**: Fixed floating-point arithmetic precision issue

## [0.1.7] - 2025-09-27

### Added

- **Hash Combination Function**: New `combine()` function for composing hash values

### Changed

- **Hash Infrastructure Robustness**: Enhanced cross-platform compilation safety

  - **SSE4.2 Headers**: Added proper `#include <nmmintrin.h>` for CRC32 intrinsics
  - **CRC32 Safety**: Added `static_assert` fallback to prevent silent failures on unsupported platforms
  - **Thread Optimization**: Removed unnecessary `thread_local` from CPU feature detection (global hardware capability)
  - **Code Consistency**: Centralized FNV-1a logic using template functions to eliminate duplication

### Fixed

- **FNV-1a Algorithm Correctness**: Fixed critical algorithm implementation bug

  - **Before**: `return (ch ^ hash) * FnvPrime;` (incorrect order)
  - **After**: `hash ^= ch; hash *= FnvPrime; return hash;` (correct FNV-1a specification)

## [0.1.6] - 2025-09-27

### Added

- **ChdHashMap, HashMap, StringMap ,StringSet**: Added extensive ASCII art diagrams showing Robin Hood hashing structure

### Changed

- **HashMap Template Configurability**: HashMap is now templated with configurable FNV hash constants

  - **Template Signature**: `template<typename TKey, typename TValue, uint32_t FnvOffsetBasis = DEFAULT_FNV_OFFSET_BASIS, uint32_t FnvPrime = DEFAULT_FNV_PRIME>`
  - **Ecosystem Consistency**: HashMap now uses the same template pattern as ChdHashMap for unified hash behavior across all containers
  - **External Customization**: Projects can specify custom FNV constants: `HashMap<std::string, int, 0x12345678, 0x9ABCDEF0>`

- **HashMapHashFunctor Template Infrastructure**: HashMapHashFunctor is now templated for configurable hash behavior

  - **Template Signature**: `template<uint32_t FnvOffsetBasis, uint32_t FnvPrime> struct HashMapHash`
  - **Dual-Path Hashing**: SSE4.2 CRC32 with FNV-1a fallback using configurable constants
  - **Zero-Copy String Operations**: Heterogeneous lookup support with `std::string`, `std::string_view`, and `const char*`

## [0.1.5] - 2025-09-27

### Added

#### Enhanced Hash Algorithm Configuration System

- **Configurable FNV Hash Constants**: ChdHashMap now supports custom FNV-1a hash parameters for external project customization

  - Template parameters `FnvOffsetBasis` and `FnvPrime` with academic naming and default values
  - `DEFAULT_FNV_OFFSET_BASIS = 0x811C9DC5` and `DEFAULT_FNV_PRIME = 0x01000193` constants in Hashing.h

- **Comprehensive Hash Constant Library**: Extended Hashing.h with academically-named hashing constants

  - **FNV-1a Constants**: `DEFAULT_FNV_OFFSET_BASIS`, `DEFAULT_FNV_PRIME` with Fowler-Noll-Vo algorithm attribution
  - **Integer Hashing Constants**: `DEFAULT_INTEGER_HASH_32` (Donald Knuth), `DEFAULT_INTEGER_HASH_64_C1/C2` (Thomas Wang)
  - **Generic Mixing**: `DEFAULT_HASH_MIX_64` universal hashing constant for bit avalanche mixing

- **Template-Based Hash Function Infrastructure**: Enhanced core hashing with configurable parameters
  - `template<uint32_t FnvPrime> fnv1a()` - Configurable FNV-1a single-step hashing
  - `template<uint32_t FnvOffsetBasis, uint32_t FnvPrime> hashStringView()` - Parameterized string hashing

### Changed

#### ChdHashMap Architecture Modernization

- **Template Parameter Enhancement**: ChdHashMap signature updated to support configurable hash constants

  - From: `template<typename TValue>`
  - To: `template<typename TValue, uint32_t FnvOffsetBasis = DEFAULT_FNV_OFFSET_BASIS, uint32_t FnvPrime = DEFAULT_FNV_PRIME>`
  - External projects can now customize hash behavior: `ChdHashMap<int, 0x12345678, 0x9ABCDEF0>`

- **Improved Code Organization and Encapsulation**

  - **ThrowHelper Encapsulation**: Moved ThrowHelper class from global anonymous namespace to private nested class
  - **Exception Class Encapsulation**: Moved KeyNotFoundException and InvalidOperationException to nested classes within ChdHashMap
  - **Consistent Access Patterns**: All exception classes now accessed as `ChdHashMap<T>::KeyNotFoundException`

## [0.1.4] - 2025-09-26

### Fixed

- **String Splitter Safety Enhancements**: Eliminated unsafe pointer arithmetic in string splitting operations

#### Code Cleanup and Maintenance

- **DateTime Chrono Function Removal**: Cleaned up unused chrono interoperability validation function

## [0.1.3] - 2025-09-26

### Added

#### Enhanced High-Precision Arithmetic Integration

- **Int128↔Decimal Cross-Type Conversions**: Bidirectional constructors enabling seamless conversion between 128-bit integers and 96-bit precision decimals

  - `Int128(const Decimal&)` - Truncates fractional parts
  - `Decimal(const Int128&)` - Converts with overflow clamping to Decimal's 96-bit capacity (2^96 - 1)

- **Float Constructor for Int128**: `Int128(float)` and `Int128(double)` constructors with C++ standard compliance

  - Truncation toward zero for fractional parts (matches `static_cast<int>(float)` behavior)
  - NaN and infinity values converted to zero
  - Overflow protection with range clamping

- **Float Constructor for Decimal**: `Decimal(float)` constructor with IEEE 754-2008 compatibility
  - NaN and infinity detection with zero conversion
  - Precision-aware conversion with ~6-7 significant digit accuracy

#### Comprehensive Cross-Type Comparison Operators

- **Decimal Mixed-Type Comparisons**: Complete comparison operator suite (`==`, `!=`, `<`, `<=`, `>`, `>=`)

  - Built-in integer types: `int32_t`, `int64_t`, `uint64_t`
  - Built-in floating-point types: `float`, `double`
  - High-precision type: `Int128`
  - Precision-aware floating-point comparisons with documented limitations

- **Int128 Mixed-Type Comparisons**: Full comparison operator coverage
  - Built-in integer types: `int`, `int64_t`, `uint64_t`
  - Built-in floating-point types: `float`, `double`
  - High-precision type: `Decimal`
  - Special handling for unsigned comparisons and sign differences

#### Extensive Test Coverage Enhancement

- **Int128 Construction Tests**: Comprehensive test suite for new constructors

  - Float/double conversion with truncation behavior verification
  - NaN/infinity handling validation
  - Overflow and underflow boundary testing
  - Decimal-to-Int128 conversion with fractional truncation

- **Cross-Type Comparison Tests**: Exhaustive comparison testing
  - Mixed-type arithmetic comparisons between Int128↔Decimal
  - Boundary value testing with large numbers (28-digit precision)
  - Sign handling and fractional part detection
  - Symmetry verification for bidirectional comparisons

### Fixed

#### Cross-Platform String Conversion Consistency

- **Decimal toString() MSVC Compatibility**: Fixed platform-specific differences in high-precision decimal string conversion
  - **Root Cause**: MSVC code path incorrectly used 64-bit modulo operation (`mantissa.toLow() % 10`) instead of full 128-bit arithmetic
  - **Impact**: Cross-platform test failures with different decimal representations between GCC and MSVC builds

#### API Consistency Improvements

- **Decimal Comparison Const-Correctness**: Fixed missing `const` qualifiers on comparison operators
- **String Parsing Normalization**: Enhanced Decimal string parsing with automatic trailing zero removal

## [0.1.2] - 2025-09-24

### Changed

#### Dependency Updates

- **fmt Library**: Updated fmt dependency from version 11.2.0 to 12.0.0

## [0.1.1] - 2025-09-23

### Security

- **GITHUB_TOKEN Permissions**: Added explicit minimal permissions to all CI workflows

## [0.1.0] - 2025-09-23

### Added

#### CI/CD Performance Optimization

- **Build Caching**: Added caching across all GitHub Actions workflows

### Changed

#### Major Architecture Reorganization

- **Implementation File Restructure**: Moved all `.inl` files to organized `detail/` directory structure

## [0.0.10] - 2025-09-23

### Added

#### Build System & Infrastructure Enhancements

- **Dependency Management**: Enhanced CMake with system package detection using `find_package()` for fmt, GoogleTest, and Google Benchmark, automatically falling back to FetchContent when system packages are unavailable

#### Documentation Enhancements

- **Enhanced API Documentation**: Comprehensive inline documentation improvements for `Int128` and `Decimal`.

### Changed

#### Build System Architecture

- **Modernized Dependency Management**: Hybrid system detection approach

  - Prioritizes system-installed packages for faster builds
  - Falls back to FetchContent for missing dependencies
  - Better integration with package managers and CI environments

- **Artifact Path Standardization**: Simplified CI/CD artifact collection
  - GitHub Actions workflows updated to use standardized paths
  - Changed from version-specific paths (`build/nfx-core-*/`) to standard paths (`build/`)
  - Improved artifact consistency across platforms

### Fixed

#### Build System Reliability

- **Circular Dependency Resolution**: Fixed benchmark target naming conflict

  - Resolved conflict between Google Benchmark's `benchmark` target and convenience target
  - Renamed convenience target to `benchmarks` (plural) to avoid collision
  - Eliminates CMake configuration errors and build failures

- **Cross-Platform Linking**: Comprehensive shared library integration
  - All executables now properly link to and use shared library implementations
  - Windows DLL deployment ensures runtime dependency satisfaction
  - Consistent behavior between static and shared library builds

## [0.0.9] - 2025-09-22

### Added

- **Documentation Build Support**: New `NFX_CORE_BUILD_DOCUMENTATION` CMake option enables Doxygen documentation generation
- **Continuous Integration**: GitHub Actions CI/CD workflows for automated testing across Windows MSVC, Linux GCC, and Linux Clang platforms

## [0.0.8] - 2025-09-21

### Added

#### Comprehensive Performance Benchmark Suite

- **Container Benchmarks**: Complete benchmark coverage for all container components
  - `BM_ChdHashMap.cpp` - Perfect hashing performance validation
  - `BM_HashMap.cpp` - Robin Hood hashing benchmarks with heterogeneous operations
  - `BM_StringMap.cpp` - String-optimized mapping performance analysis
  - `BM_StringSet.cpp` - String collection benchmarks with zero-allocation lookups
- **Core Performance Benchmarks**: Low-level component benchmarking
  - `BM_Hashing.cpp` - Hash function performance (FNV-1a, CRC32, vs std::hash)
- **String Processing Benchmarks**: Enhanced string component validation
  - `BM_StringBuilderPool.cpp` - Pooled string building vs standard alternatives
  - `BM_StringUtilities.cpp` - Character classification and string operation benchmarks
  - `BM_Splitter.cpp` - Enhanced string splitting with manual vs Splitter vs splitView() comparisons

#### Core Hashing Framework

- **Centralized Hash Utilities**: New `nfx/core/hashing/Hashing.h` module
  - Unified hash function interface
  - Hardware-accelerated hashing (SSE4.2 CRC32)
  - Cross-platform hash compatibility

#### Container Functor Reorganization

- **HashMapHashFunctor**: Dedicated hash functor system for HashMap components
- **Enhanced StringFunctors**: Improved string-specific functors with better organization
- **Modular Design**: Better separation of concerns for container hash and equality operations

### Changed

#### API Modernization

- **MemoryCache → LruCache**: Renamed for clarity and better API semantics
  - `nfx::memory::MemoryCache` → `nfx::memory::LruCache`
  - `MemoryCacheOptions` → `LruCacheOptions`

## [0.0.7] - 2025-09-20

### Added

#### Google Benchmark Integration

- **Performance Measurement Framework**: Integrated Google Benchmark library
- **String Splitter Benchmark Suite**: Comprehensive benchmark comparing _manual_ vs `Splitter` vs `splitView()` performance
- **Real-world Test Scenarios**: CSV parsing, path splitting, and configuration data processing benchmarks

## [0.0.6] - 2025-09-20

### Changed

#### String Processing API Improvements

- **StringViewSplitter → Splitter**: Renamed `StringViewSplitter` class to `Splitter` for cleaner API design within `nfx::string` namespace
  - **Templated API**: Replaced multiple constructor/factory overloads with unified template-based approach
    - `template<typename StringType> explicit Splitter(StringType&& str, char delimiter)` - single templated constructor
    - `template<typename StringType> splitView(StringType&& str, char delimiter)` - unified factory function
    - Perfect forwarding with automatic conversion to `std::string_view` for zero overhead
    - Supports any string-convertible type: `std::string`, `std::string_view`, `const char*`, etc.
  - **Performance Improvements**: Updated iterator implementation with optimized start/end position tracking
  - **Complete Ecosystem Update**: All samples, tests, and documentation updated to reflect new API
  - Maintains full backward compatibility at the functional level (only class name and template improvements)

## [0.0.5] - 2025-09-20

### Fixed

#### HashMap API Consistency

- **Unified tryGetValue API**: Updated `HashMap::tryGetValue()` to match `ChdHashMap` pattern for consistent container APIs
  - Method signature changed from `const TValue* tryGetValue(const KeyType& key) const noexcept` to `bool tryGetValue(const KeyType& key, TValue*& outValue) noexcept`
  - Provides consistent API experience across all NFX container types
  - Enables mutable value access through `TValue*&` reference parameter by design
  - Maintains zero-copy heterogeneous lookup performance (string_view -> string)
  - Updated all samples and tests to use new unified API pattern
  - Eliminates API inconsistencies between similar container implementations

## [0.0.4] - 2025-09-18

### Fixed

#### ChdHashMap Mutable Value Access

- **Restored intended mutable access**: Fixed `ChdHashMap::tryGetValue()` to properly support mutable value modification as designed
  - Method signature corrected from `bool tryGetValue(std::string_view key, const TValue*& outValue) const noexcept` to `bool tryGetValue(std::string_view key, TValue*& outValue) noexcept`
  - By design, `TValue*&` parameter is a reference to a pointer that enables in-place value modification
  - Fixes API to work as originally intended, allowing direct modification of values without removal and re-insertion
  - Maintains O(1) lookup performance while restoring intended mutable access capability
  - Updated `at()` method signature from `const TValue& at(std::string_view key) const` to `const TValue& at(std::string_view key)` to support the corrected API

## [0.0.3] - 2025-09-14

### Added

#### Enhanced String Validation

- **isAllDigits()**: New string validation function to check if string contains only ASCII digits

#### URI Character Classification

- **isURIReserved()**: RFC 3986 Section 2.2 compliant reserved character validation
  - Character variant: `isURIReserved(char)` - validates single characters against 18 reserved characters
  - String variant: `isURIReserved(std::string_view)` - validates all characters in string are reserved
  - Supports gen-delims (`: / ? # [ ] @`) and sub-delims (`! $ & ' ( ) * + , ; =`)
- **isURIUnreserved()**: RFC 3986 Section 2.3 compliant unreserved character validation
  - Character variant: `isURIUnreserved(char)` - validates ALPHA, DIGIT, and special chars (`- . _ ~`)
  - String variant: `isURIUnreserved(std::string_view)` - validates all characters in string are unreserved
  - Leverages existing `isAlphaNumeric()` function for optimal performance
  - All functions marked `constexpr` for compile-time evaluation

#### Decimal Arithmetic Enhancements

- **decimalPlacesCount()**: New Decimal utility method to count actual decimal places
  - Returns the number of significant decimal places (excludes trailing zeros)
  - Uses optimized Int128 arithmetic for accurate 96-bit mantissa processing
  - Cross-platform compatible with both native \_\_int128 and manual implementation
  - Zero values return 0, integers return 0, decimals return actual precision
  - Example: `Decimal("123.4500").decimalPlacesCount()` returns 2 (ignores trailing zeros)

## [0.0.2] - 2025-09-12

### Added

#### Enhanced String Processing

- **Character Classification Functions**: High-performance character classification utilities

  - `isWhitespace()`: Constexpr whitespace detection for standard whitespace characters
  - `isDigit()`: Fast ASCII digit detection using optimized bit manipulation
  - `isAlpha()`: ASCII alphabetic character detection for a-z and A-Z
  - `isAlphaNumeric()`: Combined alphanumeric detection for ASCII characters
  - All functions marked constexpr for compile-time evaluation

- **String Trimming Suite**: Zero-allocation and efficient trimming functions

  - `trimStart()`: Remove leading whitespace, returns std::string_view
  - `trimEnd()`: Remove trailing whitespace, returns std::string_view
  - `trim()`: Remove leading and trailing whitespace, returns std::string_view
  - All trimming functions preserve original string data through string_view

- **Enhanced String Validation**: Additional validation utilities
  - `isNullOrWhiteSpace()`: Comprehensive check for empty or whitespace-only strings
  - Support for all standard whitespace characters with optimized detection

## [0.0.1] - 2025-09-05

### Added

#### 🔢 High-Precision Datatypes

- **Int128**: Cross-platform 128-bit signed integer arithmetic
  - Native `__int128` support on GCC/Clang with MSVC fallback
  - Full arithmetic operators (+, -, \*, /, %)
  - String parsing and conversion support
  - Cross-platform consistency
- **Decimal**: 96-bit mantissa fixed-point decimal arithmetic
  - 28 significant decimal digits precision
  - Zero floating-point rounding errors
  - .NET System.Decimal compatibility
  - IEEE 754-2008 input validation
  - Banker's rounding implementation

#### 📦 Advanced Container System

- **ChdHashMap**: Perfect hash implementation using CHD algorithm
  - Derived from DNV Vista SDK C# implementation
  - Cross-platform hash compatibility
  - FNV-1a hashing with SSE4.2 optimizations
  - Configurable seed search parameters
- **HashMap**: Robin Hood hashing with bounded probe distances
  - Cache-friendly probe distance management
  - Template specialization for string optimization
  - Automatic load factor management
- **StringMap/StringSet**: Zero-copy heterogeneous string containers
  - `std::string_view` lookups without temporary allocations
  - Transparent hash and equality functors
  - Support for `std::string`, `std::string_view`, and `const char*`

#### 🔤 String Processing Infrastructure

- **StringBuilderPool**: Thread-safe pooled string building
  - RAII lease management with automatic return
  - Dynamic buffer resizing with capacity optimization
  - Iterator support for range-based operations
- **StringViewSplitter**: Zero-allocation string splitting
  - Iterator-based design for range-for loops
  - No heap allocations during tokenization
  - Optimized for CSV parsing and text processing
- **String Utils**: High-performance string utilities
  - Fast parsing using `std::charconv` (bool, int, double, long)
  - Case-insensitive comparison functions
  - Validation and conversion helpers
  - `constexpr` string operations where possible

#### 💾 Memory Management

- **MemoryCache**: Thread-safe LRU cache with expiration policies
  - Sliding expiration with configurable timeouts
  - Size limits with automatic eviction
  - Intrusive LRU list for cache-friendly performance
  - .NET IMemoryCache-inspired API design

#### 🛠️ Build System & Cross-Platform Support

- **CMake 3.20+**: Modern CMake configuration
  - Modular component compilation with feature flags
  - FetchContent integration for dependencies
  - Cross-platform compiler detection
- **Compiler Support**:
  - MSVC 2022+ (tested with Visual Studio 19.44+)
  - GCC 10+ with C++20 support
  - Clang 12+ with C++20 support
- **Platform Support**: Windows, Linux
- **Dependencies**: fmt library (header-only mode), GoogleTest (tests only)

#### 📚 Documentation & Testing

- Extensive code documentation with detailed header comments
- Professional README with usage examples
- Complete unit test suite with GoogleTest
- Sample applications demonstrating key features
- MIT License with proper third-party attributions

### Technical Details

- **Static/shared library** with header-only interface and `.inl` implementation files
- **C++20 required** with extensive use of `constexpr` and concepts
- **Performance-optimized** with zero-copy operations and memory pooling
- **Thread-safe components** where appropriate (cache, string pool)
