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
| **Linux**   | Google Benchmark v1.9.4 | GCC 12.0-x64         | v0.1.7           |
| **Linux**   | Google Benchmark v1.9.4 | Clang 16.0.6         | v0.1.7           |

---

# Performance Results

## Core Components

### Hashing Algorithms

> **Hashing functions optimized for string and integer**

#### String Hashing Performance

##### nfx::HashStringView

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | 154 ns    | 211 ns      | 162 ns      | 319 ns       |
| **Medium (10-25)** | 397 ns    | 392 ns      | 381 ns      | 562 ns       |
| **Long (50-200)**  | 3,775 ns  | 3,810 ns    | 3,712 ns    | 5,048 ns     |

##### Manual FNV-1a

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | 267 ns    | 289 ns      | 188 ns      | 240 ns       |
| **Medium (10-25)** | 557 ns    | 538 ns      | 514 ns      | 637 ns       |
| **Long (50-200)**  | 7,205 ns  | 7,140 ns    | 7,017 ns    | 7,444 ns     |

##### Manual CRC32

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | 154 ns    | 204 ns      | 145 ns      | 246 ns       |
| **Medium (10-25)** | 397 ns    | 387 ns      | 380 ns      | 525 ns       |
| **Long (50-200)**  | 3,775 ns  | 3,780 ns    | 3,607 ns    | 4,888 ns     |

##### std::hash

| String Length      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Short (3-8)**    | 407 ns    | 372 ns      | 312 ns      | 346 ns       |
| **Medium (10-25)** | 396 ns    | 397 ns      | 391 ns      | 642 ns       |
| **Long (50-200)**  | 1,066 ns  | 1,074 ns    | 1,106 ns    | 7,480 ns     |

#### Low-Level Performance

| Operation                          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC | Description                  |
| ---------------------------------- | --------- | ----------- | ----------- | ------------ | ---------------------------- |
| **CPU Feature Detection (SSE4.2)** | 0.136 ns  | 0.387 ns    | 0.137 ns    | 0.976 ns     | Hardware capability check    |
| **Single Hash Step (FNV-1a)**      | 0.027 ns  | 0.215 ns    | 0.023 ns    | 0.770 ns     | Individual hash operation    |
| **Single Hash Step (CRC32)**       | 0.027 ns  | 0.216 ns    | 0.027 ns    | 0.726 ns     | Hardware-accelerated hashing |
| **Single Hash Step (Larson)**      | 0.027 ns  | 0.216 ns    | 0.027 ns    | 0.697 ns     | Alternative hash function    |
| **Seed Mix Function**              | 0.027 ns  | 0.216 ns    | 0.027 ns    | 0.700 ns     | CHD algorithm component      |

#### Integer Hashing Performance

##### nfx::hashInteger

| Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------- | --------- | ----------- | ----------- | ------------ |
| **uint32** | 164 ns    | 250 ns      | 164 ns      | 653 ns       |
| **uint64** | 550 ns    | 487 ns      | 550 ns      | 789 ns       |
| **int32**  | 164 ns    | 249 ns      | 164 ns      | 658 ns       |

##### std::hash

| Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------- | --------- | ----------- | ----------- | ------------ |
| **uint32** | 81.0 ns   | 54.6 ns     | 79.3 ns     | 958 ns       |
| **uint64** | 80.4 ns   | 80.5 ns     | 80.4 ns     | 1,842 ns     |

##### Single String Comparison

| Algorithm               | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::hashStringView** | 8.27 ns   | 8.13 ns     | 8.00 ns     | 10.7 ns      |
| **std::hash**           | 6.02 ns   | 5.51 ns     | 5.62 ns     | 18.6 ns      |

---

## Container Components

### ChdHashMap - Compress, Hash, and Displace Algorithm

> **Perfect hash maps using CHD (Compress, Hash, and Displace) algorithm for read-heavy workloads**

#### Construction Performance

| Benchmark        | linux GCC | linux clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **Small (10)**   | 1.36 μs   | 1.19 μs     | 3.12 μs     | 4.31 μs      |
| **Medium (100)** | 12.7 μs   | 11.4 μs     | 19.8 μs     | 21.8 μs      |
| **Large (500)**  | 56.0 μs   | 49.6 μs     | 188 μs      | 90.0 μs      |

#### ChdHashMapPerformance

| Operation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Lookup**    | 606 ns    | 601 ns      | 741 ns      | 1005 ns      |
| **Bracket Access**    | 634 ns    | 567 ns      | 667 ns      | 1132 ns      |
| **C-String Lookup**   | 656 ns    | 676 ns      | 1015 ns     | 1342 ns      |
| **StringView Lookup** | 597 ns    | 599 ns      | 741 ns      | 1028 ns      |
| **Complex Objects**   | 301 ns    | 280 ns      | 375 ns      | 508 ns       |

#### std::unordered_map Performance

| Operation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Lookup**    | 749 ns    | 741 ns      | 888 ns      | 946 ns       |
| **Bracket Access**    | 742 ns    | 744 ns      | 903 ns      | 1335 ns      |
| **C-String Lookup**   | 1,886 ns  | 1,883 ns    | 2635 ns     | 2895 ns      |
| **StringView Lookup** | 1,820 ns  | 1,806 ns    | 2463 ns     | 2358 ns      |
| **Complex Objects**   | 369 ns    | 369 ns      | 467 ns      | 604 ns       |

#### Iterator Performance

| Pattern                    | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Range-based For**        | 96.1 ns   | 253 ns      | 109 ns      | 115 ns       |
| **Complex Objects**        | 51.9 ns   | 146 ns      | 57.4 ns     | 66.4 ns      |
| **Conditional Processing** | 139 ns    | 270 ns      | 161 ns      | 200 ns       |
| **Early Termination**      | 20.9 ns   | 22.6 ns     | 7.64 ns     | 6.30 ns      |
| **Key Filtering**          | 327 ns    | 431 ns      | 543 ns      | 580 ns       |

#### Enumerator Performance

| Pattern                    | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Range-based For**        | 108 ns    | 113 ns      | 123 ns      | 129 ns       |
| **Complex Objects**        | 66.4 ns   | 48.2 ns     | 64.5 ns     | 84.2 ns      |
| **Conditional Processing** | 133 ns    | 135 ns      | 156 ns      | 160 ns       |
| **Early Termination**      | 7.63 ns   | 8.40 ns     | 6.62 ns     | 6.89 ns      |
| **Key Filtering**          | 312 ns    | 297 ns      | 575 ns      | 560 ns       |

### HashMap - Robin Hood Hashing

> **Hash map with Robin Hood collision resolution for general-purpose mapping**

#### HashMap Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | 5.65 μs   | 4.96 μs     | 7.71 μs     | 8.20 μs      |
| **String Lookup**     | 0.623 μs  | 0.577 μs    | 0.834 μs    | 1.07 μs      |
| **C-String Lookup**   | 0.800 μs  | 0.840 μs    | 1.31 μs     | 1.52 μs      |
| **StringView Lookup** | 0.600 μs  | 0.676 μs    | 0.953 μs    | 1.25 μs      |
| **Complex Insert**    | 4.84 μs   | 4.47 μs     | 7.31 μs     | 5.71 μs      |
| **Erase Operations**  | 1.10 μs   | 1.08 μs     | 1.45 μs     | 1.76 μs      |

#### std::unordered_map Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | 2.70 μs   | 2.66 μs     | 8.88 μs     | 9.95 μs      |
| **String Lookup**     | 0.746 μs  | 0.743 μs    | 0.890 μs    | 1.13 μs      |
| **C-String Lookup**   | 1.91 μs   | 1.87 μs     | 2.62 μs     | 2.51 μs      |
| **StringView Lookup** | 1.88 μs   | 1.87 μs     | 2.61 μs     | 2.46 μs      |
| **Complex Insert**    | 2.58 μs   | 2.73 μs     | 3.59 μs     | 5.15 μs      |
| **Erase Operations**  | 1.52 μs   | 1.41 μs     | 2.64 μs     | 2.71 μs      |

#### Heterogeneous Operations

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **C-String Erase**    | 1.26 μs   | 1.22 μs     | 1.69 μs     | 2.00 μs      |
| **StringView Erase**  | 1.17 μs   | 1.09 μs     | 1.53 μs     | 1.91 μs      |
| **Zero-Alloc Lookup** | 0.886 μs  | 0.874 μs    | 1.31 μs     | 1.54 μs      |

#### Large Dataset Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Insert 1K items**   | 118 μs    | 107 μs      | 149 μs      | 127 μs       |
| **Lookup 1K items**   | 7.27 μs   | 6.50 μs     | 8.91 μs     | 10.3 μs      |
| **Worst-case Insert** | 13.0 μs   | 12.2 μs     | 24.2 μs     | 25.7 μs      |
| **Mixed Operations**  | 3.83 μs   | 3.47 μs     | 5.73 μs     | 5.06 μs      |

### StringMap - String-Optimized Mapping

> **Hash map optimized for string keys with heterogeneous lookup capabilities**

#### StringMap Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | 3.63 μs   | 3.34 μs     | 13.2 μs     | 7.68 μs      |
| **C-String Lookup**   | 1.29 μs   | 1.15 μs     | 1.91 μs     | 1.80 μs      |
| **StringView Lookup** | 1.09 μs   | 0.971 μs    | 1.42 μs     | 1.42 μs      |
| **Complex Insert**    | 4.07 μs   | 3.88 μs     | 5.60 μs     | 5.91 μs      |

#### std::unordered_map<std::string, T> Performance

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **Integer Insert**    | 2.67 μs   | 2.70 μs     | 17.9 μs     | 7.90 μs      |
| **C-String Lookup**   | 1.91 μs   | 1.85 μs     | 2.66 μs     | 2.58 μs      |
| **StringView Lookup** | 1.86 μs   | 1.85 μs     | 2.52 μs     | 2.54 μs      |
| **Complex Insert**    | 3.66 μs   | 3.63 μs     | 5.27 μs     | 5.86 μs      |

#### Specialized Operations

| Operation Type             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **Complex try_emplace**    | 4.07 μs   | 4.15 μs     | 5.76 μs     | 5.98 μs      |
| **Cache insert_or_assign** | 1.64 μs   | 1.66 μs     | 3.62 μs     | 3.42 μs      |
| **Mixed Operations**       | 3.78 μs   | 3.59 μs     | 6.46 μs     | 5.39 μs      |
| **Zero-Allocation Lookup** | 1.27 μs   | 1.09 μs     | 1.82 ns     | 1.87 μs      |

### StringSet - Heterogeneous String Collections

> **String set with zero-allocation heterogeneous lookup capabilities for string collections**

#### StringSet Performance

| Operation Type          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **String Insert**       | 3.67 μs   | 3.54 μs     | 7.89 μs     | 6.71 μs      |
| **C-String Insert**     | 4.35 μs   | 4.16 μs     | 9.27 μs     | 7.29 μs      |
| **StringView Insert**   | 4.37 μs   | 4.14 μs     | 8.76 μs     | 7.19 μs      |
| **C-String Find**       | 1.15 μs   | 1.17 μs     | 1.63 μs     | 1.79 μs      |
| **StringView Find**     | 1.07 μs   | 0.972 μs    | 1.33 μs     | 1.45 μs      |
| **C-String Contains**   | 1.07 μs   | 0.979 μs    | 1.46 μs     | 1.21 μs      |
| **StringView Contains** | 1.09 μs   | 0.976 μs    | 1.46 μs     | 1.19 μs      |

#### std::unordered_set<std::string> Performance

| Operation Type          | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ----------------------- | --------- | ----------- | ----------- | ------------ |
| **String Insert**       | 3.03 μs   | 2.94 μs     | 7.70 μs     | 6.74 μs      |
| **C-String Insert**     | 3.47 μs   | 3.38 μs     | 8.18 μs     | 7.44 μs      |
| **StringView Insert**   | 3.51 μs   | 3.40 μs     | 8.18 μs     | 7.16 μs      |
| **C-String Find**       | 1.91 μs   | 1.85 μs     | 2.43 μs     | 2.32 μs      |
| **StringView Find**     | 1.86 μs   | 1.88 μs     | 2.43 μs     | 2.35 μs      |
| **C-String Contains**   | 1.91 μs   | 1.85 μs     | 2.39 μs     | 2.37 μs      |
| **StringView Contains** | 1.86 μs   | 1.88 μs     | 2.43 μs     | 2.54 μs      |

#### Advanced Operations

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Mixed Operations**   | 3.60 μs   | 3.39 μs     | 6.95 μs     | 4.19 μs      |
| **Set Operations**     | 6.10 μs   | 5.87 μs     | 12.6 μs     | 9.53 μs      |
| **Zero-Alloc Lookup**  | 1.16 μs   | 1.17 μs     | 1.74 μs     | 1.76 μs      |
| **Duplicate Handling** | 4.12 μs   | 3.78 μs     | 16.8 μs     | 4.85 μs      |

#### Large Dataset Performance

| Operation Type      | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------- | --------- | ----------- | ----------- | ------------ |
| **Insert 1K items** | 96.7 μs   | 93.7 μs     | 136 μs      | 111 μs       |
| **Lookup 1K items** | 11.8 μs   | 11.3 μs     | 16.5 μs     | 12.6 μs      |

---

## String Components

### Splitter - Zero-Copy String Splitting

> **String splitting with zero memory allocations**

#### Manual Split Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | 23.4 ns   | 21.3 ns     | 34.2 ns     | 24.7 ns      |
| **Path Data (7 segments)** | 19.3 ns   | 18.0 ns     | 30.7 ns     | 22.2 ns      |
| **Config Data (6 pairs)**  | 14.9 ns   | 14.7 ns     | 32.5 ns     | 19.5 ns      |

#### nfx::Splitter Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | 23.0 ns   | 23.0 ns     | 32.1 ns     | 24.5 ns      |
| **Path Data (7 segments)** | 19.3 ns   | 19.1 ns     | 27.7 ns     | 25.5 ns      |
| **Config Data (6 pairs)**  | 15.2 ns   | 16.1 ns     | 32.8 ns     | 20.6 ns      |

#### nfx::splitView Performance

| Data Type                  | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **CSV Data (8 fields)**    | 23.1 ns   | 23.2 ns     | 32.4 ns     | 24.7 ns      |
| **Path Data (7 segments)** | 19.4 ns   | 19.1 ns     | 27.5 ns     | 25.8 ns      |
| **Config Data (6 pairs)**  | 15.3 ns   | 15.6 ns     | 33.3 ns     | 20.9 ns      |

#### Zero-Allocation Operations

| Operation Type     | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **Zero-Alloc CSV** | 21.5 ns   | 22.8 ns     | 30.7 ns     | 21.2 ns      |

### StringBuilderPool - Thread-Safe String Building

> **String building with three-tier optimization strategy for high-performance applications**

#### StringBuilderPool Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | 42.5 ns   | 36.3 ns     | 115 ns      | 67.7 ns      |
| **Medium Strings (4)** | 61.3 ns   | 54.1 ns     | 121 ns      | 90.6 ns      |
| **Large Strings (3)**  | 70.3 ns   | 53.4 ns     | 116 ns      | 83.0 ns      |
| **Mixed Operations**   | 164 ns    | 162 ns      | 234 ns      | 260 ns       |

#### std::string Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | 27.9 ns   | 27.0 ns     | 48.7 ns     | 49.4 ns      |
| **Medium Strings (4)** | 64.0 ns   | 59.9 ns     | 138 ns      | 154 ns       |
| **Large Strings (3)**  | 60.8 ns   | 58.9 ns     | 114 ns      | 119 ns       |
| **Mixed Operations**   | 73.8 ns   | 77.6 ns     | 133 ns      | 200 ns       |

#### std::ostringstream Performance

| String Size Category   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **Small Strings (5)**  | 239 ns    | 228 ns      | 192 ns      | 376 ns       |
| **Medium Strings (4)** | 268 ns    | 258 ns      | 218 ns      | 657 ns       |
| **Large Strings (3)**  | 290 ns    | 283 ns      | 293 ns      | 684 ns       |
| **Mixed Operations**   | 483 ns    | 470 ns      | 434 ns      | 1,496 ns     |

#### Pool-Specific Features

| Feature             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC | Description                          |
| ------------------- | --------- | ----------- | ----------- | ------------ | ------------------------------------ |
| **Zero-Allocation** | 49.6 ns   | 44.3 ns     | 114 ns      | 70.0 ns      | string_view access without copy      |
| **Pool Efficiency** | 377 ns    | 370 ns      | 970 ns      | 769 ns       | Rapid lease/return cycles (10x)      |
| **Buffer Reuse**    | 548 ns    | 570 ns      | 905 ns      | 970 ns       | Multiple consecutive operations (5x) |
| **Rapid Cycles**    | 150 ns    | 136 ns      | 339 ns      | 430 ns       | std::string equivalent comparison    |

### StringUtilities - Fast String Operations

> **Optimized string utility functions**

#### Character Classification Performance

##### nfx::string::Utils

| Operation Type   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **isDigit**      | 3.90 ns   | 4.53 ns     | 3.84 ns     | 10.4 ns      |
| **isWhitespace** | 5.14 ns   | 5.65 ns     | 5.02 ns     | 18.1 ns      |
| **isAlpha**      | 4.04 ns   | 4.59 ns     | 3.82 ns     | 24.2 ns      |

##### Standard Library

| Operation Type   | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------- | --------- | ----------- | ----------- | ------------ |
| **isDigit**      | 3.88 ns   | 4.48 ns     | 3.87 ns     | 30.0 ns      |
| **isWhitespace** | 21.4 ns   | 20.3 ns     | 20.9 ns     | 27.6 ns      |
| **isAlpha**      | 21.0 ns   | 25.7 ns     | 22.1 ns     | 29.3 ns      |

#### String Validation Performance

##### nfx::string::Utils

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **isEmpty**            | 3.06 ns   | 6.11 ns     | 2.98 ns     | 14.9 ns      |
| **isNullOrWhiteSpace** | 20.2 ns   | 15.7 ns     | 22.7 ns     | 33.3 ns      |
| **isAllDigits**        | 32.9 ns   | 16.8 ns     | 28.7 ns     | 36.5 ns      |

##### Standard/Manual Implementation

| Operation Type         | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ---------------------- | --------- | ----------- | ----------- | ------------ |
| **isEmpty**            | 3.04 ns   | 6.10 ns     | 3.00 ns     | 15.1 ns      |
| **isNullOrWhiteSpace** | 49.7 ns   | 42.3 ns     | 52.0 ns     | 55.7 ns      |
| **isAllDigits**        | 32.3 ns   | 18.4 ns     | 27.8 ns     | 72.3 ns      |

#### String Operations Performance

##### nfx::string::Utils

| Operation Type | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------- | --------- | ----------- | ----------- | ------------ |
| **startsWith** | 46.6 ns   | 40.5 ns     | 11.7 ns     | 30.9 ns      |
| **endsWith**   | 39.4 ns   | 8.65 ns     | 12.3 ns     | 25.2 ns      |
| **contains**   | 51.6 ns   | 49.7 ns     | 68.9 ns     | 147 ns       |

##### Standard Library

| Operation Type | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------- | --------- | ----------- | ----------- | ------------ |
| **startsWith** | 45.4 ns   | 43.0 ns     | 8.10 ns     | 26.8 ns      |
| **endsWith**   | 41.4 ns   | 10.4 ns     | 9.80 ns     | 29.2 ns      |
| **contains**   | 53.5 ns   | 43.7 ns     | 74.2 ns     | 151 ns       |

#### String Trimming Performance

| Implementation        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::string::trim** | 38.2 ns   | 35.0 ns     | 31.0 ns     | 46.2 ns      |
| **Manual trim**       | 126 ns    | 30.5 ns     | 37.8 ns     | 251 ns       |

#### Case Conversion Performance

##### nfx::string::Utils

| Operation Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------- | --------- | ----------- | ----------- | ------------ |
| **toLower (char)**   | 3.22 ns   | 4.67 ns     | 4.94 ns     | 12.7 ns      |
| **toUpper (char)**   | 3.20 ns   | 5.79 ns     | 3.57 ns     | 12.8 ns      |
| **toLower (string)** | 390 ns    | 308 ns      | 333 ns      | 761 ns       |
| **toUpper (string)** | 409 ns    | 379 ns      | 335 ns      | 762 ns       |

##### Standard Library

| Operation Type       | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------- | --------- | ----------- | ----------- | ------------ |
| **toLower (char)**   | 24.2 ns   | 25.6 ns     | 13.6 ns     | 28.9 ns      |
| **toUpper (char)**   | 21.7 ns   | 21.0 ns     | 13.4 ns     | 34.1 ns      |
| **toLower (string)** | 484 ns    | 475 ns      | 389 ns      | 494 ns       |
| **toUpper (string)** | 448 ns    | 408 ns      | 389 ns      | 599 ns       |

#### Parsing Performance

##### nfx::string::Utils

| Operation Type     | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| ------------------ | --------- | ----------- | ----------- | ------------ |
| **tryParseInt**    | 15.4 ns   | 13.6 ns     | 10.8 ns     | 40.0 ns      |
| **tryParseDouble** | 42.4 ns   | 41.2 ns     | 52.7 ns     | 124 ns       |

##### Standard Library (std::from_chars)

| Operation Type        | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| --------------------- | --------- | ----------- | ----------- | ------------ |
| **from_chars_int**    | 13.6 ns   | 11.7 ns     | 10.8 ns     | 26.1 ns      |
| **from_chars_double** | 39.7 ns   | 39.2 ns     | 52.4 ns     | 115 ns       |

#### URI Classification Performance

| Implementation             | Linux GCC | Linux Clang | Windows GCC | Windows MSVC |
| -------------------------- | --------- | ----------- | ----------- | ------------ |
| **nfx::isURIReserved**     | 6.94 ns   | 7.24 ns     | 5.59 ns     | 22.2 ns      |
| **Manual isURIReserved**   | 11.4 ns   | 6.29 ns     | 5.53 ns     | 22.1 ns      |
| **nfx::isURIUnreserved**   | 7.52 ns   | 9.43 ns     | 7.54 ns     | 21.5 ns      |
| **Manual isURIUnreserved** | 7.49 ns   | 9.49 ns     | 7.57 ns     | 24.3 ns      |

---

_Benchmarked on September 28, 2025_
