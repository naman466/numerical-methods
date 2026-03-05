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

// classification

bool BFloat16::isZero() const {
    // zero if exponent and mantissa are  zero 
    return (bits_ & ~SIGN_MASK) == 0;
}

bool BFloat16::isInfinity() const {
    // infinity if exponent is all 1s and mantissa is zero
    return (bits_ & EXPONENT_MASK) == EXPONENT_MASK && 
           (bits_ & MANTISSA_MASK) == 0;
}

bool BFloat16::isNaN() const {
    // nan if exponent is all 1s and mantissa is not zero
    return (bits_ & EXPONENT_MASK) == EXPONENT_MASK && 
           (bits_ & MANTISSA_MASK) != 0;
}

bool BFloat16::isNormal() const {
    uint8_t exp = exponent();
    return exp != 0 && exp != 0xFF;
}

bool BFloat16::isSubnormal() const {
    return exponent() == 0 && mantissa() != 0;
}

bool BFloat16::isFinite() const {
    return !isInfinity() && !isNaN();
}

bool BFloat16::isNegative() const {
    return sign();
}

// conversion

float BFloat16::toFloat() const {
    // bf16 -> float by shifting 16 places
    uint32_t fp32_bits = static_cast<uint32_t>(bits_) << 16;
    float result;
    std::memcpy(&result, &fp32_bits, sizeof(float));
    return result;
}

double BFloat16::toDouble() const {
    return static_cast<double>(toFloat());
}

std::string BFloat16::toBinary() const {
    std::string result;
    result.reserve(19); // 16 bits + 2 spaces + null
    
    for (int i = 15; i >= 0; --i) {
        result += ((bits_ >> i) & 1) ? '1' : '0';
        // add spaces for readability: S|EEEEEEEE|MMMMMMM
        if (i == 15 || i == 7) {
            result += ' ';
        }
    }
    
    return result;
}

std::string BFloat16::toHex() const {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << std::setw(4) 
        << std::setfill('0') << bits_;
    return oss.str();
}

// conversion to / from fp32

uint32_t BFloat16::toFP32Bits() const {
    return static_cast<uint32_t>(bits_) << 16;
}

BFloat16 BFloat16::fromFP32Bits(uint32_t fp32_bits) {
    // round to nearest even for truncation
    uint32_t rounding_bias = 0x7FFF + ((fp32_bits >> 16) & 1);
    uint32_t rounded = fp32_bits + rounding_bias;
    return BFloat16(static_cast<uint16_t>(rounded >> 16));
}

// helper functions

uint16_t BFloat16::packBits(bool sign, uint8_t exp, uint8_t mant) {
    return ((sign ? 1u : 0u) << 15) | 
           (static_cast<uint16_t>(exp) << MANTISSA_BITS) | 
           (mant & MANTISSA_MASK);
}

std::string BFloat16::getComponentsString() const {
    std::ostringstream oss;
    oss << "Sign: " << (sign() ? "1 (negative)" : "0 (positive)") << "\n";
    oss << "Exponent (biased): " << static_cast<int>(exponent()) 
        << " (0x" << std::hex << std::uppercase << static_cast<int>(exponent()) << ")\n";
    oss << "Exponent (unbiased): " << std::dec << unbiasedExponent() << "\n";
    oss << "Mantissa: 0x" << std::hex << std::uppercase << std::setw(2) 
        << std::setfill('0') << static_cast<int>(mantissa()) << std::dec << "\n";
    
    if (isNormal()) {
        oss << "Type: Normal\n";
        oss << "Implicit bit: 1\n";
    } else if (isSubnormal()) {
        oss << "Type: Subnormal\n";
        oss << "Implicit bit: 0\n";
    } else if (isZero()) {
        oss << "Type: Zero\n";
    } else if (isInfinity()) {
        oss << "Type: Infinity\n";
    } else if (isNaN()) {
        oss << "Type: NaN\n";
    }
    
    return oss.str();
}

void BFloat16::printDetails(std::ostream& os) const {
    os << "BFloat16 Details--\n";
    os << "Binary: " << toBinary() << "\n";
    os << "Hex: " << toHex() << "\n";
    os << getComponentsString();
    os << "Decimal value: " << toDouble() << "\n";
}
