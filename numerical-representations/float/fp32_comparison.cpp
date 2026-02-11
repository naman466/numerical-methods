#include "FP32.h"
#include <cmath>

int FP32::compareImpl(const FP32& a, const FP32& b) {
    // handle NaN (unordered for all)
    if (a.isNaN() || b.isNaN()) {
        return 2; // special return value
    }
    
    // handle zeros
    if (a.isZero() && b.isZero()) {
        return 0;
    }
    
    // different signs
    bool sign_a = a.sign();
    bool sign_b = b.sign();
    
    if (sign_a != sign_b) {
        return sign_a ? -1 : 1;
    }
    
    // same sign = compare bits
    // pos numbers, larger bits = larger value
    // neg numbers, larger bits = smaller value 
    if (a.bits_ == b.bits_) {
        return 0;
    }
    
    if (sign_a) {
        // both neg, reverse comp
        return (a.bits_ > b.bits_) ? -1 : 1;
    } else {
        // both pos. normal comp
        return (a.bits_ < b.bits_) ? -1 : 1;
    }
}

bool FP32::operator==(const FP32& other) const {
    // NaN is not equal to anything
    if (isNaN() || other.isNaN()) {
        return false;
    }
    
    // pos and neg zero are equal
    if (isZero() && other.isZero()) {
        return true;
    }
    
    return bits_ == other.bits_;
}

bool FP32::operator!=(const FP32& other) const {
    return !(*this == other);
}

bool FP32::operator<(const FP32& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == -1;
}

bool FP32::operator<=(const FP32& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == -1 || cmp == 0;
}

bool FP32::operator>(const FP32& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == 1;
}

bool FP32::operator>=(const FP32& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == 1 || cmp == 0;
}


FP32 FP32::abs() const {
    // clear the sign bit
    return FP32(bits_ & ~SIGN_MASK);
}

FP32 FP32::sqrt() const {
    // handle special cases
    if (isNaN()) return FP32::nan();
    
    if (isNegative() && !isZero()) {
        return FP32::nan(); // sqrt of negative number
    }
    
    if (isZero()) return *this;
    if (isInfinity()) return FP32::infinity(false);
    
    // use Newton-Raphson iteration on the actual float value
    
    float val = toFloat();
    float result = std::sqrt(val);
    return FP32(result);
}

FP32 abs(const FP32& x) {
    return x.abs();
}

FP32 sqrt(const FP32& x) {
    return x.sqrt();
}