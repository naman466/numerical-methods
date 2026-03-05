#include "BF16.h"
#include <algorithm>

uint16_t BFloat16::roundToNearest(uint32_t value, int shift) {
    // round to nearest, ties to even (bankers rounding)
    if (shift <= 0) return static_cast<uint16_t>(value);
    
    uint32_t mask = (1u << shift) - 1;
    uint32_t halfway = 1u << (shift - 1);
    uint32_t remainder = value & mask;
    uint32_t result = value >> shift;
    
    if (remainder > halfway) {
        // round up
        result++;
    } else if (remainder == halfway) {
        // tie, round down (lsb = 0)
        if (result & 1) {
            result++;
        }
    }
    
    return static_cast<uint16_t>(result);
}

BFloat16 BFloat16::normalize(bool sign, int exp, uint32_t significand) {
    // normalize the significand and adjust exponent
    
    if (significand == 0) {
        return BFloat16::zero(sign);
    }
    
    // find the position of the leading 1
    int leading_bit = 31;
    while (leading_bit >= 0 && !((significand >> leading_bit) & 1)) {
        leading_bit--;
    }
    
    // adjust exponent based on leading bit position
    // want the leading bit at position 7 for implicit bit
    int shift = leading_bit - MANTISSA_BITS;
    exp += shift;
    
    // check for overflow / underflow
    if (exp >= 255) {
        return BFloat16::infinity(sign);
    }
    
    if (exp <= 0) {
        // subnormal or underflow
        int denorm_shift = 1 - exp;
        if (denorm_shift > 31) {
            return BFloat16::zero(sign);
        }
        
        uint16_t mant = roundToNearest(significand, denorm_shift + shift);
        return BFloat16(packBits(sign, 0, static_cast<uint8_t>(mant)));
    }
    
    // normal number, round and remove implicit bit
    uint16_t mant;
    if (shift > 0) {
        mant = roundToNearest(significand, shift) & MANTISSA_MASK;
    } else if (shift < 0) {
        mant = (static_cast<uint16_t>(significand) << (-shift)) & MANTISSA_MASK;
    } else {
        mant = static_cast<uint16_t>(significand) & MANTISSA_MASK;
    }
    
    // check for rounding overflow
    if (mant == 0 && ((significand >> (shift > 0 ? shift - 1 : 0)) & 1)) {
        exp++;
        if (exp >= 255) {
            return BFloat16::infinity(sign);
        }
    }
    
    return BFloat16(packBits(sign, static_cast<uint8_t>(exp), static_cast<uint8_t>(mant)));
}

BFloat16 BFloat16::addImpl(const BFloat16& a, const BFloat16& b) {
    // handle special cases
    if (a.isNaN() || b.isNaN()) return BFloat16::nan();
    
    if (a.isInfinity()) {
        if (b.isInfinity() && a.sign() != b.sign()) {
            return BFloat16::nan(); // Inf - Inf
        }
        return a;
    }
    
    if (b.isInfinity()) return b;
    
    if (a.isZero()) return b;
    if (b.isZero()) return a;
    
    // extract components
    bool sign_a = a.sign();
    bool sign_b = b.sign();
    int exp_a = a.unbiasedExponent();
    int exp_b = b.unbiasedExponent();
    
    // get significands with implicit bit
    uint32_t sig_a = a.mantissa();
    uint32_t sig_b = b.mantissa();
    
    if (a.isNormal()) sig_a |= (1u << MANTISSA_BITS);
    if (b.isNormal()) sig_b |= (1u << MANTISSA_BITS);
    
    // align significands, add extra bits for precision
    sig_a <<= 3;
    sig_b <<= 3;
    
    // make exponents equal by shifting the smaller one
    if (exp_a > exp_b) {
        int diff = exp_a - exp_b;
        if (diff > 15) {
            sig_b = 0;
        } else {
            sig_b >>= diff;
        }
        exp_b = exp_a;
    } else if (exp_b > exp_a) {
        int diff = exp_b - exp_a;
        if (diff > 15) {
            sig_a = 0;
        } else {
            sig_a >>= diff;
        }
        exp_a = exp_b;
    }
    
    // perform addition or subtraction
    uint32_t result_sig;
    bool result_sign;
    
    if (sign_a == sign_b) {
        // ssame sign = add
        result_sig = sig_a + sig_b;
        result_sign = sign_a;
    } else {
        // different sign = subtract smaller from larger
        if (sig_a >= sig_b) {
            result_sig = sig_a - sig_b;
            result_sign = sign_a;
        } else {
            result_sig = sig_b - sig_a;
            result_sign = sign_b;
        }
    }
    
    // normalize result
    return normalize(result_sign, exp_a + EXPONENT_BIAS - 3, result_sig);
}

BFloat16 BFloat16::operator+(const BFloat16& other) const {
    return addImpl(*this, other);
}

BFloat16 BFloat16::operator-(const BFloat16& other) const {
    return addImpl(*this, -other);
}

BFloat16& BFloat16::operator+=(const BFloat16& other) {
    *this = *this + other;
    return *this;
}

BFloat16& BFloat16::operator-=(const BFloat16& other) {
    *this = *this - other;
    return *this;
}

// multiplication

BFloat16 BFloat16::multiplyImpl(const BFloat16& a, const BFloat16& b) {
    if (a.isNaN() || b.isNaN()) return BFloat16::nan();
    
    if (a.isInfinity() || b.isInfinity()) {
        if (a.isZero() || b.isZero()) {
            return BFloat16::nan(); // 0 * Inf
        }
        return BFloat16::infinity(a.sign() != b.sign());
    }
    
    if (a.isZero() || b.isZero()) {
        return BFloat16::zero(a.sign() != b.sign());
    }
    
    // extract components
    bool result_sign = a.sign() != b.sign();
    int exp_a = a.unbiasedExponent();
    int exp_b = b.unbiasedExponent();
    
    // get significands with implicit bit
    uint32_t sig_a = a.mantissa();
    uint32_t sig_b = b.mantissa();
    
    if (a.isNormal()) sig_a |= (1u << MANTISSA_BITS);
    if (b.isNormal()) sig_b |= (1u << MANTISSA_BITS);
    
    // multiply significands
    // sig_a and sig_b are 8-bit fixed-point (1 integer bit + 7 fractional)
    // product will be 16-bit fixed-point, with leading bit at position 14 or 15
    uint32_t result_sig = sig_a * sig_b;
    
    // calculate the final exponent
    // similar to FP32: result_exp = (exp_a + exp_b + BIAS) - expected_shift
    // expected_shift is approx 7 for products in range [1, 4)
    int result_exp = exp_a + exp_b + EXPONENT_BIAS - MANTISSA_BITS;
    
    return normalize(result_sign, result_exp, result_sig);
}

BFloat16 BFloat16::operator*(const BFloat16& other) const {
    return multiplyImpl(*this, other);
}

BFloat16& BFloat16::operator*=(const BFloat16& other) {
    *this = *this * other;
    return *this;
}

// division

BFloat16 BFloat16::operator/(const BFloat16& other) const {
    if (isNaN() || other.isNaN()) return BFloat16::nan();
    
    if (isInfinity()) {
        if (other.isInfinity()) {
            return BFloat16::nan(); // Inf / Inf
        }
        return BFloat16::infinity(sign() != other.sign());
    }
    
    if (other.isInfinity()) {
        return BFloat16::zero(sign() != other.sign());
    }
    
    if (other.isZero()) {
        if (isZero()) {
            return BFloat16::nan(); // 0 / 0
        }
        return BFloat16::infinity(sign() != other.sign());
    }
    
    if (isZero()) {
        return BFloat16::zero(sign() != other.sign());
    }
    
    bool result_sign = sign() != other.sign();
    int exp_a = unbiasedExponent();
    int exp_b = other.unbiasedExponent();
    
    uint32_t sig_a = mantissa();
    uint32_t sig_b = other.mantissa();
    
    if (isNormal()) sig_a |= (1u << MANTISSA_BITS);
    if (other.isNormal()) sig_b |= (1u << MANTISSA_BITS);
    
    sig_a <<= MANTISSA_BITS;
    
    uint32_t result_sig = sig_a / sig_b;
    
    int result_exp = exp_a - exp_b + EXPONENT_BIAS;
    
    return normalize(result_sign, result_exp, result_sig);
}

BFloat16& BFloat16::operator/=(const BFloat16& other) {
    *this = *this / other;
    return *this;
}

// negation

BFloat16 BFloat16::operator-() const {
    // flip sign bit
    return BFloat16(static_cast<uint16_t>(bits_ ^ SIGN_MASK));
}