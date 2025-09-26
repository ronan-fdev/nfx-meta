# NFX CORE - Performance Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×10 cores)                                            |
| **L1 Instruction Cache** | 32 KiB (×10 cores)                                            |
| **L2 Unified Cache**     | 1280 KiB (×10 cores)                                          |
| **L3 Unified Cache**     | 24576 KiB (×1 shared)                                         |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

| Platform    | Benchmark Framework     | C++ Compiler         | NFX Core Version |
| ----------- | ----------------------- | -------------------- | ---------------- |
| **Windows** | Google Benchmark v1.9.4 | VC++ 19.44.35214-x64 | v0.1.3           |
| **Windows** | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64 | v0.1.3           |
| **Linux**   | Google Benchmark v1.9.4 | GCC 12.0-x64         | v0.1.3           |
| **Linux**   | Google Benchmark v1.9.4 | Clang 16.0.6         | v0.1.3           |

---

# Performance Results

## Core Components

### Hashing Algorithms

> **Hashing functions optimized for string and integer**

#### String Hashing Performance

##### nfx::HashStringView

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | ns        | ns          | 162 ns      | 319 ns       |
| **Medium (10-25)** | ns        | ns          | 381 ns      | 562 ns       |
| **Long (50-200)**  | ns        | ns          | 3,712 ns    | 5,048 ns     |

##### Manual FNV-1a

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | ns        | ns          | 188 ns      | 240 ns       |
| **Medium (10-25)** | ns        | ns          | 514 ns      | 637 ns       |
| **Long (50-200)**  | ns        | ns          | 7,017 ns    | 7,444 ns     |

##### Manual CRC32

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | ns        | ns          | 145 ns      | 246 ns       |
| **Medium (10-25)** | ns        | ns          | 380 ns      | 525 ns       |
| **Long (50-200)**  | ns        | ns          | 3,607 ns    | 4,888 ns     |

##### std::hash

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | ns        | ns          | 312 ns      | 346 ns       |
| **Medium (10-25)** | ns        | ns          | 391 ns      | 642 ns       |
| **Long (50-200)**  | ns        | ns          | 1,106 ns    | 7,480 ns     |

#### Low-Level Performance

| Operation                          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC | Description                  |
| ---------------------------------- | --------- | ----------- | ----------- | ------------ | ---------------------------- |
| **CPU Feature Detection (SSE4.2)** | ns        | ns          | 0.137 ns    | 0.976 ns     | Hardware capability check    |
| **Single Hash Step (FNV-1a)**      | ns        | ns          | 0.023 ns    | 0.770 ns     | Individual hash operation    |
| **Single Hash Step (CRC32)**       | ns        | ns          | 0.027 ns    | 0.726 ns     | Hardware-accelerated hashing |
| **Single Hash Step (Larson)**      | ns        | ns          | 0.027 ns    | 0.697 ns     | Alternative hash function    |
| **Seed Mix Function**              | ns        | ns          | 0.027 ns    | 0.700 ns     | CHD algorithm component      |

#### Integer Hashing Performance

##### nfx::hashInteger

| Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------- | --------- | ----------- | ----------- | ------------ |
| **uint32** | ns        | ns          | 164 ns      | 653 ns       |
| **uint64** | ns        | ns          | 550 ns      | 789 ns       |
| **int32**  | ns        | ns          | 164 ns      | 658 ns       |

##### std::hash

| Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------- | --------- | ----------- | ----------- | ------------ |
| **uint32** | ns        | ns          | 79.3 ns     | 958 ns       |
| **uint64** | ns        | ns          | 80.4 ns     | 1,842 ns     |

##### Single String Comparison

| Algorithm               | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::hashStringView** | ns        | ns          | 8.00 ns     | 10.7 ns      |
| **std::hash**           | ns        | ns          | 5.62 ns     | 18.6 ns      |

---

## Container Components

### ChdHashMap - Compress, Hash, and Displace Algorithm

> **Perfect hash maps using CHD (Compress, Hash, and Displace) algorithm for read-heavy workloads**

#### Construction Performance

| Benchmark        | linux GCC | linux clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **Small (10)**   | μs        | μs          | 3.12 μs     | 4.31 μs      |
| **Medium (100)** | μs        | μs          | 19.8 μs     | 21.8 μs      |
| **Large (500)**  | μs        | μs          | 188 μs      | 90.0 μs      |

#### ChdHashMapPerformance

| Operation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Lookup**    | ns        | ns          | 741 ns      | 1005 ns      |
| **Bracket Access**    | ns        | ns          | 667 ns      | 1132 ns      |
| **C-String Lookup**   | ns        | ns          | 1015 ns     | 1342 ns      |
| **StringView Lookup** | ns        | ns          | 741 ns      | 1028 ns      |
| **Complex Objects**   | ns        | ns          | 375 ns      | 508 ns       |

#### std::unordered_map Performance

| Operation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Lookup**    | ns        | ns          | 888 ns      | 946 ns       |
| **Bracket Access**    | ns        | ns          | 903 ns      | 1335 ns      |
| **C-String Lookup**   | ns        | ns          | 2635 ns     | 2895 ns      |
| **StringView Lookup** | ns        | ns          | 2463 ns     | 2358 ns      |
| **Complex Objects**   | ns        | ns          | 467 ns      | 604 ns       |

#### Iterator Performance

| Pattern                    | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Range-based For**        | ns        | ns          | 109 ns      | 115 ns       |
| **Complex Objects**        | ns        | ns          | 57.4 ns     | 66.4 ns      |
| **Conditional Processing** | ns        | ns          | 161 ns      | 200 ns       |
| **Early Termination**      | ns        | ns          | 7.64 ns     | 6.30 ns      |
| **Key Filtering**          | ns        | ns          | 543 ns      | 580 ns       |

#### Enumerator Performance

| Pattern                    | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Range-based For**        | ns        | ns          | 123 ns      | 129 ns       |
| **Complex Objects**        | ns        | ns          | 64.5 ns     | 84.2 ns      |
| **Conditional Processing** | ns        | ns          | 156 ns      | 160 ns       |
| **Early Termination**      | ns        | ns          | 6.62 ns     | 6.89 ns      |
| **Key Filtering**          | ns        | ns          | 575 ns      | 560 ns       |

### HashMap - Robin Hood Hashing

> **Hash map with Robin Hood collision resolution for general-purpose mapping**

#### HashMap Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | ns        | ns          | 7.71 μs     | 8.20 μs      |
| **String Lookup**     | ns        | ns          | 0.834 μs    | 1.07 μs      |
| **C-String Lookup**   | ns        | ns          | 1.31 μs     | 1.52 μs      |
| **StringView Lookup** | ns        | ns          | 0.953 μs    | 1.25 μs      |
| **Complex Insert**    | ns        | ns          | 7.31 μs     | 5.71 μs      |
| **Erase Operations**  | ns        | ns          | 1.45 μs     | 1.76 μs      |

#### std::unordered_map Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | ns        | ns          | 8.88 μs     | 9.95 μs      |
| **String Lookup**     | ns        | ns          | 0.890 μs    | 1.13 μs      |
| **C-String Lookup**   | ns        | ns          | 2.62 μs     | 2.51 μs      |
| **StringView Lookup** | ns        | ns          | 2.61 μs     | 2.46 μs      |
| **Complex Insert**    | ns        | ns          | 3.59 μs     | 5.15 μs      |
| **Erase Operations**  | ns        | ns          | 2.64 μs     | 2.71 μs      |

#### Heterogeneous Operations

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **C-String Erase**    | ns        | ns          | 1.69 μs     | 2.00 μs      |
| **StringView Erase**  | ns        | ns          | 1.53 μs     | 1.91 μs      |
| **Zero-Alloc Lookup** | ns        | ns          | 1.31 μs     | 1.54 μs      |

#### Large Dataset Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Insert 1K items**   | ns        | ns          | 149 μs      | 127 μs       |
| **Lookup 1K items**   | ns        | ns          | 8.91 μs     | 10.3 μs      |
| **Worst-case Insert** | ns        | ns          | 24.2 μs     | 25.7 μs      |
| **Mixed Operations**  | ns        | ns          | 5.73 μs     | 5.06 μs      |

### StringMap - String-Optimized Mapping

> **Hash map optimized for string keys with heterogeneous lookup capabilities**

#### StringMap Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | ns        | ns          | 13.2 μs     | 7.68 μs      |
| **C-String Lookup**   | ns        | ns          | 1.91 μs     | 1.80 μs      |
| **StringView Lookup** | ns        | ns          | 1.42 μs     | 1.42 μs      |
| **Complex Insert**    | ns        | ns          | 5.60 μs     | 5.91 μs      |

#### std::unordered_map<std::string, T> Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | ns        | ns          | 17.9 μs     | 7.90 μs      |
| **C-String Lookup**   | ns        | ns          | 2.66 μs     | 2.58 μs      |
| **StringView Lookup** | ns        | ns          | 2.52 μs     | 2.54 μs      |
| **Complex Insert**    | ns        | ns          | 5.27 μs     | 5.86 μs      |

#### Specialized Operations

| Operation Type             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Complex try_emplace**    | ns        | ns          | 5.76 μs     | 5.98 μs      |
| **Cache insert_or_assign** | ns        | ns          | 3.62 μs     | 3.42 μs      |
| **Mixed Operations**       | ns        | ns          | 6.46 μs     | 5.39 μs      |
| **Zero-Allocation Lookup** | ns        | ns          | 1.82 ns     | 1.87 μs      |

### StringSet - Heterogeneous String Collections

> **String set with zero-allocation heterogeneous lookup capabilities for string collections**

#### StringSet Performance

| Operation Type          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **String Insert**       | ns        | ns          | 7.89 μs     | 6.71 μs      |
| **C-String Insert**     | ns        | ns          | 9.27 μs     | 7.29 μs      |
| **StringView Insert**   | ns        | ns          | 8.76 μs     | 7.19 μs      |
| **C-String Find**       | ns        | ns          | 1.63 μs     | 1.79 μs      |
| **StringView Find**     | ns        | ns          | 1.33 μs     | 1.45 μs      |
| **C-String Contains**   | ns        | ns          | 1.46 μs     | 1.21 μs      |
| **StringView Contains** | ns        | ns          | 1.46 μs     | 1.19 μs      |

#### std::unordered_set<std::string> Performance

| Operation Type          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **String Insert**       | ns        | ns          | 7.70 μs     | 6.74 μs      |
| **C-String Insert**     | ns        | ns          | 8.18 μs     | 7.44 μs      |
| **StringView Insert**   | ns        | ns          | 8.18 μs     | 7.16 μs      |
| **C-String Find**       | ns        | ns          | 2.43 μs     | 2.32 μs      |
| **StringView Find**     | ns        | ns          | 2.43 μs     | 2.35 μs      |
| **C-String Contains**   | ns        | ns          | 2.39 μs     | 2.37 μs      |
| **StringView Contains** | ns        | ns          | 2.43 μs     | 2.54 μs      |

#### Advanced Operations

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Mixed Operations**   | ns        | ns          | 6.95 μs     | 4.19 μs      |
| **Set Operations**     | ns        | ns          | 12.6 μs     | 9.53 μs      |
| **Zero-Alloc Lookup**  | ns        | ns          | 1.74 μs     | 1.76 μs      |
| **Duplicate Handling** | ns        | ns          | 16.8 μs     | 4.85 μs      |

#### Large Dataset Performance

| Operation Type      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------- | --------- | ----------- | ----------- | ------------ |
| **Insert 1K items** | ns        | ns          | 136 μs      | 111 μs       |
| **Lookup 1K items** | ns        | ns          | 16.5 μs     | 12.6 μs      |

---

## String Components

### Splitter - Zero-Copy String Splitting

> **String splitting with zero memory allocations**

#### Manual Split Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | ns        | ns          | 34.2 ns     | 24.7 ns      |
| **Path Data (7 segments)** | ns        | ns          | 30.7 ns     | 22.2 ns      |
| **Config Data (6 pairs)**  | ns        | ns          | 32.5 ns     | 19.5 ns      |

#### nfx::Splitter Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | ns        | ns          | 32.1 ns     | 24.5 ns      |
| **Path Data (7 segments)** | ns        | ns          | 27.7 ns     | 25.5 ns      |
| **Config Data (6 pairs)**  | ns        | ns          | 32.8 ns     | 20.6 ns      |

#### nfx::splitView Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | ns        | ns          | 32.4 ns     | 24.7 ns      |
| **Path Data (7 segments)** | ns        | ns          | 27.5 ns     | 25.8 ns      |
| **Config Data (6 pairs)**  | ns        | ns          | 33.3 ns     | 20.9 ns      |

#### Zero-Allocation Operations

| Operation Type     | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Zero-Alloc CSV** | ns        | ns          | 30.7 ns     | 21.2 ns      |

### StringBuilderPool - Thread-Safe String Building

> **String building with three-tier optimization strategy for high-performance applications**

#### StringBuilderPool Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | ns        | ns          | 115 ns      | 67.7 ns      |
| **Medium Strings (4)** | ns        | ns          | 121 ns      | 90.6 ns      |
| **Large Strings (3)**  | ns        | ns          | 116 ns      | 83.0 ns      |
| **Mixed Operations**   | ns        | ns          | 234 ns      | 260 ns       |

#### std::string Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | ns        | ns          | 48.7 ns     | 49.4 ns      |
| **Medium Strings (4)** | ns        | ns          | 138 ns      | 154 ns       |
| **Large Strings (3)**  | ns        | ns          | 114 ns      | 119 ns       |
| **Mixed Operations**   | ns        | ns          | 133 ns      | 200 ns       |

#### std::ostringstream Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | ns        | ns          | 192 ns      | 376 ns       |
| **Medium Strings (4)** | ns        | ns          | 218 ns      | 657 ns       |
| **Large Strings (3)**  | ns        | ns          | 293 ns      | 684 ns       |
| **Mixed Operations**   | ns        | ns          | 434 ns      | 1,496 ns     |

#### Pool-Specific Features

| Feature             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC | Description                          |
| ------------------- | --------- | ----------- | ----------- | ------------ | ------------------------------------ |
| **Zero-Allocation** | ns        | ns          | 114 ns      | 70.0 ns      | string_view access without copy      |
| **Pool Efficiency** | ns        | ns          | 970 ns      | 769 ns       | Rapid lease/return cycles (10x)      |
| **Buffer Reuse**    | ns        | ns          | 905 ns      | 970 ns       | Multiple consecutive operations (5x) |
| **Rapid Cycles**    | ns        | ns          | 339 ns      | 430 ns       | std::string equivalent comparison    |

### StringUtilities - Fast String Operations

> **Optimized string utility functions**

#### Character Classification Performance

##### nfx::string::Utils

| Operation Type   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **isDigit**      | ns        | ns          | 3.84 ns     | 10.4 ns      |
| **isWhitespace** | ns        | ns          | 5.02 ns     | 18.1 ns      |
| **isAlpha**      | ns        | ns          | 3.82 ns     | 24.2 ns      |

##### Standard Library

| Operation Type   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **isDigit**      | ns        | ns          | 3.87 ns     | 30.0 ns      |
| **isWhitespace** | ns        | ns          | 20.9 ns     | 27.6 ns      |
| **isAlpha**      | ns        | ns          | 22.1 ns     | 29.3 ns      |

#### String Validation Performance

##### nfx::string::Utils

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **isEmpty**            | ns        | ns          | 2.98 ns     | 14.9 ns      |
| **isNullOrWhiteSpace** | ns        | ns          | 22.7 ns     | 33.3 ns      |
| **isAllDigits**        | ns        | ns          | 28.7 ns     | 36.5 ns      |

##### Standard/Manual Implementation

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **isEmpty**            | ns        | ns          | 3.00 ns     | 15.1 ns      |
| **isNullOrWhiteSpace** | ns        | ns          | 52.0 ns     | 55.7 ns      |
| **isAllDigits**        | ns        | ns          | 27.8 ns     | 72.3 ns      |

#### String Operations Performance

##### nfx::string::Utils

| Operation Type | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------- | --------- | ----------- | ----------- | ------------ |
| **startsWith** | ns        | ns          | 11.7 ns     | 30.9 ns      |
| **endsWith**   | ns        | ns          | 12.3 ns     | 25.2 ns      |
| **contains**   | ns        | ns          | 68.9 ns     | 147 ns       |

##### Standard Library

| Operation Type | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------- | --------- | ----------- | ----------- | ------------ |
| **startsWith** | ns        | ns          | 8.10 ns     | 26.8 ns      |
| **endsWith**   | ns        | ns          | 9.80 ns     | 29.2 ns      |
| **contains**   | ns        | ns          | 74.2 ns     | 151 ns       |

#### String Trimming Performance

| Implementation        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::string::trim** | ns        | ns          | 31.0 ns     | 46.2 ns      |
| **Manual trim**       | ns        | ns          | 37.8 ns     | 251 ns       |

#### Case Conversion Performance

##### nfx::string::Utils

| Operation Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------- | --------- | ----------- | ----------- | ------------ |
| **toLower (char)**   | ns        | ns          | 4.94 ns     | 12.7 ns      |
| **toUpper (char)**   | ns        | ns          | 3.57 ns     | 12.8 ns      |
| **toLower (string)** | ns        | ns          | 333 ns      | 761 ns       |
| **toUpper (string)** | ns        | ns          | 335 ns      | 762 ns       |

##### Standard Library

| Operation Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------- | --------- | ----------- | ----------- | ------------ |
| **toLower (char)**   | ns        | ns          | 13.6 ns     | 28.9 ns      |
| **toUpper (char)**   | ns        | ns          | 13.4 ns     | 34.1 ns      |
| **toLower (string)** | ns        | ns          | 389 ns      | 494 ns       |
| **toUpper (string)** | ns        | ns          | 389 ns      | 599 ns       |

#### Parsing Performance

##### nfx::string::Utils

| Operation Type     | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **tryParseInt**    | ns        | ns          | 10.8 ns     | 40.0 ns      |
| **tryParseDouble** | ns        | ns          | 52.7 ns     | 124 ns       |

##### Standard Library (std::from_chars)

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **from_chars_int**    | ns        | ns          | 10.8 ns     | 26.1 ns      |
| **from_chars_double** | ns        | ns          | 52.4 ns     | 115 ns       |

#### URI Classification Performance

| Implementation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::isURIReserved**     | ns        | ns          | 5.59 ns     | 22.2 ns      |
| **Manual isURIReserved**   | ns        | ns          | 5.53 ns     | 22.1 ns      |
| **nfx::isURIUnreserved**   | ns        | ns          | 7.54 ns     | 21.5 ns      |
| **Manual isURIUnreserved** | ns        | ns          | 7.57 ns     | 24.3 ns      |

---

_Benchmarked on September 26, 2025_
