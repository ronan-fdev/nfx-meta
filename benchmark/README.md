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

| Platform    | Benchmark Framework     | C++ Compiler         | OS Version |
| ----------- | ----------------------- | -------------------- | ---------- |
| **Windows** | Google Benchmark v1.9.4 | VC++ 19.44.35214-x64 | Windows 10 |

---

# Performance Results

## Core Components

### Hashing Algorithms

> **Hashing functions optimized for string and integer**

#### String Hashing Performance

| Algorithm               | Short (3-8 chars) | Medium (10-25 chars) | Long (50-200 chars) | vs `std::hash` |
| ----------------------- | ----------------- | -------------------- | ------------------- | -------------- |
| **nfx::HashStringView** | **315 ns**        | **537 ns**           | **5,079 ns**        | **43% faster** |
| Manual FNV-1a           | 222 ns            | 594 ns               | 7,374 ns            | 23% faster     |
| Manual CRC32            | 229 ns            | 492 ns               | 4,959 ns            | 33% faster     |
| `std::hash`             | 324 ns            | 586 ns               | 7,410 ns            | _baseline_     |

#### Low-Level Performance

| Operation                          | Time         | Description                  |
| ---------------------------------- | ------------ | ---------------------------- |
| **CPU Feature Detection (SSE4.2)** | **0.681 ns** | Hardware capability check    |
| **Single Hash Step (FNV-1a)**      | **0.639 ns** | Individual hash operation    |
| **Single Hash Step (CRC32)**       | **0.649 ns** | Hardware-accelerated hashing |
| **Single Hash Step (Larson)**      | **0.642 ns** | Alternative hash function    |

#### Integer Hashing

| Type              | nfx::Hash   | `std::hash` | **Performance Gain** |
| ----------------- | ----------- | ----------- | -------------------- |
| **uint32**        | **620 ns**  | 926 ns      | **33% faster**       |
| **uint64**        | **748 ns**  | 1,698 ns    | **56% faster**       |
| **Single String** | **10.3 ns** | 18.1 ns     | **43% faster**       |

---

## Container Components

### ChdHashMap - Compress, Hash, and Displace Algorithm

> **Perfect hash maps using CHD (Compress, Hash, and Displace) algorithm for read-heavy workloads**

#### Construction Performance

| Dataset Size     | Construction Time | Use Case            |
| ---------------- | ----------------- | ------------------- |
| **Small (100)**  | **3.93 μs**       | Configuration maps  |
| **Medium (500)** | **20.9 μs**       | Lookup tables       |
| **Large (1000)** | **85.0 μs**       | Static dictionaries |

#### Lookup Performance vs `std::unordered_map`

| Operation             | ChdHashMap   | `std::unordered_map` | **Performance Gain** |
| --------------------- | ------------ | -------------------- | -------------------- |
| **Integer Lookup**    | 967 ns       | **897 ns**           | -8%                  |
| **Bracket Access**    | **997 ns**   | 1,148 ns             | **13% faster**       |
| **C-String Lookup**   | **1,253 ns** | 2,296 ns             | **45% faster**       |
| **StringView Lookup** | **968 ns**   | 2,158 ns             | **55% faster**       |
| **Complex Objects**   | **484 ns**   | 571 ns               | **15% faster**       |

#### Iterator vs Enumerator Performance

| Pattern                    | Iterator    | Enumerator |
| -------------------------- | ----------- | ---------- |
| **Range-based For**        | **111 ns**  | 123 ns     |
| **Complex Objects**        | **62.6 ns** | 79.6 ns    |
| **Conditional Processing** | 195 ns      | **150 ns** |
| **Early Termination**      | **6.21 ns** | 6.33 ns    |
| **Key Filtering**          | 555 ns      | **542 ns** |

### HashMap - Robin Hood Hashing

> **Hash map with Robin Hood collision resolution for general-purpose mapping**

#### Performance vs `std::unordered_map`

| Operation Type        | HashMap     | `std::unordered_map` | **Performance Gain** |
| --------------------- | ----------- | -------------------- | -------------------- |
| **Integer Insert**    | 8.43 μs     | **6.92 μs**          | -22%                 |
| **String Lookup**     | **1.06 μs** | 1.17 μs              | **9% faster**        |
| **C-String Lookup**   | **1.57 μs** | 2.49 μs              | **37% faster**       |
| **StringView Lookup** | **1.30 μs** | 2.52 μs              | **48% faster**       |
| **Complex Insert**    | 6.09 μs     | **4.57 μs**          | -33%                 |
| **Erase Operations**  | **1.79 μs** | 2.76 μs              | **35% faster**       |

#### Heterogeneous Operations

- **C-String Erase:** **1.98 μs**
- **StringView Erase:** **1.93 μs**
- **Zero-Alloc Lookup:** **1.56 μs**

#### Large Dataset Performance

- **Insert 10K items:** **123 μs**
- **Lookup 1K items:** **10.4 μs**
- **Worst-case Insert:** **26.4 μs**
- **Mixed Operations:** **5.20 μs** per cycle

### StringMap - String-Optimized Mapping

> **Hash map optimized for string keys with heterogeneous lookup capabilities**

#### Performance vs `std::unordered_map<std::string, T>`

| Operation Type        | StringMap   | `std::unordered_map` | **Performance Gain** |
| --------------------- | ----------- | -------------------- | -------------------- |
| **Integer Insert**    | **6.69 μs** | 6.88 μs              | **3% faster**        |
| **C-String Lookup**   | **1.78 μs** | 2.39 μs              | **25% faster**       |
| **StringView Lookup** | **1.40 μs** | 2.53 μs              | **45% faster**       |
| **Complex Insert**    | 7.02 μs     | **5.88 μs**          | -19%                 |

#### Specialized Operations

- **Complex `try_emplace`:** **6.20 μs**
- **Cache `insert_or_assign`:** **3.52 μs**
- **Mixed Operations:** **4.49 μs** per cycle
- **Zero-Allocation Lookup:** **1.83 μs** _(StringView heterogeneous)_

### StringSet - Heterogeneous String Collections

> **String set with zero-allocation heterogeneous lookup capabilities for string collections**

#### Performance vs `std::unordered_set<std::string>`

| Operation Type          | StringSet   | `std::unordered_set` | **Performance Gain** |
| ----------------------- | ----------- | -------------------- | -------------------- |
| **String Insert**       | **6.73 μs** | 6.73 μs              | **Equivalent**       |
| **C-String Insert**     | **7.24 μs** | 7.28 μs              | **Equivalent**       |
| **StringView Insert**   | **7.29 μs** | 7.42 μs              | **2% faster**        |
| **C-String Find**       | **1.77 μs** | 2.31 μs              | **23% faster**       |
| **StringView Find**     | **1.41 μs** | 2.36 μs              | **40% faster**       |
| **C-String Contains**   | **1.21 μs** | 2.34 μs              | **48% faster**       |
| **StringView Contains** | **1.18 μs** | 2.49 μs              | **53% faster**       |

#### Advanced Operations

- **Mixed Operations:** **4.10 μs** per cycle
- **Set Operations:** **9.57 μs** _(union, intersection, etc.)_
- **Zero-Alloc Lookup:** **1.76 μs**
- **Duplicate Handling:** **4.93 μs**

#### Large Dataset Performance

- **Insert 10K items:** **104 μs**
- **Lookup 1K items:** **12.6 μs**

---

## String Components

### Splitter - Zero-Copy String Splitting

> **String splitting with zero memory allocations**

#### Performance Comparison

| Data Type                  | Manual Split | nfx::Splitter | nfx::splitView | **Performance Gain** |
| -------------------------- | ------------ | ------------- | -------------- | -------------------- |
| **CSV Data (8 fields)**    | 28.3 ns      | **27.3 ns**   | **27.2 ns**    | **4% faster**        |
| **Path Data (7 segments)** | **22.3 ns**  | 25.2 ns       | 24.8 ns        | -11% _(slower)_      |
| **Config Data (6 pairs)**  | **17.9 ns**  | 20.3 ns       | 20.6 ns        | -13% _(slower)_      |

### StringBuilderPool - Thread-Safe String Building

> **String building with three-tier optimization strategy for high-performance applications**

#### Performance vs Standard Alternatives

| String Size Category   | StringBuilderPool | `std::string` | `std::ostringstream` |
| ---------------------- | ----------------- | ------------- | -------------------- |
| **Small Strings (5)**  | 68.8 ns           | **51.7 ns**   | 383 ns               |
| **Medium Strings (4)** | **89.0 ns**       | 1014 ns       | 1508 ns              |
| **Large Strings (3)**  | **82.1 ns**       | 115 ns        | 1629 ns              |
| **Mixed Operations**   | **258 ns**        | 219 ns        | 2298 ns              |

#### Performance Gains

| Operation Type        | StringBuilderPool | vs `std::string` | vs `std::ostringstream` |
| --------------------- | ----------------- | ---------------- | ----------------------- |
| **Small String Ops**  | 68.8 ns           | **25% slower**   | **457% faster**         |
| **Medium String Ops** | **89.0 ns**       | **1039% faster** | **1595% faster**        |
| **Large String Ops**  | **82.1 ns**       | **29% faster**   | **1885% faster**        |
| **Mixed Operations**  | 258 ns            | **15% slower**   | **790% faster**         |

#### Pool-Specific Features

| Feature             | Performance | Description                          |
| ------------------- | ----------- | ------------------------------------ |
| **Zero-Allocation** | **68.6 ns** | string_view access without copy      |
| **Pool Efficiency** | 748 ns      | Rapid lease/return cycles (10x)      |
| **Buffer Reuse**    | 5,032 ns    | Multiple consecutive operations (5x) |
| **Memory Pressure** | 1,925 ns    | Large string concatenation (20x)     |

### StringUtilities - Fast String Operations

> **Optimized string utility functions**

#### Character Classification Performance

| Operation Type   | nfx::string::Utils | Standard Library | **Performance Gain** |
| ---------------- | ------------------ | ---------------- | -------------------- |
| **isDigit**      | **10.3 ns**        | 30.1 ns          | **192% faster**      |
| **isWhitespace** | **18.2 ns**        | 27.5 ns          | **51% faster**       |
| **isAlpha**      | **23.7 ns**        | 27.7 ns          | **17% faster**       |

#### String Validation Performance

| Operation Type         | nfx::string::Utils | Manual Implementation | **Performance Gain** |
| ---------------------- | ------------------ | --------------------- | -------------------- |
| **isEmpty**            | 15.1 ns            | **14.7 ns**           | -3% _(equivalent)_   |
| **isNullOrWhiteSpace** | **29.8 ns**        | 54.7 ns               | **83% faster**       |
| **isAllDigits**        | **37.9 ns**        | 71.7 ns               | **89% faster**       |

#### String Operations Performance

| Operation Type | nfx::string::Utils | Standard Library | **Performance Gain** |
| -------------- | ------------------ | ---------------- | -------------------- |
| **startsWith** | **33.1 ns**        | **32.9 ns**      | **Equivalent**       |
| **endsWith**   | **31.6 ns**        | 31.8 ns          | **1% faster**        |
| **contains**   | **138 ns**         | 139 ns           | **1% faster**        |

#### String Trimming Performance

| Operation Type | nfx::string::Utils | Manual Implementation | **Performance Gain** |
| -------------- | ------------------ | --------------------- | -------------------- |
| **trim**       | **40.6 ns**        | 246 ns                | **506% faster**      |

#### Case Conversion Performance

| Operation Type       | nfx::string::Utils | Standard Library | **Performance Gain** |
| -------------------- | ------------------ | ---------------- | -------------------- |
| **toLower (char)**   | **12.9 ns**        | 28.9 ns          | **124% faster**      |
| **toUpper (char)**   | **12.9 ns**        | 32.3 ns          | **150% faster**      |
| **toLower (string)** | **56.4 ns**        | 89.7 ns          | **59% faster**       |
| **toUpper (string)** | **55.1 ns**        | 89.8 ns          | **63% faster**       |

#### Parsing Performance

| Operation Type     | nfx::string::Utils | Standard Library | **Performance Gain** |
| ------------------ | ------------------ | ---------------- | -------------------- |
| **tryParseInt**    | **45.2 ns**        | 67.8 ns          | **50% faster**       |
| **tryParseDouble** | **78.9 ns**        | 134 ns           | **70% faster**       |

#### URI Classification Performance

| Operation Type      | nfx::string::Utils | Manual Implementation | **Performance Gain** |
| ------------------- | ------------------ | --------------------- | -------------------- |
| **isURIReserved**   | **21.4 ns**        | 21.5 ns               | **Equivalent**       |
| **isURIUnreserved** | **20.9 ns**        | 23.9 ns               | **12% faster**       |

---

_Benchmarked on September 21, 2025_
