# IEEE-754 FP32 Implementation

An implementation of IEEE-754 single-precision (binary32) floating-point arithmetic in C++ for experimental purposes.

## Overview 

This implements floating-point arithmetic at the bit level, demonstrating how IEEE-754 single-precision numbers work internally. This implementation works directly on the 32-bit representation.

## IEEE-754 Binary32 Format

Sign (1 bit) | Exponent (8 bits) | Mantissa / Significand (23 bits)

- Sign bit 0 implies positive, 1 is negative
- Exponent is biased by 127 (stored value - 127 = actual value) 
- Mantissa has an implicit leading 1 for normal numbers 

### Special Values

| Value | Exponent | Mantissa | Example |
|-------|----------|----------|---------|
| Zero | 0 | 0 | ±0 |
| Subnormal | 0 | ≠ 0 | Tiny numbers |
| Normal | 1-254 | Any | Regular numbers |
| Infinity | 255 | 0 | ±∞ |
| NaN | 255 | ≠ 0 | Not a Number |

### Calculating the Value

The value $V$ is calculated using the following formula:

$$V = (-1)^S \times (1.M) \times 2^{E - 127}$$

## Features 

### Core Features

- Bit-level representation using uint32_t
- Construction from various types (float, double, int, raw bits)
- Complete arithmetic operations (+, -, *, /)
- Comparison operators (==, !=, <, <=, >, >=)
- Mathematical functions (abs, sqrt)
- Special value handling (NaN, Infinity, Zero)
- Subnormal number support

## Educational Features

- Bit pattern visualization
- Component extraction
- Classification methods 

## Project Structure

.
├── FP32.h
├── Makefile
├── README.md
├── example.cpp
├── fp32_arithmetic.cpp
├── fp32_basic.cpp
├── fp32_comparison.cpp
├── fp32_io.cpp
└── fp32_test.cpp

## Building

### Prerequisites

- C++ 17 
- Make 

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
```

## Testing

The included tests check:

- Construction methods
- Special value handling
- Arithmetic operations
- Comparison operations
- Mathematical functions
- Edge cases
- Precision loss scenerios

## References

- [IEEE-754 Standard](https://en.wikipedia.org/wiki/IEEE_754)
- [What Every Computer Scientist Should Know About Floating-Point Arithmetic](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
- [Floating Point Demystified](http://www.volkerschatz.com/science/float.html)
