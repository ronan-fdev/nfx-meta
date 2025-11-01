# nfx-meta

<!-- Project Info -->

[![License](https://img.shields.io/github/license/ronan-fdev/nfx-meta?style=flat-square)](https://github.com/ronan-fdev/nfx-meta/blob/main/LICENSE)<br/>

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square)](#) [![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg?style=flat-square)](#) [![Cross Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey?style=flat-square)](#) <br/>

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-meta/build-linux-gcc.yml?branch=main&label=Linux%20GCC&style=flat-square)](https://github.com/ronan-fdev/nfx-meta/actions/workflows/build-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-meta/build-linux-clang.yml?branch=main&label=Linux%20Clang&style=flat-square)](https://github.com/ronan-fdev/nfx-meta/actions/workflows/build-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-meta/build-windows-mingw.yml?branch=main&label=Windows%20MinGW&style=flat-square)](https://github.com/ronan-fdev/nfx-meta/actions/workflows/build-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-meta/build-windows-msvc.yml?branch=main&label=Windows%20MSVC&style=flat-square)](https://github.com/ronan-fdev/nfx-meta/actions/workflows/build-windows-msvc.yml)

> A modern C++ utility library featuring cross-platform 128-bit arithmetic, zero-copy containers, advanced string processing, thread-safe memory caching, and JSON serialization.

> **⚠️ WARNING: API NOT STABLE - LIBRARY UNDER ACTIVE DEVELOPMENT**  
> This library is in active development and the API may change without notice. Use with caution in production environments.

## Overview

nfx-meta is a modular C++20 library designed to provide robust components for applications requiring precise calculations, efficient string processing, and optimized data structures.
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

### 📄 JSON Serialization

- **Document**: Comprehensive JSON parsing, manipulation, and serialization with RFC 6901 JSON Pointer support
- **Serializer<T>**: Templated JSON serializer with automatic type detection and customizable strategies
- **ArrayEnumerator/FieldEnumerator**: Efficient JSON traversal with stateful positioning and type-safe access
- **SchemaValidator**: JSON Schema Draft 7 validation with detailed error reporting
- **SerializationTraits<T>**: Extensible framework for custom type serialization with cross-platform compatibility

## Quick Start

### Requirements

- C++20 compatible compiler:
  - **MSVC 2022+** (19.44+ tested)
  - **GCC 12+** (12.2.0 tested)
  - **Clang 14+** (14.0.6 tested)
- CMake 3.20 or higher
- Multi-compiler builds supported

### CMake Integration

The library supports modular compilation through CMake options:

```cmake
# Library build types
option(NFX_META_BUILD_STATIC         "Build static library"                ON  )
option(NFX_META_BUILD_SHARED         "Build shared library"                OFF )

# Build optimization
option(NFX_META_USE_CACHE            "Enable compiler cache"               ON  )

# Components
option(NFX_META_WITH_CONTAINERS      "Enable container utilities"          ON  )
option(NFX_META_WITH_DATATYPES       "Enable mathematical datatypes"       ON  )
option(NFX_META_WITH_MEMORY          "Enable memory management utilities"  ON  )
option(NFX_META_WITH_JSON            "Enable JSON serialization support"   ON  )
option(NFX_META_WITH_STRING          "Enable string utilities"             ON  )
option(NFX_META_WITH_TIME            "Enable temporal classes"             ON  )

# Development (automatically enabled for standalone builds, disabled for submodule usage)
option(NFX_META_BUILD_TESTS          "Build tests"                         AUTO )
option(NFX_META_BUILD_SAMPLES        "Build samples"                       AUTO )
option(NFX_META_BUILD_BENCHMARKS     "Build benchmarks"                    AUTO )
option(NFX_META_BUILD_DOCUMENTATION  "Build Doxygen documentation"         AUTO )

# Installation (automatically enabled for standalone builds)
option(NFX_META_INSTALL_PROJECT      "Install project"                     AUTO )

# Packaging (automatically enabled for standalone builds)
option(NFX_META_PACKAGE_SOURCE       "Enable source package generation"    AUTO )
option(NFX_META_PACKAGE_ARCHIVE      "Enable TGZ/ZIP package generation"   AUTO )
option(NFX_META_PACKAGE_DEB          "Enable DEB package generation"       AUTO )
option(NFX_META_PACKAGE_RPM          "Enable RPM package generation"       AUTO )
option(NFX_META_PACKAGE_NSIS         "Enable NSIS Windows installer"       AUTO )
```

### Using in Your Project

#### Option 1: Using FetchContent (Build from source)

```cmake
include(FetchContent)
FetchContent_Declare(
  nfx-meta
  GIT_REPOSITORY https://github.com/ronan-fdev/nfx-meta.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(nfx-meta)

# Link with static library (recommended for most use cases)
target_link_libraries(your_target nfx-meta::static)

# Or link with shared library
# target_link_libraries(your_target nfx-meta::nfx-meta)
```

#### Option 2: As a Git Submodule

**Setup Steps (one-time):**

```bash
# From your project root directory
git submodule add https://github.com/ronan-fdev/nfx-meta.git third-party/nfx-meta
git commit -m "Add nfx-meta as submodule"
```

**CMakeLists.txt:**

```cmake
# Add nfx-meta as a subdirectory
# Note: Development features (tests, samples, benchmarks, docs, packaging)
# are automatically disabled when used as a submodule
add_subdirectory(third-party/nfx-meta)

# Link with the library
target_link_libraries(your_target nfx-meta::static)
```

**Clone/Update Commands:**

```bash
# When cloning your project (for new developers)
# Replace with your actual project URL:
git clone --recursive https://github.com/YOUR-USERNAME/YOUR-PROJECT.git

# Or if already cloned without --recursive
git submodule update --init --recursive

# To update the submodule to latest version
cd third-party/nfx-meta
git pull origin main
cd ../..
git add third-party/nfx-meta
git commit -m "Update nfx-meta submodule"
```

**Automatic Development Feature Disabling:**

nfx-meta automatically detects when it's used as a submodule and disables development features:

nfx-meta automatically detects its usage context and adjusts build behavior accordingly:

- **Standalone builds**: When building nfx-meta directly, all development features are enabled by default (tests, samples, benchmarks, documentation, packaging)
- **Submodule/FetchContent usage**: When used as a dependency, only the core library is built to avoid cluttering the parent project

When used as a submodule, these features are **automatically disabled**:

- Tests, samples, benchmarks, documentation generation
- Installation targets and package generation
- Only the core library targets (`nfx-meta::static`, `nfx-meta::nfx-meta`) are built

#### Option 3: Using find_package (for installed libraries)

```cmake
# Find the installed nfx-meta library
find_package(nfx-meta REQUIRED)

# Link with static library (recommended for most use cases)
target_link_libraries(your_target nfx-meta::static)

# Or link with shared library
# target_link_libraries(your_target nfx-meta::nfx-meta)
```

#### Integration Method Comparison

| Method           | Best For                              | Pros                                              | Cons                                              |
| ---------------- | ------------------------------------- | ------------------------------------------------- | ------------------------------------------------- |
| **find_package** | Production builds, system packages    | Fast builds, version control, clean separation    | Requires pre-installation                         |
| **FetchContent** | CI/CD, development, specific versions | Always available, version pinning, no pre-install | Longer build times, downloads on each clean build |
| **Submodule**    | Long-term projects, offline builds    | Full source control, offline capability           | Manual updates, Git complexity                    |

### Building

```bash
# Clone the repository
git clone https://github.com/ronan-fdev/nfx-meta.git
cd nfx-meta

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release

# Run tests (optional)
ctest -C Release
```

### Documentation

nfx-meta includes comprehensive API documentation generated with Doxygen.

#### Building Documentation

```bash
# Configure with documentation enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DNFX_META_BUILD_DOCUMENTATION=ON

# Build the documentation
cmake --build . --target documentation

# Documentation will be generated in build/nfx-meta-{version}/{compiler}/Release/doc/html/
```

#### Requirements

- **Doxygen** - Documentation generation tool
- **Graphviz Dot** (optional) - For generating class diagrams and dependency graphs

#### Accessing Documentation

After building, open the documentation in your browser:

```bash
# Linux
xdg-open build/nfx-meta-*/*/Release/doc/html/index.html     # Most Linux systems
firefox build/nfx-meta-*/*/Release/doc/html/index.html      # If Firefox is installed

# Windows
start build/nfx-meta-*/*/Release/doc/html/index.html
```

## Installation & Packaging

nfx-meta provides comprehensive packaging options for distribution across multiple platforms and package managers.

### Package Generation

The library supports generating packages in multiple formats:

```bash
# Configure with packaging options (including both static and shared libraries)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNFX_META_BUILD_STATIC=ON \
         -DNFX_META_BUILD_SHARED=ON \
         -DNFX_META_PACKAGE_ARCHIVE=ON \
         -DNFX_META_PACKAGE_DEB=ON \
         -DNFX_META_PACKAGE_RPM=ON \
         -DNFX_META_PACKAGE_NSIS=ON \
         -DNFX_META_PACKAGE_SOURCE=ON

# Generate binary packages
cmake --build . --target package

# Generate source packages (recommended method)
cd build
cpack --config CPackSourceConfig.cmake
```

### Supported Package Formats

| Format      | Platform       | Description                              | Requirements |
| ----------- | -------------- | ---------------------------------------- | ------------ |
| **TGZ/ZIP** | Cross-platform | Compressed archive packages              | None         |
| **DEB**     | Debian/Ubuntu  | Native Debian packages with dependencies | `dpkg-dev`   |
| **RPM**     | RedHat/SUSE    | Native RPM packages with dependencies    | `rpm-build`  |
| **NSIS**    | Windows        | Professional Windows installer (.exe)    | `NSIS 3.03+` |
| **Source**  | Cross-platform | Source code distribution                 | None         |

### Package Options

Control which packages are generated with CMake options:

```cmake
# Package generation control
option(NFX_META_PACKAGE_ARCHIVE      "Generate TGZ/ZIP packages"          ON   )
option(NFX_META_PACKAGE_DEB          "Generate DEB packages (Linux)"      ON   )
option(NFX_META_PACKAGE_RPM          "Generate RPM packages (Linux)"      ON   )
option(NFX_META_PACKAGE_NSIS         "Generate NSIS installer (Windows)"  ON   )
option(NFX_META_PACKAGE_SOURCE       "Generate source packages"           ON   )
```

### Linux Package Dependencies

**DEB packages** automatically include runtime dependencies:

- `libc6`, `libstdc++6`, `libgcc-s1` (core runtime)
- `nlohmann-json3-dev` (if JSON support enabled)

**RPM packages** automatically include runtime dependencies:

- `glibc`, `libstdc++` (core runtime)
- `nlohmann-json-devel` (if JSON support enabled)

### Windows Installer Features

The NSIS installer provides:

- **64-bit Program Files installation**
- **Automatic PATH modification** for easy command-line access
- **Proper uninstaller registration** in Windows Add/Remove Programs
- **DPI-aware installation** for modern displays

### Cross-Platform Installation

```bash
# Linux (DEB-based systems)
sudo dpkg -i nfx-meta-*.deb
sudo apt-get install -f  # Fix dependencies if needed

# Linux (RPM-based systems)
sudo rpm -ivh nfx-meta-*.rpm

# Windows
# Run the .exe installer with administrator privileges
nfx-meta-*-win64.exe

# Manual installation (all platforms)
# Extract TGZ/ZIP to desired location
tar -xzf nfx-meta-*.tar.gz -C /usr/local/
# or
unzip nfx-meta-*.zip -d "C:\Program Files\"
```

### Package Contents

All packages include:

- **Headers** (`include/nfx/`) - Public API headers
- **Libraries** (`lib/`) - Static/shared libraries
- **Documentation** (`doc/`) - API documentation (if built)
- **Licenses** (`doc/licenses/`) - All license files
- **Examples** (`samples/`) - Usage examples (if built)

## Usage Example

```cpp
#include <iostream>
#include <cmath>
#include <chrono>
#include <string>
#include <vector>

#include <nfx/containers/StringMap.h>
#include <nfx/datatypes/Int128.h>
#include <nfx/datatypes/Decimal.h>
#include <nfx/memory/LruCache.h>
#include <nfx/serialization/json/Serializer.h>
#include <nfx/string/StringBuilderPool.h>

struct ExpensiveObject
{
	std::vector<double> largeDataset;
	std::string computedResult;
	std::chrono::steady_clock::time_point creationTime;

	ExpensiveObject()
		: creationTime{ std::chrono::steady_clock::now() }
	{
		// Simulate expensive computation
		largeDataset.reserve( 10000 );
		for ( int i = 0; i < 10000; ++i )
		{
			largeDataset.push_back( std::sin( i ) * std::cos( i ) );
		}

		// Simulate expensive string processing
		computedResult = "Computed result after expensive operations: ";
		computedResult += std::to_string( largeDataset.size() );
		computedResult += " data points processed";
	}
};

int main()
{
	// Cross-platform 128-bit arithmetic
	nfx::datatypes::Int128 largeNumber{ 1234567890123456789ULL, 0ULL };
	auto result = largeNumber * nfx::datatypes::Int128{ 42 };
	std::cout << "Int128 arithmetic: " << largeNumber.toString() << " * 42 = " << result.toString() << std::endl;

	// Exact decimal arithmetic (no floating-point errors)
	auto decimal1 = nfx::datatypes::Decimal::parse( "123.456789" );
	auto decimal2 = nfx::datatypes::Decimal::parse( "987.654321" );
	auto preciseCalc = decimal1 * decimal2;
	std::cout << "Decimal arithmetic: " << decimal1.toString() << " * " << decimal2.toString() << " = " << preciseCalc.toString() << std::endl;

	// High-performance string building with pooling
	auto builderLease = nfx::string::StringBuilderPool::lease();
	auto builder = builderLease.create();
	builder << "Result: " << preciseCalc.toString();
	std::string finalResult = builderLease.toString();
	std::cout << "String building: " << finalResult << std::endl;

	// Zero-copy string container operations
	nfx::containers::StringMap<int> stringMap;
	stringMap["key"] = 42;
	// No string allocation for lookups:
	auto value = stringMap.find( std::string_view{ "key" } );
	if ( value != stringMap.end() )
	{
		std::cout << "StringMap lookup: key = " << value->second << std::endl;
	}

	// Thread-safe LRU cache with expiration
	auto expensiveObject = ExpensiveObject{};
	nfx::memory::LruCache<std::string, ExpensiveObject> cache{
		nfx::memory::LruCacheOptions{ 1000, std::chrono::minutes{ 30 } } };
	cache.getOrCreate( "cache_key", [&expensiveObject]() { return expensiveObject; } );
	std::cout << "LruCache: Added entry, cache size = " << cache.size() << std::endl;

	// JSON serialization with cross-platform compatibility
	auto serializer = nfx::serialization::json::Serializer<nfx::datatypes::Decimal>{};
	std::string jsonString = serializer.serializeToString( preciseCalc );
	auto deserializedDecimal = serializer.deserializeFromString( jsonString );
	std::cout << "JSON serialization roundtrip: " << deserializedDecimal.toString() << std::endl;
}

```

**Sample Output:**

```
	Int128 arithmetic: 1234567890123456789 * 42 = 51851851385185185138
	Decimal arithmetic: 123.456789 * 987.654321 = 121932.631112635269
	String building: Result: 121932.631112635269
	StringMap lookup: key = 42
	LruCache: Added entry, cache size = 1
	JSON serialization roundtrip: 121932.631112635269
```

### Sample Applications

The `samples/` directory contains comprehensive examples demonstrating all nfx-meta features.

To build and run samples:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNFX_META_BUILD_SAMPLES=ON
```

## Project Structure

```
nfx-meta/
├── benchmark/             # Performance benchmarks with Google Benchmark
├── cmake/                 # CMake modules and configuration
├── include/nfx/           # Public headers with .inl implementation files
│   ├── containers/        # HashMap, ChdHashMap, StringMap, StringSet
│   ├── core/              # Core utilities, hash utilities and CPU feature detection
│   ├── datatypes/         # Int128, Decimal with high-precision arithmetic
│   ├── detail/            # Implementation details and inline files
│   ├── memory/            # LruCache with sliding expiration
│   ├── serialization/     # JSON Document, Serializer, SchemaValidator
│   │   └── json/          # JSON serialization framework
│   └──string/            # StringBuilderPool
├── licenses/              # Third-party license files
├── samples/               # Example usage and demonstrations
├── src/                   # Implementation files
└── test/                  # Comprehensive unit tests with GoogleTest
```

## Version History

For detailed version history, feature additions, and breaking changes, see [CHANGELOG.md](CHANGELOG.md).

## Performance

nfx-meta is designed with performance as a primary concern. For detailed performance metrics and benchmarking results, see [benchmark/README.md](benchmark/README.md).

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

## CPU Architecture Detection

nfx-meta includes CPU feature detection to optimize performance across different hardware generations.

### Build-Time CPU Detection

The library automatically detects CPU capabilities during CMake configuration:

- **Sandy Bridge CPUs** (i7-2xxx series, 2011-2012): Supports AVX but not AVX2/FMA
  - Automatically falls back to SSE4.2 + AVX baseline
  - Prevents illegal instruction crashes on older hardware
- **Haswell+ CPUs** (i7-4xxx series, 2013+): Full AVX2/FMA support
  - Enables advanced optimizations for maximum performance

### Binary Distribution Considerations

⚠️ **Important for Binary Distribution:**

The current implementation performs CPU detection at **configure time**, which means:

- ✅ **Development builds**: Optimal performance for your specific hardware
- ✅ **Local compilation**: Automatically adapts to target machine CPU
- ❌ **Binary distribution**: Built binaries are CPU-specific and may not run on different hardware generations

### Future Enhancement: Runtime CPU Detection

- **Planned Feature**: Runtime CPU capability detection for truly portable binaries

A future version will implement runtime CPU feature detection to enable single binaries that work across all hardware generations while maintaining optimal performance through dynamic dispatch.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Dependencies & Third-Party Attributions

- **ChdHashMap algorithm**: Derived from [DNV Vista SDK](https://github.com/dnv-opensource/vista-sdk) (MIT License)
- **[nlohmann/json](https://github.com/nlohmann/json)**: JSON for Modern C++ (MIT License)
- **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License)
- **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License)

## Related Projects

- [DNV Vista SDK C++ Port](https://github.com/ronan-fdev/vista-sdk/tree/cpp)

---

_Updated on October 9, 2025_
