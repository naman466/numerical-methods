#include "FP32.h"
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

FP32::FP32() : bits_(0) {
    // default = 0
}

FP32::FP32(uint32_t bits) : bits_(bits) {
    // raw bits constructor
}

FP32::FP32(float value) {
    std::memcpy(&bits_, &value, sizeof(float));
}  

FP32::FP32(double value) : FP32(static_cast<float>(value)) {
    // convert through float
}

FP32::FP32(int value) : FP32(static_cast<float>(value)) {
    // convert through float
}

FP32 FP32::fromBits(uint32_t bits) {
    return FP32(bits);
}

FP32 FP32::zero(bool negative) {
    return FP32(negative ? SIGN_MASK : 0);
}

FP32 FP32::nan() {
    return FP32(EXPONENT_MASK | 0x00400000); // quiet NaN
}

FP32 FP32::epsilon() {
    // smallest value such that 1.0 + epsilon != 1.0
    // for FP32: 2^-23
    return FP32(0x34000000); // exponent = 104 (127-23), mantissa = 0
}

bool FP32::sign() const {
    return (bits_ & SIGN_MASK) != 0;
}

uint8_t FP32::exponent() const {
    return static_cast<uint8_t>((bits_ & EXPONENT_MASK) >> MANTISSA_BITS);
}

uint32_t FP32::mantissa() const {
    return bits_ & MANTISSA_MASK;
}

int FP32::biasedExponent() const {
    return exponent();
}

int FP32::unbiasedExponent() const {
    int exp = exponent();
    if (exp == 0) {
        // subnormal numbers or zero
        return 1 - EXPONENT_BIAS;
    }
    return exp - EXPONENT_BIAS;
}

bool FP32::isZero() const {
    // zero if exponent and mantissa are both zero 
    return (bits_ & ~SIGN_MASK) == 0;
}

bool FP32::isInfinity() const {
    // infinity if exponent is all 1s and mantissa is zero
    return (bits_ & EXPONENT_MASK) == EXPONENT_MASK && 
           (bits_ & MANTISSA_MASK) == 0;
}

bool FP32::isNaN() const {
    // NaN if exponent is all 1s and mantissa is non-zero
    return (bits_ & EXPONENT_MASK) == EXPONENT_MASK && 
           (bits_ & MANTISSA_MASK) != 0;
}

bool FP32::isNormal() const {
    uint8_t exp = exponent();
    return exp != 0 && exp != 0xFF;
}

bool FP32::isSubnormal() const {
    return exponent() == 0 && mantissa() != 0;
}

bool FP32::isFinite() const {
    return !isInfinity() && !isNaN();
}

bool FP32::isNegative() const {
    return sign();
}

float FP32::toFloat() const {
    float result;
    std::memcpy(&result, &bits_, sizeof(float));
    return result;
}

double FP32::toDouble() const {
    return static_cast<double>(toFloat());
}

std::string FP32::toBinary() const {
    std::string result;
    result.reserve(35); // 32 bits + 3 spaces
    
    for (int i = 31; i >= 0; --i) {
        result += ((bits_ >> i) & 1) ? '1' : '0';
        // add spaces for readability: S|EEEEEEEE|MMMMMMMMMMMMMMMMMMMMMMM
        if (i == 31 || i == 23) {
            result += ' ';
        }
    }
    
    return result;
}

std::string FP32::toHex() const {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << std::setw(8) 
        << std::setfill('0') << bits_;
    return oss.str();
}

uint32_t FP32::packBits(bool sign, uint8_t exp, uint32_t mant) {
    return ((sign ? 1u : 0u) << 31) | 
           (static_cast<uint32_t>(exp) << MANTISSA_BITS) | 
           (mant & MANTISSA_MASK);
}

std::string FP32::getComponentsString() const {
    std::ostringstream oss;
    oss << "Sign: " << (sign() ? "1" : "0") << "\n";
    oss << "Exponent (biased): " << static_cast<int>(exponent()) 
        << " (0x" << std::hex << std::uppercase << static_cast<int>(exponent()) << ")\n";
    oss << "Exponent (unbiased): " << std::dec << unbiasedExponent() << "\n";
    oss << "Mantissa: 0x" << std::hex << std::uppercase << std::setw(6) 
        << std::setfill('0') << mantissa() << std::dec << "\n";
    
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

void FP32::printDetails(std::ostream& os) const {
    os << "Binary: " << toBinary() << "\n";
    os << "Hex: " << toHex() << "\n";
    os << getComponentsString();
    os << "Decimal value: " << toDouble() << "\n";
}




