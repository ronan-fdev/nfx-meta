# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

#### Enhanced String Validation

- **isAllDigits()**: New string validation function to check if string contains only ASCII digits

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
