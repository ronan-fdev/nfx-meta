# NFX-Core

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)](#)
[![Cross Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)](#)

> A modern C++ utility library featuring cross-platform 128-bit arithmetic, zero-copy containers, advanced string processing, thread-safe memory caching, and high-precision temporal calculations

> **⚠️ WARNING: API NOT STABLE - LIBRARY UNDER ACTIVE DEVELOPMENT**  
> This library is in active development and the API may change without notice. Use with caution in production environments.

## Overview

NFX-Core is a modular C++20 library designed to provide robust components for applications requiring precise calculations, efficient string processing, and optimized data structures.
Originally developed as foundational infrastructure for the C++ port of the [DNV Vista SDK](https://github.com/dnv-opensource/vista-sdk).

## Features

### 🔢 High-Precision Datatypes

- **Int128**: Cross-platform 128-bit signed integer with native `__int128` support on GCC/Clang
- **Decimal**: 96-bit mantissa fixed-point arithmetic (28 decimal digits, .NET System.Decimal compatible)
- Zero floating-point rounding errors for financial calculations
- String parsing and cross-platform consistency

### 📦 Advanced Containers

- **ChdHashMap**: Perfect hash implementation using CHD (Compress, Hash, and Displace) algorithm (derived from Vista SDK)
- **HashMap**: Robin Hood hashing with bounded probe distances and optimal cache performance
- **StringMap/StringSet**: Zero-copy heterogeneous string lookups with `std::string_view` support
- **StringFunctors**: Transparent hash/equality functors for `std::string`/`std::string_view` interoperability
- Cross-platform hash compatibility and SSE4.2 optimizations where available

### 🔤 String Processing

- **StringBuilderPool**: Thread-safe pooled string building with RAII lease management and automatic buffer reuse
- **Splitter**: Zero-allocation iterator-based string splitting with real-world performance optimizations
- **Utils**: Fast parsing utilities using `std::charconv` with comprehensive validation (bool, int, double, URI components)
- Heterogeneous container lookups without temporary string allocations
- Character classification and case conversion optimized for ASCII performance

### 💾 Memory Management

- **LruCache**: Thread-safe LRU cache with sliding expiration (.NET IMemoryCache-inspired)
- Intrusive LRU lists for cache-friendly performance
- Configurable size limits and automatic eviction policies

### 🕒 Temporal Processing (ISO 8601 compliant)

- **DateTime**: High-precision date and time handling
- **DateTimeOffset**: Timezone-aware temporal calculations
- **TimeSpan**: Duration and interval arithmetic

## Quick Start

### Requirements

- C++20 compatible compiler:
  - **MSVC 2022+** (19.44+ tested)
  - **GCC 10+** (14.2.0 tested)
  - **Clang 12+** (19.1.5 tested)
- CMake 3.20 or higher
- **Multi-compiler builds supported** across x64/x86 architectures

### Building

```bash
# Clone the repository
git clone https://github.com/ronan-fdev/nfx-core.git
cd nfx-core

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release

# Run tests (optional)
ctest --config Release
```

### CMake Integration

```cmake
include(FetchContent)
FetchContent_Declare(
  nfx-core
  GIT_REPOSITORY https://github.com/ronan-fdev/nfx-core.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(nfx-core)
target_link_libraries(your_target nfx-core-static)
```

### Basic Usage

```cpp
#include <nfx/datatypes/Int128.h>
#include <nfx/datatypes/Decimal.h>
#include <nfx/string/StringBuilderPool.h>
#include <nfx/containers/StringMap.h>
#include <nfx/memory/LruCache.h>

// Cross-platform 128-bit arithmetic
nfx::datatypes::Int128 large_number{1234567890123456789ULL, 0ULL};
auto result = large_number * nfx::datatypes::Int128{42};

// Exact decimal arithmetic (no floating-point errors)
auto decimal1 = nfx::datatypes::Decimal::parse("123.456789");
auto decimal2 = nfx::datatypes::Decimal::parse("987.654321");
auto precise_calc = decimal1 * decimal2;

// High-performance string building with pooling
auto builder_lease = nfx::string::StringBuilderPool::lease();
auto builder = builder_lease.builder();
builder << "Result: " << precise_calc.toString();
std::string final_result = builder_lease.toString();

// Zero-copy string container operations
nfx::containers::StringMap<int> string_map;
string_map["key"] = 42;
// No string allocation for lookups:
auto value = string_map.find(std::string_view{"key"});

// Thread-safe LRU cache with expiration
nfx::memory::LruCache<std::string, ExpensiveObject> cache{
    nfx::memory::LruCacheOptions{1000, std::chrono::minutes{30}}
};
cache.set("cache_key", expensive_object);
```

## Configuration Options

The library supports modular compilation through CMake options:

```cmake
# Library build types
option(NFX_CORE_BUILD_STATIC     "Build static library"                ON )
option(NFX_CORE_BUILD_SHARED     "Build shared library"                ON )

# Components
option(NFX_CORE_WITH_CONTAINERS  "Enable container utilities"          ON )
option(NFX_CORE_WITH_DATATYPES   "Enable mathematical datatypes"       ON )
option(NFX_CORE_WITH_MEMORY      "Enable memory management utilities"  ON )
option(NFX_CORE_WITH_STRING      "Enable string utilities"             ON )
option(NFX_CORE_WITH_TIME        "Enable temporal classes"             ON )

# Development
option(NFX_CORE_BUILD_TESTS      "Build tests"                         ON )
option(NFX_CORE_BUILD_SAMPLES    "Build samples"                       ON )
option(NFX_CORE_BUILD_BENCHMARKS "Build performance benchmarks"        OFF)
```

## Project Structure

```
nfx-core/
├── benchmark/             # Performance benchmarks with Google Benchmark
├── cmake/                 # CMake modules and configuration
├── include/nfx/           # Public headers with .inl implementation files
│   ├── containers/        # HashMap, ChdHashMap, StringMap, StringSet
│   ├── core/              # Core utilities
│   │   └── hashing/       # Hash algorithms (FNV-1a, CRC32, etc.)
│   ├── datatypes/         # Int128, Decimal with high-precision arithmetic
│   ├── memory/            # LruCache with sliding expiration
│   ├── string/            # StringBuilderPool, Splitter, Utils
│   └── time/              # DateTime, DateTimeOffset, TimeSpan
├── licenses/              # Third-party license files
├── samples/               # Example usage and demonstrations
├── src/                   # Implementation files (.cpp)
│   ├── datatypes/         # Core mathematical type implementations
│   ├── string/            # String processing internals
│   └── time/              # Temporal calculation implementations
└── test/                  # Comprehensive unit tests with GoogleTest
```

## Dependencies

- **fmt**: Modern formatting library (header-only mode)
- **GoogleTest**: Testing framework (test builds only)
- **Google Benchmark**: Performance benchmarking framework (benchmark builds only)

## Performance

NFX-Core is designed with performance as a primary concern. For detailed performance metrics and benchmarking results, see [benchmark/README.md](benchmark/README.md).

### Zero-Copy Operations

- `std::string_view` heterogeneous lookups in all string containers
- Iterator-based string splitting without heap allocations
- Transparent functors for mixed string type operations

### Memory Optimization

- Thread-safe object pooling (StringBuilderPool)
- Intrusive LRU cache implementation
- Robin Hood hashing with bounded probe distances

### Cross-Platform Optimizations

- Native `__int128` on GCC/Clang, optimized fallback on MSVC
- SSE4.2 string hashing where available
- Template metaprogramming for compile-time optimization

### Hash Algorithm Implementation

- CHD perfect hashing for static datasets
- Cache-friendly data layouts and access patterns

## Compatibility

- **Platforms**: Windows, Linux
- **Compilers**:
  - **MSVC 2022+** (tested with Visual Studio 19.44.35214.0)
  - **GCC 10+** (tested with GCC 14.2.0)
  - **Clang 12+** (tested with Clang 19.1.5 with GNU/MSVC CLI)
- **Architectures**: x64, x86 (multi-architecture build matrix)
- **Standards**: C++20 required

## Contributing

This project adheres to [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) for commit messages and [Semantic Versioning](https://semver.org/spec/v2.0.0.html) for releases.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-Party Attributions

- **ChdHashMap algorithm**: Derived from [DNV Vista SDK](https://github.com/dnv-opensource/vista-sdk) (MIT License)
- **fmt library**: Used for formatting support (MIT License)
- **Google Benchmark**: Used for performance measurement (Apache 2.0 License)
- **Google Test**: Used for unit testing (BSD 3-Clause License)

## Related Projects

- [DNV Vista SDK C++ Port](https://github.com/ronan-fdev/vista-sdk/tree/cpp)

---

_Updated on September 21, 2025_
