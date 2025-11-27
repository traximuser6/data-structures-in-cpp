### C++ Fundamental Data Types – Sizes & Ranges (Typical 64-bit systems)

| Category       | Type          | Size (bytes) | Typical Range                                                |
|----------------|---------------|--------------|--------------------------------------------------------------|
| Boolean        | `bool`        | 1            | `false` (0) or `true` (1)                                    |
| Characters     | `char`        | 1            | -128 to 127 (signed) or 0 to 255 (unsigned)                  |
|                | `wchar_t`     | 2 or 4       | Windows: 0 to 65 535<br>Linux/macOS: 0 to 1 114 111 (UTF-32) |
|                | `char16_t`    | 2            | 0 to 65 535                                                  |
|                | `char32_t`    | 4            | 0 to 4 294 967 295                                           |
| Integers       | `short`       | 2            | -32 768 to 32 767                                            |
|                | `int`         | 4            | -2 147 483 648 to 2 147 483 647                              |
|                | `long`        | 4 or 8       | ≥ -2³¹ to 2³¹-1 (8 bytes on Windows/Linux/macOS 64-bit)      |
|                | `long long`   | 8            | -9.22×10¹⁸ to 9.22×10¹⁸                                      |
| Floating-point | `float`       | 4            | ≈ ±3.4 × 10³⁸ (7 decimal digits)                             |
|                | `double`      | 8            | ≈ ±1.7 × 10³⁰⁸ (15 decimal digits)                           |
|                | `long double` | 8, 12 or 16  | Usually same as `double` or 80-bit extended (~18–20 digits)  |

### Common sizes on 64-bit platforms (2025)

```cpp
sizeof(bool)        == 1
sizeof(char)        == 1
sizeof(short)       == 2
sizeof(int)         == 4
sizeof(long)        == 8    // Linux/macOS (LP64), also Windows 64-bit
sizeof(long long)   == 8
sizeof(float)       == 4
sizeof(double)      == 8
sizeof(long double) == 16   // x86-64 with 80-bit extended precision (padded)
```

### Exact limits (use these in code)

```cpp
#include <limits>
std::numeric_limits<int>::max();          // 2147483647
std::numeric_limits<long long>::max();    // 9223372036854775807
std::numeric_limits<double>::max();       // ~1.797e308
```

