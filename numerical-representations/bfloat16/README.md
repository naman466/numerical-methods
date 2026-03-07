# BFloat16 Educational Implementation

An implementation of BFloat16 (Brain Floating Point) format in C++.

## Overview

BFloat16 is a 16-bit floating-point format developed by Google Brain for machine learning applications. Unlike IEEE-754 half-precision (FP16), BFloat16 truncates FP32 to 16 bits while preserving the 8-bit exponent, trading precision for dynamic range.

### BFloat16 Format (16-bit)

```
Sign (1 bit) | Exponent (8 bits) | Mantissa (7 bits)
     S       |    EEEEEEEE       |   MMMMMMM
```

- **Sign bit**: 0 = positive, 1 = negative
- **Exponent**: 8 bits, biased by 127 (same as FP32!)
- **Mantissa**: 7 bits with implicit leading 1 for normal numbers

### Comparison with Other Formats

| Format | Total Bits | Sign | Exponent | Mantissa | Precision | Range |
|--------|-----------|------|----------|----------|-----------|-------|
| FP32 | 32 | 1 | 8 | 23 | ~7 digits | ±3.4e38 |
| FP16 | 16 | 1 | 5 | 10 | ~3 digits | ±6.5e4 |
| **BFloat16** | **16** | **1** | **8** | **7** | **~2-3 digits** | **±3.4e38** |


## Features

### Core Functionality
- ✅ Bit-level representation using `uint16_t`
- ✅ Construction from various types (float, double, int, raw bits)
- ✅ Complete arithmetic operations (+, -, *, /)
- ✅ Comparison operators (==, !=, <, <=, >, >=)
- ✅ Mathematical functions (abs, sqrt)
- ✅ Special value handling (NaN, Infinity, Zero)
- ✅ Subnormal number support
- ✅ Easy FP32 ↔ BFloat16 conversion


## Project Structure

```
.
├── BFloat16.h              # Main header file with class declaration
├── BFloat16_basic.cpp      # Constructors, accessors, conversions
├── BFloat16_arithmetic.cpp # Addition, subtraction, multiplication, division
├── BFloat16_comparison.cpp # Comparison operators and math functions
├── BFloat16_io.cpp         # Stream I/O operators
├── test_bfloat16.cpp       # Comprehensive test suite
├── example_bfloat16.cpp    # Usage examples
├── Makefile_bfloat16       # Build configuration
└── README_BFLOAT16.md      # This file
```

## Building

### Using Make

```bash
# Build the test program
make

# Build and run
make run

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Build with debug symbols
make debug

## Testing

The test suite demonstrates:
- ✓ Construction and conversion
- ✓ Special value handling
- ✓ Arithmetic operations
- ✓ Comparison operations
- ✓ Mathematical functions
- ✓ Edge cases (division by zero, overflow, underflow)
- ✓ FP32 ↔ BFloat16 conversion accuracy
- ✓ Precision loss scenarios
- ✓ Dynamic range verification

Run tests:
```bash
make test
```

## References

- [BFloat16: The secret to high performance on Cloud TPUs](https://cloud.google.com/blog/products/ai-machine-learning/bfloat16-the-secret-to-high-performance-on-cloud-tpus)
- [BFloat16 on Wikipedia](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)
