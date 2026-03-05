#include "BF16.h"
#include <cmath>

// comparison impl

int BFloat16::compareImpl(const BFloat16& a, const BFloat16& b) {
    // nan is unordered with everything
    if (a.isNaN() || b.isNaN()) {
        return 2; // special return for unordered
    }
    
    // pos and neg zero equal
    if (a.isZero() && b.isZero()) {
        return 0;
    }
    
    // diff signs
    bool sign_a = a.sign();
    bool sign_b = b.sign();
    
    if (sign_a != sign_b) {
        return sign_a ? -1 : 1;
    }
    
    // same sign, compare bits
    if (a.bits_ == b.bits_) {
        return 0;
    }
    
    if (sign_a) {
        // both neg, reverse comparison
        return (a.bits_ > b.bits_) ? -1 : 1;
    } else {
        // both pos, normal comparison
        return (a.bits_ < b.bits_) ? -1 : 1;
    }
}

bool BFloat16::operator==(const BFloat16& other) const {
    // nan is not equal to anything 
    if (isNaN() || other.isNaN()) {
        return false;
    }
    
    // +0 = -0
    if (isZero() && other.isZero()) {
        return true;
    }
    
    return bits_ == other.bits_;
}

bool BFloat16::operator!=(const BFloat16& other) const {
    return !(*this == other);
}

bool BFloat16::operator<(const BFloat16& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == -1;
}

bool BFloat16::operator<=(const BFloat16& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == -1 || cmp == 0;
}

bool BFloat16::operator>(const BFloat16& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == 1;
}

bool BFloat16::operator>=(const BFloat16& other) const {
    int cmp = compareImpl(*this, other);
    return cmp == 1 || cmp == 0;
}

// mathematical functions

BFloat16 BFloat16::abs() const {
    // clear sign bit
    return BFloat16(static_cast<uint16_t>(bits_ & ~SIGN_MASK));
}

BFloat16 BFloat16::sqrt() const {
    // handle special cases
    if (isNaN()) return BFloat16::nan();
    
    if (isNegative() && !isZero()) {
        return BFloat16::nan(); // sqrt of negative number
    }
    
    if (isZero()) return *this;
    if (isInfinity()) return BFloat16::infinity(false);
    
    // float conversion for simplicity
    float val = toFloat();
    float result = std::sqrt(val);
    return BFloat16(result);
}

// non member func 
BFloat16 abs(const BFloat16& x) {
    return x.abs();
}

BFloat16 sqrt(const BFloat16& x) {
    return x.sqrt();
}