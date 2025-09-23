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

- **Centralized Hash Utilities**: New `nfx/core/hashing/Hash.h` module
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
