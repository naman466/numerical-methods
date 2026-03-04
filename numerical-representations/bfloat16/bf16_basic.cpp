#include "BF16.h"
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

// constructors

BFloat16::BFloat16() : bits_(0) {
    // default constructor zero (pos)
}

BFloat16::BFloat16(uint16_t bits) : bits_(bits) {
}

BFloat16::BFloat16(float value) {
    // bf16 is  the upper 16 bits of FP32
    //  can convert by truncating with rounding
    uint32_t fp32_bits;
    std::memcpy(&fp32_bits, &value, sizeof(float));
    
    // round to nearest even 
    // check bit 16 (first bit being truncated) and bits 15-0
    uint32_t rounding_bias = 0x7FFF + ((fp32_bits >> 16) & 1);
    uint32_t rounded = fp32_bits + rounding_bias;
    
    // take upper 16 bits
    bits_ = static_cast<uint16_t>(rounded >> 16);
}

BFloat16::BFloat16(double value) : BFloat16(static_cast<float>(value)) {
    // convert through float
}

BFloat16::BFloat16(int value) : BFloat16(static_cast<float>(value)) {
}

// named constructors

BFloat16 BFloat16::fromBits(uint16_t bits) {
    return BFloat16(bits);
}

BFloat16 BFloat16::zero(bool negative) {
    return BFloat16(static_cast<uint16_t>(negative ? SIGN_MASK : 0));
}

BFloat16 BFloat16::infinity(bool negative) {
    return BFloat16(static_cast<uint16_t>((negative ? SIGN_MASK : 0) | EXPONENT_MASK));
}

BFloat16 BFloat16::nan() {
    // quiet nan exponent all 1s, mantissa non-zero with MSB set
    return BFloat16(static_cast<uint16_t>(EXPONENT_MASK | 0x0040u));
}

BFloat16 BFloat16::epsilon() {
    // machine epsilon for BFloat16: 2^-7 (7 bit mantissa)
    // smallest value such that 1.0 + epsilon != 1.0
    return BFloat16::fromBits(0x3C00u); // exponent = 120 (127-7), mantissa = 0
}

// accessors

bool BFloat16::sign() const {
    return (bits_ & SIGN_MASK) != 0;
}

uint8_t BFloat16::exponent() const {
    return static_cast<uint8_t>((bits_ & EXPONENT_MASK) >> MANTISSA_BITS);
}

uint8_t BFloat16::mantissa() const {
    return bits_ & MANTISSA_MASK;
}

int BFloat16::biasedExponent() const {
    return exponent();
}

int BFloat16::unbiasedExponent() const {
    int exp = exponent();
    if (exp == 0) {
        // subnormal numbers or zero
        return 1 - EXPONENT_BIAS;
    }
    return exp - EXPONENT_BIAS;
}

